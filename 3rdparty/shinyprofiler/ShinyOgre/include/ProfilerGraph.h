// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#pragma once

#include "HistoryGraph.h"
#include "ProfilerAddon.h"


namespace R3 { // tolua_export


//-----------------------------------------------------------------------------

	PROFILE_SHARED_EXTERN(DisplaySystem);
	PROFILE_SHARED_EXTERN(ScriptSystem);
	PROFILE_SHARED_EXTERN(PhysicsSystem);
	PROFILE_SHARED_EXTERN(Debug);


//-----------------------------------------------------------------------------

	class ProfilerGraph : // tolua_export
		public ProfilerAddon // tolua_export
	{ // tolua_export
	private:

		enum GRAPH {
			GRAPH_DISPLAY,
			GRAPH_SCRIPT,
			GRAPH_PHYSICS,
			GRAPH_DEBUG,

			GRAPH_MAX,
			GRAPH_ZORDER = 100
		};

		PROFILERMODE m_mode;

		HistoryGraph m_Graphs[GRAPH_MAX];

		void show(void);
		void hide(void);

		virtual void profilerLoad();
		virtual void profilerMode(PROFILERMODE mode);

		virtual void profilerUpdate(void);

	public: // tolua_begin

		ProfilerGraph(void);
		~ProfilerGraph() {}
	};
	// tolua_end

} // namespace // tolua_export
