// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___DEFAULTDRONE_H___
#define ___DEF___DEFAULTDRONE_H___


#include "..\util\refinedtrack.h"
#include "drone.h"
namespace Shmoulette{

typedef enum TAG_FIRING_OPTION{
	FIRING_START,
	FIRING_STOP
}FIRING_OPTION;

typedef struct TAG_FIRING_TIME{
	float time;
	int mount;
	bool cease; // true if the flag represents
	FIRING_OPTION firingOption;
	Ogre::Vector3 inertia;
}FIRING_TIME;


class _ShmouletteExport DefaultDrone:public Drone{
	typedef Drone Parent;
	list<Ogre::AnimationState*>mAnim;
protected:
	virtual void updateAlive(double timeDelta, double parentTime);
	virtual void updateDead(double timeDelta, double parentTime);
	virtual void show();

	public:

	DefaultDrone();
	virtual void init(DroneDBE* dbe); // used as passthrough for children
	//PathedDrone(XML* xml, float timeDelta);
	//virtual void build();
	//virtual void updateDead();
	//virtual void fire(int type=0){};
};


class _ShmouletteExport DefaultDroneDBE:public DroneDBE{
	typedef DroneDBE Parent;
protected:
	list<FIRING_TIME> mFiringTime;
	Coordinator::TransformCoordinator* mTransformCoordinator;
	//list<DRONE_TIMED_ANIM> mTimedAnim;
public:
	list<FIRING_TIME>* getFiringTime(){return &mFiringTime;}
	//list<DRONE_TIMED_ANIM>* getTimedAnim(){return &mTimedAnim;}

	virtual bool isStatic(){return !mTransformCoordinator->hasChanged();}

	DefaultDroneDBE(string s):Parent(s){}
	//PathFromXML(XML* xml);

	virtual Ogre::TransformKeyFrame getKeyFrame(float time);
	virtual void init(XML& dronestatXml, XML& objectMovementXml);
	virtual DefaultDrone* createDrone();

	//MACRO_CREATE_DRONE(DefaultDrone);

};

}

#endif
