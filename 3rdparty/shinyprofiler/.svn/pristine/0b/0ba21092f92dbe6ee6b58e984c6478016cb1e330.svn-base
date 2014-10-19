// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#include "R3/FirstInclude.h" // this header must be first
#include "ProfilerOutput.h"
#include "Profiler.h"


namespace R3 {


//-----------------------------------------------------------------------------

	ProfilerOutput::ProfilerOutput(void) {
		m_Overlay = Ogre::OverlayManager::getSingleton().getByName("Profiler/Overlay");
		m_Output = Ogre::OverlayManager::getSingleton().getOverlayElement("Profiler/Output");
		m_Variables = Ogre::OverlayManager::getSingleton().getOverlayElement("Profiler/Variables");

		m_mode = PM_OFF;
		profilerMode(Profiler::getSingleton().getMode());
		Profiler::getSingleton().addon(this);
	}


//-----------------------------------------------------------------------------

	void ProfilerOutput::profilerMode(PROFILERMODE mode) {
		bool oldToggle = (m_mode == PM_TREE) || (m_mode == PM_FLAT);
		bool newToggle = (mode   == PM_TREE) || (mode   == PM_FLAT);

		if (!oldToggle && newToggle) {
			m_Overlay->show();

		} else if (oldToggle && !newToggle) {
			m_Overlay->hide();
		}

		m_mode = mode;
	}


//-----------------------------------------------------------------------------

	void ProfilerOutput::profilerUpdate(void) {
		PROFILE_FUNC();

		if ((m_mode != PM_TREE) && (m_mode != PM_FLAT)) return;

		m_Variables->setCaption(getVariablesAsString());
		
		String output;
		if (m_mode == PM_TREE) {
			output = PROFILE_GET_TREE_STRING();

		} else {
			output = PROFILE_GET_FLAT_STRING();
		}
		
		size_t i = 0;
		while ((i = output.find('^', i)) != String::npos) {

			i = output.find(' ', i + 2);
			if (i == String::npos) break;

			output.insert(i, "  ");
			i += 3;

			i = output.find('\n', i);
			if (i == String::npos) break;

			output.insert(i, "^7");
			i += 3;
		}

		m_Output->setCaption(output);
	}


//-----------------------------------------------------------------------------

	String ProfilerOutput::getVariablesAsString(void) {
#if SHINY_IS_COMPILED == TRUE
		const int OUTPUT_WIDTH_SUM = 79;
		const int VARIABLE_SPACE = 6;
		char buffer[OUTPUT_WIDTH_SUM + 1];

		snprintf(buffer, OUTPUT_WIDTH_SUM,
			"Frames: %-*d Damping [%%]: %-*.0f Table Usage [%%]: %-*.0f Lookup Rate [%%]: %-*.0f",
			VARIABLE_SPACE, Profiler::getSingleton().getFrames(),
			VARIABLE_SPACE, Profiler::getSingleton().getDamping() * 100.0f,
			VARIABLE_SPACE, ShinyManager_tableUsage(&Shiny_instance) * 100.0f,
			VARIABLE_SPACE, ShinyManager_lookupRate(&Shiny_instance) * 100.0f);

		buffer[OUTPUT_WIDTH_SUM] = '\0';
		return buffer;
#else
		return "";
#endif
	}

} // namespace
