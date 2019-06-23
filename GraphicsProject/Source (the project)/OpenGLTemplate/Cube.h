#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"
// Class for generating a unit cube
class CCube
{
public:
	CCube();
	~CCube();
	void Create(string filename);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	vector<glm::vec3> m_vertices;
	vector<glm::vec2> m_texCoords;
	vector<glm::vec3> normals;
};