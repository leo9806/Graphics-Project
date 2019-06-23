#pragma once
#include "Common.h"

class CCatmullRom;
class CCamera;

class ShipPosition
{
public:
	ShipPosition();
	~ShipPosition();
	void Initialise();
	void Update(float a_delta, string a_view);
	void ChangeView(string a_view);
	glm::vec3 GetPosition();
	glm::mat4 GetOrientation();
	glm::vec2 GetDistance();

private:
	
	void UpdatePosition();
	void PlayerMovement();
	void ShipAdvance(float a_direction);
	void ShipSteer(float a_direction);
	CCatmullRom *m_catmull;
	CCamera *m_camera;
	glm::vec3 m_currentPosition;
	glm::vec3 m_nextPosition;
	float m_delta;
	glm::vec3 m_t;
	glm::vec3 m_n;
	glm::vec3 m_b;
	glm::vec2 m_currentDistance;
	glm::mat4 m_orientation;
};