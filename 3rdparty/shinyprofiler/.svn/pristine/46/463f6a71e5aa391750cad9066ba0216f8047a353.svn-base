// Copyright (c) 2009 Aidin Abedi. All Rights Reserved. //
//******************************************************//

#pragma once

#include "Common.h"
#include "Singleton.h"
#include "System.h"
#include "ProfilerAddon.h"
#include "Action.h"


namespace R3 { // tolua_export


//-----------------------------------------------------------------------------

	class Profiler : // tolua_export
		public Singleton<Profiler>,
		public System /* if you think public is a mistake, think again */ // tolua_export
		,
		private Action
	{ // tolua_export
	private:

		uint m_nFramesElapsed;
		uint m_nFramesMax;
		float m_fDamping;

		int m_mode;
		bool m_firstUpdate;
		
		typedef vector<ProfilerAddon*> AddonVector;
		
		AddonVector mAddons;

		static const char* skipNamespace(const char* name);
		void removeNamespaces(void);

		virtual bool actionPressed(int clicks);
		virtual String actionName(void) { return "CycleProfiler"; }
		virtual bool actionUnload(void) { return true; }

		virtual void systemLoad(void);
		virtual bool systemUnload(void);
		virtual void systemUpdate(void);

		virtual String systemName(void) { return "Profiler"; }

		virtual int systemPriority(void) { return SP_PROFILER; }

	public: // tolua_export

		// tolua_begin
		Profiler(void);
		~Profiler(void) { destroyAllAddons(); }
		// tolua_end

		// tolua_begin
		void setMode(PROFILERMODE mode);
		PROFILERMODE getMode(void) { return static_cast<PROFILERMODE>(m_mode); }

		void setFrames(uint a_nFrames);
		uint getFrames(void) { return m_nFramesMax; }

		void setDamping(float a_fDamping) { m_fDamping = a_fDamping; }
		float getDamping(void) { return m_fDamping; }

		static Profiler& getSingleton(void) { return Singleton<Profiler>::getSingleton(); }
		static Profiler* getSingletonPtr(void) { return Singleton<Profiler>::getSingletonPtr(); }

		void addon(ProfilerAddon* p);
		void removeAddon(ProfilerAddon* p);
		
		void updateAllAddons(void);
		void destroyAllAddons(void);

		void clear(void) { PROFILE_DESTROY(); }
		// tolua_end
	}; // tolua_export

} // namespace // tolua_export
