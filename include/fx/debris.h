// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___DEBRIS_H___
#define ___DEF___DEBRIS_H___

//#include "..\main.h"
#include "fxobject.h"
namespace Shmoulette{
	typedef enum TAG_DEBRIS_DIRECTION{
		DEBRIS_DIRECTION_FORWARD,
		DEBRIS_DIRECTION_RANDOM
	}DEBRIS_DIRECTION;

	typedef struct TAG_DEBRIS_DETAIL{
		string mMesh;
		float mSpinMultiplier;
		DEBRIS_DIRECTION mDirection;
		string mPython;
		string mFinalFX;
	}DEBRIS_DETAIL;

	class Debris:public GameObject{
		typedef GameObject Parent;
		Ogre::Vector3 mSpeed;
		Ogre::Vector3 mSpin;
	public:
		virtual void init(XML* xml);
		virtual void init(string mesh, Ogre::Vector3 starting_pos);
		Debris(){};
		virtual bool update(double timeDelta, double parentTime);
		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual void hide();
	};

}

#endif
