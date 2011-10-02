// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___PLAYERSHIP_H___
#define ___DEF___PLAYERSHIP_H___

#include "gameobject.h"
#include "weapon/weapon.h"
#include "weapon/superweapon.h"

namespace Shmoulette{

class _ShmouletteExport PlayerShip:public GameObject{
	typedef GameObject Parent;
protected:
	Ogre::Vector3 mWanted;
	Ogre::Vector3 mSpeed;

	int cursor;

	PlayerDetails mPlayerDetails;

	float mShield;
	//float mShieldRegenerationFactor;
	float mHealth;
	//float mEnergy;

	list<Weapon*>mPrimaryWeapon;
	list<Weapon*>mSecondaryWeapon;

	EnergyWeaponSupply* mPrimaryWeaponSupply;
	AmmoWeaponSupply* mSecondaryWeaponSupply;
	SuperWeaponSupply* mSuperWeaponSupply;

	float mTimeToReset;

	FX* mDeathFX;

	//list<Weapon*>mTertiaryWeapon;
	Weapon* mSuperWeapon;
	int currentWeapon;


	//int mSecondaryAmmo;

	//Path path;

	int mScore;

	float axisBehave(float wanted, float current, float speed);
	int mLockLevel;

	list<Weapon*>::iterator mPrimaryWeaponIterator;
	list<Weapon*>::iterator mSecondaryWeaponIterator;

	float mTimeToFire[3];

	Ogre::SceneNode* mTargetSceneNode;
	Ogre::Entity* mTargetEntity;

	float mInvulnerabilityTimer;
	bool mWeaponsFree;

	
	public:

	EnergyWeaponSupply* getPrimaryWeaponSupply(){return mPrimaryWeaponSupply;}
	AmmoWeaponSupply* getSecondaryWeaponSupply(){return mSecondaryWeaponSupply;}
	SuperWeaponSupply* getSuperWeaponSupply(){return mSuperWeaponSupply;}

	Ogre::Vector3 getSpeed();

	//PlayerShip();
	virtual void init(PlayerDetails& pd);
	~PlayerShip();
	void resetStats();
	void fire(int weaponGroup);
	void ceaseFire(int weaponGroup);
	void lock();
	void unlock();
	void hit(DamagePackage& pack, GameObject* hitter, GameObject* child, float distance, double time);
	virtual void takeDamage(DamagePackage& pack, double time);
	void weaponsHold(){mWeaponsFree = false;}
	void weaponsFree(){mWeaponsFree = true;}

	//int getSecondaryAmmo(){return mSecondaryAmmo;}
	float getShield(){return mShield;}
	float getMaxShield(){return mPlayerDetails.mShield;}
	float getHealth(){return mHealth;}
	float getMaxHealth(){return mPlayerDetails.mArmour;}
	float getEnergyGage(){return mPrimaryWeaponSupply->getGageValue();}
	float getAmmoGage(){return mSecondaryWeaponSupply->getGageValue();}
	/*float getEnergy(){return mPrimaryWeaponSupplymEnergy;}
	float getMaxEnergy(){return mPlayerDetails.mEnergy;}*/
	float getSuperWeaponPercent();
	virtual void specialEvent(string eventDesc);
	int getScore(){return mScore;}
	void score(int points);

	// Input module updates these variables

	bool mPrimaryActivated;
	bool mSecondaryActivated;
	bool mSuperActivated;
	Ogre::Vector2 mDeltaMovement;

	// Input module speaks to playership through these methods

	void activatePrimary();
	void deactivatePrimary();

	void activateSecondary();
	void deactivateSecondary();

	void activateSuper();
	void deactivateSuper();

	void setDeltaMovement(Ogre::Vector2 d);
	Ogre::Vector2 getDeltaMovement(){ return mDeltaMovement; };

	// End of input module bridge


	virtual void build();
	virtual void destroy();


	virtual bool update(double timeDelta, double parentTime);
	virtual bool postFrameUpdate(double timeDelta, double parentTime);

};

}

#endif
