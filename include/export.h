// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___EXPORT_H___
#define ___DEF___EXPORT_H___

#ifdef SHMOULETTE_EXPORTS
	#define _ShmouletteExport __declspec( dllexport )
#else 
	#define _ShmouletteExport __declspec( dllimport )
#endif

#endif
