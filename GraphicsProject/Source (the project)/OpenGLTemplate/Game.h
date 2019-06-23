#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CCube;
class CCylinder;
class ShipPosition;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pCorvette;
	COpenAssetImportMesh *m_pResonance;
	COpenAssetImportMesh *m_pRoadLight;
	COpenAssetImportMesh *m_pRoadBorders;
	COpenAssetImportMesh *m_pAsteroid;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CHighResolutionTimer *m_pTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pCatmull;
	CCube *m_pCube;
	CCylinder *m_pCylinder;
	ShipPosition *m_pShipPosition;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	int m_timePassed;	// to save the total time elapsed
	bool m_appActive;
	glm::vec2 m_currentDistance;
	float m_cameraSpeed;
	string view;

	// Path members
	glm::vec3 p, pNext, up;
	// Fernet frame
	glm::vec3 T;
	glm::vec3 B;
	glm::vec3 N;

	float m_cameraRotation;

	// Player ship members
	glm::vec3 m_spaceShipPosition;
	glm::vec3 m_nextSpaceShipPosition;
	glm::mat4 m_spaceShipOrientation;
	float m_speed;

	// Observer ship members
	float m_updateTime;
	glm::vec3 m_observerPosition;
	glm::mat4 m_observerOrientation;

	// Spotlight member
	bool m_lightsOff;

	//asteroid members
	float m_scaleUpdate;	//rotation for the cube/cylinder
	float m_sinYRotation;	//rotation on y axis for cylinder/cube

	// Count of laps
	int m_lapCount;


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void DisplayTime();
	void DisplayLapsElapsed();
	void HitTheLights(CShaderProgram *p, glm::mat4 &vMatrix, glm::mat3 &vNomralMatrix); // Turns off the directional light and enables the spotlights accross the map
	void EnableSpotlights(CShaderProgram *pMainProgram, glm::mat4 &vMatrix, glm::mat3 &vNomralMatrix);
	void ChangeCameraView(string view);
	void PlayerMovement();
	void ShipAdvance(double direction);
	void ShipStrafe(double direction);
	void SetShipPosition();
	void SetObserverPosition();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
};
