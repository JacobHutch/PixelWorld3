#pragma once

#include "glinc.h"
#include <glm/glm.hpp>
#include <string>



class Game {
    public:
    Game(const glm::vec3 backgroundColor = glm::vec3(0.2f, 0.4f, 0.6f),const glm::uvec2 winSize = {1200,800},
            const glm::uvec2 worldSize = {500,500}, const glm::uvec2 viewSizeArg = {1000,1000},
            const char * title = "Pixel World 3");

    void setup();
    void gameLoop();
    void processInput(GLFWwindow* window);

    private:
    GLFWwindow* mWindow;
    unsigned int mShader;

    const glm::vec3 mBackgroundColor;
    const glm::uvec2 mWinSize;
    const glm::uvec2 mWorldSize;
    const glm::uvec2 mViewSizeArg;
    const char* mTitle;

    //if mWorldSize[i] is smaller than mViewSizeArg[i], mViewSizeArg[i] is unnecessary since it's too big
    const glm::uvec2 mViewSize = {glm::min(mWorldSize.x, mViewSizeArg.x), glm::min(mWorldSize.y, mViewSizeArg.y)};
};