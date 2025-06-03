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
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "gearwheel.h"
#include "granny_clock.h"  // Dodane

float speed = 0;

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

// Inicjalizacja
void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// Zwolnienie zasobów
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
}

// Rysowanie zębatki
void drawGear(glm::mat4 Mt) {
    Mt = glm::scale(Mt, glm::vec3(0.0075f, 0.0075f, 0.0075f));
    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mt));
    glUniform4f(spLambert->u("color"), 1, 1, 0, 1);
    Models::gearwheel.drawSolid();
}

// Rysowanie zegara
void drawClock(float time) {
    glm::mat4 base = glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));

    // Korpus
    glm::mat4 Mk1 = glm::scale(base, glm::vec3(1.0f));
    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk1));
    glUniform4f(spLambert->u("color"), 1, 0, 1, 1);
    Models::grannyClock.drawSolid();

    // Tarcza
    glUniform4f(spLambert->u("color"), 1, 1, 1, 1);
    Models::grannyClockFace.drawSolid();

    // WSKAZÓWKI – obroty wokół osi Z względem środka tarczy (na bazie macierzy bazowej)

   // Duża wskazówka (minutowa) – pełny obrót co 60s
    float bigHandAngle = glm::radians((fmod(time, 60.0f) / 60.0f) * 360.0f);
    glm::mat4 Mk3 = glm::mat4(1.0f);
    Mk3 = glm::translate(Mk3, glm::vec3(0.0f, 3.1f, 0.4f));  // Ustalenie punktu zaczepienia
    Mk3 = glm::rotate(Mk3, -bigHandAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    Mk3 = glm::rotate(Mk3, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));  // obrót w stronę widoku
    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk3));
    glUniform4f(spLambert->u("color"), 1, 0.5, 0.5, 1);
    Models::grannyClockHand.drawSolid();

    // Mała wskazówka (godzinowa) – pełny obrót co 3600s
    float smallHandAngle = glm::radians((fmod(time, 3600.0f) / 3600.0f) * 360.0f);
    glm::mat4 Mk4 = glm::mat4(1.0f);
    Mk4 = glm::translate(Mk4, glm::vec3(0.0f, 3.1f, 0.45f));
    Mk4 = glm::rotate(Mk4, -smallHandAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    Mk4 = glm::rotate(Mk4, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
    Mk4 = glm::scale(Mk4, glm::vec3(0.7f));
    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk4));
    glUniform4f(spLambert->u("color"), 0.5, 0.5, 0.5, 1);
    Models::grannyClockHand.drawSolid();

    // WAHADŁO – bujanie LEWO↔PRAWO sinusoidą co sekundę
    float pendulumAngle = sin(time * PI) * glm::radians(20.0f); // 20° w lewo/prawo
    glm::mat4 pendulum = base;
    pendulum = glm::translate(pendulum, glm::vec3(-0.1f, 0.1f, 0.0f));
    pendulum = glm::rotate(pendulum, pendulumAngle, glm::vec3(1.0f, 0.0f, 0.0f));  // obrót wokół osi X
    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(pendulum));
    glUniform4f(spLambert->u("color"), 0.2, 0.7, 1, 1);
    Models::pendulum.drawSolid();
}






// Zębatki 2
void gears2(float angle) {
    glm::mat4 I = glm::mat4(1.0f);


    glm::mat4 Mt1 = glm::translate(I, glm::vec3(-0.1575f, -0.1f, -0.05f));
    Mt1 = glm::rotate(Mt1, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    drawGear(Mt1);

    glm::mat4 Mt2 = glm::translate(I, glm::vec3(0.1575f, -0.1f, -0.05f));
    Mt2 = glm::rotate(Mt2, -angle + glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    drawGear(Mt2);
}

// Rysowanie sceny
void drawScene(GLFWwindow* window, float angle) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
    glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    spLambert->use();
    glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

    drawClock(glfwGetTime());
    gears2(glfwGetTime() * PI * 0.2f);

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
