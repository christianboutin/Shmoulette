#include "stdafx.h"

#include "weapon\superweapon.h"


namespace Shmoulette{
	namespace SuperWeapons{
		SuperWeapon::SuperWeapon(WeaponDBE* dbe, WeaponSupply* ws):Weapon(dbe, ws){
			mEnergyRequirements = 200;
			reset();
		}
		void SuperWeapon::addEnergy(float add){
			if (mState == STATE_STANDBY){
				if (mEnergy < mEnergyRequirements - add){
					mEnergy += add;
				}else{
					mEnergy = mEnergyRequirements;
					setState(STATE_READY);
				}
			}
		}

		void SuperWeapon::fire(){
			if (isReady()){
				setState(STATE_FIRING);
			}
			mEnergy = 0;
		}
		void SuperWeapon::reset(){
			mState = STATE_STANDBY;
			mEnergy = 0;
		}

		void SuperWeapon::testEnergy(){
			if (mEnergy == mEnergyRequirements){
				setState(STATE_READY);
			}
		}

		bool SuperWeapon::setState(SUPERWEAPON_STATE state){
			if (mState != state){
				mState = state;
				return true;
			}
			mState = state;
			return false;
			
		}
		float SuperWeapon::getReadyPercent(){
			return mEnergy/mEnergyRequirements;
		}
		void SuperWeapon::addReadyValue(float f){
			if (mState != STATE_FIRING){
				if (mEnergy < mEnergyRequirements){
					mEnergy += f;
					if (mEnergy >= mEnergyRequirements){
						mEnergy = mEnergyRequirements;
						setState(STATE_READY);
					}
				}
			}
		}


		void DestroyAll::fire(){
			mHitList = Level::getSingleton()->getCurrentSegment()->getActiveDronesList();
			if (mHitList.size() != 0){
				mCurrentTarget = *mHitList.begin();
				mState = STATE_FIRING;
			}
		}
		bool DestroyAll::update(double timeDelta, double parentTime){
			DamagePackage pack;
			pack.setDamageCycle(DAM_CYCLE_INSTANT);
			pack.setDamageTotal(DAM_TOTAL_ADD);
			pack.addDamage(DAM_TYPE_CONCUSSION, FLT_MAX);
			if (mState == STATE_FIRING){
				if (mCurrentTarget->isAlive()){
					mCurrentTarget->hit(pack, NULL, NULL, FLT_MAX, timeDelta);
				}else{
					mHitList = Level::getSingleton()->getCurrentSegment()->getActiveDronesList();
					if (mHitList.size() == 0){
						mState = STATE_STANDBY;
					}else{
						mCurrentTarget = *mHitList.begin();
					}

					/*mIt++;
					if (mIt == mHitList.end()){
						mState == SWSTATE_STANDBY;
						mHitList.empty();
					}*/
				}
			}
			return false;
		}
	}
}