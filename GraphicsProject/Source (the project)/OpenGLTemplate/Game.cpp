/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "Cube.h"
#include "Cylinder.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pFtFont = NULL;
	m_pCorvette = NULL;
	m_pResonance = NULL;
	m_pRoadLight = NULL;
	m_pRoadBorders = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;

	m_pAudio = NULL;
	m_pCatmull = NULL;
	m_pCube = NULL;
	m_pCylinder = NULL;
	m_pTimer = NULL;

	m_framesPerSecond = 0;
	m_timePassed = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = glm::vec2(0.0f);
	m_cameraSpeed = 0.1f;
	m_cameraRotation = 0.0f;
	m_sinYRotation = 0.0f;
	m_lapCount = 0;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pFtFont;
	delete m_pCorvette;
	delete m_pResonance;
	delete m_pRoadLight;
	delete m_pRoadBorders;
	delete m_pAsteroid;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmull;
	delete m_pCube;
	delete m_pCylinder;
	delete m_pTimer;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pFtFont = new CFreeTypeFont;
	m_pCorvette = new COpenAssetImportMesh;
	m_pResonance = new COpenAssetImportMesh;
	m_pRoadLight = new COpenAssetImportMesh;
	m_pRoadBorders = new COpenAssetImportMesh;
	m_pAsteroid = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmull = new CCatmullRom;
	m_pCube = new CCube;
	m_pCylinder = new CCylinder();
	m_pTimer = new CHighResolutionTimer();

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	m_spaceShipPosition = glm::vec3(0, 0, 0);
	m_spaceShipOrientation = glm::mat4(0);

	//If lights are on or off
	m_lightsOff = false;

	// View of camera
	view = "first";

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("toonShader.vert");
	sShaderFileNames.push_back("toonShader.frag");
	sShaderFileNames.push_back("discardShader.vert");
	sShaderFileNames.push_back("discardShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create a shader program for fonts
	CShaderProgram *pToonShader = new CShaderProgram;
	pToonShader->CreateProgram();
	pToonShader->AddShaderToProgram(&shShaders[4]);
	pToonShader->AddShaderToProgram(&shShaders[5]);
	pToonShader->LinkProgram();
	m_pShaderPrograms->push_back(pToonShader);

	CShaderProgram *pDiscardShader = new CShaderProgram;
	pDiscardShader->CreateProgram();
	pDiscardShader->AddShaderToProgram(&shShaders[6]);
	pDiscardShader->AddShaderToProgram(&shShaders[7]);
	pDiscardShader->LinkProgram();
	m_pShaderPrograms->push_back(pDiscardShader);
	// You can follow this pattern to load additional shaders

	// Create the skybox
	m_pSkybox->Create(2500.0f);
	
	m_pFtFont->LoadFont("resources\\fonts\\Berkel.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pCorvette->Load("resources\\models\\playerShip\\playerShipCustom.obj");
	m_pResonance->Load("resources\\models\\Resonance\\Resonance.obj");
	m_pRoadLight->Load("resources\\models\\RoadLights1\\light.obj");
	m_pRoadBorders->Load("resources\\models\\BorderSphere\\borderSphere.obj");
	m_pAsteroid->Load("resources\\models\\Asteroid\\asteroidobj.obj");

	//glEnable(GL_CULL_FACE);

	//create centreline
	m_pCatmull->CreateCentreline();
	m_pCatmull->CreateOffsetCurves();
	m_pCatmull->CreateTrack();

	//Create a cube
	m_pCube->Create("resources\\textures\\pickup.jpg");

	//Create a cylinder
	m_pCylinder->Create();

	m_pTimer->Start();

	// Initialise audio and play background music
	/*m_pAudio->Initialise();
	m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	m_pAudio->PlayMusicStream();
	*/
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);
	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light[0].position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light[0].La", glm::vec3(.80f));		// Ambient colour of light
	pMainProgram->SetUniform("light[0].Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light[0].Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	// Checks and turns the lights on or off 
	HitTheLights(pMainProgram, viewMatrix, viewNormalMatrix);

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance
	
	//render the player ship
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_spaceShipPosition);
		modelViewMatrixStack *= m_spaceShipOrientation;
		//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(1.0f);
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCorvette->Render();
	modelViewMatrixStack.Pop(); 
	
	//render the observing ship
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_observerPosition);
		modelViewMatrixStack.Scale(20.0f);
		modelViewMatrixStack *= m_observerOrientation;
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pResonance->Render();
	modelViewMatrixStack.Pop();

	for (int i = 0; i < m_pCatmull->GetCentreline().size(); i+=50)
	{
		//render the RoadLight
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pCatmull->GetCentreline()[i] + (glm::vec3(0.f, 1.0f, .0f))*20.0f);
		modelViewMatrixStack.Scale(0.02f);
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRoadLight->Render();
		modelViewMatrixStack.Pop();
	}

	for (int i = 0; i < m_pCatmull->GetCentreline().size(); i+=30)
	{
		//render the Cube
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pCatmull->GetCentreline()[i] + glm::vec3(0,2,0));
		modelViewMatrixStack.Scale(3.0f);
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCube->Render();
		modelViewMatrixStack.Pop();
	}
	
	//Renders the track
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix",
			m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCatmull->RenderTrack();
	modelViewMatrixStack.Pop();

	for (int i = 10; i < m_pCatmull->GetCentreline().size(); i+=10)
	{
		// Render the road borders
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(m_pCatmull->GetCentreline()[i] + ((glm::vec3(.0f, .0f, .0f))));
			modelViewMatrixStack.Scale(.02f);
			pMainProgram->SetUniform("bUseTexture", true); // turn on texturing
			pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pRoadBorders->Render();
		modelViewMatrixStack.Pop();
	}

	//render the observing ship
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pCatmull->GetCentreline()[14]);
		modelViewMatrixStack.Scale(35.0f);
		// Set the cylinder to look at the next point
		glm::mat4 l_rot = glm::lookAt(glm::vec3(0), m_pCatmull->GetCentreline()[15], glm::vec3(0.f, 1.f, 0.f));
		modelViewMatrixStack.ApplyMatrix(l_rot);
		modelViewMatrixStack.Rotate(glm::vec3(0.f, 0.f, 1.f), 90.0f);
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCylinder->Render();
	modelViewMatrixStack.Pop();

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CShaderProgram *pToonShader = (*m_pShaderPrograms)[2];
	pToonShader->UseProgram();
	pToonShader->SetUniform("bUseTexture", false);
	pToonShader->SetUniform("levels", 6);

	// Set light and materials in toon programme
	pToonShader->SetUniform("material1.Ma", glm::vec3(1.0f, 1.0f, 0.0f));
	pToonShader->SetUniform("material1.Md", glm::vec3(1.0f, 1.0f, 0.0f));
	pToonShader->SetUniform("material1.Ms", glm::vec3(1.0f, 1.0f, 1.0f));
	pToonShader->SetUniform("material1.shininess", 50.0f);
	pToonShader->SetUniform("light1.La", glm::vec3(0.15f, 0.15f, 0.15f));
	pToonShader->SetUniform("light1.Ld", glm::vec3(1.0f, 0.0f, 0.0f));
	pToonShader->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pToonShader->SetUniform("light1.position", viewMatrix);

	for (int i = 0; i < m_pCatmull->GetRightOffsets().size(); i += 20)
	{
		// Render the cube
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pCatmull->GetRightOffsets()[i] + ((glm::vec3(.0f, 5.0f, .0f))));
		
		glm::mat4 l_tmp = glm::inverse(glm::lookAt(glm::vec3(0), glm::normalize(m_pCatmull->GetRightOffsets()[i + 1] - m_pCatmull->GetRightOffsets()[i]), glm::vec3(0.f, 1.f, 0.f)));

		modelViewMatrixStack.ApplyMatrix(l_tmp);
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), m_sinYRotation);
		modelViewMatrixStack.Scale(5.0f, 5.0f, 5.0f);
		pToonShader->SetUniform("bUseTexture", true); // turn on texturing
		pToonShader->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pToonShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pToonShader->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCube->Render();
		modelViewMatrixStack.Pop();
	}

	for (int i = 0; i < m_pCatmull->GetLeftOffsets().size(); i += 20)
	{
		// Render the cube
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pCatmull->GetLeftOffsets()[i] + ((glm::vec3(.0f, 5.0f, .0f))));
		glm::mat4 l_tmp = glm::inverse(glm::lookAt(glm::vec3(0), glm::normalize(m_pCatmull->GetRightOffsets()[i + 1] - m_pCatmull->GetRightOffsets()[i]), glm::vec3(0.f, 1.f, 0.f)));
		modelViewMatrixStack.ApplyMatrix(l_tmp);
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), m_sinYRotation);
		pToonShader->SetUniform("bUseTexture", true); // turn on texturing
		modelViewMatrixStack.Scale(5.f, 5.0f, 5.f);
		pToonShader->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pToonShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pToonShader->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCube->Render();
		modelViewMatrixStack.Pop();
	}

	CShaderProgram *pDiscardShader = (*m_pShaderPrograms)[3];
	pDiscardShader->UseProgram();
	pDiscardShader->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	pDiscardShader->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pDiscardShader->SetUniform("vlightDirection", glm::normalize(glm::vec3(1.0f)));
	pDiscardShader->SetUniform("sampler0", 0);
	pDiscardShader->SetUniform("t", (float)m_pTimer->Elapsed());
	
	// Render the asteroid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(200.f, 20.0f, .0f));
	modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.0f, 0.f), m_sinYRotation);
	modelViewMatrixStack.Scale(m_scaleUpdate);
	pDiscardShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	m_pAsteroid->Render();
	modelViewMatrixStack.Pop();

	// Render the asteroid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(790.f, 20.0f, 395.0f));
	modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.0f, 0.f), m_sinYRotation);
	modelViewMatrixStack.Scale(m_scaleUpdate);
	pDiscardShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	m_pAsteroid->Render();
	modelViewMatrixStack.Pop();
	
	// Render the asteroid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(205.0f, 20.0f, -500.0f));
	modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.0f, 0.f), m_sinYRotation);
	modelViewMatrixStack.Scale(m_scaleUpdate);
	pDiscardShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	m_pAsteroid->Render();
	modelViewMatrixStack.Pop();
	
	// Render the asteroid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(635.f, 20.0f, 60.0f));
	modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.0f, 0.f), m_sinYRotation);
	modelViewMatrixStack.Scale(m_scaleUpdate);
	pDiscardShader->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	m_pAsteroid->Render();
	modelViewMatrixStack.Pop();
	
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();
	DisplayTime();
	DisplayLapsElapsed();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	// Updating rotation on y axis
	m_sinYRotation += .10f * m_dt;
	m_scaleUpdate = sin(m_sinYRotation / 200.0f)*30;	
	
	SetObserverPosition();

	PlayerMovement();
	SetShipPosition();

	// Calls the function that sets the camera's position and view
	// It accepts a string argument which specifies which view mode to set
	// The string is updated by pressing the specified keys on the keyboard
	ChangeCameraView(view);

	//m_pAudio->Update();
}

//checks if the lights are on or off
// if the lightsOff bool is true it truns of all the lights
// else it leaves them on by setting lightsOff to false
void Game::HitTheLights(CShaderProgram *pMainProgram, glm::mat4 &vMatrix, glm::mat3 &vNormalMatrix)
{
	if (!m_lightsOff)
	{
		pMainProgram->SetUniform("lightsOff", false);
	}
	else
	{
		// Turn off the lights 
		pMainProgram->SetUniform("light[0].La", glm::vec3(.0f));		// Ambient colour of light
		pMainProgram->SetUniform("light[0].Ld", glm::vec3(.0f));		// Diffuse colour of light
		pMainProgram->SetUniform("light[0].Ls", glm::vec3(.0f));		// Specular colour of light
		pMainProgram->SetUniform("material1.Ma", glm::vec3(.0f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
		pMainProgram->SetUniform("material1.shininess", .0f);		// Shininess material property

		EnableSpotlights(pMainProgram, vMatrix, vNormalMatrix);
	}
}

// when the lights are off it enables the spotlights by making the required changes to the main vertex
void Game::EnableSpotlights(CShaderProgram *pMainProgram, glm::mat4 &vMatrix, glm::mat3 &vNomralMatrix)
{
	// Set light and materials in sphere programme
	pMainProgram->SetUniform("lightsOff", true);
	for(int i = 0; i < m_pCatmull->GetCentreline().size() /100; i++)
	{
		string lightVar = "light[" + std::to_string(i) + "]";
		pMainProgram->SetUniform(lightVar + ".position", vMatrix*glm::vec4(m_pCatmull->GetCentreline()[i*100] + (glm::vec3(.0f, 22.0f, .0f)), 1)); // Light position in eye coordinates
		pMainProgram->SetUniform(lightVar + ".La", glm::vec3(.005f));
		pMainProgram->SetUniform(lightVar + ".Ld", glm::vec3(1.0f));
		pMainProgram->SetUniform(lightVar + ".Ls", glm::vec3(1.0f, 1.0f, 1.0f));
		pMainProgram->SetUniform(lightVar + ".direction",
			glm::normalize(vNomralMatrix*glm::vec3(0, -1, 0)));
		pMainProgram->SetUniform(lightVar + ".exponent", 5.0f);
		pMainProgram->SetUniform(lightVar + ".cutoff", 30.0f);
		pMainProgram->SetUniform("material1.shininess", 15.0f);
		pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));
	}
		//pMainProgram->SetUniform("colour", l_spotlightColours[1]);
		string lightVar = "light[11]";
		pMainProgram->SetUniform(lightVar + ".position", vMatrix*glm::vec4(m_spaceShipPosition + (glm::vec3(.0f, 5.0f, .0f)), 1)); // Light position in eye coordinates
		pMainProgram->SetUniform(lightVar + ".La", glm::vec3(.005f));
		pMainProgram->SetUniform(lightVar + ".Ld", glm::vec3(1.0f, 0.f, 0.f));
		pMainProgram->SetUniform(lightVar + ".Ls", glm::vec3(1.0f, 1.0f, 1.0f));
		pMainProgram->SetUniform(lightVar + ".direction",
			glm::normalize(glm::vec3(0, 0, -1)));
		pMainProgram->SetUniform(lightVar + ".exponent", .50f);
		pMainProgram->SetUniform(lightVar + ".cutoff", 30.0f);
		pMainProgram->SetUniform("material1.shininess", 15.0f);
		pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));
		
}

// Changes the view of the camea based on the argument it is given
void Game::ChangeCameraView(string view)
{
	if (view == "top")
	{
		//top view
		m_pCamera->Set(m_spaceShipPosition + (B *500.0f), m_nextSpaceShipPosition, B);
	}
	if (view == "third")
	{
		//third person view
		m_pCamera->Set(m_spaceShipPosition + (-T*40.0f) + (B*20.0f), m_spaceShipPosition, B);
	}
	if (view == "side")
	{
		//side view
		m_pCamera->Set(m_spaceShipPosition + (N*200.0f) + (B*50.0f), m_spaceShipPosition, B);
	}
	if (view == "first")
	{
		//first person view
		m_pCamera->Set(m_spaceShipPosition + (B*4.0f) + (T*-2.0f), m_spaceShipPosition + (T*150.0f), B);
	}
	if (view == "free") 
	{
		m_pCamera->Update(m_dt);
	}
}

// Defines the position of the observer ship moving around the map
// also defines the orientation of the ship by using the fernet frame
void Game::SetObserverPosition()
{
	// Setting the position to a circle over the map
	m_updateTime += 0.0001f * (float)m_dt;
	float radius = 180.0f;
	m_observerPosition = radius * cos(m_updateTime) * glm::vec3(5, 0, 0) + 50.0f * glm::vec3(0, 1, 0) + radius * sin(m_updateTime) * glm::vec3(0,0,5);

	// Defining the fernet frame to calculate the ship orientation
	glm::vec3 T = glm::normalize(-radius * sin(m_updateTime) * glm::vec3(1, 0, 0) + radius * cos(m_updateTime) * glm::vec3(0, 0, 1));
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	m_observerOrientation = glm::mat4(glm::mat3(T, B, N));
}

// Defines the keyboard keys for the movement of the player
void Game::PlayerMovement()
{
	if (GetKeyState('W') & 0x80)
	{
		ShipAdvance(1.0f*m_dt);
	}
	if (GetKeyState('S') & 0x80)
	{
		ShipAdvance(-1.0f*m_dt);
	}
	if (GetKeyState('A') & 0x80)
	{
		ShipStrafe(-1.0f*m_dt);
	}
	if ((GetKeyState('D') & 0x80))
	{
		ShipStrafe(1.0f*m_dt);
	}
}

void Game::SetShipPosition()
{
	m_pCatmull->Sample((m_currentDistance.y + 1.0f), m_nextSpaceShipPosition);
	m_pCatmull->Sample(m_currentDistance.y, m_spaceShipPosition);

	T = glm::normalize(m_nextSpaceShipPosition - m_spaceShipPosition);
	N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	B = glm::normalize(glm::cross(N, T));

	m_spaceShipOrientation = glm::mat4(glm::mat3(T, B, N));

	m_spaceShipPosition += N * m_currentDistance.x;
}

// Allows the player ship to move forward
// It accepts one double argument which is the direction
void Game::ShipAdvance(double direction)
{
	// define the speed of the ship
	m_speed = (float)(0.004 * direction);
	m_currentDistance.y += m_dt * m_speed;
}

void Game::ShipStrafe(double direction)
{
	float temp = (float)(0.05 * direction);
	m_currentDistance.x += temp;
}

void Game::DisplayFrameRate()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;


	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}


// Displays the time in seconds
void Game::DisplayTime()
{

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	m_timePassed += m_dt;
	
	m_pFtFont->Render(20, height - 500, 20, "TIME: %d", int(m_timePassed*0.001f));

}

// Counts the laps based on the distance covered by the player ship
void Game::DisplayLapsElapsed()
{
	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	m_pFtFont->Render(20, height - 520, 20, "LAPS: %d", m_pCatmull->CurrentLap(m_currentDistance.y) );
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/

	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch (w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		case '2':
			if(m_lightsOff)
				m_lightsOff = false;
			else
				m_lightsOff = true;
			break;
		case '3':
			view = "top";
			break;
		case '4':
			view = "third";
			break;
		case '5':
			view = "side";
			break;
		case '6':
			view = "first";
			break;
		case '7':
			view = "free";
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
