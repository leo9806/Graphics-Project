#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"
// Class for generating a unit cube
class CCylinder
{
public:
	CCylinder();
	~CCylinder();
	void Create();
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	vector<glm::vec3> circle_pts;
	vector<glm::vec3> topParts;
};