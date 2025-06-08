/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License...
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "gearwheel.h"
#include "granny_clock.h"  // Dodane

float speed = 0;
GLuint texClockHouse;
GLuint texClockHand;
GLuint texClockFace;
GLuint texPendulum;

// Kamera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 250.0f, lastY = 250.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Obsługa myszki
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.4f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

// Obsługa ruchu WASD
void processMovement(GLFWwindow* window) {
    float cameraSpeed = 5.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// Obsługa błędów
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

// Obsługa klawiatury
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed = -PI;
        if (key == GLFW_KEY_RIGHT) speed = PI;
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    }
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) speed = 0;
    }
}

GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);
    if (error) {
        std::cerr << "Texture loading error: " << lodepng_error_text(error) << std::endl;
        return 0;
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

// Inicjalizacja
void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    texClockHouse = readTexture("clock_house.png");
    texClockHand = readTexture("clock_hand.png");
    texClockFace = readTexture("clock_face.png");
    texPendulum = readTexture("pendulum.png");
}

// Zwolnienie zasobów
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    glDeleteTextures(1, &texClockHouse);
    
}

// Rysowanie zębatki
void drawGear(glm::mat4 Mt, glm::mat4 P, glm::mat4 V) {
    spLambert->use();
    glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

    Mt = glm::scale(Mt, glm::vec3(0.0075f, 0.0075f, 0.0075f));
    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mt));
    glUniform4f(spLambert->u("color"), 0.5f, 0.5f, 0.5f, 1);
    Models::gearwheel.drawSolid();
}

// Rysowanie zegara
void drawClock(float angle, glm::mat4 P, glm::mat4 V) {
    // Załóżmy, że wskazówka minutowa przesuwa się o 1 obrót co 10 obrotów zębatki:
    float gearToMinuteHandRatio = 1.0f / 10.0f;
    float bigHandAngle = angle * gearToMinuteHandRatio;

    // A wskazówka godzinowa jeszcze wolniej
    float gearToHourHandRatio = 1.0f / 600.0f;
    float smallHandAngle = angle * gearToHourHandRatio;

    glm::mat4 base = glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
// HOUSE
    glm::mat4 Mk1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    Mk1 = glm::rotate(Mk1, PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Use textured shader
    spTextured->use();
    glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(Mk1));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texClockHouse);
    glUniform1i(spTextured->u("tex0"), 0);

    glUniform1i(spTextured->u("textureIndex"), 0);
    Models::grannyClock.drawSolid();

// FACE
    glm::mat4 Mk2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    Mk2 = glm::rotate(Mk2, PI/2, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(Mk2));

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, texClockFace);
    glUniform1i(spTextured->u("tex"), 0);

    Models::grannyClockFace.drawSolid();

    // WSKAZÓWKI – obroty wokół osi Z względem środka tarczy (na bazie macierzy bazowej)

   // Duża wskazówka (minutowa) – pełny obrót co 60s
    glm::mat4 Mk3 = glm::mat4(1.0f);
    Mk3 = glm::translate(Mk3, glm::vec3(0.0f, 3.1f, 0.4f));  // Ustalenie punktu zaczepienia (przod/tyl,gora/dol,prawo/lewo)
    Mk3 = glm::rotate(Mk3, -bigHandAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    Mk3 = glm::rotate(Mk3, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));  // obrót w stronę widoku
    glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(Mk3));

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, texClockHand);
    glUniform1i(spTextured->u("tex"), 0);

    Models::grannyClockHand.drawSolid();

    // Mała wskazówka (godzinowa) – pełny obrót co 3600s
    glm::mat4 Mk4 = glm::mat4(1.0f);
    Mk4 = glm::translate(Mk4, glm::vec3(0.0f, 3.1f, 0.45f));
    Mk4 = glm::rotate(Mk4, -smallHandAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    Mk4 = glm::rotate(Mk4, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
    Mk4 = glm::scale(Mk4, glm::vec3(0.7f));

    glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(Mk4));

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, texClockHand);
    glUniform1i(spTextured->u("tex"), 0);

    Models::grannyClockHand.drawSolid();

    // WAHADŁO – bujanie LEWO↔PRAWO sinusoidą co sekundę
    float pendulumAngle = sin(glfwGetTime() * PI) * glm::radians(20.0f); // 20° w lewo/prawo
    glm::mat4 pendulum = base;
    pendulum = glm::translate(pendulum, glm::vec3(-0.1f, 0.1f, 0.0f));
    pendulum = glm::rotate(pendulum, pendulumAngle, glm::vec3(1.0f, 0.0f, 0.0f));  // obrót wokół osi X
    glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(pendulum));

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, texPendulum);
    glUniform1i(spTextured->u("tex"), 0);

    Models::pendulum.drawSolid();
}


// Zębatki 2
void gears2(float angle, glm::mat4 P, glm::mat4 V) {
    glm::mat4 I = glm::mat4(1.0f);


    glm::mat4 Mt1 = glm::translate(I, glm::vec3(-0.1575f, -0.1f, -0.05f));
    Mt1 = glm::rotate(Mt1, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    drawGear(Mt1,P,V);

    glm::mat4 Mt2 = glm::translate(I, glm::vec3(0.1575f, -0.1f, -0.05f));
    Mt2 = glm::rotate(Mt2, -angle + glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    drawGear(Mt2,P,V);
}

// Rysowanie sceny
void drawScene(GLFWwindow* window, float angle) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
    glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


    drawClock(glfwGetTime(),P,V);
    gears2(glfwGetTime() * PI * 0.2f,P,V);

    glfwSwapBuffers(window);
}

// Główna funkcja
int main(void) {
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Can't initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLenum err;
    if ((err = glewInit()) != GLEW_OK) {
        fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window);

    float angle = 0;
    glfwSetTime(0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processMovement(window);
        angle += speed * deltaTime;
        drawScene(window, angle);
        glfwPollEvents();
    }

    freeOpenGLProgram(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
