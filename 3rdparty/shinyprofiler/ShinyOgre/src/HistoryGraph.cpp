// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#include "R3/FirstInclude.h" // this header must be first
#include "HistoryGraph.h"
#include "DisplaySystem.h"

#include "Ogre/OgreManualObject.h"
#include "Ogre/OgreSceneManager.h"
/*
#include "Ogre/OgreOverlay.h"
#include "Ogre/OgreSceneNode.h"
#include "Ogre/OgreOverlayManager.h"
*/

namespace R3 {


//-----------------------------------------------------------------------------

	HistoryGraph::HistoryGraph(void) {
		mObject = NULL;
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::reset(void) {
		resize(mPoints.size());
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::resize(uint count) {
		mPoints.assign(count, -1);

		if (!mObject) return;

		mObject->estimateVertexCount(mPoints.size());
		mObject->beginUpdate(0);
		refresh();
		mObject->end();
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::create(const String& material) {
		String name = SceneSystem::getSingleton().getUniqueName();
		Ogre::SceneManager *mgr = R3::DisplaySystem::getSingleton().getManager();
		
		mObject = mgr->createManualObject(name);

		//*
		mObject->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
		mgr->getRootSceneNode()->attachObject(mObject);
		/*/
		mNode = mgr->createSceneNode();
		mNode->attachObject(mObject);
		mOverlay = Ogre::OverlayManager::getSingleton().create(name + "Overlay");
		mOverlay->add3D(mNode);
		mOverlay->show();
		//*/

		mObject->setUseIdentityProjection(true);
		mObject->setUseIdentityView(true);
		mObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		mObject->setDynamic(true);
		mObject->setCastShadows(false);
		mObject->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

		mObject->estimateVertexCount(mPoints.size());
		mObject->begin(material, Ogre::RenderOperation::OT_LINE_STRIP);
		refresh();
		mObject->end();
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::destroy(void) {
		Ogre::SceneManager *mgr = Ogre::Root::getSingleton()._getCurrentSceneManager();

		/*
		Ogre::OverlayManager::getSingleton().destroy(mOverlay);
		mgr->destroySceneNode(mNode);
		mOverlay = NULL;
		mNode = NULL;
		*/

		mgr->destroyManualObject(mObject);
		mObject = NULL;
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::show(void) {
		mObject->setVisible(true);
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::hide(void) {
		mObject->setVisible(false);
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::refresh(void) {
		Real scale = 2.0f / (Real) (mPoints.size() - 1);

		for (size_t i = 0; i < mPoints.size(); i++) {
			mObject->position(i * scale - 1, mPoints[i], -1);
		}
	}
	

//-----------------------------------------------------------------------------

	void HistoryGraph::addPoint(Ogre::Real value) {
		PROFILE_FUNC();

		int i;
		Real *p = &mPoints[0];

		if (!mObject) {
			i = mPoints.size();

			while (--i) {
				*p = *(p + 1);
				p++;
			}

			return;
		}

		int size = mPoints.size() - 1;
		Real scale = 2.0f / (Real) (size);

		mObject->estimateVertexCount(mPoints.size());
		mObject->beginUpdate(0);

		for (i = 0; i < size; i++) {
			Real y = *p = *(p + 1);
			p++;

			mObject->position(i * scale - 1, y, -1);
		}

		Real y = *p = value * mScale - 1;
		mObject->position(i * scale - 1, y, -1);
		mObject->end();
	}

} // namespace
