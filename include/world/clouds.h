// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___CLOUDS_H___
#define ___DEF___CLOUDS_H___

#ifndef CLOUDS_H
#define CLOUDS_H


#include "..\elements\gameobject.h"
#include "..\level.h"

namespace Shmoulette{
	using namespace Ogre;
	typedef struct TAG_CLOUD_PARTICLE{
		Ogre::SceneNode* mSceneNode;
		Ogre::Entity* mEntity;
		Ogre::Vector3 mSpeed;
		Ogre::Vector3 mSpin;
		Ogre::MaterialPtr mMaterial;
		Ogre::Pass* mMaterialPass;
		Ogre::ColourValue mColour;
	}CLOUD_PARTICLE;

	class CloudZone:public GameObject{
		typedef GameObject Parent;
		list<CLOUD_PARTICLE>mCloudParticle;
		Vector3 mMinSpeed;
		Vector3 mMaxSpeed;
		Vector3 mMinSize;
		Vector3 mMaxSize;
		Vector3 mZoneSize;

		Vector3 mPreviousPosition;

		CLOUD_PARTICLE mInfiniteLayer;
		string mInfiniteLayerMaterial;
		bool mCreateInfiniteLayer;
		bool mInfiniteLayerIsOnTop;

		bool mFade;
		bool mSpin;
		
		string mMaterialName;
		int mDensity;
		//Ogre::SceneNode* mSceneNode;
	public:
		virtual void init(XML* xml);
		//~CloudZone();
		void build();
		void destroy();
		void show();
		void hide();
		bool update(double timeDelta, double parentTime);
		void setMaterial(string fileName){mMaterialName = fileName;}
		void setBottomMaterial(string fileName){mMaterialName = fileName;}
		void setDensity(int density){mDensity = density;}
		void setZoneSize(float x, float y, float z){mZoneSize.x = x; mZoneSize.y=y; mZoneSize.z=z;}
		void setMinSpeed(float x, float y, float z){mMinSpeed.x = x; mMinSpeed.y=y; mMinSpeed.z=z;}
		void setMaxSpeed(float x, float y, float z){mMaxSpeed.x = x; mMaxSpeed.y=y; mMaxSpeed.z=z;}
		void setMinSize(float x, float y, float z){mMinSize.x = x; mMinSize.y=y; mMinSize.z=z;}
		void setMaxSize(float x, float y, float z){mMaxSize.x = x; mMaxSize.y=y; mMaxSize.z=z;}
		void setCreateInfiniteLayer(bool create){mCreateInfiniteLayer = create;}
		void setInfiniteLayerMaterial(string mat){mInfiniteLayerMaterial = mat;}
		void setInfiniteLayerOnTop(bool top){mInfiniteLayerIsOnTop = top;}
	};

}

#endif

#endif
