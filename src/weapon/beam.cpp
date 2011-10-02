#include "stdafx.h"

#include "main.h"
#include "sound.h"
#include "fx\explosion.h"
#include "weapon\weapon.h"
#include "weapon\beam.h"

namespace Shmoulette{

	void BeamGO::init(BeamWeaponDBE* dbe){
		Parent::init("BeamWeaponObj", dbe->getMesh(), true);
		this->setAutoManaged(false);
		//string mesh = xml->getString("weapon","mesh");
		//setFilename(mesh);
			
		mBeamWeaponDBE = dbe;
		string s = dbe->getImpactFX();
		if (s != ""){
			mImpactFX = new FX();
			mImpactFX->init(s);
		}else{
			mImpactFX = NULL;
		}

		mCurrentReach = 1;
		mReachSpeed = (mBeamWeaponDBE->getReachLength()-1)/mBeamWeaponDBE->getTimeToReach();
		mRetractSpeed = (mBeamWeaponDBE->getReachLength()-1)/mBeamWeaponDBE->getTimeToRetract();

		mDamagePackage.addFromDamagePackage(dbe->getDamagePackage());
		this->mCastShadows = false;

		setTimeOn(0);
		setTimeOff(FLT_MAX);
		setTimeBuild(0);
		setTimeDestroy(FLT_MAX);
		mLastFrameHit = false;
		mIsFiring = false;
	}
	void BeamGO::hit(DamagePackage& pack, GameObject* hitter, GameObject* forwardFrom, float distance){
		mCurrentReach = distance;
		setLength(distance);
		if (mImpactFX){
			mImpactFX->activate();
			mImpactFX->show();
		}
		mLastFrameHit = true;
	}
	void BeamGO::takeDamage(DamagePackage& dp){
		int g=0;
	}
	void BeamGO::setLength(float l){
		//Ogre::Vector3 s = getNode()->getScale();
		getNode()->setScale(mBeamWeaponDBE->getScale(), mBeamWeaponDBE->getScale(), l);
		if (mImpactFX){
			mImpactFX->setScale(Ogre::Vector3(1,1,1/l));
		}
	}
	void BeamGO::build(){
		mCurrentReach = 1;
		mLastFrameHit = false;
		mIsFiring = false;

		if (!mCollisionStructure->hasChildren()){
			throw("Beam Object should have a collision structure with a child that had 1 ray element.  Please correct this.");
		}
		SceneQueryFlag::SceneQueryFlag f;
		f = this->mParentConnector.mGameObject->getQueryFlags();
		if (f&SceneQueryFlag::DRONE_ENEMY){
			this->setQueryFlags(SceneQueryFlag::AMMO_ENEMY);
		}else{
			this->setQueryFlags(SceneQueryFlag::AMMO_FRIENDLY);
		}
		Parent::build();
		getNode()->setScale(mBeamWeaponDBE->getScale(), mBeamWeaponDBE->getScale(), 1);
		
		if (mImpactFX){
			mImpactFX->setParent(this, "tip");
			mImpactFX->build();
		};
		hide();
	}
	void BeamGO::show(){
		Parent::show();
	}
	void BeamGO::fire(){
		mIsFiring = true;
	}
	void BeamGO::ceaseFire(){
		mIsFiring = false;
	}
	bool BeamGO::update(double timeDelta, double parentTime){
		Parent::update(timeDelta, parentTime);
		if (!mLastFrameHit){
			if (mImpactFX){
				mImpactFX->deactivate();
			}
			double td = timeDelta;//Level::getSingleton()->getTimeDelta();
			/*if (mAnimState)
				mAnimState->addTime(td);*/

			if (mIsFiring){
				if (!isOn()){
					show();
				}
				//mEntity->setVisible(true);
				if (mCurrentReach < mBeamWeaponDBE->getReachLength()){
					mCurrentReach += td*mReachSpeed;
				}else{
					mCurrentReach = mBeamWeaponDBE->getReachLength();
				}
			}else{
				if (isOn()){
					mCurrentReach -= td*mRetractSpeed;
					if (mCurrentReach < 1){
						hide();
						mCurrentReach = 1;
					}
				}
			}
		}else{
			mLastFrameHit = false;
		}
		setLength(mCurrentReach);
		
		return false;
	}

	/*BeamWeapon::BeamWeapon(XML* xml, WeaponSupply* ws):Weapon(xml,ws){
		mSupplyCostPerSecond = mWeaponDBE->getSupplyCost();
		string t = xml->getString("weapon","mesh");

		mBeamObject = new BeamGO(xml);
	}*/

	BeamWeapon::BeamWeapon(BeamWeaponDBE* dbe, WeaponSupply* ws):Weapon(dbe,ws){
		mSupplyCostPerSecond = mWeaponDBE->getSupplyCost();

		mBeamObject = new BeamGO();
		mBeamObject->init(dbe);

	}


	void BeamWeapon::build(){
		//GAME_OBJECT_CHILD c;
		//c.gameObject = mBeamObject;
		//c.bone = mMount.boneLabel;

		mBeamObject->setParentConnector(mMount.obj, mMount.boneLabel, false);
		mBeamObject->build();
		float s = mBeamObject->getNode()->getScale().x;
		mParticleZone.setExtents(Ogre::Vector3(0-s, 0-s, 0), Ogre::Vector3(s, s, 1));

		

		//mMount.obj->addChild(c);
		//mount.obj->attachChild(this);
		//mount.obj->createChild(string type, string bone, string label, string params);

		Parent::build();

		/*Ogre::Entity* ent = mMount.obj->getEntity();
		Ogre::Skeleton* skel = ent->getSkeleton();
		mount.bone = skel->getBone(mount.boneLabel);*/

		//mBeamObject->build();

		//mCollisionElement = new RayCollisionElement(mount.obj, mount.boneLabel);

		//ent->attachObjectToBone(mount.boneLabel, mEntity);

		//mEntity->setVisible(false);

		/*if (mAnim != ""){
			mAnimState = mEntity->getAnimationState(mAnim);
			mAnimState->setEnabled(true);
		}else{
			mAnimState = NULL;
		}*/

	}

	void BeamWeapon::ceaseFire(){
		mBeamObject->ceaseFire();
		Parent::ceaseFire();
		mFX->deactivate();
	}

	void BeamWeapon::_actualFiring(){
		mBeamObject->fire();
		Parent::_actualFiring();
	}
	void BeamWeapon::destroy(){
		mBeamObject->destroy();
		Parent::destroy();
	}

	bool BeamWeapon::update(double timeDelta, double parentTime){
		float supplyCost = mWeaponDBE->getSupplyCost() * timeDelta;//Level::getSingleton()->getTimeDelta();
		if (mIsFiring){
			if (!mWeaponSupply->requestFire(supplyCost)){
				ceaseFire();
			}
		}
		Parent::update(timeDelta, parentTime);
		mBeamObject->update(timeDelta, parentTime);
		mParticleZone.setMaximumZ(mBeamObject->getNode()->getScale().z);
		//Util::Log("PartMax"+ts(mBeamObject->getNode()->getScale().z),0);
		mFX->limitParticleZone(mParticleZone);


#if 0
		if (mEntity->isVisible()){
			Ogre::Ray ray;
			//Ogre::Matrix4 mat = mEntity->getParentNode()->_getFullTransform();

			Ogre::Vector3 raySource = mEntity->getParentNode()->_getDerivedPosition();
			ray.setOrigin(raySource);
			ray.setDirection(mEntity->getParentNode()->_getDerivedOrientation()*Ogre::Vector3::UNIT_Z);

			Ogre::RaySceneQuery* raySceneQuery = Level::getSingleton()->getSceneManager()->createRayQuery(ray);
			raySceneQuery->setSortByDistance(true, 5);

			raySceneQuery->setQueryMask(SceneQueryFlag::ALL_NPC);	
			//raySceneQuery->setQueryMask(SceneQueryFlag::ALL_MATERIAL);
			raySceneQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
			
			Ogre::RaySceneQueryResult& qryResult = raySceneQuery->execute();
			Ogre::RaySceneQueryResult::iterator i = qryResult.begin();

			
			mObstacleReach = mCurrentReach;
			
			bool impact=false;
			Util::Log("Beam Test "+ts(qryResult.size()), 0);
			for (i=qryResult.begin();i!=qryResult.end();i++){
				if (i != qryResult.end() && i->movable)
				{
					//Ogre::Camera* cam = Level::getSingleton()->getCamera();
					Util::Log("Beam Connect A : "+i->movable->getName(), 0);
					if (
						//i->movable != (Ogre::MovableObject*)cam &&
						i->movable != mEntity && 
						i->movable != mount.obj->getEntity()){
						Util::Log("Beam Connect B : "+ts(i->distance) + "/"+ts(mObstacleReach), 0);

						
						/*i->movable->getParentNode()->_getDerivedPosition();
						Level::getSingleton()->getCurrentSegment()->getObjectBySceneNode(i->movable->getParentSceneNode());
						float newDistance = raySource.distance(i->movable->getParentNode()->_getDerivedPosition());*/
						float newDistance = i->distance;
						if (newDistance < mObstacleReach && newDistance > 1){
							mCollisionElement->update();
							
							GameObject* go = Level::getSingleton()->getCurrentSegment()->getObjectBySceneNode(i->movable->getParentSceneNode());

							std::pair<bool, Real> rv = go->getCollisionStructure()->intersects(mCollisionElement);

							if (rv.second < mObstacleReach && rv.second > 1){
								Util::Log("Beam Connect C : "+i->movable->getName(), 0);
								mObstacleReach = rv.second;
								impact = true;
								break;
							}
						}
					}
				}
			}
			//mEntity->getParentNode()->setScale(mScale, mScale, mObstacleReach);
			/*if (mImpactFX){
				mImpactFX->setScale(Ogre::Vector3(1,1,1/mObstacleReach));
				mEntity->getParentNode()->setScale(mScale, mScale, mObstacleReach);
			}*/
		}
#endif

		/*if (mObstacleReach < mCurrentReach){
			mEntity->getParentNode()->setScale(mScale, mScale, mCurrentReach);
		}else{
			mEntity->getParentNode()->setScale(mScale, mScale, mObstacleReach);
		}*/


		return false;
	}

	bool BeamWeapon::postFrameUpdate(double timeDelta, double parentTime){
		mBeamObject->postFrameUpdate(timeDelta, parentTime);
		return false;
	}
	BeamWeaponDBE::BeamWeaponDBE(string it, XML& xml):Parent(it, xml){
		mMesh = xml.getString("weapon","mesh");
		
		string dpxmlfn = xml.getString("weapon","damagepackage")+".xml";
		if (xml.getError() != XML_OK){
			throw("Beam Factory requires damagepackage");
		}
		XML dpxml(dpxmlfn, "media//damage");

		mDamagePackage.addFromXml(&dpxml);
		
		mScale = xml.getFloat("weapon","scale");
		if (xml.getError() != XML_OK){
			mScale = 1;
		}
		mReachLength = xml.getFloat("weapon","reach_length");
		//mTimeAtReach = xml->getFloat("weapon","time_at_reach");
		mTimeToReach = xml.getFloat("weapon","time_to_reach");
		mTimeToRetract = xml.getFloat("weapon","time_to_retract");
		string s = xml.getString("weapon","impact_fx");
		
		if (xml.getError() == XML_OK){
			mImpactFX = s;
		}else{
			mImpactFX = "";
		}
	}
	Weapon* BeamWeaponDBE::spawn(WeaponSupply* ws){
		Weapon* rv = new BeamWeapon(this, ws);
		return rv;
	}

}