// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#pragma once

#include "Common.h"
#include "ProfilerAddon.h"


namespace R3 { // tolua_export


//-----------------------------------------------------------------------------

	class ProfilerOutput : // tolua_export
		public ProfilerAddon // tolua_export
	{ // tolua_export
	private:

		PROFILERMODE m_mode;

		Ogre::Overlay* m_Overlay;
		Ogre::OverlayElement* m_Output;
		Ogre::OverlayElement* m_Variables;

		string getVariablesAsString(void);

	public: // tolua_begin

		ProfilerOutput(void);
		~ProfilerOutput() {}

		void profilerMode(PROFILERMODE mode);

		void profilerUpdate(void);
	};
	// tolua_end

} // namespace // tolua_export
