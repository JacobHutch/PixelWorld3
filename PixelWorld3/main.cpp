#include "DebugMsg.h"
#include "ShaderProgram.h"
#include "World.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <vector>

int main();

void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
    //app args
    const glm::uvec2 winSize = {1200,800};
    const glm::uvec2 worldSize = {100,100};
    const glm::uvec2 viewSizeArg = {1000,1000};
    const char* title = "Pixel World 3";
    const glm::vec3 backgroundColor = glm::vec3(0.2f, 0.4f, 0.6f);

    //if worldSize[i] is smaller than viewSizeArg[i], viewSizeArg[i] is unnecessary since it's too big
    const glm::uvec2 viewSize = {glm::min(worldSize.x, viewSizeArg.x), glm::min(worldSize.y, viewSizeArg.y)};

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(winSize.x, winSize.y, title, nullptr, nullptr);
    if (window == NULL) {
        DebugMessenger::message(DebugMessenger::Error, "Failed to open window!!");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        DebugMessenger::message(DebugMessenger::Error, "Failed to initialize GLAD!!");
        return -1;
    }

    ShaderProgram squareShader = ShaderProgram();
    squareShader.createShader(ShaderProgram::Vertex, "SquareShaderVertex.glsl");
    squareShader.createShader(ShaderProgram::Fragment, "SquareShaderFragment.glsl");
    squareShader.linkShaders();
    unsigned int shader = squareShader.getProgramID();
    squareShader.useProgram();

    int winWidth = winSize.x;
    int winHeight = winSize.y;

    //I'm used to the top left being the origin, with x+ facing right and y+ facing down
    glm::mat4 projection = glm::ortho(0.0, (double)(worldSize.x+2), (double)(worldSize.y+2), 0.0, 0.1, 100.0);
    glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3(1.0, 1.0, -1.0));

    double tileSize = glm::min(winWidth / viewSize.x, winHeight / viewSize.y);
    double tileX, tileY, tileSizeX, tileSizeY;

    World world(worldSize.x, worldSize.y);

    std::vector<float> vertices = world.getVertices();
    std::vector<int> indices = world.getIndices();

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    world.wfcGen();
    int timeE, totalTime;
    bool finished = false;
    int timeS = std::time(nullptr);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //window resize math
        glfwGetWindowSize(window, &winWidth, &winHeight);

        tileSizeX = ((double)winWidth) / (viewSize.x + 2);
        tileSizeY = ((double)winHeight) / (viewSize.y + 2);
        tileSize = glm::min(tileSizeX, tileSizeY);
        if (tileSizeX < tileSizeY) {
            tileX = (double)viewSize.x + 2;
            tileY = ((double)viewSize.x + 2) * ((double)winHeight / winWidth);
        } else {
            tileX = ((double)viewSize.y + 2) * ((double)winWidth / winHeight);
            tileY = (double)viewSize.y + 2;
        }

        view = glm::translate(glm::mat4(1.0), glm::vec3((tileX - viewSize.x) / 2.0, (tileY - viewSize.y) / 2.0, -1.0));
        projection = glm::ortho(0.0, tileX, tileY, 0.0, 0.1, 100.0);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &(view[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, &(projection[0][0]));

        if (!world.finished()) {
            world.step();
        } else if (!finished) {
            finished = true;
            timeE = std::time(nullptr);
            totalTime = timeE - timeS;
            std::cout << "Time (seconds): " << totalTime << std::endl;
        }
        world.draw();

        //glfwSwapBuffers(window);
        glFlush();
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader);

    glfwTerminate();

    return 0;
}



void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}



void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}