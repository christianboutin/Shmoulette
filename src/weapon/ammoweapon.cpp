#include "stdafx.h"

#include "main.h"
#include "weapon\ammoweapon.h"
namespace Shmoulette{
	AmmoWeaponDBE::AmmoWeaponDBE(string id, XML& xml):Parent(id, xml){
		XMLIterator it(&xml);
		it.setElemName("weapon>ammo");
		it.gotoZero();
		while(it.gotoNext() == true){
			WEAPON_AMMO weaponAmmo;
			XML factXml = it.getElem();
			string s = factXml.getString("type");
			if (factXml.getError() == XML_OK){
				weaponAmmo.mAmmoFactory = AmmoFactoryFactory::getSingleton()->getFactory(s);
				
			}else{
				throw("Weapons Require Ammo Factory");
				weaponAmmo.mAmmoFactory = NULL;
			}
			
			//XML ammoXml = it.getElem();
			Ogre::Vector2 v = vector2FromString(factXml.getString("deflection"));
			if (factXml.getError() != XML_OK){
				v = Ogre::Vector2(0,0);
			}

			weaponAmmo.mXDeflection = (Ogre::Radian)(Ogre::Degree)v.x;
			weaponAmmo.mYDeflection = (Ogre::Radian)(Ogre::Degree)v.y;

			weaponAmmo.mDelayFire = factXml.getFloat("delay_fire");
			if (factXml.getError() != XML_OK){
				weaponAmmo.mDelayFire = 0;
			}
			mAmmo.push_back(weaponAmmo);
		}
	}

	Weapon* AmmoWeaponDBE::spawn(WeaponSupply* ws){
		Weapon* rv = new AmmoWeapon(this, ws);
		return rv;
	}

	void AmmoWeapon::setMount(GameObject* obj, string bone){
		Parent::setMount(obj, bone);

		/*for (list<WEAPON_AMMO>::iterator it = mAmmo.begin();it!= mAmmo.end();it++){
			obj->getLevelSegment()->addPreloadEntity((*it).mAmmoFactory->getMeshName());
		}

		for (list<WEAPON_AMMO>::iterator it =  mAmmo.begin();it!=mAmmo.end();it++){
			this->mMount.obj->getLevelSegment()->addPreloadEntity((*it).mAmmoFactory->getMeshName());
		}*/
	}

	AmmoWeapon::AmmoWeapon(AmmoWeaponDBE* dbe, WeaponSupply* ws):Weapon(dbe,ws){
		mAmmoWeaponDBE = dbe;

		/*XMLIterator it(xml);
		it.setElemName("weapon>ammo");
		it.gotoZero();
		while(it.gotoNext() == true){
			WEAPON_AMMO weaponAmmo;
			XML factXml = it.getElem();
			string s = factXml.getString("type");
			if (factXml.getError() == XML_OK){
				weaponAmmo.mAmmoFactory = AmmoFactoryFactory::getSingleton()->getFactory(s);
				
			}else{
				throw("Weapons Require Ammo Factory");
				weaponAmmo.mAmmoFactory = NULL;
			}
			
			//XML ammoXml = it.getElem();
			Ogre::Vector2 v = vector2FromString(factXml.getString("deflection"));
			if (factXml.getError() != XML_OK){
				v = Ogre::Vector2(0,0);
			}

			weaponAmmo.mXDeflection = (Ogre::Radian)(Ogre::Degree)v.x;
			weaponAmmo.mYDeflection = (Ogre::Radian)(Ogre::Degree)v.y;

			weaponAmmo.mDelayFire = factXml.getFloat("delay_fire");
			if (factXml.getError() != XML_OK){
				weaponAmmo.mDelayFire = 0;
			}
			mAmmo.push_back(weaponAmmo);
		}*/
	}
	void AmmoWeapon::fire(){
		Parent::fire();
	}

	void AmmoWeapon::_actualFiring(){
		Parent::_actualFiring();
		mVolley = mAmmoWeaponDBE->getAmmo();
		mVolleyTime = 0;
	}


	void AmmoWeapon::ceaseFire(){
		mIsFiring = false;
	}
	bool AmmoWeapon::update(double timeDelta, double parentTime){
		bool rv = Parent::update(timeDelta, parentTime);

		mVolleyTime += timeDelta;//Level::getSingleton()->getCurrentSegment()->getTimeDelta();
		if (mVolley.size() > 0){
			while((*mVolley.begin()).mDelayFire < mVolleyTime){
				MACRO_PROFILER_GET("A", "AmmoFire")->start();
				list<WEAPON_AMMO>::iterator it = mVolley.begin(); // Less complex to write afterwards than always mVolley.begin().

				Ammo* ammo;
				MountPosition mt(getMountTransform(), (*it).mXDeflection,(*it).mYDeflection);
				ammo = (*it).mAmmoFactory->spawn(mMount, mt);

				MACRO_PROFILER_GET("A", "AmmoFire")->end();
				MACRO_PROFILER_GET("B", "AmmoFire")->start();

				if (mMount.mInertia.x == FLT_MAX){
					ammo->setLauncherInertia(mMount.obj->getInertia());
				}else{
					ammo->setLauncherInertia(mMount.mInertia);
				}
				
				MACRO_PROFILER_GET("B", "AmmoFire")->end();
				MACRO_PROFILER_GET("C", "AmmoFire")->start();
				SceneQueryFlag::SceneQueryFlag sq = mMount.obj->getQueryFlags();

				if (sq&SceneQueryFlag::SceneQueryFlag::DRONE_ENEMY){
					ammo->setQueryFlags(SceneQueryFlag::SceneQueryFlag::AMMO_ENEMY);
				}else if (sq&SceneQueryFlag::SceneQueryFlag::DRONE_FRIENDLY){
					ammo->setQueryFlags(SceneQueryFlag::SceneQueryFlag::AMMO_FRIENDLY);
				}else if (sq&SceneQueryFlag::SceneQueryFlag::PLAYER_SHIP){
					ammo->setQueryFlags(SceneQueryFlag::SceneQueryFlag::AMMO_FRIENDLY);
				}else if (sq&SceneQueryFlag::SceneQueryFlag::INANIMATE){
					ammo->setQueryFlags(SceneQueryFlag::SceneQueryFlag::INANIMATE);
				}else{
					ammo->setQueryFlags(SceneQueryFlag::SceneQueryFlag::IMMATERIAL);
				}

				ammo->overrideQueryFlags();

				MACRO_PROFILER_GET("C", "AmmoFire")->end();
				MACRO_PROFILER_GET("D", "AmmoFire")->start();
				
				mVolley.pop_front();
				if (mVolley.size() == 0){
					break;
				}

				MACRO_PROFILER_GET("D", "AmmoFire")->end();

			}
		}else{
			if (mVolleyTime > .06){
			}
			/*mTimeToReady -= Level::getSingleton()->getCurrentSegment()->getTimeDelta();
			if (mTimeToReady < 0){
				mTimeToReady = 0;
			}*/
		}
		
		return rv;
	}
}