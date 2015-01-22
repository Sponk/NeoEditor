// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#include "R3/FirstInclude.h" // this header must be first
#include "ProfilerGraph.h"
#include "Profiler.h"
#include "Application.h"
#include "SceneSystem.h"

namespace R3 {


//-----------------------------------------------------------------------------

	/*
	PROFILE_SHARED_DEFINE(DisplaySystem);
	PROFILE_SHARED_DEFINE(ScriptSystem);
	PROFILE_SHARED_DEFINE(PhysicsSystem);
	/*/
	_PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(DisplaySystem), "^1" "DisplaySystem");
	_PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(ScriptSystem), "^3" "ScriptSystem");
	_PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(PhysicsSystem), "^5" "PhysicsSystem");
	_PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(Debug), "^6" "Debug");
	//*/


//-----------------------------------------------------------------------------

	ProfilerGraph::ProfilerGraph(void) {
		m_mode = PM_OFF;
		profilerMode(Profiler::getSingleton().getMode());
		Profiler::getSingleton().addon(this);
	}


//-----------------------------------------------------------------------------

	void ProfilerGraph::profilerLoad(void) {
		Ninja::Section& iniSection =
			Application::getSingleton().getSystemINI()["Profiler.Graph"];

		int nPoints = iniSection["Points"].toInt();
		float fScale = iniSection["Scale"].toDouble() * ShinyGetTickInvFreq();

		for (int i = 0; i < GRAPH_MAX; i++) {
			m_Graphs[i].resize(nPoints);
			m_Graphs[i].setScale(fScale);
		}

		m_Graphs[GRAPH_DEBUG].create("BasicColour/Magenta");
		m_Graphs[GRAPH_SCRIPT].create("BasicColour/Yellow");
		m_Graphs[GRAPH_PHYSICS].create("BasicColour/Cyan");
		m_Graphs[GRAPH_DISPLAY].create("BasicColour/Red");

		hide();
	}


//-----------------------------------------------------------------------------

	void ProfilerGraph::profilerMode(PROFILERMODE mode) {
		if (m_mode == PM_OFF && mode != PM_OFF) {
			show();

		} else if (m_mode != PM_OFF && mode == PM_OFF) {
			hide();
		}
		
		m_mode = mode;
	}


//-----------------------------------------------------------------------------

	void ProfilerGraph::profilerUpdate(void) {
		PROFILE_FUNC();

		m_Graphs[GRAPH_DISPLAY].addPoint(PROFILE_GET_SHARED_TOTAL_TICKS(DisplaySystem));
		m_Graphs[GRAPH_SCRIPT].addPoint(PROFILE_GET_SHARED_TOTAL_TICKS(ScriptSystem));
		m_Graphs[GRAPH_PHYSICS].addPoint(PROFILE_GET_SHARED_TOTAL_TICKS(PhysicsSystem));
		m_Graphs[GRAPH_DEBUG].addPoint(PROFILE_GET_SHARED_TOTAL_TICKS(Debug));
	}


//-----------------------------------------------------------------------------

	void ProfilerGraph::show(void) {
		for (int i = 0; i < GRAPH_MAX; i++) {
			m_Graphs[i].show();
		}
	}


//-----------------------------------------------------------------------------

	void ProfilerGraph::hide(void) {
		for (int i = 0; i < GRAPH_MAX; i++) {
			m_Graphs[i].hide();
		}
	}

} // namespace
