#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>

#include "src/glad.c"

struct Game
{
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

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    game->r = std::clamp(xpos / game->X, 0.0, 1.0);
    game->g = std::clamp(ypos / game->Y, 0.0, 1.0);

    std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
    std::cout << "r: " << game->r << " g: " << game->g << " b: " << game->b << std::endl;
}

void clearWindow(GLFWwindow *window, Game *game)
{
    glClearColor(game->r, game->g, 0.4f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderWindow(GLFWwindow *window, Game *game)
{
    clearWindow(window, game);
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
    glfwSetWindowSize(window, game.X - 1, game.Y);
    glfwPollEvents();
    glfwSetWindowSize(window, game.X, game.Y);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, &game);

        renderWindow(window, &game);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}