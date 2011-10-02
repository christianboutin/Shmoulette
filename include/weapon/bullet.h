// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___BULLET_H___
#define ___DEF___BULLET_H___

#include "..\elements\gameobject.h"
#include "ammo.h"
namespace Shmoulette{

/*typedef enum TAG_RAY_QUERY_MASK{
	SOLID_MASK = 1<<8;
	FRIENDLY_MASK = 1<<9;
	ENEMY_MASK = 1<<10;
}RAY_QUERY_MASK;*/


class _ShmouletteExport Bullet:public Ammo{
	typedef Ammo Parent;
	protected:
	string mImpactFX;
	string mDeathFX;

	float mSpeed;

	
	public:
		Bullet();
		virtual void init(class BulletFactory* bf);
		virtual void init();
		virtual void build();
		virtual void show();
		void hide(){
			Parent::hide();
		}
		void destroy(){
			Parent::destroy();
		}
		virtual bool update(double timeDelta, double parentTime);
		virtual void hit(DamagePackage& pack, Shmoulette::GameObject* hitter, Shmoulette::GameObject* child, float distance, double time);
		//virtual void impact(int type);
};
class _ShmouletteExport BulletFactory:public AmmoFactory{
	protected:
		float mVelocity;
		float mAcceleration;
		//DAMAGE_TYPE mDamageType;
		//float mDamages;
		string mImpactFX;
		string mDeathFX;
		string mMaterial;
		string mPython;
	public:
		float getVelocity(){return mVelocity;}
		float getAcceleration(){return mAcceleration;}
		//DAMAGE_TYPE getDamageType(){return mDamageType;}
		//float getDamages(){return mDamages;}
		string getImpactFX(){return mImpactFX;}
		string getDeathFX(){return mDeathFX;}
		//string getImpactSoundFilename(){return mImpactSoundFilename;}

		BulletFactory(string s, XML* xml);

		BulletFactory():AmmoFactory("bullet", NULL){
		}
		virtual Bullet* spawn(MountInfo& mount, MountPosition& pos);
};

}


#endif
