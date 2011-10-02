#include "stdafx.h"

#include "fx\debris.h"

namespace Shmoulette{
	int debrisDirectionToggler = 0;
	using namespace Ogre;
	void Debris::init(XML* xml){
		Parent::init(xml);
	}
	void Debris::init(string mesh, Ogre::Vector3 starting_pos){
		Parent::init("debris_"+mesh, mesh, true);
		/*if (parent!=NULL){
			parent->attach(this);
		}else{
			this->mHooked = true;
		}*/
		/*mType = OBJ_TYPE_LOADED_MESH;
		setFilename(mesh);*/
		mSceneNode->setPosition(starting_pos);
		//setTimeOff(mLevelSegment->getTimeTrack()+1);
		//setTimeDestroy(mLevelSegment->getTimeTrack()+1);
		//setCollisionGroup(COLLGROUP_ALL);
	};

	void Debris::build(){
		switch(Level::getSingleton()->getCurrentSegment()->getRailType()){
			case RAIL_VERTICAL:
				mSpeed.x = Ogre::Math::RangeRandom(-10, 10);
				while(fabs(mSpeed.y) < 20){
					mSpeed.y = Ogre::Math::RangeRandom(-40, 40);
				}
				mSpeed.z = Ogre::Math::RangeRandom(-10, 10);
			break;
			case RAIL_DEPTH:
				mSpeed.x = Ogre::Math::RangeRandom(20, 40);
				mSpeed.y = Ogre::Math::RangeRandom(20, 40);
				mSpeed.z = Ogre::Math::RangeRandom(20, 40);
				if (debrisDirectionToggler==0){
					mSpeed.x*=-1;
				}
				if (debrisDirectionToggler==1){
					mSpeed.y*=-1;
				}
				if (debrisDirectionToggler==2){
					mSpeed.z*=-1;
				}
				if (debrisDirectionToggler==3){
					mSpeed.x*=-1;
					mSpeed.y*=-1;
				}
				if (debrisDirectionToggler==4){
					mSpeed.y*=-1;
					mSpeed.z*=-1;
				}
				if (debrisDirectionToggler==5){
					mSpeed.x*=-1;
					mSpeed.z*=-1;
				}
				debrisDirectionToggler++;
				debrisDirectionToggler=debrisDirectionToggler%7;


			break;

		}
			mSpin.x = Math::RangeRandom(0-(Ogre::Math::PI*2), (Ogre::Math::PI*2));
			mSpin.y = Math::RangeRandom(0-(Ogre::Math::PI*2), (Ogre::Math::PI*2));
			mSpin.z = Math::RangeRandom(0-(Ogre::Math::PI*2), (Ogre::Math::PI*2));
		Parent::build();
	}
	void Debris::destroy(){
		Parent::destroy();
	}
	void Debris::show(){
		Parent::show();
	}
	void Debris::hide(){
		Parent::hide();
	}
	bool Debris::update(double timeDelta, double parentTime){
		Parent::update(timeDelta, parentTime);
		/*mSceneNode->translate(mSpeed*mLevelSegment->getTimeDelta());
		mSceneNode->pitch((Ogre::Radian)mSpin.x*mLevelSegment->getTimeDelta());
		mSceneNode->roll((Ogre::Radian)mSpin.y*mLevelSegment->getTimeDelta());
		mSceneNode->yaw((Ogre::Radian)mSpin.z*mLevelSegment->getTimeDelta());*/

		return false;
	}

}