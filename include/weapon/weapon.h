// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___WEAPON_H___
#define ___DEF___WEAPON_H___

#include "..\levelsegment.h"
#include "ammo.h"
#include "ammofactoryfactory.h"
#include "fx/fx.h"
#include "weapon/weaponsupply.h"
//#include "OgreSingleton.h"
namespace Shmoulette{

	class _ShmouletteExport WeaponDBE{
		protected:
			float mSupplyCost;
			float mReloadTime;
			string mFX;
			string mId;

		public:
			float getSupplyCost(){return mSupplyCost;}
			float getReloadTime(){return mReloadTime;}
			string getId(){return mId;}
			string getFX(){return mFX;}

			~WeaponDBE();
			WeaponDBE(string id, XML& xml);
			virtual class Weapon* spawn(WeaponSupply* ws)=0;
	};


	
	//#include "phaserdb.h"




	class _ShmouletteExport Weapon{
	protected:
		FX* mFX;
		MountInfo mMount;
		
		float mTimeToReady;
		//float mTimeToReload;
		//float mSupplyCost;

		bool mIsFiring;

		bool mIsBuild;

		WeaponSupply* mWeaponSupply;
		WeaponDBE* mWeaponDBE;
		virtual void _actualFiring();  // the method "fire" only puts the firing flag to on.  _actualFiring does the job of firing, 1 bullet or activate beam or whatever.

		public:
			virtual void setInertia(Ogre::Vector3 inertia){mMount.mInertia = inertia;}
			Weapon(WeaponDBE* dbe, WeaponSupply* ws);
			//Weapon(XML* xml, WeaponSupply* weaponSupply);
			virtual float getSupplyCost(){return mWeaponDBE->getSupplyCost();}
			virtual float getReloadTime(){return mWeaponDBE->getReloadTime();}

			//virtual void setHooked(bool hooked){mMount.mHooked = hooked;}

			//virtual void setLevelSegment(LevelSegment* ls){mLevelSegment = ls;}
			virtual void build();
			virtual void destroy();
			virtual void fire();
			virtual void ceaseFire();
			virtual bool update(double timeDelta, double parentTime);
			virtual bool postFrameUpdate(){return false;} // Updates the collision structure, if applicable
			virtual Ogre::Matrix4 getMountTransform();
			virtual float getReadyPercent();
			virtual void addReadyValue(float value);
			virtual bool isReady(){
				return (mTimeToReady == 0) && (mWeaponSupply->canSupply(mWeaponDBE->getSupplyCost()) == true);
			}
			virtual void setMount(GameObject* obj, string bone){
				if (bone.find("/") == string::npos){
					mMount.obj = obj;
					if (bone == "")
						throw("Trying to mount a weapon with a blank bone it.  Try using the 'root' bone if available");
					mMount.boneLabel = bone;
				}else{
					string childObj = bone.substr(0, bone.rfind("/"));
					mMount.obj = obj->getChild(childObj);
					mMount.boneLabel = bone.substr(bone.rfind("/")+1);
				}
			}
	};



	class XmlDocInstance{
		XML* mInstance;
		int mNbInstance;
	public:
		XmlDocInstance(XML* xml){
			mInstance = xml;
			mNbInstance = 1;
		}
		XML* getInstance(){
			return mInstance;
		}
		void addInstance(){
			mNbInstance++;
		}
		bool removeInstance(){
			mNbInstance--;
			if (mNbInstance == 0){
				return true;
			}else{
				return false;
			}
		}

	};
	// WeaponInfo contains information that can mount a weapon from a weapon database entry (DBE) and mount information (bone, inertia etc.)
	// When a weapon is spawned from a WeaponInfo that info is applied.  You can also manually spawn a weapon and set those data yourself.
	class _ShmouletteExport WeaponInfo{
		static void addXmlInstance(XML* xml);
		static bool removeXmlInstance(XML* xml);
		static vector<XmlDocInstance*> sXmlDocInstance;

		Ogre::Vector3 mInertia; // Only useful for projectile weapons.  If mInertia.x == FLT_MAX, then use the mount's inertia
		//bool mHooked; // Create and hook in relation to Hookspace, if false then use Globalspace, only useful for projectile weapons

	protected:
			//string mDll;
			WeaponDBE* mWeaponDBE;
			//string mXmlFilename;
			string mMount;
			//string mType;
			//string mSubtype;
			//XML* mXml;
		public:
			WeaponInfo(XML* xml);
			WeaponInfo(const WeaponInfo& source);
	
			~WeaponInfo();
			Weapon* spawn(GameObject* launcher, WeaponSupply* ws);
	};

	/*class _ShmouletteExport WeaponInfoFactory: public Singleton<WeaponInfoFactory>{
		private:
			list<WeaponInfo*> wi;
			WeaponInfoFactory();
		public:
			WeaponInfo* getWeaponInfo(string id);
	};*/


	}

	#include "bullet.h"
	#include "beam.h"

#endif
