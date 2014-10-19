// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#include "R3/FirstInclude.h" // this header must be first
#include "Profiler.h"
#include "Application.h"
#include "ActionManager.h"
#include "SystemManager.h"


namespace R3 {


//-----------------------------------------------------------------------------

	template<>
	Profiler* Singleton<Profiler>::m_pSingleton = NULL;


//-----------------------------------------------------------------------------

	const struct { const char* str; size_t len; } g_Namespace = { "R3::", 4 };


//-----------------------------------------------------------------------------

	const char* Profiler::skipNamespace(const char* name) {
		return (!(strlen(name) > g_Namespace.len
				&& memcmp(name, g_Namespace.str, g_Namespace.len) == 0))?
				name : name + g_Namespace.len;
	}


//-----------------------------------------------------------------------------

	Profiler::Profiler(void) :
	  m_nFramesElapsed(0),
	  m_mode(PM_OFF),
	  m_firstUpdate(true)
	{
		SystemManager::getSingleton().addSystem(this);
		SystemManager::getSingleton().enableUpdateable(this);
	}


//-----------------------------------------------------------------------------

	void Profiler::systemLoad(void) {
		Application::getSingleton().addLibraryVersionInfo(
			SHINY_FULLNAME " " SHINY_VERSION);

		Ninja::Section& iniSection =
			Application::getSingleton().getSystemINI()["Profiler"];

		m_nFramesMax = iniSection["Frames"].toInt();
		m_fDamping = iniSection["Damping"].toDouble();

		ActionManager::getSingleton().addAction(this);

		setMode(PM_OFF);
	}


//-----------------------------------------------------------------------------

	bool Profiler::systemUnload(void) {
		while (!mAddons.empty()) {
			ProfilerAddon* p = mAddons.back();
			mAddons.pop_back();

			p->profilerUnload();
			delete p;
		}

		PROFILE_DESTROY();
		
		return false;
	}


//-----------------------------------------------------------------------------

	void Profiler::systemUpdate(void) {
		if (m_firstUpdate) { 
			PROFILE_CLEAR();
			m_firstUpdate = false;
			return;
		}

		PROFILE_FUNC();

		if (++m_nFramesElapsed != m_nFramesMax) return;
		m_nFramesElapsed = 0;

		PROFILE_SET_DAMPING(m_fDamping);
		PROFILE_CODE(PROFILE_UPDATE());
		removeNamespaces();

		updateAllAddons();
	}


//-----------------------------------------------------------------------------

	void Profiler::setFrames(uint a_nFrames) {
		if (m_nFramesElapsed < m_nFramesMax && m_nFramesElapsed > a_nFrames) {
			ShinyManager_updateClean(&Shiny_instance);
		}

		m_nFramesMax = a_nFrames;
		m_nFramesElapsed = 0;
	}


//-----------------------------------------------------------------------------

	void Profiler::setMode(PROFILERMODE mode) {
		m_mode = (mode > PM_MIN)? ((mode < PM_MAX)? mode:PM_MAX):PM_MIN;
		
		AddonVector Buffer = mAddons;

		BOOST_FOREACH(ProfilerAddon *p, Buffer) {
			p->profilerMode(mode);
		}
	}


//-----------------------------------------------------------------------------

	void Profiler::addon(ProfilerAddon* p) {
		p->profilerLoad();
		mAddons.push_back(p);
	}


//-----------------------------------------------------------------------------

	void Profiler::removeAddon(ProfilerAddon* p) {
		AddonVector::iterator iter;

		iter = find(mAddons.begin(), mAddons.end(), p);
		if (iter == mAddons.end()) return;
		
		mAddons.erase(iter);

		// we can't call virtual-function if caller is destructor
		//p->profilerUnload();
	}


//-----------------------------------------------------------------------------

	void Profiler::updateAllAddons(void) {
		AddonVector Buffer = mAddons;

		BOOST_FOREACH(ProfilerAddon* p, Buffer) {
			p->profilerUpdate();
		}
	}
	
	
//-----------------------------------------------------------------------------

	void Profiler::destroyAllAddons(void) {
		while (!mAddons.empty()) {
			ProfilerAddon* p = mAddons.back();
			mAddons.pop_back();

			if (!p->profilerUnload())
				delete p;
		}
	}


//-----------------------------------------------------------------------------

	bool Profiler::actionPressed(int clicks) {
		if (m_mode++ == PM_MAX) m_mode = PM_MIN;

		setMode(getMode());
		return true;
	}


//-----------------------------------------------------------------------------

	void Profiler::removeNamespaces(void) {
		ShinyZone *pZone = Shiny_instance.rootZone.next;

		while (pZone) {
			pZone->name = skipNamespace(pZone->name);
			pZone = pZone->next;
		}
	}

} // namespace
