#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Renderer::Renderer(GLFWwindow* window) :
    m_window(window),
    // TODO: do memory and sizes correctly
    m_vertices(6 * sizeof(Vertex)),
	m_shader("shader_vertex.glsl", "shader_fragment.glsl")
{
    m_shader.bind();
    m_vertices.bind();

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Set vertex layout for shader
	glEnableVertexAttribArray(0);
    glVertexAttribIPointer(
        0,
        2,
        GL_SHORT,
        sizeof(Vertex),
        0
    ); // position
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(
        1,
        2,
        GL_UNSIGNED_BYTE,
        sizeof(Vertex),
        (const void*)(sizeof(int16_t) * 2)
    ); // color

    glClearColor(1.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    swap();
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::swap() const
{
    glfwSwapBuffers(m_window);
}

void Renderer::pushTriangle(const void* vertices, size_t size) const
{
    m_vertices.setData(vertices, size);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::pushQuad(const void* vertices, size_t size) const
{
    m_vertices.setData(vertices, size);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
