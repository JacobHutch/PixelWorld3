#include "DebugMsg.h"

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
unsigned int createShader(unsigned int type, const char* shaderSource);
unsigned int linkShaders(unsigned int length, unsigned int* shaders);

std::vector<float> genVertices(const glm::uvec2 worldSize);
std::vector<int> genIndices(const glm::uvec2 worldSize);

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "uniform mat4 proj;\n"
    "uniform mat4 view;\n"
    "uniform mat4 model;\n"
    "out vec3 color;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = proj * view * model * vec4(aPos, 1.0f);\n"
    "   color = aColor;\n"
    "}\0";
const char* fragmentShaderSource = "#version 330 core\n"
    "in vec3 color;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(color, 1.0f);\n"
    "}\n\0";

int main() {
    //app args
    const glm::uvec2 winSize = {800,600};
    const glm::uvec2 worldSize = {17,10};
    const glm::uvec2 viewSizeArg = {41,41};
    const char* title = "Pixel World 3";

    //if worldSize[i] is smaller than viewSizeArg[i], viewSizeArg[i] is unnecessary since it's too big
    const glm::uvec2 viewSize = {glm::min(worldSize.x, viewSizeArg.x), glm::min(worldSize.y, viewSizeArg.y)};

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    unsigned int vertexShader = createShader(0, vertexShaderSource);
    unsigned int fragmentShader = createShader(1, fragmentShaderSource);

    unsigned int shaders[2] = {vertexShader, fragmentShader};
    unsigned int shader = linkShaders(2, shaders);
    glUseProgram(shader);

    int winWidth = winSize.x;
    int winHeight = winSize.y;

    //I'm used to the top left being the origin, with x+ facing right and y+ facing down
    glm::mat4 projection = glm::ortho(0.0, (double)(worldSize.x+2), (double)(worldSize.y+2), 0.0, 0.1, 100.0);
    glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3(1.0, 1.0, -1.0));
    const glm::mat4 model(1.0);

    glUniformMatrix4fv(glGetUniformLocation(shader,"model"),1,GL_FALSE,&(model[0][0]));

    //create tiles
    double tileSize = glm::min(winWidth / viewSize.x, winHeight / viewSize.y);
    double tileX, tileY, tileSizeX, tileSizeY;

    const unsigned int viewXHalf = viewSize.x / 2;
    const unsigned int viewYHalf = viewSize.y / 2;

    const unsigned int winPadX = (winSize.x - (tileSize * viewSize.x)) / 2;
    const unsigned int winPadY = (winSize.y - (tileSize * viewSize.y)) / 2;

    std::vector<float> vertices = genVertices(worldSize);
    std::vector<int> indices = genIndices(worldSize);

    std::cout << vertices.size() << " " << indices.size() << " " << indices.back() << std::endl;

    for (int i = 0; i < 6000; i+=6) {
        //std::cout << vertices[i] << vertices[i+1] << vertices[i+2] << " " << vertices[i + 3] << vertices[i + 4] << vertices[i + 5] << "\n" << std::endl;
    }

    for (int i = indices.size()-1; i > 21000; i--) {
        //std::cout << indices[i] << std::endl;
    }

    /*float vertices[] = {
         0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         1.0f, 1.0f, 0.0f, 0.2f, 0.5f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f
    };

    
    int indices[] = {
        0,2,3,
        3,2,1
    };*/

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //indices = std::vector<int>{0,1,2,1,2,3,4,5,6,5,6,7};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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
        //glDrawArrays(GL_TRIANGLES, 0, vertices.size()/4);
        //my math for genIndices is busted rn
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
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



//0 - vertex, 1 - fragment, will add more when needed
unsigned int createShader(unsigned int type, const char* shaderSource) {
    unsigned int shader = 0;
    switch (type) {
        case 0:
            shader = glCreateShader(GL_VERTEX_SHADER);
            break;
        case 1:
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        default:
            DebugMessenger::message(DebugMessenger::Error, "Invalid shader type!!");
            exit(-1);
            break;
    }
    
    glShaderSource(shader, 1, &shaderSource, NULL);
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
    return shader;
}



unsigned int linkShaders(unsigned int length, unsigned int* shaders) {
    unsigned int shaderProgram = glCreateProgram();

    for (unsigned int i = 0; i < length; i++) {
        glAttachShader(shaderProgram, shaders[i]);
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

    for (unsigned int i = 0; i < length; i++) {
        glDeleteShader(shaders[i]);
    }
    return shaderProgram;
}



std::vector<float> genVertices(const glm::uvec2 gridSize) {
    //each square has 4 vertices
    //vertex order in list: tl, tr, bl, br -- needs to be consistent with genIndices!!
    //vertices currently have position3 and color3, so multiply by 6 for stride
    std::vector<float> verts;
    for (int i = 0; i < gridSize.x; i++) {
        for (int j = 0; j < gridSize.y; j++) {

            //tl
            verts.push_back(0.0f + i);
            verts.push_back(0.0f + j);
            verts.push_back(0.0f);

            verts.push_back(1.0f);
            verts.push_back(0.0f);
            verts.push_back(0.0f);

            //tr
            verts.push_back(1.0f + i);
            verts.push_back(0.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(1.0f);
            verts.push_back(0.0f);

            //bl
            verts.push_back(0.0f + i);
            verts.push_back(1.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(0.0f);
            verts.push_back(0.0f);

            //tr and bl again while i fix the element array
            //tr
            /*verts.push_back(1.0f + i);
            verts.push_back(0.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(1.0f);
            verts.push_back(0.0f);

            //bl
            verts.push_back(0.0f + i);
            verts.push_back(1.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(0.0f);
            verts.push_back(0.0f);*/

            //br
            verts.push_back(1.0f + i);
            verts.push_back(1.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(0.0f);
            verts.push_back(1.0f);
        }
    }
    return verts;
}



std::vector<int> genIndices(const glm::uvec2 gridSize) {
    //as with genVertices, corner order is tl,tr,bl,br
    //triangles will be drawn as top left and bottom right (tl,bl,tr, tr,bl,br)
    std::vector<int> inds;
    int base, tl, tr, bl, br;
    for (int i = 0; i < gridSize.x; i++) {
        for (int j = 0; j < gridSize.y; j++) {

            base = (i + (j * gridSize.x)) * 4;
            tl = base;
            tr = base + 1;
            bl = base + 2;
            br = base + 3;

            inds.push_back(tl);
            inds.push_back(bl);
            inds.push_back(tr);

            inds.push_back(tr);
            inds.push_back(bl);
            inds.push_back(br);
        }
    }
    return inds;
}