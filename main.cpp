#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "src/glad.c"

const char *vertex_shader =
    "#version 400 core\n"
    "layout (location = 0) in vec3 aPos;"
    "void main()"
    "{"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
    "}";

const char *fragment_shader =
    "#version 400 core\n"
    "out vec4 FragColor;"
    "uniform vec4 ourColor;"
    "void main()"
    "{"
    "    FragColor = ourColor;"
    "}";

struct Game
{
    unsigned int shaderProgram;
    unsigned int VAO;
    unsigned int color_location;

    GLFWwindow *window;

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

void setup_shaders(Game &game)
{
    // create shaders
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // create shader program and link shaders with it
    game.shaderProgram = glCreateProgram();
    glAttachShader(game.shaderProgram, vertexShader);
    glAttachShader(game.shaderProgram, fragmentShader);
    glLinkProgram(game.shaderProgram);

    glGetProgramiv(game.shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(game.shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    game.color_location = glGetUniformLocation(game.shaderProgram, "ourColor");

    // delete shaders once they are linked, we don't need them anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

    glClearColor(0.8f, greenValue, 0.4f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(game.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_TRIANGLES, 3, 3);
    glDrawArrays(GL_TRIANGLES, 6, 3);
    glDrawArrays(GL_TRIANGLES, 9, 3);
    glDrawArrays(GL_TRIANGLES, 12, 3);

    glUniform4f(game.color_location, game.r, game.g, 0.0f, 0.0f);
}

int main()
{
    Game game = Game{.X = 800, .Y = 600, .a = 1};

    if (init_gl(game) != 0)
        return -1;

    setup_shaders(game);

    glGenVertexArrays(1, &game.VAO);

    glBindVertexArray(game.VAO);

    float vertices[] = {
        -0.1f, -0.1f, 0.0f,
        0.1f, -0.1f, 0.0f,
        0.0f, 0.1f, 0.0f,

        -0.8f, -0.8f, 0.0f,
        -0.4f, -0.8f, 0.0f,
        -0.4f, 0.0f, 0.0f,

        0.8f, 0.8f, 0.0f,
        0.4f, 0.8f, 0.0f,
        0.4f, 0.0f, 0.0f,

        0.8f, -0.8f, 0.0f,
        0.4f, -0.8f, 0.0f,
        0.4f, 0.0f, 0.0f,

        -0.8f, 0.8f, 0.0f,
        -0.4f, 0.8f, 0.0f,
        -0.4f, 0.0f, 0.0f};

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(game.window))
    {
        processInput(game);

        renderWindow(game);

        glfwSwapBuffers(game.window);
        glfwPollEvents();
    }

    return 0;
}