#include "stdafx.h"

#include "main.h"
#include "sound.h"
#include "fx\explosion.h"
#include "weapon\weapon.h"
namespace Shmoulette{

	void Bullet::init(BulletFactory* bf){
		Parent::init(bf->getMeshName(), true);
		mType = OBJ_TYPE_LOADED_MESH;
		//setId(bf->getMeshName(), true);
		//setFilename(bf->getMeshName());
		spawn(6, false);
		mLauncherInertia.x = 0;
		mLauncherInertia.y = 0;
		mLauncherInertia.z = 0;
		mSpeed = bf->getVelocity();
		mDeathFX = bf->getDeathFX();
		mImpactFX = bf->getImpactFX();
		mDamagePackage.copyFromDamagePackage(bf->getDamagePackage());
		//mLight = NULL;
		//setCollisionGroup(COLLGROUP_NONE);
	}

	void Bullet::init(){
		Parent::init("bullet", true);
		mType = OBJ_TYPE_LOADED_MESH;
		//mScl.x = 2;
		//mScl.y = 2;
		//mScl.z = 10;
		setId();
		spawn(3, false);
		mLauncherInertia.x = 0;
		mLauncherInertia.y = 0;
		mLauncherInertia.z = 0;
		mSpeed = 60;
		mDeathFX="";
		mImpactFX;
	//	mLight = NULL;

		//setCollisionGroup(COLLGROUP_NONE);
		//Util::Log("Bullet Created");
	}

	Bullet::Bullet():Parent(){
	}
	void Bullet::build(){
		Parent::build();

		//if (!Level::getSingleton()->getSceneManager()->hasLight("particle_light")){
	/*	mLight = Level::getSingleton()->getSceneManager()->createLight("particle_light_"+ts(GameElement::getNextIdCounter()));
			mLight->setType(Ogre::Light::LT_POINT);
			mLight->setDiffuseColour(.1,.2,.5);
			//mLight->setPowerScale(.5);
			//mLight->setAttenuation(1, .005f, 0.0005f, 0.5f); 
			mLight->setAttenuation(80,1,0.0005,0);
			mLight->setCastShadows(false);
			mSceneNode->attachObject(mLight);*/
		//}else{
		//	mLight = NULL;
		//}

		/*if (mBirthSoundFilename != ""){
			mBirthSound = SoundManager::getSingleton()->createSound(mBirthSoundFilename);
		}
		if (mDeathSoundFilename != ""){
			mDeathSound = SoundManager::getSingleton()->createSound(mDeathSoundFilename);
		}*/
		mEntity->setCastShadows(false);
		
		mEntity->setRenderQueueGroup(mLevelSegment->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+3);

	}

	void Bullet::show(){
		Parent::show();
	}

	void Bullet::hit(DamagePackage& pack, Shmoulette::GameObject* hitter, Shmoulette::GameObject* child, float distance, double time){
		/*if (mLight != NULL){
			mSceneNode->detachObject(mLight);
			Level::getSingleton()->getSceneManager()->destroyLight(mLight);
			mLight = NULL;
		}*/

		Parent::hit(pack, hitter, child, distance, time);
		markForTermination();
		
		if (mImpactFX != ""){
			FXObject* fo = new FXObject();
			fo->init(mImpactFX, mSceneNode);
			mLevelSegment->addGameObj(fo);
		}

		/*if (mImpactParticlesFilename!="XML_ERROR"){
			Explosion* exp = new Explosion(getSceneNode()->_getDerivedPosition(),.1,NULL,mImpactParticlesFilename);
			mLevelSegment->addGameObj(exp);
		}*/
	}

	bool Bullet::update(double timeDelta, double parentTime){
		//Util::Log("Bullet Update 1\n");
		Parent::update(timeDelta, parentTime);
	/*		Util::Log("-----");
			Util::Log("Bul  : "+toString(getRX())+"\t\t"+toString(getRY())+"\t\t"+toString(getRZ()));
			Util::Log("Rail : "+toString(getHook()->getRX())+"\t\t"+toString(getHook()->getRY())+"\t\t"+toString(getHook()->getRZ()));
			Util::Log("ObjA : "+
				toString(dbObjectAngleX(objNbGet()))+
				"\t\t"+toString(dbObjectAngleY(objNbGet()))+
				"\t\t"+toString(dbObjectAngleZ(objNbGet())));
			Util::Log("-----");

			float totalAbsolute = getRY() + getHook()->getRY();
			float actualAbsolute = dbObjectAngleY(objNbGet());
			if (totalAbsolute != actualAbsolute){
				int alert = 23;
			}
		();*/
		//if (this->mIFF == IFF_TARGET_ENEMY){

		if (mHooked){
			ZoneInfo* zi = mLevelSegment->getZoneInfo();
			Ogre::Vector3 zoneSize = zi->mAmmoSize.current;

			if (mSceneNode->getPosition().z < 0-zoneSize.z){ markForTermination(); return true; }
			if (mSceneNode->getPosition().z > zoneSize.z){ markForTermination(); return true; }
			if (mSceneNode->getPosition().y < 0-zoneSize.y){ markForTermination(); return true; }
			if (mSceneNode->getPosition().y > zoneSize.y){ markForTermination(); return true; }
			if (mSceneNode->getPosition().x < 0-zoneSize.x){ markForTermination(); return true; }
			if (mSceneNode->getPosition().x > zoneSize.x){ markForTermination(); return true; }
		}
		
			//}

		//Util::Log("Bullet Update 2\n");
		if (parentTime > getTimeDestroy()){
			return true;
		}else{
			move(mSpeed*timeDelta, true);
			mSceneNode->translate(
				mLauncherInertia*timeDelta
				);
			//GameObject* targetHit;
			/*COLLISION_RESULT cr;
			cr = mLevelSegment->collisionTest(this, mIFF);
			if (cr.mOccured){
				cr.mTarget->hit(this, cr.mChild);
				impact(1);
				//targetHit->hit(this, cr.mChild);
				return true;
			}*/
		}
		//Util::Log("Bullet Update 3\n");

		/*mRailPos.z +=1.5;

		setRot(rand(), rand(), rand());*/
		//pitch(30, false);
		//roll(30, false);

		return false;

	}

	BulletFactory::BulletFactory(string s, XML* xml):AmmoFactory(s, xml){
		mMeshName = xml->getString("mesh");
		string mat = xml->getString("material"); 
		if (xml->getError() == XML_OK){
			mMaterial = mat;
		}else{
			mMaterial = "";
		}
		mPython = xml->getString("python");
		if (xml->getError() != XML_OK){
			mPython = "";
		}
		mVelocity = xml->getFloat("velocity");
		mAcceleration = xml->getFloat("acceleration");
		string xmlstring = xml->getString("damagepackage");
		if (xml->getError() != XML_OK){
			throw("Bullet Factory "+s+" requires damagepackage");
		}
		XML dpxml(xmlstring+".xml", "media//damage");
		mDamagePackage.addFromXml(&dpxml);

		mImpactFX = xml->getString("impactfx");
		if (xml->getError() != XML_OK){
			mImpactFX = "";
		}

		mDeathFX = xml->getString("deathfx");
		if (xml->getError() != XML_OK){
			mDeathFX = "";
		}

	}



	Bullet* BulletFactory::spawn(MountInfo& mount, MountPosition& pos){
		MACRO_PROFILER_GET("A", "BulletSpawn")->start();

		Bullet* rv = new Bullet();
		rv->init(this);
		if (mPython != ""){
			rv->setPythonCallback(mPython);
		}

		MACRO_PROFILER_GET("A", "BulletSpawn")->end();
		MACRO_PROFILER_GET("B", "BulletSpawn")->start();

		if (mMaterial != ""){
			rv->addMaterialReplace("",mMaterial,0);
		}
		rv->setHooked(true);//ryumount.obj->isHooked());


		MACRO_PROFILER_GET("B", "BulletSpawn")->end();
		MACRO_PROFILER_GET("C", "BulletSpawn")->start();

		rv->build();
		rv->show();

		MACRO_PROFILER_GET("C", "BulletSpawn")->end();
		MACRO_PROFILER_GET("D", "BulletSpawn")->start();

		Ogre::Matrix4 mat = pos.mBaseTransform;
		rv->getSceneNode()->setPosition(mat.getTrans());
		rv->getSceneNode()->setOrientation(mat.extractQuaternion());

		if (Level::getSingleton()->getCurrentSegment()->getRailType() == RAIL_VERTICAL){
			rv->getSceneNode()->yaw(pos.mXDeflection);
		}else if (Level::getSingleton()->getCurrentSegment()->getRailType() == RAIL_HORIZONTAL){
			rv->getSceneNode()->pitch(pos.mYDeflection);
		}else{
			rv->getSceneNode()->yaw(pos.mXDeflection);
			rv->getSceneNode()->pitch(pos.mYDeflection);
		}

		rv->setOwner(mount.obj);
			
		Level::getSingleton()->getCurrentSegment()->addGameObj(rv);
			
		//rv->setCollisionGroup(mount.obj->getCollisionGroup()|COLLGROUP_BULLET);
		
		if (
			mount.obj->getEntity()->getQueryFlags()|SceneQueryFlag::PLAYER_SHIP || 
			mount.obj->getEntity()->getQueryFlags()|SceneQueryFlag::DRONE_FRIENDLY
			){
			rv->getEntity()->setQueryFlags(SceneQueryFlag::AMMO_FRIENDLY);
		}else{
			rv->getEntity()->setQueryFlags(SceneQueryFlag::AMMO_ENEMY);
		}


		MACRO_PROFILER_GET("D", "BulletSpawn")->end();


		//mVelocity, mAcceleration, mDamages, mDamageType, mImpactParticles, mBirthSound, mDeathSound, mImpactSound
		/*rv->setFilename(mMeshName);
		rv->setVelocity(mVelocity);
		rv->setAcceleration(mAcceleration);
		rv->setDamages(mDamages);
		rv->setDamageType(mDamageType);
		rv->setImpactParticles(mImpactParticles);
		rv->setBirthSound(mBirthSound);
		rv->setImpactSound(mImpactSound);
		rv->setDeathSound(mDeathSound);*/

	//	rv->build();
		return rv;
	}



}