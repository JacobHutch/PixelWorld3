#include "Game.h"
#include "DebugMsg.h"
#include "ShaderProgram.h"
#include "World.h"

#include "glinc.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <vector>
#include <ctime>



Game::Game(const glm::vec3 backgroundColor,const glm::uvec2 winSize,
            const glm::uvec2 worldSize, const glm::uvec2 viewSizeArg,
            const char * title) : mBackgroundColor(backgroundColor),
            mWinSize(winSize),mWorldSize(worldSize),
            mViewSizeArg(viewSizeArg),mTitle(title) {}



void Game::setup() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    mWindow = glfwCreateWindow(mWinSize.x, mWinSize.y, mTitle, nullptr, nullptr);
    if (mWindow == NULL) {
        DebugMessenger::message(DebugMessenger::Error, "Failed to open window!!");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, Util::framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        DebugMessenger::message(DebugMessenger::Error, "Failed to initialize GLAD!!");
        exit(-1);
    }
}



void Game::gameLoop() {
    ShaderProgram squareShader = ShaderProgram();
    squareShader.createShader(ShaderProgram::Vertex, "shaders/SquareShaderVertex.glsl");
    squareShader.createShader(ShaderProgram::Fragment, "shaders/SquareShaderFragment.glsl");
    squareShader.linkShaders();
    unsigned int mShader = squareShader.getProgramID();
    squareShader.useProgram();

    int winWidth = mWinSize.x;
    int winHeight = mWinSize.y;

    //I'm used to the top left being the origin, with x+ facing right and y+ facing down
    glm::mat4 projection = glm::ortho(0.0, (double)(mWorldSize.x+2), (double)(mWorldSize.y+2), 0.0, 0.1, 100.0);
    glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3(1.0, 1.0, -1.0));

    //double tileSize = glm::min(winWidth / mViewSize.x, winHeight / mViewSize.y);
    double tileX, tileY, tileSizeX, tileSizeY;

    World world(mWorldSize.x, mWorldSize.y);

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
    bool pause = false;

	while (!glfwWindowShouldClose(mWindow)) {
        processInput(mWindow);
        glClearColor(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //window resize math
        glfwGetWindowSize(mWindow, &winWidth, &winHeight);

        tileSizeX = ((double)winWidth) / (mViewSize.x + 2);
        tileSizeY = ((double)winHeight) / (mViewSize.y + 2);
        //tileSize = glm::min(tileSizeX, tileSizeY);
        if (tileSizeX < tileSizeY) {
            tileX = (double)mViewSize.x + 2;
            tileY = ((double)mViewSize.x + 2) * ((double)winHeight / winWidth);
        } else {
            tileX = ((double)mViewSize.y + 2) * ((double)winWidth / winHeight);
            tileY = (double)mViewSize.y + 2;
        }

        view = glm::translate(glm::mat4(1.0), glm::vec3((tileX - mViewSize.x) / 2.0, (tileY - mViewSize.y) / 2.0, -1.0));
        projection = glm::ortho(0.0, tileX, tileY, 0.0, 0.1, 100.0);
        glUniformMatrix4fv(glGetUniformLocation(mShader, "view"), 1, GL_FALSE, &(view[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(mShader, "proj"), 1, GL_FALSE, &(projection[0][0]));

        //glUniform1i(glGetUniformLocation(shader, "uTime"), std::time(nullptr));

        if (!world.finished() && !pause) {
            world.step();
        } else if (!finished) {
            finished = true;
            timeE = std::time(nullptr);
            totalTime = timeE - timeS;
            int minutes = totalTime / 60;
            int seconds = totalTime % 60;
            double frameRate = (double)(mWorldSize.x * mWorldSize.y) / totalTime;
            std::cout << "Time: " << minutes << "m" << seconds << "s" << std::endl;
            std::cout << "Average FPS: " << frameRate << std::endl;
        }
        world.draw();

        //glfwSwapBuffers(window);
        glFlush();
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(mShader);

    glfwTerminate();
}



void Game::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}