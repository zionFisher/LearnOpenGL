/* Homework 2.3 sierpinski
 * base on GLFW, GLAD and VSCODE
 */

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "camera.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace glm;

// multiply WIDTH by 2 / sqrt(3) to ensure the triangle is a regular triangle
const unsigned int WIDTH = 500 * 2 / sqrt(3);
const unsigned int HEIGHT = 500;

// triangle vertices array
const float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.0f, 0.5f, 0.0f,
        0.0f, -0.5f, 0.5f};

const int NumPoints = 5000;

vec3 points[2 * NumPoints];

// local path to access shader program
const std::string SHADER_PATH = "./homework/shader/";

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

unsigned int init();
void display();
void triangle(vec3 a, vec3 b, vec3 c, int colorIndex);
void tetra(vec3 a, vec3 b, vec3 c, vec3 d);
void divideTerra(vec3 a, vec3 b, vec3 c, vec3 d, int count);

int main()
{
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CG homework 1", NULL, NULL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);

    // initialize GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    
    unsigned int shaderProgram = init();

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // clear the viewport
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, NumPoints);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

unsigned int init()
{
    vec3 vertices[4] =
        {
            vec3(0.0, 0.0, -1.0),
            vec3(0.0, 0.942809, 0.333333),
            vec3(-0.816497, -0.471405, 0.333333),
            vec3(0.816497, -0.471405, 0.333333)};

    divideTerra(vertices[0], vertices[1], vertices[2], vertices[3], 4);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // load shader
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    vertexShaderFile.open(SHADER_PATH + "2.3-sierpinski.vs");
    fragmentShaderFile.open(SHADER_PATH + "2.3-sierpinski.fs");

    std::stringstream vertexShaderStream;
    std::stringstream fragmentShaderStream;
    vertexShaderStream << vertexShaderFile.rdbuf();
    fragmentShaderStream << fragmentShaderFile.rdbuf();

    std::string vertexCode;
    std::string fragmentCode;
    vertexCode = vertexShaderStream.str();
    fragmentCode = fragmentShaderStream.str();
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    vertexShaderFile.close();
    fragmentShaderFile.close();

    // generate shader and shader program
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // optional
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0, 1.0, 1.0, 1.0);

    return shaderProgram;
}

void triangle(vec3 a, vec3 b, vec3 c, int colorIndex)
{
    static vec3 base_color[] =
        {
            vec3(1.0, 0.0, 0.0),
            vec3(0.0, 1.0, 0.0),
            vec3(0.0, 0.0, 1.0),
            vec3(0.0, 0.0, 0.0),
        };

    static int index = 0;
    points[index++] = a;
    points[index++] = base_color[colorIndex];
    points[index++] = b;
    points[index++] = base_color[colorIndex];
    points[index++] = c;
    points[index++] = base_color[colorIndex];
}

void tetra(vec3 a, vec3 b, vec3 c, vec3 d)
{
    triangle(a, c, b, 0);
    triangle(a, c, d, 1);
    triangle(a, b, d, 2);
    triangle(b, c, d, 3);
}

void divideTerra(vec3 a, vec3 b, vec3 c, vec3 d, int count)
{
    if (count == 0)
        tetra(a, b, c, d);
    else
    {
        vec3 mid1 = vec3((a.x + b.x) / 2.0, (a.y + b.y) / 2.0, (a.z + b.z) / 2.0);
        vec3 mid2 = vec3((a.x + c.x) / 2.0, (a.y + c.y) / 2.0, (a.z + c.z) / 2.0);
        vec3 mid3 = vec3((a.x + d.x) / 2.0, (a.y + d.y) / 2.0, (a.z + d.z) / 2.0);
        vec3 mid4 = vec3((b.x + c.x) / 2.0, (b.y + c.y) / 2.0, (b.z + c.z) / 2.0);
        vec3 mid5 = vec3((c.x + d.x) / 2.0, (c.y + d.y) / 2.0, (c.z + d.z) / 2.0);
        vec3 mid6 = vec3((b.x + d.x) / 2.0, (b.y + d.y) / 2.0, (b.z + d.z) / 2.0);

        divideTerra(a, mid1, mid2, mid3, count - 1);
        divideTerra(mid1, b, mid4, mid6, count - 1);
        divideTerra(mid2, mid4, c, mid5, count - 1);
        divideTerra(mid3, mid6, mid5, d, count - 1);
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}