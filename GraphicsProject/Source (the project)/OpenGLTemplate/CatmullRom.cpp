#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	m_offsetWidth = 60.0f;
	m_roadTexture.Load("resources\\textures\\roadTexture.png");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;
}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	m_controlPoints.push_back(glm::vec3(0, 0, 0));
	m_controlPoints.push_back(glm::vec3(70, 2, 70));
	m_controlPoints.push_back(glm::vec3(150, 6, 150));
	m_controlPoints.push_back(glm::vec3(230, 8, 210));
	m_controlPoints.push_back(glm::vec3(300, 10, 310));//4
	m_controlPoints.push_back(glm::vec3(390, -10, 290));
	m_controlPoints.push_back(glm::vec3(300, -40, -20));
	m_controlPoints.push_back(glm::vec3(210, -90, 60));
	m_controlPoints.push_back(glm::vec3(300, -40, 300));
	m_controlPoints.push_back(glm::vec3(390, -20, 380));
	m_controlPoints.push_back(glm::vec3(475, -10, 300));
	m_controlPoints.push_back(glm::vec3(555, -20, 380));
	m_controlPoints.push_back(glm::vec3(635, -10, 460));
	m_controlPoints.push_back(glm::vec3(715, 10, 380));
	m_controlPoints.push_back(glm::vec3(535, 10, 280));
	m_controlPoints.push_back(glm::vec3(665, 20, 120));
	m_controlPoints.push_back(glm::vec3(745, 30, 40));
	m_controlPoints.push_back(glm::vec3(665, 40, -40));
	m_controlPoints.push_back(glm::vec3(585, 50, -120));
	m_controlPoints.push_back(glm::vec3(505, 60, -200));
	m_controlPoints.push_back(glm::vec3(585, 50, -360));
	m_controlPoints.push_back(glm::vec3(505, 40, -440));
	m_controlPoints.push_back(glm::vec3(425, 30, -520));
	m_controlPoints.push_back(glm::vec3(345, 20, -600));
	m_controlPoints.push_back(glm::vec3(185, 10, -680));
	m_controlPoints.push_back(glm::vec3(25, 5, -520));
	m_controlPoints.push_back(glm::vec3(345, -5, -360));
	m_controlPoints.push_back(glm::vec3(285, -5, -240));
	m_controlPoints.push_back(glm::vec3(105, -15, -200));
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}

	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}
}


void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();

	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(500);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(0, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	//create a vbo
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) 
	{
		vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
	}
	
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

}


void CCatmullRom::CreateOffsetCurves()
{
	// special cases - first position
	{
		int i = 0;
		glm::vec3 pPrev = m_centrelinePoints[m_centrelinePoints.size() - 1/*i - 1*/];
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[i + 1];
		//computing the TNB frame
		glm::vec3 t_vector = glm::normalize(glm::normalize(pNext - p) + glm::normalize(p - pPrev));
		glm::vec3 n_vector = glm::normalize(glm::cross(t_vector, glm::vec3(0.0f, 1.0f, 0.0f)));

		glm::vec3 l = p - (m_offsetWidth / 2) * n_vector; //left offset points
		glm::vec3 r = p + (m_offsetWidth / 2) * n_vector; //right offset points

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	for (int i = 1; i+1 < m_centrelinePoints.size(); ++i) 
	{
		glm::vec3 pPrev = m_centrelinePoints[i - 1];
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[i + 1];
		//computing the TNB frame
		glm::vec3 t_vector = glm::normalize(glm::normalize(pNext - p) + glm::normalize(p - pPrev));
		glm::vec3 n_vector = glm::normalize(glm::cross(t_vector, glm::vec3(0.0f, 1.0f, 0.0f)));

		glm::vec3 l = p - (m_offsetWidth / 2) * n_vector; //left offset points
		glm::vec3 r = p + (m_offsetWidth / 2) * n_vector; //right offset points

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}
	// special case - last position
	{
		int i = m_centrelinePoints.size() - 1;
		glm::vec3 pPrev = m_centrelinePoints[i - 1];
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[0/*i + 1*/];
		//computing the TNB frame
		glm::vec3 t_vector = glm::normalize(glm::normalize(pNext - p) + glm::normalize(p - pPrev));
		glm::vec3 n_vector = glm::normalize(glm::cross(t_vector, glm::vec3(0.0f, 1.0f, 0.0f)));

		glm::vec3 l = p - (m_offsetWidth / 2) * n_vector; //left offset points
		glm::vec3 r = p + (m_offsetWidth / 2) * n_vector; //right offset points

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}
	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	CVertexBufferObject leftVBO;
	leftVBO.Create();
	leftVBO.Bind();
	for (int i = 0; i < m_leftOffsetPoints.size(); ++i) 
	{
		leftVBO.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
	}
	leftVBO.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei stride =  sizeof(glm::vec3);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	CVertexBufferObject rightVBO;
	rightVBO.Create();
	rightVBO.Bind();
	for (int i = 0; i < m_rightOffsetPoints.size(); ++i)
	{
		rightVBO.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
	}
	// Note it is possible to only use one VAO / VBO with all the points instead.

	rightVBO.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
}


void CCatmullRom::CreateTrack()
{
	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);
	CVertexBufferObject triangleVBO;
	triangleVBO.Create();
	triangleVBO.Bind();

	for (int i = 0; i < m_leftOffsetPoints.size(); ++i)
	{
		tempOrder.push_back(m_leftOffsetPoints[i]);
		tempOrder.push_back(m_rightOffsetPoints[i]);
	}

	m_count = 0;
	vector<glm::vec2> texCoords =
	{
		glm::vec2(.0f, .0f),
		glm::vec2(.0f, 1.0f),
		glm::vec2(1.0f, .0f),
		glm::vec2(1.0f, 1.0f)
	};

	float textCoordY = 0;
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	
	//texCoords[3].y = textCoordY;
	textCoordY += 1.0f / 3.0f;
	texCoords[3].y = textCoordY;
	triangleVBO.AddData(&tempOrder[tempOrder.size() - 1], sizeof(glm::vec3));
	triangleVBO.AddData(&texCoords[3], sizeof(glm::vec2));
	triangleVBO.AddData(&normal, sizeof(glm::vec3));

	textCoordY += 1.0f / 3.0f;
	texCoords[2].y = textCoordY;
	triangleVBO.AddData(&tempOrder[tempOrder.size() - 2], sizeof(glm::vec3));
	triangleVBO.AddData(&texCoords[2], sizeof(glm::vec2));
	triangleVBO.AddData(&normal, sizeof(glm::vec3));
	
	
	for (int i = 0; i < tempOrder.size(); ++i)
	{
		textCoordY += 1.0f / 3.0f;
		texCoords[m_count].y = textCoordY;
		triangleVBO.AddData(&tempOrder[i], sizeof(glm::vec3));
		triangleVBO.AddData(&texCoords[m_count], sizeof(glm::vec2));
		m_count++;
		triangleVBO.AddData(&normal, sizeof(glm::vec3));
		m_count++;
		if (m_count == 4)
			m_count = 0;
	}
	
	textCoordY += 1.0f / 3.0f;
	texCoords[1].y = textCoordY;
	triangleVBO.AddData(&tempOrder[1], sizeof(glm::vec3));
	triangleVBO.AddData(&texCoords[1], sizeof(glm::vec2));
	triangleVBO.AddData(&normal, sizeof(glm::vec3));
		
	textCoordY += 1.0f / 3.0f;
	texCoords[0].y = textCoordY;
	triangleVBO.AddData(&tempOrder[0], sizeof(glm::vec3));
	triangleVBO.AddData(&texCoords[0], sizeof(glm::vec2));
	triangleVBO.AddData(&normal, sizeof(glm::vec3));
	
	triangleVBO.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}

vector<glm::vec3> CCatmullRom::GetLeftOffsets()
{
	return m_leftOffsetPoints;
}

vector<glm::vec3> CCatmullRom::GetRightOffsets()
{
	return m_rightOffsetPoints;
}

vector<glm::vec3> CCatmullRom::GetCentreline()
{
	return m_centrelinePoints;
}

void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
}


void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffsetPoints.size());
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	glBindVertexArray(m_vaoTrack);
	m_roadTexture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0 , tempOrder.size()+4); 
	
}

int CCatmullRom::CurrentLap(float d)
{
	return (int)(d / m_distances.back());
}