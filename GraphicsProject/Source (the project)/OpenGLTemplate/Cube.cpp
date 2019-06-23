#include "Cube.h"
CCube::CCube()
{}
CCube::~CCube()
{
	Release();
}
void CCube::Create(string filename)
{
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();
	// Write the code to add interleaved vertex attributes to the VBO
	m_vertices =
	{
		//front
		glm::vec3(-.5f,-.5f,.5f),
		glm::vec3(.5f,-.5f,.5f),
		glm::vec3(-.5f,.5f,.5f),
		glm::vec3(.5f,.5f,.5f),
		
		//right
		glm::vec3(.5f, -.5f, .5f),
		glm::vec3(.5f, -.5f, -.5f),
		glm::vec3(.5f, .5f, .5f),
		glm::vec3(.5f, .5f, -.5f),
		
		//back
		glm::vec3(.5f, -.5f, -.5f),
		glm::vec3(-.5f, -.5f, -.5f),
		glm::vec3(.5f, .5f, -.5f),
		glm::vec3(-.5f, .5f, -.5f),

		//left
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f),

		//top
		glm::vec3(-0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),

		//bottom
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, 0.5f),
		glm::vec3(0.5f, -0.5f, 0.5f)
	};

	m_texCoords = 
	{
		glm::vec2(.0f, .0f),
		glm::vec2(.0f, 1.0f),
		glm::vec2(1.0f, .0f),
		glm::vec2(1.0f, 1.0f)
	};
	normals =
	{
		glm::vec3(1.0f, .0f, .0f),
		glm::vec3(.0f, .0f, 1.0f),
		glm::vec3(-1.0f, .0f, .0f),
		glm::vec3(.0f, .0f, -1.0f),
		glm::vec3(.0f, 1.0f, .0f),
		glm::vec3(.0f, -1.0f, .0f)
	};
	// Upload data to GPU
	int count = 0;
	int normalCount = 0;
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		m_vbo.AddData(&m_vertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_texCoords[count], sizeof(glm::vec2));
		count++;
		m_vbo.AddData(&normals[normalCount], sizeof(glm::vec3));

		if (count == 4) 
		{
			count = 0;
			normalCount += 1;
		}
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}
void CCube::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
}
void CCube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}