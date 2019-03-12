#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>

#include "src/glad.c"

const char *vertex_shader =
    "#version 400\n"
    "in vec3 vp;"
    "void main() {"
    "  gl_Position = vec4(vp, 1.0);"
    "}";

const char *fragment_shader =
    "#version 400\n"
    "out vec4 frag_colour;"
    "void main() {"
    "  frag_colour = vec4(0.5, 1.0, 0.5, 1.0);"
    "}";

struct Game
{
    GLuint vao;

    GLuint shader;

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

void processInput(GLFWwindow *window, Game *game)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_KEY_UP)
    {
        game->r += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_KEY_UP)
    {
        game->g += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_KEY_UP)
    {
        game->b += 0.1f;
    }

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    game->r = std::clamp(xpos / game->X, 0.0, 1.0);
    game->g = std::clamp(ypos / game->Y, 0.0, 1.0);

    std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
    std::cout << "r: " << game->r << " g: " << game->g << " b: " << game->b << std::endl;
}

void clearWindow(GLFWwindow *window, Game *game)
{
    glClearColor(1, 0, 0.4f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderWindow(GLFWwindow *window, Game *game)
{
    clearWindow(window, game);

    glUseProgram(game->shader);
    glBindVertexArray(game->vao);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "conway", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Game game;
    game.X = 800;
    game.Y = 600;
    game.a = 1;
    glViewport(0, 0, game.X, game.Y);

    // this is a stupid workaround around a bug in Mojave. Figure out why this sucks'
    // https://stackoverflow.com/questions/52509427/mac-mojave-opengl
    // this doesn't work, it makes the rendering look incorrect when the program starts
    glfwSetWindowSize(window, game.X-1, game.Y);
    glfwPollEvents();
    glfwSetWindowSize(window, game.X, game.Y);

    float points[] = {0.0f, 0.5f, 0.0f,
                      0.5f, -0.5f, 0.0f,
                      -0.5f, -0.5f, 0.0f};

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

    game.vao = 0;
    glGenVertexArrays(1, &game.vao);
    glBindVertexArray(game.vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    game.shader = glCreateProgram();
    glAttachShader(game.shader, fs);
    glAttachShader(game.shader, vs);
    glLinkProgram(game.shader);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, &game);

        renderWindow(window, &game);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}