// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#pragma once

#include "ShinyOgre.h"

namespace ShinyOgre {


//-----------------------------------------------------------------------------

	class HistoryGraph {
	private:

		Ogre::ManualObject* mObject;
		/*
		Ogre::Overlay* mOverlay;
		Ogre::SceneNode* mNode;
		*/

		std::vector<Real> mPoints;

		float mScale;

		void initialize(void);
		void refresh(void);

	public:

		HistoryGraph(void);
		~HistoryGraph() { destroy(); }

		void resize(uint count);
		void reset(void); 

		void addPoint(Ogre::Real value);
		void setScale(Ogre::Real scale) { mScale = scale * 2; }

		void create(const String& material = "BaseWhiteNoLighting");
		void destroy(void);

		void show(void);
		void hide(void);
	};

} // namespace
