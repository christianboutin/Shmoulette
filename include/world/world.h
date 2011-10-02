// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___WORLD_H___
#define ___DEF___WORLD_H___

#include "clouds.h"
#include "water.h"
#include "groundplane.h"
namespace Shmoulette{

typedef enum TAG_SKY_TYPE{
	SKY_DOME,
	SKY_CUBE,
	SKY_PLANE,
	SKY_NONE
}SKY_TYPE;

typedef struct TAG_SKY_DEF{
	SKY_TYPE mType;
	string mMaterial;
	float mDistance; // For cube maps only
}SKY_DEF;

typedef struct TAG_TERRAIN_DEF{
		SceneType mType;
		string mConfig;
}TERRAIN_DEF;


typedef struct TAG_FOG_VECTOR{
		FOG_INFO current, wanted, speed;
}FOG_VECTOR;

class World:public GameElement{
	typedef GameElement Parent;
	vector<GameObject*>mVectorEnviroZone;
	SKY_DEF mSky;
	TERRAIN_DEF mTerrain;
	bool mLoadGeometry;

	LevelSegment* mLevelSegment;

	Ogre::ShadowTechnique mShadow;
	
	//string mGroundPlaneTexture;
	struct{
		Ogre::ColourValue current;
		Ogre::ColourValue wanted;
		Ogre::ColourValue speed;
	}mAmbientLight;
	FOG_VECTOR mFog;
	Ogre::Real mFogTimeLeft;
	bool mFogOn;

	public:
		static Ogre::ShadowTechnique shadowTypeFromString(string s);
		virtual void init(XML* xml, LevelSegment* ls);
		virtual void init(LevelSegment* ls);
		~World();
		void build();
		void destroy();
		float getHeightAt(float x, float z);
		SceneType getType(){return mTerrain.mType;}
		virtual bool update(double timeDelta, double parentTime);
		void configChange(string filename);
		string getSceneName(){return mTerrain.mConfig;}
		void fogChange(FOG_INFO newFog, Ogre::Real timer);
};
}

#endif
