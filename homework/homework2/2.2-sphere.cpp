/* Homework 2.2 sphere
 * base on GLFW, GLAD and VSCODE
 */

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include "camera.hpp"

const unsigned int WIDTH = 500;
const unsigned int HEIGHT = 500;

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

const float fan1[] =
    {
        0.0f, 1.0f, 0.0f,
        0.5f, 0.866f, 0.0f,
        0.433f, 0.866f, 0.25f,
        0.25f, 0.866f, 0.433f,
        0.0f, 0.866f, 0.5f,
        -0.25f, 0.866f, 0.433f,
        -0.433f, 0.866f, 0.25f,
        -0.5f, 0.866f, 0.0f,
        -0.433f, 0.866f, -0.25f,
        -0.25f, 0.866f, -0.433f,
        -0.0f, 0.866f, -0.5f,
        0.25f, 0.866f, -0.433f,
        0.433f, 0.866f, -0.25f};

const float fan2[] =
    {
        0.0f, -1.0f, 0.0f,
        0.5f, -0.866f, 0.0f,
        0.433f, -0.866f, 0.25f,
        0.25f, -0.866f, 0.433f,
        0.0f, -0.866f, 0.5f,
        -0.25f, -0.866f, 0.433f,
        -0.433f, -0.866f, 0.25f,
        -0.5f, -0.866f, 0.0f,
        -0.433f, -0.866f, -0.25f,
        -0.25f, -0.866f, -0.433f,
        -0.0f, -0.866f, -0.5f,
        0.25f, -0.866f, -0.433f,
        0.433f, -0.866f, -0.25f};

const float strip1[] =
    {
        0.5f, 0.866f, 0.0f,
        0.433f, 0.866f, 0.25f,
        0.75f, 0.5f, 0.433f,
        0.25f, 0.866f, 0.433f,
        0.433f, 0.5f, 0.75f,
        0.0f, 0.866f, 0.5f,
        0.0f, 0.5f, 0.866f,
        -0.25f, 0.866f, 0.433f,
        -0.433f, 0.5f, 0.75f,
        -0.433f, 0.866f, 0.25f,
        -0.75f, 0.5f, 0.433f,
        -0.5f, 0.866f, 0.0f,
        -0.866f, 0.5f, 0.0f,
        -0.433f, 0.866f, -0.25f,
        -0.75f, 0.5f, -0.433f,
        -0.25f, 0.866f, -0.433f,
        -0.433f, 0.5f, -0.75f,
        0.0f, 0.866f, -0.5f,
        0.0f, 0.5f, -0.866f,
        0.25f, 0.866f, -0.433f,
        0.433f, 0.5f, -0.75f,
        0.433f, 0.866f, -0.25f,
        0.75f, 0.5f, -0.433f,
        0.5f, 0.866f, 0.0f,
        0.866f, 0.5f, 0.0f,
        0.75f, 0.5f, 0.433f};

const float strip2[] =
    {
        0.5f, -0.866f, 0.0f,
        0.433f, -0.866f, 0.25f,
        0.75f, -0.5f, 0.433f,
        0.25f, -0.866f, 0.433f,
        0.433f, -0.5f, 0.75f,
        0.0f, -0.866f, 0.5f,
        0.0f, -0.5f, 0.866f,
        -0.25f, -0.866f, 0.433f,
        -0.433f, -0.5f, 0.75f,
        -0.433f, -0.866f, 0.25f,
        -0.75f, -0.5f, 0.433f,
        -0.5f, -0.866f, 0.0f,
        -0.866f, -0.5f, 0.0f,
        -0.433f, -0.866f, -0.25f,
        -0.75f, -0.5f, -0.433f,
        -0.25f, -0.866f, -0.433f,
        -0.433f, -0.5f, -0.75f,
        0.0f, -0.866f, -0.5f,
        0.0f, -0.5f, -0.866f,
        0.25f, -0.866f, -0.433f,
        0.433f, -0.5f, -0.75f,
        0.433f, -0.866f, -0.25f,
        0.75f, -0.5f, -0.433f,
        0.5f, -0.866f, 0.0f,
        0.866f, -0.5f, 0.0f,
        0.75f, -0.5f, 0.433f};

const float strip3[] =
    {
        0.866f, 0.5f, 0.0f,
        0.75f, 0.5f, 0.433f,
        0.866f, 0.0f, 0.5f,
        0.433f, 0.5f, 0.75f,
        0.5f, 0.0f, 0.866f,
        0.0f, 0.5f, 0.866f,
        0.0f, 0.0f, 1.0f,
        -0.433f, 0.5f, 0.75f,
        -0.5f, 0.0f, 0.866f,
        -0.75f, 0.5f, 0.433f,
        -0.866f, 0.0f, 0.5f,
        -0.866f, 0.5f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -0.75f, 0.5f, -0.433f,
        -0.866f, 0.0f, -0.5f,
        -0.433f, 0.5f, -0.75f,
        -0.5f, 0.0f, -0.866f,
        0.0f, 0.5f, -0.866f,
        0.0f, 0.0f, -1.0f,
        0.433f, 0.5f, -0.75f,
        0.5f, 0.0f, -0.866f,
        0.75f, 0.5f, -0.433f,
        0.866f, 0.0f, -0.5f,
        0.866f, 0.5f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.866f, 0.0f, 0.5f};

const float strip4[] =
    {
        0.866f, -0.5f, 0.0f,
        0.75f, -0.5f, 0.433f,
        0.866f, -0.0f, 0.5f,
        0.433f, -0.5f, 0.75f,
        0.5f, -0.0f, 0.866f,
        0.0f, -0.5f, 0.866f,
        0.0f, -0.0f, 1.0f,
        -0.433f, -0.5f, 0.75f,
        -0.5f, -0.0f, 0.866f,
        -0.75f, -0.5f, 0.433f,
        -0.866f, -0.0f, 0.5f,
        -0.866f, -0.5f, 0.0f,
        -1.0f, -0.0f, 0.0f,
        -0.75f, -0.5f, -0.433f,
        -0.866f, -0.0f, -0.5f,
        -0.433f, -0.5f, -0.75f,
        -0.5f, -0.0f, -0.866f,
        0.0f, -0.5f, -0.866f,
        0.0f, -0.0f, -1.0f,
        0.433f, -0.5f, -0.75f,
        0.5f, -0.0f, -0.866f,
        0.75f, -0.5f, -0.433f,
        0.866f, -0.0f, -0.5f,
        0.866f, -0.5f, 0.0f,
        1.0f, -0.0f, 0.0f,
        0.866f, -0.0f, 0.5f};

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

    unsigned int VAOs[6], VBOs[6];

    glGenVertexArrays(6, &VAOs[0]);

    glGenBuffers(6, &VBOs[0]);

    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan1), fan1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Vertex attributes stay the same
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan2), fan2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Vertex attributes stay the same
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[2]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(strip1), strip1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Vertex attributes stay the same
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[3]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(strip2), strip2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Vertex attributes stay the same
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[4]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(strip3), strip3, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Vertex attributes stay the same
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[5]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(strip4), strip4, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Vertex attributes stay the same
    glEnableVertexAttribArray(0);

    // load shader
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    vertexShaderFile.open(SHADER_PATH + "2.2-sphere.vs");
    fragmentShaderFile.open(SHADER_PATH + "2.2-sphere.fs");

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

    // render loop
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

        glUseProgram(shaderProgram);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 13);

        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 13);

        glBindVertexArray(VAOs[2]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 26);

        glBindVertexArray(VAOs[3]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 26);

        glBindVertexArray(VAOs[4]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 26);

        glBindVertexArray(VAOs[5]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 26);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate glfw
    glfwTerminate();
    return 0;
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