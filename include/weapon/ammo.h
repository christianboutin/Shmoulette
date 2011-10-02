// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___AMMO_H___
#define ___DEF___AMMO_H___

#include "..\level.h"
#include "..\elements\gameobject.h"
#include "level.h"

namespace Shmoulette{
	class _ShmouletteExport MountPosition{
		public:
			Ogre::Matrix4 mBaseTransform;
			Ogre::Radian mXDeflection;
			Ogre::Radian mYDeflection;
			MountPosition(Ogre::Matrix4 bt, Ogre::Radian xd, Ogre::Radian yd){
				mBaseTransform = bt;
				mXDeflection = xd;
				mYDeflection = yd;
			}
	};

	class _ShmouletteExport MountInfo{
		public:
			GameObject* obj;
			string boneLabel;
			Ogre::Bone* bone;
			Ogre::Vector3 mInertia; // Only useful for projectile weapons.  If mInertia.x == FLT_MAX, then use the mount's inertia
			//bool mHooked; // Create and hook in relation to Hookspace, if false then use Globalspace, only useful for projectile weapons
		};

	class _ShmouletteExport Ammo:public GameObject{
		typedef GameObject Parent;
		protected:
			Ogre::Vector3 mLauncherInertia; // Speed of the launcher

			virtual void init(string id, bool append_number = false){
				Parent::init(id, append_number);
			};
			Ammo(){}
		public:
		virtual void setLauncherInertia(Ogre::Vector3 v){
			mLauncherInertia = v;
		}
	};
	
	class _ShmouletteExport AmmoFactory{
		protected:
			DamagePackage mDamagePackage;
			string mId;
			string mMeshName;
		public:
			virtual DamagePackage& getDamagePackage(){return mDamagePackage;}
			virtual std::string getId(){return mId;}
			virtual Ammo* spawn(MountInfo& mount, MountPosition& pos)=0;
			string getMeshName(){return mMeshName;}
			AmmoFactory(string id, XML* xml);
	};
}




#endif
