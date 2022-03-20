#pragma once
#include "buffers.h"
#include "shader.h"

struct GLFWwindow;

class Renderer
{
public:
	struct Vertex {
		int16_t x;
		int16_t y;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	Renderer(GLFWwindow* window);

	void swap() const;
	void pushTriangle(const void* vertices, size_t size) const;
	void pushQuad(const void* vertices, size_t size) const;
private:
	GLFWwindow* m_window;
	VertexBuffer m_vertices;
	uint32_t m_VAO;
	Shader m_shader;
};
