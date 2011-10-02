#include "stdafx.h"

#include "weapon\weaponsupply.h"
namespace Shmoulette{
	WeaponSupply* WeaponSupply::sDefaultWeaponSupply = NULL;

	bool WeaponSupply::requestFire(float value){
		return true;
	}
	bool WeaponSupply::canSupply(float value){
		return true;
	}
	bool WeaponSupply::update(double timeDelta, double parentTime){
		return false;
	}
	float AmmoWeaponSupply::getGageValue(){
		return mNbAmmo;
	}

	AmmoWeaponSupply::AmmoWeaponSupply(int nbAmmo){
		mNbAmmo = nbAmmo;
		mMaxAmmo = nbAmmo;
	}
	bool AmmoWeaponSupply::canSupply(float value){
		if (mNbAmmo > 0){
			return true;
		}
		return false;
	}

	bool AmmoWeaponSupply::requestFire(float value){
		bool rv =canSupply(value); 
		if (rv){
			mNbAmmo--;
		}
		return rv;
	}
	void AmmoWeaponSupply::addAmmo(int nbAmmo){
		mNbAmmo += nbAmmo;
		if (mNbAmmo > mMaxAmmo){
			mNbAmmo  = mMaxAmmo;
		}
	}
	
	EnergyWeaponSupply::EnergyWeaponSupply(float max, float regen){
		mCurrent = max;
		mRegen = regen;
		mMax = max;
	}

	bool EnergyWeaponSupply::canSupply(float nbEnergy){
		if (mCurrent >= nbEnergy){
			return true;
		}else{
			return false;
		}
	}

	
	bool EnergyWeaponSupply::requestFire(float nbEnergy){
		if (canSupply(nbEnergy)){
			mCurrent -= nbEnergy;
			return true;
		}else{
			return false;
		}
	}
	float EnergyWeaponSupply::getGageValue(){
		Util::Log("Energy Gage : "+ts(mCurrent),5);
		return mCurrent/mMax;
	}
	bool EnergyWeaponSupply::update(double timeDelta, double parentTime){
		mCurrent += timeDelta*mRegen; //Level::getSingleton()->getTimeDelta()
		if (mCurrent > mMax){
			mCurrent = mMax;
		}
		return false;

	}
}