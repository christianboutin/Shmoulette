// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___BEAM_H___
#define ___DEF___BEAM_H___

#include "..\elements\gameobject.h"
#include "ammo.h"
namespace Shmoulette{

	class BeamGO:public GameObject{
		typedef GameObject Parent;
	
		//float mScale, mTimeToReach, mTimeToRetract, mReachLength; //mTimeAtReach
		float mCurrentReach, mReachSpeed, mRetractSpeed;
		float mObstacleReach;
		class FX *mImpactFX;
		bool mIsFiring;
		bool mLastFrameHit;

		class BeamWeaponDBE* mBeamWeaponDBE;


		public:
			virtual void hit(DamagePackage& pack, GameObject* hitter, GameObject* forwardFrom = NULL, float distance = FLT_MAX);
			virtual void takeDamage(DamagePackage& pack);
			virtual void setLength(float length);
			virtual void build();  // FUCK I HATE THIS FUCKING FUCKED UP SHIT JUST TO BE ABLE TO PUT A FUCKING BREAKPOINT
			virtual void show();// FUCK I HATE THIS FUCKING FUCKED UP SHIT

			virtual void fire();
			virtual void ceaseFire();
			virtual bool update(double timeDelta, double parentTime);

			BeamGO(){}
			virtual void init(class BeamWeaponDBE* dbe);
	};

	class _ShmouletteExport BeamWeapon:public Weapon{
		typedef Weapon Parent;
		protected:

		BeamGO* mBeamObject;

		string mAnim;
		Ogre::AxisAlignedBox mParticleZone;
		float mSupplyCostPerSecond;

		virtual void _actualFiring();  // the method "fire" only puts the firing flag to on.  _actualFiring does the job of firing, 1 bullet or activate beam or whatever.

		public:
			BeamWeapon(class BeamWeaponDBE* dbe, WeaponSupply* ws);
			virtual void build();
			virtual void destroy();
			virtual bool update(double timeDelta, double parentTime);
			virtual void ceaseFire();
			virtual bool postFrameUpdate(double timeDelta, double parentTime); // Updates the collision structure

	};

	class _ShmouletteExport BeamWeaponDBE:public WeaponDBE{
		typedef WeaponDBE Parent;
			string mMesh;
			float mScale;
			float mReachLength;
			float mTimeToReach;
			float mTimeToRetract;
			DamagePackage mDamagePackage;
			string mImpactFX;


		public:
			BeamWeaponDBE(string id, XML& xml);
			~BeamWeaponDBE();
			string getMesh(){return mMesh;}
			string getImpactFX(){return mImpactFX;}
			float getReachLength(){return mReachLength;}
			float getTimeToReach(){return mTimeToReach;}
			float getTimeToRetract(){return mTimeToRetract;}
			float getScale(){return mScale;}
			DamagePackage& getDamagePackage(){return mDamagePackage;}

			virtual class Weapon* spawn(WeaponSupply* ws);
	};
}


#endif
