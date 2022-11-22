#include "ShaderProgram.h"
#include "DebugMsg.h"

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>



ShaderProgram::ShaderProgram() {
    mCount = 0;
    mShadersList = std::vector<unsigned int>();
    mProgram = 0;
}



void ShaderProgram::createShader(shaderType type, const char* shaderSource) {
    unsigned int shader = 0;
    std::ifstream inFile;
    inFile.open(shaderSource);
    std::stringstream fileStream;
    fileStream << inFile.rdbuf();
    std::string shaderString = fileStream.str();
    const char* shaderChar = shaderString.c_str();

    switch (type) {
        case Vertex:
            shader = glCreateShader(GL_VERTEX_SHADER);
            break;
        case Fragment:
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        default:
            DebugMessenger::message(DebugMessenger::Error, "Invalid shader type!!");
            exit(-1);
            break;
    }

    glShaderSource(shader, 1, &shaderChar, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string error = "Shader type:";
        error.append(std::to_string(type));
        error.append(" compilation failed!!\n");
        error.append(shaderSource);
        DebugMessenger::message(DebugMessenger::Error, error);
        exit(-1);
    }
    mShadersList.push_back(shader);
    mCount += 1;
}



void ShaderProgram::linkShaders() {
    unsigned int shaderProgram = glCreateProgram();

    for (unsigned int i = 0; i < mCount; i++) {
        glAttachShader(shaderProgram, mShadersList[i]);
    }

    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        DebugMessenger::message(DebugMessenger::Error, "Shader linking failed!!");
        exit(-1);
    }

    for (unsigned int i = 0; i < mCount; i++) {
        glDeleteShader(mShadersList[i]);
    }
    mProgram = shaderProgram;
}



unsigned int ShaderProgram::getProgramID() {
    return mProgram;
}



void ShaderProgram::useProgram() {
    glUseProgram(mProgram);
}