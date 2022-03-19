#include "cpu.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main()
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed!\n";
        abort();
    }

    GLFWwindow* window = glfwCreateWindow(1024, 512, "PSX Emulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed!\n";
        abort();
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD loader failed!\n";
        abort();
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    CPU cpu;
    while (!glfwWindowShouldClose(window))
    {
        cpu.runNextInstruction();
    }
    
    glfwTerminate();
    return 0;
}
