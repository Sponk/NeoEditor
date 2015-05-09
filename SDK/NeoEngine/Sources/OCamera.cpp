//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#include "../Includes/NeoEngine.h"

using namespace Neo;

// constructor
OCamera::OCamera(void):
Object3d(),
m_ortho(false),
m_fog(false),
m_fov(60),
m_fogDistance(0),
m_clippingNear(1),
m_clippingFar(1000),
m_clearColor(0.2f, 0.3f, 0.4f),
m_fogColor(1.0f, 1.0f, 1.0f),
m_sceneLayer(0),
m_renderColorTexture(NULL),
m_renderDepthTexture(NULL)
{
	m_currentViewport[0] = 0;
	m_currentViewport[1] = 0;
	m_currentViewport[2] = 0;
	m_currentViewport[3] = 0;
}

// destructor
OCamera::~OCamera(void)
{
	Object3d::clearObject3d();
}

// copy constructor
OCamera::OCamera(const OCamera & camera):
Object3d(camera),
m_ortho(camera.m_ortho),
m_fog(camera.m_fog),
m_fov(camera.m_fov),
m_fogDistance(camera.m_fogDistance),
m_clippingNear(camera.m_clippingNear),
m_clippingFar(camera.m_clippingFar),
m_clearColor(camera.m_clearColor),
m_fogColor(camera.m_fogColor),
m_sceneLayer(camera.m_sceneLayer),
m_renderColorTexture(NULL),
m_renderDepthTexture(NULL)
{}

Vector3 OCamera::getProjectedPoint(const Vector3 & point) const
{
	Vector4 v = m_currentViewMatrix * Vector4(point);
	v = m_currentProjMatrix * v;
	v.x = v.x / v.w;
	v.y = v.y / v.w;
	v.z = v.z / v.w;

	v.x = m_currentViewport[0] + (m_currentViewport[2] * ((v.x + 1) / 2.0f));
	v.y = m_currentViewport[1] + (m_currentViewport[3] * ((v.y + 1) / 2.0f));
	v.z = (v.z + 1) / 2.0f;

	return Vector3(v.x, v.y, v.z);
}

Vector3 OCamera::getUnProjectedPoint(const Vector3 & point) const
{
	Vector4 nPoint;

	nPoint.x = (2 * ((point.x - m_currentViewport[0]) / ((float)m_currentViewport[2]))) - 1;
	nPoint.y = (2 * ((point.y - m_currentViewport[1]) / ((float)m_currentViewport[3]))) - 1;
	nPoint.z = (2 * point.z) - 1;
	nPoint.w = 1;

	Matrix4x4 matrix = (m_currentProjMatrix * m_currentViewMatrix).getInverse();
	Vector4 v = matrix * nPoint;
	
	if(v.w == 0)
		return getTransformedPosition();
	
	float iw = 1.0f / v.w;
	return Vector3(v.x, v.y, v.z)*iw;
}

void OCamera::updateListener(void)
{
	SoundContext * soundContext = NeoEngine::getInstance()->getSoundContext();
	if(soundContext)
	{
		Vector3 position = getTransformedPosition();
		Vector3 direction = getRotatedVector(Vector3(0, 0, -1));
		Vector3 up = getRotatedVector(Vector3(0, 1, 0));
		soundContext->updateListenerPosition(position, direction, up);
	}
}

void OCamera::enable(void)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
	

	// get viewport
	render->getViewport(m_currentViewport);

	// projection mode
	render->setMatrixMode(MATRIX_PROJECTION);
	render->loadIdentity();

	float ratio = (m_currentViewport[2] / (float)m_currentViewport[3]);

	Vector3 scale = getTransformedScale();
	Vector3 iScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);

	Matrix4x4 iScaleMatrix;
	iScaleMatrix.setScale(iScale);

	Matrix4x4 inverseMatrix = ((*getMatrix()) * iScaleMatrix).getInverse();

	// perspective view
	if(! isOrtho())
	{
		// normal perspective projection
		render->setPerspectiveView(m_fov, ratio, m_clippingNear, m_clippingFar);

		// model view mode
		render->setMatrixMode(MATRIX_MODELVIEW);
		render->loadIdentity();

		render->multMatrix(&inverseMatrix);

		// get current matrices
		render->getModelViewMatrix(&m_currentViewMatrix);
		render->getProjectionMatrix(&m_currentProjMatrix);
		return;
	}

	// ortho view
	float height = m_fov * 0.5f;
	float width = height * ratio;

	render->setOrthoView(-width, width, -height, height, m_clippingNear, m_clippingFar);

	// model view mode
	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();
	
	render->multMatrix(&inverseMatrix);

	// get current matrices
	render->getModelViewMatrix(&m_currentViewMatrix);
	render->getProjectionMatrix(&m_currentProjMatrix);
}
