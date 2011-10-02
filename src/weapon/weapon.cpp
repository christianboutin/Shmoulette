#include "stdafx.h"

#include "main.h"
#include "weapon\weapon.h"
#include "weapon\weapondb.h"

namespace Shmoulette{

	vector<XmlDocInstance*> WeaponInfo::sXmlDocInstance;

	
	WeaponDBE::WeaponDBE(string id, XML& xml){
		mId = id;
		mReloadTime = xml.getFloat("weapon","reload_time");
		if (xml.getError() != XML_OK){
			throw("Weapon requires a reload_time field :"+xml.getContent());
		}
		mSupplyCost = xml.getFloat("weapon","supply_cost");
		if (xml.getError() != XML_OK){
			throw("Weapon requires a supply_cost :"+xml.getContent());
		}
		mFX = xml.getString("weapon","fx");
		if (xml.getError() != XML_OK){
			mFX = "";
		}


	}

	WeaponDBE::~WeaponDBE(){
	}

	/*Weapon* WeaponDBE::spawn(GameObject* launcher, WeaponSupply* ws){
		Weapon* rv;
		if (mType == "dll"){
			typedef Shmoulette::Weapon* (CALLBACK* DLLCallback)(XML* xml, WeaponSupply* ws);
			#ifdef _DEBUG
				string name = mSubtype+"d";
				HMODULE weaponModule = LoadLibrary(name.c_str());
			#else
				HMODULE weaponModule = LoadLibrary(mSubtype.c_str());
			#endif
			DLLCallback dllFunc = (DLLCallback)::GetProcAddress(weaponModule, "getInstance");
			if (dllFunc == NULL){
				throw("Weapons DLL must have getInstance function");
			}
			string s=mXml->getContent();
			rv = dllFunc(mXml,ws);
		}else if (mType == "internal"){
			if (mSubtype == "ammo"){
				rv = new AmmoWeapon(mXml,ws);
			}else if (mSubtype == "beam"){
				rv = new BeamWeapon(mXml,ws);
			}else{
				throw("Subtype : "+mSubtype+" not recognized");
			}
		}else{
			throw("Type : "+mType+" not recognized");
		}
		rv->setMount(launcher, mMount);
		return rv;
	}*/

	//WeaponDB* Weapon::mWeaponDB = NULL;



	Weapon::Weapon(WeaponDBE* dbe, WeaponSupply* ws){
		//mLevelSegment = Level::getSingleton()->getCurrentSegment();
		mWeaponDBE = dbe;
		mWeaponSupply = ws;
		mTimeToReady = 0;
		mIsFiring = false;
		mIsBuild = false;


		string fxName = dbe->getFX();
		if (fxName != ""){
			mFX = new FX();
			mFX->init(fxName);
		}else{
			mFX = NULL;
		}
	}

	
	void Weapon::_actualFiring(){
		if (mFX){
			mFX->activate();
		}
		mTimeToReady = mWeaponDBE->getReloadTime();
	}

	void Weapon::fire(){
//		if (mAmmoFactory != NULL){
		
		/*list<WEAPON_AMMO>::iterator it;
		for (it=mAmmo.begin();it!=mAmmo.end();it++){
			Ammo* ammo;
			ammo = (*it).mAmmoFactory->spawn();
			ammo->setHooked(true);
			ammo->build();

			Ogre::Matrix4 mat = getMountTransform();
			ammo->getSceneNode()->setPosition(mat.getTrans());
			ammo->getSceneNode()->setOrientation(mat.extractQuaternion());

			if (Level::getSingleton()->getCurrentSegment()->getRailType() == RAIL_VERTICAL){
				ammo->getSceneNode()->yaw((*it).mXDeflection);
			}else if (Level::getSingleton()->getCurrentSegment()->getRailType() == RAIL_HORIZONTAL){
						ammo->getSceneNode()->pitch((*it).mYDeflection);
			}else{
				ammo->getSceneNode()->yaw((*it).mXDeflection);
				ammo->getSceneNode()->pitch((*it).mYDeflection);
			}

			//ammo->getSceneNode()->pitch((Ogre::Radian)(0-Ogre::Math::PI/2));

			ammo->setOwner(mount.obj);
			
			Level::getSingleton()->getCurrentSegment()->addGameObj(ammo);
			
			ammo->setCollisionGroup(this->mount.obj->getCollisionGroup()|COLLGROUP_BULLET);
		}
//		}*/
		mIsFiring = true;

		Util::Log("Resetting Weapon");
	}

	void Weapon::ceaseFire(){
		mIsFiring = false;
	}

	Ogre::Matrix4 Weapon::getMountTransform(){
		
		//nodeB is rail
		Ogre::Node* nodeB;
		if (mMount.obj->isHooked() || 1 ){ // Now Mount Transform is always in relation to the current hook
			mMount.obj->forceUpdate();
			Level::getSingleton()->getCurrentSegment()->getCurrentHook()->forceUpdate();

			nodeB = Level::getSingleton()->getCurrentSegment()->getCurrentHook()->getSceneNode();
		}else{
			nodeB = Level::getSingleton()->getSceneManager()->getRootSceneNode();
		}

		Ogre::Matrix4 matrix = nodeB->_getFullTransform().inverse();
		Ogre::Matrix4 transform = matrix * (mMount.obj->getNode()->_getFullTransform()*mMount.bone->_getFullTransform());

		Ogre::Vector3 v = transform.getTrans();
		return transform;
		//return transform.getTrans();
		//newOri = transform.extractQuaternion(); 
		//return Vector3(0,0,0);

	}

	bool Weapon::update(double timeDelta, double parentTime){
		if (mFX){
			mFX->update(timeDelta, parentTime);
		}
		mTimeToReady -= Level::getSingleton()->getCurrentSegment()->getTimeDelta();
		if (mTimeToReady < 0){
			mTimeToReady = 0;
		}
		if (mIsFiring){
			if (mTimeToReady == 0){
				if (mWeaponSupply->requestFire(mWeaponDBE->getSupplyCost()) == true){
					_actualFiring();
				}
			}
		}

		return false;
	}

	float Weapon::getReadyPercent(){
		return 1.0f-(mTimeToReady/mWeaponDBE->getReloadTime());
	}
	void Weapon::addReadyValue(float f){
	}



	void Weapon::build(){
		Ogre::Entity* ent = mMount.obj->getEntity();
		Ogre::Skeleton* skel = ent->getSkeleton();
		if (skel == NULL){
			throw("Trying to attach a weapon to a mesh without a skeleton" +mMount.obj->getId());
		}
		mMount.bone = skel->getBone(mMount.boneLabel);
		
		if (mFX){
			mFX->setParent(mMount.obj, mMount.boneLabel);
			mFX->build();
		};
		mTimeToReady = 0;
		mIsFiring = false;
		mIsBuild = true;

	};

	void Weapon::destroy(){
		if (mFX){
			mFX->destroy();
		};
		mMount.bone = NULL;
		mIsBuild = false;

	};
	
	WeaponInfo::WeaponInfo(XML* xml){
		/*mDll = xml->getString("dll");
		if (xml->getError() != XML_OK){
			mDll = "";
		}
		
		mType = xml->getString("type");*/
		string filename = xml->getString("xml");
		if (filename=="" || xml->getError() != XML_OK){
			throw("Weapon has no name? "+xml->getContent());
		}
		mWeaponDBE = WeaponDB::getSingleton().get(filename);

		//mXmlFilename = xml->getString("xml");
		mMount = xml->getString("mount");
		mInertia = vector3FromString(xml->getString("inertia"));
		if (xml->getError() != XML_OK){
			mInertia = Ogre::Vector3 (0,0,0);
		}
		/*string hookedString = xml->getString("hooked");
		if (xml->getError() != XML_OK){
			mHooked = true;
		}else{
			mHooked = boolFromString(hookedString);
			
		}*/

		//WeaponInfo::addXmlInstance(mXml);
	}
	WeaponInfo::WeaponInfo(const WeaponInfo& source){
		//mType = source.mType;
		//mSubtype = source.mSubtype;
		//mXmlFilename = source.mXmlFilename;
		mMount = source.mMount;
		mWeaponDBE = source.mWeaponDBE;
		mInertia = source.mInertia;
//	mHooked = source.mHooked;
		//mXml = source.mXml;
		//WeaponInfo::addXmlInstance(mXml);
	}

	void WeaponInfo::addXmlInstance(XML* xml){
		for (vector<XmlDocInstance*>::iterator it = sXmlDocInstance.begin();it!=sXmlDocInstance.end();it++){
			if ((*it)->getInstance() == xml){
				(*it)->addInstance();
				return;
			}
		}
		sXmlDocInstance.push_back(new XmlDocInstance(xml));
	}

	bool WeaponInfo::removeXmlInstance(XML* xml){
		for (vector<XmlDocInstance*>::iterator it = sXmlDocInstance.begin();it!=sXmlDocInstance.end();it++){
			if ((*it)->getInstance() == xml){
				if ((*it)->removeInstance()){
					delete (*it);
					sXmlDocInstance.erase(it);
					return true;
				}
				return false;
			}
		}
		return false;
	}

	WeaponInfo::~WeaponInfo(){
		/*if (WeaponInfo::removeXmlInstance(mXml)){
			delete mXml;
		}*/
	}

	Weapon* WeaponInfo::spawn(Shmoulette::GameObject* launcher, WeaponSupply* ws){
		Weapon* rv = mWeaponDBE->spawn(ws);
		rv->setMount(launcher, mMount);
		//rv->setHooked(mHooked);
		rv->setInertia(mInertia);
		return rv;
	}


}