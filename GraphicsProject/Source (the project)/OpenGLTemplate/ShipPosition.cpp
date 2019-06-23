#include "ShipPosition.h"
#include "CatmullRom.h"
#include "Camera.h"

ShipPosition::ShipPosition()
{
	m_currentDistance = glm::vec2(0, 0);
	m_currentPosition = glm::vec3(0, 0, 0);
	m_orientation = glm::mat4(0);

	m_catmull = NULL;
	m_camera = NULL;
}

ShipPosition::~ShipPosition() 
{
	delete m_catmull;
	delete m_camera;
}

void ShipPosition::Initialise()
{
	m_catmull = new CCatmullRom;
	m_camera = new CCamera;
}

void ShipPosition::Update(float a_delta, string a_view) 
{
	m_delta = a_delta;
	PlayerMovement();
	UpdatePosition();
}

void ShipPosition::PlayerMovement()
{
	if (GetKeyState('W') & 0x80)
	{
		ShipAdvance(1.0f*m_delta);
	}
	if (GetKeyState('A') & 0x80)
	{
		ShipSteer(-1.0f*m_delta);
	}
	if ((GetKeyState('D') & 0x80))
	{
		ShipSteer(1.0f*m_delta);
	}
}

void ShipPosition::UpdatePosition() 
{
	m_catmull->Sample((m_currentDistance.y + 1.0f), m_nextPosition);
	m_catmull->Sample(m_currentDistance.y, m_currentPosition);
	
	m_t = glm::normalize(m_nextPosition - m_currentPosition);
	m_n = glm::normalize(glm::cross(m_t, glm::vec3(0, 1, 0)));
	m_b = glm::normalize(glm::cross(m_n, m_t));

	m_orientation = glm::mat4(glm::mat3(m_t, m_b, m_n));

	m_currentPosition += m_n * m_currentDistance.x;
}

void ShipPosition::ShipAdvance(float a_direction)
{
	float l_speed = a_direction * 0.004;

	m_currentDistance.y += m_delta * l_speed;
}

void ShipPosition::ShipSteer(float a_direction)
{
	float l_speed = a_direction * 0.05;

	m_currentDistance.x += m_delta * l_speed;
}

void ShipPosition::ChangeView(string a_view)
{
	if (a_view == "top")
	{
		//top view
		m_camera->Set(m_currentPosition + (m_b *500.0f), m_nextPosition, m_b);
	}
	if (a_view == "third")
	{
		//third person view
		m_camera->Set(m_currentPosition + (-m_t*40.0f) + (m_b*20.0f), m_currentPosition, m_b);
	}
	if (a_view == "side")
	{
		//side view
		m_camera->Set(m_currentPosition + (m_n*200.0f) + (m_b*50.0f), m_currentPosition, m_b);
	}
	if (a_view == "first")
	{
		//first person view
		m_camera->Set(m_currentPosition + (m_b*4.0f) + (m_t*-2.0f), m_currentPosition + (m_t*150.0f), m_b);
	}
	if (a_view == "free")
	{
		m_camera->Update(m_delta);
	}
}

glm::vec3 ShipPosition::GetPosition()
{
	return m_currentPosition;
}

glm::mat4 ShipPosition::GetOrientation()
{
	return m_orientation;
}

glm::vec2 ShipPosition::GetDistance()
{
	return m_currentDistance;
}