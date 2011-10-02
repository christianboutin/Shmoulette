// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___GROUNDPLANE_H___
#define ___DEF___GROUNDPLANE_H___

#if 0

#include "..\dynamicobj.h"

namespace Shmoulette{
	class GroundPlane:public StaticObj{
		typedef StaticObj Parent;
		Ogre::Vector2 mZoneSize;
		Ogre::Vector2 mUVTile;
		string mMaterialName;
	public:
		GroundPlane(XML* xml);
		//~GroundPlane();
		void build();
		void destroy();
		void show();
		void hide();
		bool update();
		void setMaterial(string fileName){mMaterialName = fileName;}
		void setZoneSize(float x, float y){mZoneSize.x = x; mZoneSize.y=y;}
	};
}
#endif

#endif
