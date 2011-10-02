#include "stdafx.h"
#include "main.h"
#include "weapon\\weapon.h"
#include "elements\playership.h"
#include "elements\light.h"
#include "world\world.h"
#include <math.h>
#include "elements\camera.h"
#include "util\file.h"
namespace Shmoulette{

	void PlayerShip::resetStats(){
		mShield = mPlayerDetails.mShield;
		mHealth = mPlayerDetails.mArmour;
		mScore = 0;
		currentWeapon = 0;
		mLockLevel = 0;
		mInvulnerabilityTimer = 0;
		mWanted.x = 0;
		mWanted.y = 0;
		mWanted.z = 0;
		mSpeed.x = 0;
		mSpeed.y = 0;
		mSpeed.z = 0;
		mWeaponsFree = true;
		mDeltaMovement = Vector2(0,0);
		setQueryFlags(SceneQueryFlag::SceneQueryFlag::PLAYER_SHIP);
	}

	void PlayerShip::init(PlayerDetails& pd){
		Parent::init(pd.mMesh,false);
		//setCollisionGroup(COLLGROUP_FRIENDLY);
		mRootEntity = NULL;
		int tm=0;
		//mEnergy = pd.mEnergy;
		//mEnergyRegenerationFactor = 2;
		//mMaxEnergy = 100;
		//mSecondaryAmmo = pd.mAmmo;
		mSuperWeapon = NULL;

		mPrimaryActivated = false;
		mSecondaryActivated = false;
		mSuperActivated = false;
		
		mDeathFX = new FX();
		mDeathFX->init(pd.mDeathFX);
		mDeathFX->setParent(mSceneNode);

		mPlayerDetails = pd;

		mPrimaryWeaponSupply = new EnergyWeaponSupply(pd.mEnergy, pd.mEnergyRegen);
		mSecondaryWeaponSupply = new AmmoWeaponSupply(pd.mAmmo);
		mSuperWeaponSupply = new SuperWeaponSupply();

		//typedef Shmoulette::Weapon* (CALLBACK* DLLCallback)();
		list<WeaponInfo>::iterator it;
		for (it = (pd.mWeaponChain[0]).mWeapon.begin();it!=(pd.mWeaponChain[0]).mWeapon.end();it++){
			mPrimaryWeapon.push_back((*it).spawn(this, mPrimaryWeaponSupply));
		}
		for (it = (pd.mWeaponChain[1]).mWeapon.begin();it!=(pd.mWeaponChain[1]).mWeapon.end();it++){
			mSecondaryWeapon.push_back((*it).spawn(this,mSecondaryWeaponSupply));
		}

		if (pd.mWeaponChain.size() > 2){
			WeaponInfo swi = (*pd.mWeaponChain[2].mWeapon.begin());
			mSuperWeapon = swi.spawn(this, WeaponSupply::getDefault());
		}else{
			mSuperWeapon = NULL;
		}

		resetStats();

		mType = OBJ_TYPE_LOADED_MESH;

	};

	PlayerShip::~PlayerShip(){
		while(mPrimaryWeapon.size() > 0){
			delete (*mPrimaryWeapon.begin());
			mPrimaryWeapon.erase(mPrimaryWeapon.begin());
		}
	}

	#define ACC_SPD .2
	#define BRK_SPD 1.2
	#define DEAD_ZONE .1
	#define MAX_SPEED 4


	float PlayerShip::axisBehave(float wanted, float current, float speed){
		float new_speed = speed;
		bool brake = false;
		
		/*if (speed < 0){
			if (current+speed < wanted){
				_cprintf("BRAKING NEGATIVE %f, %f, %f\n", wanted, current, speed);
				brake = true;
			}
		}else{
			if (current+speed > wanted){
				_cprintf("BRAKING POSITIVE %f, %f, %f\n", wanted, current, speed);
				brake = true;
			}
		}*/


		/*if (fabs(current-wanted) < speed*4){ //)DEAD_ZONE
			brake = true;
		}else{
			if (wanted > current){
				if (speed < 0) brake = true;
				//_cprintf("SPEED + %f, %f, %f\n", wanted, current, speed);
				if (current+speed > wanted){
					brake = true;
					//new_speed -= ACC_SPD;
				}else{
					switch (mLevelSegment->getRailType()){
						case RAIL_DEPTH:
							new_speed += ACC_SPD/9;
						break;
						default:
							new_speed += ACC_SPD;
						break;
					}
				}
			}
			if (wanted < current){
				if (speed > 0) brake = true;
				//_cprintf("SPEED - %f, %f, %f\n", wanted, current, speed);
				if (current+speed < wanted){
					//new_speed += ACC_SPD;
					brake = true;
				}else{
					switch (mLevelSegment->getRailType()){
						case RAIL_DEPTH:
							new_speed -= ACC_SPD/9;
						break;
						default:
							new_speed -= ACC_SPD;
						break;
					}
				}
			}
		}
		
		if (brake){
			switch (mLevelSegment->getRailType()){
				case RAIL_DEPTH:
					new_speed /= (BRK_SPD/1.7);
				break;
				default:
					new_speed /= BRK_SPD;
				break;
			}
			
		}*/
		
		/*if (fabs(current-wanted) < DEAD_ZONE){
			new_speed /= BRK_SPD;
		}else{
			if (current < wanted){
				new_speed +=ACC_SPD;
				if (new_speed < 0) new_speed/=BRK_SPD;
				if (new_speed > MAX_SPEED) new_speed = MAX_SPEED;
			}
			if (current > wanted){
				new_speed -=ACC_SPD;
				if (new_speed < 0-MAX_SPEED) new_speed = 0-MAX_SPEED;
				if (new_speed > 0) new_speed/=BRK_SPD;
			}
		}*/

		new_speed = (wanted-current)*(mPlayerDetails.mSpeed); //1.5; // 4 was the last

		return new_speed;

	}
	void PlayerShip::destroy(){
		Level::getSingleton()->getSensorOverlay()->removeEntity(mEntity);

		mDeathFX->deactivate();
		mDeathFX->destroy();

		list<Weapon*>::iterator it;
		for (it = mPrimaryWeapon.begin();it!=mPrimaryWeapon.end();it++){
			(*it)->destroy();
		}
		for (it = mSecondaryWeapon.begin();it!=mSecondaryWeapon.end();it++){
			(*it)->destroy();
		}
		Level::getSingleton()->getSensorOverlay()->removeEntity(mEntity);
		if (mSuperWeapon) mSuperWeapon->destroy();

		Parent::destroy();
	}

	void PlayerShip::build(){
		//resetStats();
		Util::Log("Building PlayerShip...",6);

		list<Weapon*>::iterator it;

		mHooked = true;
		hook();

		Parent::build();
		Level::getSingleton()->getSensorOverlay()->addEntity(mEntity);
	//	mEntity->setRenderQueueGroup(mLevelSegment->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+1);
			//mEntity->getSubEntity(1)->getMaterial()->getTechnique(0)->cre
			//mEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
			
		for (it = mPrimaryWeapon.begin();it!=mPrimaryWeapon.end();it++){
			(*it)->build();
		}
		for (it = mSecondaryWeapon.begin();it!=mSecondaryWeapon.end();it++){
			(*it)->build();
		}

			/*Ogre::Skeleton* skel;
			skel = mEntity->getSkeleton();*/
			
			
		/*mAnimState[ANGEL_ANIM_LEGSWAVER] = mEntity->getAnimationState( "fromtop" );
		mAnimState[ANGEL_ANIM_LEGSWAVER]->setLoop( true );
		mAnimState[ANGEL_ANIM_LEGSWAVER]->setEnabled( true );*/

			/*mAnimState[ANGEL_ANIM_COMBATPOSE] = mEntity->getAnimationState( "CombatPose" );
			mAnimState[ANGEL_ANIM_COMBATPOSE]->setLoop( false );
			mAnimState[ANGEL_ANIM_COMBATPOSE]->setEnabled( true );*/
			

			//mEntity->setDisplaySkeleton(true);

			//mSceneNode->scale(Vector3(.05, .05, .05));
		Util::Log("...done",6);
		//mEntity->setCastShadows(tr);
		//Shmoulette::Light* l = new Shmoulette::Light();
		mPrimaryWeaponIterator = mPrimaryWeapon.begin();
		mSecondaryWeaponIterator = mSecondaryWeapon.begin();
		mTimeToFire[0] = mPlayerDetails.mWeaponChain[0].mTimeBetweenShots;
		mTimeToFire[1] = mPlayerDetails.mWeaponChain[1].mTimeBetweenShots;
		mTimeToFire[2] = mPlayerDetails.mWeaponChain[2].mTimeBetweenShots;
		if (mSuperWeapon) mSuperWeapon->build();
		//mSuperWeapon->testEnergy();
	}

	void PlayerShip::ceaseFire(int weaponGroup){
		if (mWeaponsFree){
			switch(weaponGroup){
				case 0:
					for (mPrimaryWeaponIterator = mPrimaryWeapon.begin();mPrimaryWeaponIterator != mPrimaryWeapon.end();mPrimaryWeaponIterator++){
						(*mPrimaryWeaponIterator)->ceaseFire();
					}
					mPrimaryWeaponIterator = mPrimaryWeapon.begin();
				break;
				case 1:
					for (mSecondaryWeaponIterator = mSecondaryWeapon.begin();mSecondaryWeaponIterator != mSecondaryWeapon.end();mSecondaryWeaponIterator ++){
						(*mSecondaryWeaponIterator)->ceaseFire();
					}
					mSecondaryWeaponIterator = mSecondaryWeapon.begin();
				break;
			}
		}
	}

	void PlayerShip::fire(int weaponGroup){
		//TODO mWeaponsFree is defective in release
		if (mWeaponsFree){
			if (mTimeToFire[weaponGroup] == 0){
				mSceneNode->_update(false, true);
				switch(weaponGroup){
					case 0:{
						if ((*mPrimaryWeaponIterator)->isReady() == true){
							(*mPrimaryWeaponIterator)->fire();
							mPrimaryWeaponIterator++;
							if (mPrimaryWeaponIterator == mPrimaryWeapon.end()){
								mPrimaryWeaponIterator = mPrimaryWeapon.begin();
							}
							mTimeToFire[0] = mPlayerDetails.mWeaponChain[0].mTimeBetweenShots;
						}
					}break;
					case 1:
						(*mSecondaryWeaponIterator)->fire();
						mSecondaryWeaponIterator++;
						if (mSecondaryWeaponIterator == mSecondaryWeapon.end()){
							mSecondaryWeaponIterator= mSecondaryWeapon.begin();
						}
						mTimeToFire[1] = mPlayerDetails.mWeaponChain[1].mTimeBetweenShots;
					break;
					case 2:
						if (mSuperWeapon){
							mSuperWeapon->fire();
						}
				}
			}
		}


		/*list<Weapon*>::iterator it;
		for (it = mPrimaryWeapon.begin();it!=mPrimaryWeapon.end();it++){
			(*it)->fire();
		}*/
		//MACRO_EACHWEAPON(fire());
		//weapon[1]->fire();
		/*for (int i=1;i<3;i++){
			weapon[i]->fire();
		}*/

	}

	Ogre::Vector3 PlayerShip::getSpeed(){
		return mSpeed;
	}

	bool PlayerShip::update(double timeDelta, double parentTime){
		mPrimaryWeaponSupply->update(timeDelta, parentTime);
		mSecondaryWeaponSupply->update(timeDelta, parentTime);
		if (mSuperWeapon) mSuperWeaponSupply->update(timeDelta, parentTime);

		/*mEnergy += mLevelSegment->getTimeDelta()*mPlayerDetails.mEnergyRegen;
		if (mEnergy > mPlayerDetails.mEnergy){
			mEnergy = mPlayerDetails.mEnergy;
		}*/
		if (mInvulnerabilityTimer > timeDelta){ //mLevelSegment->getTimeDelta()
			mInvulnerabilityTimer -= timeDelta;//mLevelSegment->getTimeDelta();
		}else{
			mInvulnerabilityTimer = 0;
		}

		for (int i=0;i<3;i++){
			if (mTimeToFire[i] > timeDelta){//mLevelSegment->getTimeDelta()){
				mTimeToFire[i]-=timeDelta;//mLevelSegment->getTimeDelta();
			}else{
				mTimeToFire[i] =0;
			}
		}

		Util::Log("updating PlayerShip...", 6);

		mShield += timeDelta*mPlayerDetails.mShieldRegen; //mLevelSegment->getTimeDelta()
		if (mShield > mPlayerDetails.mShield){
			mShield = mPlayerDetails.mShield;
		}
		

		Parent::update(timeDelta, parentTime);
		Util::Log("...",6);
		//mAnimState[ANGEL_ANIM_LEGSWAVER]->addTime(mLevelSegment->getTimeDelta());
		Util::Log("...",6);
			/*Util::Log("BEF -----");
			Util::Log("Bef  : "+toString(getRX())+"\t\t"+toString(getRY())+"\t\t"+toString(getRZ()));
			Util::Log("Rail : "+toString(getHook()->getRX())+"\t\t"+toString(getHook()->getRY())+"\t\t"+toString(getHook()->getRZ()));
			Util::Log("ObjA : "+
				toString(dbObjectAngleX(objNbGet()))+
				"\t\t"+toString(dbObjectAngleY(objNbGet()))+
				"\t\t"+toString(dbObjectAngleZ(objNbGet())));*/
			
		//Util::Log("Bef : RailY("+toString(getHook()->getRY())+")\tShipY"+toString(getRY()));

		
		    

	//And the final quaternion is obtained by Qx * Qy * Qz.

		//setRot(0,0,0);
		if (mHealth > 0){
			if (mLockLevel == 0){
				if (mLevelSegment->getCurrentHook() == NULL)
					throw("Ship active with no active rail/hook.");

				float mx = mDeltaMovement.x;//0-Core::getSingleton()->getMouse()->getDX()*500;
				float my = mDeltaMovement.y;//Core::getSingleton()->getMouse()->getDY()*500;
				//Util::Log("Mouse : "+ts(mx)+","+ts(my),0);
				switch (mLevelSegment->getRailType()){
					case RAIL_VERTICAL:
						mWanted.x += (float)mx;
						mWanted.z -= (float)my;
						mWanted.y = 0;
						//mTargetSceneNode->setVisible(false);
					break;
					case RAIL_DEPTH:
						mWanted.x += (float)mx;
						mWanted.z = 0;
						mWanted.y -= (float)my;
		//				mTargetSceneNode->setVisible(true);
					break;
					case RAIL_HORIZONTAL:
						mWanted.x = 0;
						mWanted.z -= (float)mx;
						mWanted.y -= (float)my;
		//				mTargetSceneNode->setVisible(false);
					break;
					default:
						throw("Playship active with no plane type defined");
					break;
				}
				
			}else{
				switch (mLevelSegment->getRailType()){
					case RAIL_VERTICAL:
						mWanted.x = 0;
						mWanted.y = 0;
						mWanted.z = 0;//-mLevelSegment->getZoneSize().z/2; WTF?
					break;
					case RAIL_HORIZONTAL:
						mWanted.x = 0;
						mWanted.y = 0;
						mWanted.z = 0;
					break;
					case RAIL_DEPTH:
						mWanted.x = 0;
						mWanted.y = 0;
						mWanted.z = 0;
					break;
					default:
						mWanted.x = 0;
						mWanted.y = 0;
						mWanted.z = 0;
					break;
				}
		//		mTargetSceneNode->setVisible(false);
			}
		}else{
			mTimeToReset -= timeDelta;//mLevelSegment->getTimeDelta();
			if (mTimeToReset < 0){
				Level::getSingleton()->reset();
			}
		}

		switch (mLevelSegment->getRailType()){
			case RAIL_DEPTH:
				if (mWanted.x < -mLevelSegment->getZoneSize().x) mWanted.x = -mLevelSegment->getZoneSize().x;
				if (mWanted.x > mLevelSegment->getZoneSize().x) mWanted.x = mLevelSegment->getZoneSize().x;
				if (mWanted.y < -mLevelSegment->getZoneSize().y) mWanted.y = -mLevelSegment->getZoneSize().y;
				if (mWanted.y > mLevelSegment->getZoneSize().y) mWanted.y = mLevelSegment->getZoneSize().y;
				if (mWanted.z < -mLevelSegment->getZoneSize().y) mWanted.z = -mLevelSegment->getZoneSize().y;
				if (mWanted.z > mLevelSegment->getZoneSize().y) mWanted.z = mLevelSegment->getZoneSize().y;
			break;
			default:
				if (mWanted.x < -mLevelSegment->getZoneSize().x) mWanted.x = -mLevelSegment->getZoneSize().x;
				if (mWanted.x > mLevelSegment->getZoneSize().x) mWanted.x = mLevelSegment->getZoneSize().x;
				if (mWanted.y < -mLevelSegment->getZoneSize().y) mWanted.y = -mLevelSegment->getZoneSize().y;
				if (mWanted.y > mLevelSegment->getZoneSize().y) mWanted.y = mLevelSegment->getZoneSize().y;
				if (mWanted.z < -mLevelSegment->getZoneSize().z) mWanted.z = -mLevelSegment->getZoneSize().z;
				if (mWanted.z > mLevelSegment->getZoneSize().z) mWanted.z = mLevelSegment->getZoneSize().z;
			break;
		}

		switch (mLevelSegment->getRailType()){
			case RAIL_DEPTH:
	//			mTargetSceneNode->setPosition(mWanted.x, mWanted.y, 0-(mWanted.z*2));
			break;
			default:
			break;
		}
		
		//move(mWanted.z, false);
		//move(mRailPos.x, false);
		Vector3 mPos;
		mPos = mSceneNode->getPosition();

		mSpeed.x = axisBehave(mWanted.x, mPos.x, mSpeed.x);
		mSpeed.y = axisBehave(mWanted.y, mPos.y, mSpeed.y);
		mSpeed.z = axisBehave(mWanted.z, mPos.z, mSpeed.z);

		mPos.x += (mSpeed.x*timeDelta);
		mPos.y += (mSpeed.y*timeDelta);
		mPos.z += (mSpeed.z*timeDelta);

		if (mPos.x < -mLevelSegment->getZoneSize().x){
			mPos.x = -mLevelSegment->getZoneSize().x;
			mWanted.x = mPos.x;
			mSpeed.x = 0;
		}
		if (mPos.x > mLevelSegment->getZoneSize().x){
			mPos.x = mLevelSegment->getZoneSize().x;
			mWanted.x = mPos.x;
			mSpeed.x = 0;
		}
		if (mPos.y < -mLevelSegment->getZoneSize().y){
			mPos.y = -mLevelSegment->getZoneSize().y;
			mWanted.y = mPos.y;
			mSpeed.y = 0;
		}
		if (mPos.y > mLevelSegment->getZoneSize().y){
			mPos.y = mLevelSegment->getZoneSize().y;
			mWanted.y = mPos.y;
			mSpeed.y = 0;
		}
		if (mPos.z < -mLevelSegment->getZoneSize().z){
			mPos.z = -mLevelSegment->getZoneSize().z;
			mWanted.z = mPos.z;
			mSpeed.z = 0;
		}
		if (mPos.z > mLevelSegment->getZoneSize().z){
			mPos.z = mLevelSegment->getZoneSize().z;
			mWanted.z = mPos.z;
			mSpeed.z = 0;
		}


		Util::Log("PlayerShip Pos : "+ts(mPos)+"("+ts(mLevelSegment->getRailType())+")",5);
		setPos(mPos.x, mPos.y, mPos.z);
		mSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);

		//();
		ZoneInfo* zoneInfo = mLevelSegment->getZoneInfo();
		Ogre::Vector3 zoneSize = mLevelSegment->getZoneSize();
		switch (mLevelSegment->getRailType()){
			case RAIL_HORIZONTAL:{
				float xPercent = mPos.z / zoneInfo->mSize.current.z;
				float yPercent = mPos.y / zoneInfo->mSize.current.y;

				mLevelSegment->getActiveCamera()->setHardPitch(
					(yPercent*zoneInfo->mCamTilt.current.y)*(Math::PI/180.0f)
				);
				mLevelSegment->getActiveCamera()->setHardYaw(
					0-(xPercent*zoneInfo->mCamTilt.current.x)*(Math::PI/180.0f)
				);


				/*mLevelSegment->getActiveCamera()->setHardPitch(
					((mPos.y/mLevelSegment->getZoneSize().y)/
					(Math::PI*3))//4
				);
				mLevelSegment->getActiveCamera()->setHardYaw(
					0-((mPos.z/mLevelSegment->getZoneSize().z)/
					(Math::PI*2))//3
				);*/


				mLevelSegment->getActiveCamera()->setHardPos(
					xPercent*zoneInfo->mCamRange.current.x, 
					yPercent*zoneInfo->mCamRange.current.y);

				mSceneNode->pitch((Radian)(((mPos.y-mWanted.y)/(mLevelSegment->getZoneSize().y*4))));//(mWanted.x-mRailPos.x)*5);
				{
				Vector3 absPos = mSceneNode->_getDerivedPosition();
				Real h = mLevelSegment->getWorld()->getHeightAt(absPos.x, absPos.z);
				if (absPos.y < (h+10)){
					mSceneNode->translate(Vector3(0,((h+10)-absPos.y),0), Ogre::Node::TS_WORLD);
					
				}
				}
			}break;
			case RAIL_VERTICAL:{
				float xPercent = mPos.x / zoneInfo->mSize.current.x;
				float zPercent = mPos.z / zoneInfo->mSize.current.z;
				
				mLevelSegment->getActiveCamera()->setHardPitch(
					(zPercent*zoneInfo->mCamTilt.current.y)*(Math::PI/180.0f)
				);
				mLevelSegment->getActiveCamera()->setHardYaw(
					(xPercent*zoneInfo->mCamTilt.current.x)*(Math::PI/180.0f)
				);
				mLevelSegment->getActiveCamera()->setHardPos(
					0-xPercent*zoneInfo->mCamRange.current.x, 
					zPercent*zoneInfo->mCamRange.current.y);

				/*mLevelSegment->getActiveCamera()->setHardPitch(
					0-(
					(mPos.z/mLevelSegment->getZoneSize().z)/
					(Math::PI*20)
					)
				);*/
				//mLevelSegment->getActiveCamera()->setHardPos(0, 30-(mPos.z/20));
				/*mLevelSegment->getActiveCamera()->setHardPitch(
					0-((
					(mPos.z-(0-mLevelSegment->getZoneSize().z))/mLevelSegment->getZoneSize().z)/
					(Math::PI*8)
					)
				);*/
				//roll((mWanted.x-mPos.x)*2, false);//(mWanted.x-mRailPos.x)*5);
			}break;
			case RAIL_DEPTH:
	#ifdef DGSDK
				dbPointObject( objNb, dbObjectPositionX(cursor), dbObjectPositionY(cursor), dbObjectPositionZ(cursor) );
	#endif
				//dbRotateLimb(objNb, mLeftLegLimb, 0, 0, 45);
				//dbRotateLimb(objNb, mRightLegLimb, 0, 0, 45);

				//dbSetObjectToObjectOrientation(objNb, mHook->objNbGet());
				
				mSceneNode->resetOrientation();
				if (mLockLevel == 0){
					Ogre::Vector3 tgt = mWanted;
					tgt.z = mLevelSegment->getZoneSize().z;
					mSceneNode->lookAt(tgt, Ogre::Node::TS_PARENT, Vector3::UNIT_Z);
					roll(0-((mWanted.x-mPos.x)/2));
				}
				

				mLevelSegment->getActiveCamera()->setHardPitch(
					((mPos.y/mLevelSegment->getZoneSize().y)/
					(Math::PI*4))//4
				);
				mLevelSegment->getActiveCamera()->setHardYaw(
					((mPos.x/mLevelSegment->getZoneSize().x)/
					(Math::PI*3))//3
				);
				mLevelSegment->getActiveCamera()->setHardPos(0-mPos.x/2.5, mPos.y/2.5);
				
				
				{
				Vector3 absPos = mSceneNode->_getDerivedPosition();
				Real h = mLevelSegment->getWorld()->getHeightAt(absPos.x, absPos.z);
				if (absPos.y < (h+10)){
					mSceneNode->translate(Vector3(0,((h+10)-absPos.y),0), Ogre::Node::TS_WORLD);
				}
					
				}

				
				//roll(1, false);


			break;
		}

		list<Weapon*>::iterator it;

		for (it = mPrimaryWeapon.begin();it != mPrimaryWeapon.end();it++){
			(*it)->ceaseFire();
		}
		for (it = mSecondaryWeapon.begin();it != mSecondaryWeapon.end();it++){
			(*it)->ceaseFire();
		}

		if (mLockLevel == 0 && mHealth > 0){
			if (mPrimaryActivated){
			//if (Core::getSingleton()->getMouse()->isButtonDown(0)){
				fire(0);
			}
			//if (Core::getSingleton()->getMouse()->isButtonDown(1)){
			if (mSecondaryActivated){
				fire(1);
			}
			if (mSuperActivated){
				if (mSuperWeapon){
					if (mSuperWeapon->isReady()){
						mSuperWeapon->fire();
					}
				}
			}
		}
		for (it = mPrimaryWeapon.begin();it!=mPrimaryWeapon.end();it++){
			(*it)->update(timeDelta, parentTime+timeAlive());
		}
		for (it = mSecondaryWeapon.begin();it!=mSecondaryWeapon.end();it++){
			(*it)->update(timeDelta, parentTime+timeAlive());
		}
		if (mSuperWeapon) mSuperWeapon->update(timeDelta, parentTime+timeAlive());
		//mEntity->getSkeleton()->getRootBone()->get
		//mSceneNode->needUpdate();
		
		
		//skeletonRoot->SetBoundingBox (node->getBoundingBox());
		//mSceneNode->showBoundingBox(true);
		//mEntity->getBoundingBox().set
		
		//node->GetBoundingBox().SetNull(); 	
		
		
		/*Ogre::Bone* bone;
		bone = mEntity->getSkeleton()->getBone("root");
		Ogre::AxisAlignedBox ab = mEntity->getBoundingBox();
		mEntity->getBoundingBox()->setMaximum(0,0,0);
		Ogre::Vector3 bbMin = mBBMin+bone->_getDerivedPosition();
		Ogre::Vector3 bbMax = mBBMax+bone->_getDerivedPosition();
		bbMin.x = 0;
		bbMin.y = 0;
		bbMin.z = 0;

		ab.setMinimum(bbMin);
		ab.setMaximum(bbMax);*/
		
		/*Ogre::Bone* bone;
		bone = mEntity->getSkeleton()->getBone("root");
		mEntity->getMesh()->_setBounds(
			Ogre::AxisAlignedBox(
				mBBMin+bone->_getDerivedPosition(),
				mBBMax+bone->_getDerivedPosition()
			));;*/
		
		
		
		return false;
	};
	bool PlayerShip::postFrameUpdate(double timeDelta, double parentTime){
		list<Weapon*>::iterator it;
		for (it = mPrimaryWeapon.begin();it!=mPrimaryWeapon.end();it++){
			(*it)->postFrameUpdate();
		}
		for (it = mSecondaryWeapon.begin();it!=mSecondaryWeapon.end();it++){
			(*it)->postFrameUpdate();
		}
		if (mSuperWeapon){
			mSuperWeapon->postFrameUpdate();
		}
		Parent::postFrameUpdate(timeDelta, parentTime);
		return false;
	}

	void PlayerShip::lock(){
		mLockLevel++;
	}

	void PlayerShip::unlock(){
		mLockLevel--;
	}

	void PlayerShip::takeDamage(DamagePackage& pack, double time){
		float dam=0;
		//DamagePackage& pack = hitter->getDamagePackage();
		if (pack.getDamageCycle() == DAM_CYCLE_INSTANT){
			dam = pack.getTotalDamage();
		}else if (pack.getDamageCycle() == DAM_CYCLE_CONTINUOUS){
			dam = pack.getTotalDamage()*time;//Level::getSingleton()->getTimeDelta();
		}
		if (mShield > 0){
			mShield-=dam;
			if (mShield < 0){
				mHealth += mShield;
				mShield = 0;
			}
		}else{
			mHealth-=dam;
		}
		if (mHealth <= 0){
			setQueryFlags(SceneQueryFlag::SceneQueryFlag::IMMATERIAL);
			ceaseFire(0);
			ceaseFire(1);
			mDeathFX->setParent(mSceneNode);
			mDeathFX->build();
			mDeathFX->activate();
			mEntity->setVisible(false);
			//mLevelSegment->getActiveCamera()->shake(10000);
			mTimeToReset = 3;
			//throw("You are dead");
		}
	}

	void PlayerShip::hit(DamagePackage& pack, GameObject* hitter, GameObject* child, float distance, double time){
		//DamagePackage& pack = hitter->getDamagePackage();
		if (pack.getDamageCycle() == DAM_CYCLE_INSTANT){
			if (mInvulnerabilityTimer == 0){
				takeDamage(pack, time);
				mLevelSegment->getActiveCamera()->shake();
				mInvulnerabilityTimer = .5;
			}
		}else if (pack.getDamageCycle() == DAM_CYCLE_CONTINUOUS){
				takeDamage(pack, time);
				mLevelSegment->getActiveCamera()->shake(2);
		}else{
			throw("Damage Cycle Unknown "+hitter->getId());
		}
	}
	float PlayerShip::getSuperWeaponPercent(){ 
		if (mSuperWeapon)
			return mSuperWeapon->getReadyPercent();
		else
			return 0;
		//return mSuperWeapon->getEnergy()/mSuperWeapon->getEnergyRequirements();
	}

	void PlayerShip::score(int points){
		mScore += points;
		if (mSuperWeapon)
			mSuperWeapon->addReadyValue(200);
		/*mSuperWeaponEnergy +=10;
		if (mSuperWeaponEnergy > 100) mSuperWeaponEnergy = 100;*/
	}
	void PlayerShip::specialEvent(string eventDesc){throw("Special Event Undefined : "+eventDesc);};

	void PlayerShip::activatePrimary(){
		mPrimaryActivated = true;
	}
	void PlayerShip::deactivatePrimary(){
		mPrimaryActivated = false;
		ceaseFire(0);
	}
	void PlayerShip::activateSecondary(){
		mSecondaryActivated = true;
	}
	void PlayerShip::deactivateSecondary(){
		mSecondaryActivated = false;
		ceaseFire(1);
	}
	void PlayerShip::activateSuper(){
		mSuperActivated = true;
	}
	void PlayerShip::deactivateSuper(){
		mSuperActivated = false;
	}
	void PlayerShip::setDeltaMovement(Ogre::Vector2 d){
		mDeltaMovement = d;
	}

}