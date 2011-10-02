// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___DRONEDBE_H___
#define ___DEF___DRONEDBE_H___

#include "..\weapon\ammofactoryfactory.h"
#include "..\fx\debris.h"


namespace Shmoulette{
	typedef struct TAG_DEATH_BEHAVIOUR{
		string mDLLCustomCode;
#if 0
		list<XML>mDebris;
#endif
		//string mExplosionParticles;
		//float mExplosionDecay;
		string mFX;
		float mTime;
	}DEATH_BEHAVIOUR;
	
	typedef struct TAG_DRONE_CHILD{
		string mName;
		string mBone;
		string mLabel;
		Ogre::Vector3 mDelta;
		bool mForward;
	}DRONE_CHILD;

/*	class DroneMount{
		protected:
			AmmoFactory* mAmmoFactory;
			Ogre::Vector3 mPos;
			Ogre::Radian mYaw;
			Ogre::Radian mPitch;
			string mBone;
		public:
			DroneMount(XML* xml);
			Ammo* spawn(GameObject* launcher);
	};
*/
	class DroneDBE{
		protected:
		int baseObjectNumber;  // The main object is in the DB, instances of an enemy use instances of the object
		float mMaxTime;
		string mId;
		float mMaxLife;
		//list<DroneMount*>mDroneMount;
		list<WeaponInfo*>mWeaponInfo;
		//list<AmmoFactory*>mAmmoFactory;
		string mMeshName;
		string mResourceContext;
		string mChildren;
		int mScore;
		Ogre::Vector3 mScale;
		vector<DEATH_BEHAVIOUR>mDeath;
		vector<DRONE_CHILD>mChild;
		list<XML*>mGoAnim;
		list<XML*>mAnim;
		HMODULE mDll;
		string mPythonCallback;

		void setMeshName(string name);
		void setChildrenName(string name){
			mChildren = name;
		}

		/*virtual void addAmmoFactory(AmmoFactory* fact){
			mAmmoFactory.push_back(fact);
		}
		virtual void addAmmoFactory(string s){
			mAmmoFactory.push_back(AmmoFactoryFactory::getSingleton()->getFactory(s));
		}*/

		/*virtual void addMount(AmmoFactory* fact){
			mAmmoFactory.push_back(fact);
		}*/
		/*virtual void addMount(string s){
			mAmmoFactory.push_back(AmmoFactoryFactory::getSingleton()->getFactory(s));
		}*/
		virtual void addWeaponInfo(XML* xml);
/*		virtual void addMount(XML* xml){
			mDroneMount.push_back(new DroneMount(xml));
		}
*/
		public:
			void setPythonCallback(string pc){
				mPythonCallback = pc;
			}
			string getChildrenName(){ return mChildren;}

			virtual bool isStatic(){return false;}

			list<XML*> getGoAnim(){return mGoAnim;}
			list<XML*> getAnim(){return mAnim;}
			vector<DRONE_CHILD>getChildVector(){return mChild;}
			DEATH_BEHAVIOUR* getDeathBehaviour(int index){
				if (index == -1 || mDeath.size() == 0) return NULL;
				return &mDeath[index];
			}
			int requestDeathIndex(){
				if (mDeath.size() == 0) return -1;
				return ((int)Ogre::Math::RangeRandom(0,(Ogre::Real)mDeath.size()-.0001));
			}
			Ammo* getAmmoFromMount(int ndx, GameObject* launcher);
			list<WeaponInfo*> getWeaponInfo();
			//string getBoneFromMount(int ndx);
			Ogre::Vector3 getScale(){return mScale;}
			DroneDBE(string id);
			virtual string getId(){ return mId; }
			virtual void init(XML& dronestatXml, XML& objectMovementXml);
			virtual float getX(int frame){return 0;}
			virtual float getY(int frame){return 0;}
			virtual float getZ(int frame){return 0;}
			virtual bool isExpired(float time){
				return time > mMaxTime;
			}
			virtual float getMaxLife(){return mMaxLife;}
			virtual int getScore(){return mScore;}
			virtual float getDeathTime(int index){
				if (index == -1) return 0;
				if (index < mDeath.size()){
					return mDeath[index].mTime;
				}else{
					throw("Illegal Death Index");
				}
			}
			virtual string getMeshName(){return mMeshName;}
			virtual string getResourceContext(){return mResourceContext;}
			virtual float getMaxTime(){return mMaxTime;}
			virtual int getBaseObjectNumber(){return baseObjectNumber;}
			virtual class Drone* createDrone();
			
	};
}

#endif
