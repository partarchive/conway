#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

const int MAX_INFO_LOG = 512;

struct Game
{
    unsigned int shaderProgram;
    unsigned int VAO;
    unsigned int color_location;
    unsigned int offset;

    GLFWwindow *window;

    float accum_time;

    float move_rate;

    struct {
        float previous;
        float now;
        float delta;
    } time;

    int X;
    int Y;

    float r;
    float g;
    float b;
    float a;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int file_length(std::ifstream &file)
{
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    return length;
}

unsigned int load_shader(Game &game, char const *filename, unsigned int shader_type)
{
    std::ifstream file(filename, std::ifstream::in);

    int length = file_length(file);

    char *buffer = new char[length];
    file.read(buffer, length);
    file.close();

    unsigned int shader;
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &buffer, &length);
    glCompileShader(shader);

    int success;
    char infoLog[MAX_INFO_LOG];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, MAX_INFO_LOG, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return -1;
    }

    delete[] buffer;

    return shader;
}

int setup_shaders(Game &game)
{
    // create shaders
    unsigned int vertex_shader = load_shader(game, "shader/vertex.glsl", GL_VERTEX_SHADER);
    if (vertex_shader < 0)
        return vertex_shader;

    unsigned int fragment_shader = load_shader(game, "shader/fragment.glsl", GL_FRAGMENT_SHADER);
    if (fragment_shader < 0)
        return fragment_shader;

    game.shaderProgram = glCreateProgram();
    glAttachShader(game.shaderProgram, vertex_shader);
    glAttachShader(game.shaderProgram, fragment_shader);
    glLinkProgram(game.shaderProgram);

    int success;
    char infoLog[MAX_INFO_LOG];
    glGetProgramiv(game.shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(game.shaderProgram, MAX_INFO_LOG, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    game.color_location = glGetUniformLocation(game.shaderProgram, "ourColor");
    game.offset = glGetUniformLocation(game.shaderProgram, "offset");

    // delete shaders once they are linked, we don't need them anymore
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return 0;
}

int init_gl(Game &game)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    game.window = glfwCreateWindow(game.X, game.Y, "conway", NULL, NULL);
    if (game.window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(game.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(game.window, framebuffer_size_callback);

    glViewport(0, 0, game.X, game.Y);

    // this is a stupid workaround around a bug in Mojave. Figure out why this sucks'
    // https://stackoverflow.com/questions/52509427/mac-mojave-opengl
    glfwSetWindowSize(game.window, game.X - 1, game.Y);
    glfwPollEvents();
    glfwSetWindowSize(game.window, game.X, game.Y);

    return 0;
}

void processInput(Game &game)
{
    if (glfwGetKey(game.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(game.window, true);

    double xpos, ypos;
    glfwGetCursorPos(game.window, &xpos, &ypos);

    game.r = std::clamp(xpos / game.X, 0.0, 1.0);
    game.g = std::clamp(ypos / game.Y, 0.0, 1.0);

    // std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
    // std::cout << "r: " << game->r << " g: " << game->g << " b: " << game->b << std::endl;
}

void renderWindow(Game &game)
{
    glUseProgram(game.shaderProgram);

    // glClearColor(0.8f, 0.0f, 0.4f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(game.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_TRIANGLES, 6, 3);
    // glDrawArrays(GL_TRIANGLES, 12, 3);
    // glDrawArrays(GL_TRIANGLES, 18, 3);
    // glDrawArrays(GL_TRIANGLES, 24, 3);

    glUniform4f(game.color_location, game.r, game.g, 0.0f, 0.0f);
    game.accum_time += game.time.delta;
    glUniform3f(game.offset, (sin(game.accum_time * game.move_rate) / 2.0f) , 0.0f, 0.0f);
}

int main()
{
    Game game = Game{.X = 800, .Y = 600, .a = 1};

    if (int res = init_gl(game) < 0)
        return res;

    if (int res = setup_shaders(game) < 0)
        return res;

    glGenVertexArrays(1, &game.VAO);

    glBindVertexArray(game.VAO);

    float vertices[] = {
        0.2f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
        -0.2f, -0.2f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 1.0f,   // top
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(game.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    game.time.previous = glfwGetTime();
    while (!glfwWindowShouldClose(game.window))
    {
        // common part, do this only once
        game.time.now = glfwGetTime();
        game.time.delta = game.time.now - game.time.previous;
        game.time.previous = game.time.now;

        processInput(game);

        renderWindow(game);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Triangle Shit");

            ImGui::SliderFloat("move rate", &game.move_rate, 0.0f, 5.0f, "%.3f");

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(game.window);
    }

    return 0;
}