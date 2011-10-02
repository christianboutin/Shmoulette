// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___WATER_H___
#define ___DEF___WATER_H___

#include "..\elements\gameobject.h"


#if 0
namespace Shmoulette{


class WaterZone:public StaticObj{
	typedef StaticObj Parent;
	string mWaterTexture;
	class ReflectionTextureListener* mReflectionListener;
	class RefractionTextureListener* mRefractionListener;
	Ogre::Vector2 mSeg;
	Ogre::RenderTexture* mReflectionTexture;
	Ogre::RenderTexture* mRefractionTexture;
	Ogre::Plane mReflectionPlane;
	string mMaterialName;
	
	public:
		//Ogre::Plane mReflectionPlane;
		Ogre::Plane getReflectionPlane(){return mReflectionPlane;}
		WaterZone(XML* xml);
		~WaterZone();
		void setVisible(bool _visible){ 
			mEntity->setVisible(_visible);
		}
		void destroy();
		bool update();
		void build();
};

class ReflectionTextureListener : public Ogre::RenderTargetListener{
	WaterZone* mWater;
	std::vector<Ogre::MovableObject*>belowWaterEnts;
public:
	void setWater(WaterZone* water){mWater = water;}
	void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
};

class RefractionTextureListener : public Ogre::RenderTargetListener{
	WaterZone* mWater;
	std::vector<Ogre::MovableObject*>aboveWaterEnts;
public:
	void setWater(WaterZone* water){mWater = water;}
	void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
};
}
#endif
#endif
