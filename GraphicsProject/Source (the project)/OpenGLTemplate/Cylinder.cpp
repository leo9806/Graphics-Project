#include "Cylinder.h"

CCylinder::CCylinder()
{
}

CCylinder::~CCylinder()
{}

void CCylinder::Create()
{
	m_texture.Load("resources\\textures\\cylinderTexture.jpg");
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
	vector<glm::vec2> texCoord;
	
	for (int i = 0; i <= 360; i++)
	{
		const float tc = (i / (float)(2 * M_PI));
		texCoord.push_back(glm::vec2(tc, 0.0f));
		circle_pts.push_back(glm::vec3(cos(M_PI / 180 * i), -1, sin(M_PI / 180 * i)));
		texCoord.push_back(glm::vec2(tc, 1.0f));
		circle_pts.push_back(glm::vec3(cos(M_PI / 180 * i), +1, sin(M_PI / 180 * i)));
	}

	for (int i = 0; i < circle_pts.size(); ++i)
	{
		
		m_vbo.AddData(&circle_pts[i], sizeof(glm::vec3));
		m_vbo.AddData(&texCoord[i], sizeof(glm::vec2));
		glm::vec3 normalTmp = glm::normalize(circle_pts[i] - glm::vec3(0, circle_pts[i].y, 0));
		m_vbo.AddData(&normalTmp, sizeof(glm::vec3));
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

void CCylinder::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, circle_pts.size());
}

void CCylinder::Release()
{
	//m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}