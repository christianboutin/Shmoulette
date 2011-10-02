// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___DRONE_H___
#define ___DEF___DRONE_H___

#include "..\elements\gameobject.h"
#include "dronedbe.h"
#include "..\weapon\ammo.h"
#include "..\weapon\ammofactoryfactory.h"

namespace Shmoulette{
	typedef enum{
		DRONE_ALIVE,
		DRONE_DEATH_EXPIRATION,
		DRONE_DEATH_DESTRUCTION
	}DRONE_DEATH_TYPE;
		
	class _ShmouletteExport DroneDB{
		vector<DroneDBE*>mEntry;
		bool isInitDone;
		DroneDBE* _createEntry(XML& dronestatXml, XML& objectMovementXml, string id);

		public:
		DroneDB(){
			isInitDone = false;
		}

		void init();
		/*DroneDBE* entry(DRONE_DB_ENTRY e){
			return mEntry[e];
		}*/
		DroneDBE* entry(string s);
		Drone* createDrone(string type){
			if (!isInitDone) init();
			return entry(type)->createDrone();
		}
		Drone* createDrone(PyObject* po);


		Drone* createDrone(XML& dronestatXml, XML& objectMovementXml);
		/*Drone* createDrone(string type, GameObject* parent){
			if (!isInitDone) init();
			return entry(type)->createDrone(parent);
		}*/

	};

	class _ShmouletteExport Drone:public GameObject{
		typedef GameObject Parent;
		void _createFromDbe(DroneDBE* dbe);

		protected:
			DRONE_DEATH_TYPE mDeathType;
			//Ogre::Vector3 mDelta;
			//Ogre::Quaternion mBaseAngle;
			//Ogre::Vector3 mBaseScale;
			class DroneDBE* mDbe;
			GameObject* mTarget;
			//bool mRailSpawn;
			float mDeathTime;
			int mDeathIndex;
			list<Drone*> mDroneChild;
			//bool mMarkedForStandby;




		/*Ogre::Entity* mSpawnPointEntity;
		Ogre::SceneNode* mSpawnPointSceneNode;

		Ogre::Entity* mLocalSpawnPointEntity;
		Ogre::SceneNode* mLocalSpawnPointSceneNode;*/

		double mRestorePaintTime;

		//static class DroneDB* mDroneDB;

		public:
			virtual void init(DroneDBE* dbe);
			DroneDBE* getDbe(){return mDbe;}
			virtual void init(string id);
			Drone();
			~Drone();
			//virtual void markForStandby(){mMarkedForStandby = true;}
			//virtual void markForReturnFromStandby(){mMarkedForStandby = false;}
			//virtual bool isMarkedForStandby(){return mMarkedForStandby;}
			list<Drone*> getActiveChildren();
			bool isInZone();
			virtual void setTimeAlive(float timeAlive);
			virtual void setTimeStart(float timeStart);
			virtual void notifyChildDeath(GameObject* deadChild);

			//virtual void setRailSpawn(bool isIt){mRailSpawn = isIt;}
			//static void setDroneDB(class DroneDB* droneDB){mDroneDB = droneDB;}
			virtual void build();
			virtual void destroy();
			virtual void show();
			virtual void die(DRONE_DEATH_TYPE deathType = DRONE_DEATH_DESTRUCTION);
			virtual DRONE_DEATH_TYPE getDeathType(){return mDeathType;}
			//virtual void dataFromXML(string str, int timeDelta = 0, int life = 600);
			//virtual void deltaSet(float x, float y, float z);
			//virtual void deltaSet(Ogre::Vector3 v);//{deltaSet(v.x, v.y, v.z);}
			//virtual void deltaPush(float x, float y, float z);
			//virtual void deltaPush(Ogre::Vector3 v);//{deltaPush(v.x, v.y, v.z);}
			/*virtual void baseAngleSet(Ogre::Quaternion q){
				mBaseAngle = q;
			};
			virtual void baseScaleSet(Ogre::Vector3 v){
				mBaseScale = v;
			}*/
			virtual void hit(DamagePackage& pack, GameObject* hitter, GameObject* forwardFrom, float distance, double time);
			virtual void takeDamage(DamagePackage& pack, double time);
			virtual void addChild(std::string drone, std::string bone, std::string label, bool forwardDamages, Ogre::Vector3 delta=Ogre::Vector3(0,0,0), std::string DLL="");
			virtual bool update(double timeDelta, double parentTime);
			virtual bool postFrameUpdate(double timeDelta, double parentTime);
			virtual void updateAlive(double timeDelta, double parentTime)=0;
			virtual void updateDead(double timeDelta, double parentTime){};
			//virtual void fire(int type=0, Ogre::Vector3 inertia = Ogre::Vector3(0,0,0));
			//virtual void ceaseFire(int type=0);
	};
}
#endif
