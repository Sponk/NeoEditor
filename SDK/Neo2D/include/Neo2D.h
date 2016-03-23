#ifndef __NEO2D_H__
#define __NEO2D_H__

#ifdef SWIG
#define DISOWN(a) %apply SWIGTYPE* DISOWN {a};
#else
#define DISOWN(a)
#endif

#ifdef WIN32
#ifdef _MSC_VER
		#pragma warning(disable: 4251)
	#endif

	#if defined(NEO2D_DLL)
		#define NEO2D_EXPORT __declspec( dllexport )
	#elif defined(NEO_CORE_STATIC)
		#define NEO2D_EXPORT
	#else
		#define NEO2D_EXPORT __declspec( dllimport )
	#endif

#else
#define NEO2D_EXPORT
#endif
#endif
