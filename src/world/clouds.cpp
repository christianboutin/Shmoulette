#include "stdafx.h"

#include "main.h"
#include "world\clouds.h"

namespace Shmoulette{
	#define CLOUD_FADE_SPEED 4

	void CloudZone::init(XML* xml){
		Parent::init(xml);
		mDensity = 20;
		mMinSpeed.x = -20;
		mMinSpeed.y = -160;
		mMaxSpeed.x = +20;
		mMaxSpeed.y = -40;
		mMinSize.x = 100;
		mMinSize.y = 100;
		mMaxSize.x = 500;
		mMaxSize.y = 500;
		mZoneSize.x = 300;
		mZoneSize.y = 600;
		mInfiniteLayerMaterial = "Examples/RustySteel";
		mType = OBJ_TYPE_PARTICLE;
		//setCollisionGroup(COLLGROUP_ALL);
//		mSolid = false;

		string s = xml->getString("material");
		if (xml->getError() != XML_OK) throw("Missing Cloud Zone Parameter : material");
		setMaterial(s);
		setInfiniteLayerMaterial(xml->getString("infinitelayer"));
		if (xml->getError() != XML_OK){
			setCreateInfiniteLayer(false);
		}else{
			setCreateInfiniteLayer(true);
		}
		setDensity(xml->getInt("density"));
		if (xml->getError() != XML_OK) throw("Missing Cloud Zone Parameter : density");

		{
			Ogre::Vector3 v;
			v = vector3FromString(xml->getString("minspeed"));
			if (xml->getError() != XML_OK) throw("Missing Cloud Zone Parameter : minspeed");
			setMinSpeed(v.x, v.y, v.z);
			v = vector3FromString(xml->getString("maxspeed"));
			if (xml->getError() != XML_OK) throw("Missing Cloud Zone Parameter : maxspeed");
			setMaxSpeed(v.x, v.y, v.z);
			v = vector3FromString(xml->getString("zonesize"));
			if (xml->getError() != XML_OK) throw("Missing Cloud Zone Parameter : zonesize");
			setZoneSize(v.x, v.y, v.z);
			v = vector3FromString(xml->getString("minsize"));
			if (xml->getError() != XML_OK) throw("Missing Cloud Zone Parameter : minsize");
			setMinSize(v.x, v.y,v.z);
			v = vector3FromString(xml->getString("maxsize"));
			if (xml->getError() != XML_OK) throw("Missing Cloud Zone Parameter : maxsize");
			setMaxSize(v.x, v.y,v.z);
		}

		mFade = false;
		mSpin = true;
	}

	void CloudZone::build(){
		Parent::build();
		{
			Plane p;
			p.normal = Vector3::UNIT_Y;
			p.d = 1;
			MeshManager::getSingleton().createPlane(
				getId()+"[CloudPlane]", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				p, 1, 1, 1, 1, true, 1, 1, 1, Vector3::UNIT_Z);
		}

		/*mSceneNode = Level::getSingleton()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		mSceneNode->translate(getX(), getY(), getZ(), Node::TS_WORLD);*/

		for (int i=0;i<mDensity;i++){
			CLOUD_PARTICLE p;

			p.mEntity = Level::getSingleton()->getSceneManager()->createEntity(getId()+"("+ts(i)+")", getId()+"[CloudPlane]");
	        p.mEntity->setMaterialName(mMaterialName);
			p.mEntity->setQueryFlags(SceneQueryFlag::IMMATERIAL);
			
			
			p.mSceneNode = mSceneNode->createChildSceneNode();
			p.mSceneNode->attachObject(p.mEntity);
			/*p.mSceneNode->scale(
				Math::RangeRandom(mMinSize.x, mMaxSize.x),
				1, 
				Math::RangeRandom(mMinSize.y, mMaxSize.y)
				);
			p.mSceneNode->translate(
				Math::RangeRandom(0.0f-(mZoneSize.x/2.0f), (mZoneSize.x/2.0f)),
				//p.mSceneNode->getScale().x/(mMaxSize.x/5), 
				i/20, 
				Math::RangeRandom(0.0f-(mZoneSize.y/2.0f), (mZoneSize.y/2.0f)),
				Node::TS_WORLD
				);
			p.mSpeed.x = Math::RangeRandom(mMinSpeed.x, mMaxSpeed.x);
			p.mSpeed.y = Math::RangeRandom(mMinSpeed.y, mMaxSpeed.y);*/
			p.mMaterial = static_cast<Ogre::MaterialPtr>(
				MaterialManager::getSingleton().getByName(mMaterialName)
			)->clone(mMaterialName+"_"+getId()+"_"+ts(i));
			p.mEntity->setMaterialName(mMaterialName+"_"+getId()+"_"+ts(i));
			p.mMaterialPass = p.mMaterial->getTechnique(0)->getPass(0);
			p.mColour = p.mMaterialPass->getDiffuse();
			p.mColour.a = 1;
			p.mMaterialPass->setDiffuse(p.mColour);
			mCloudParticle.push_back(p);

			//MaterialManager::getSingleton().get.getByName(mMaterial)
		}
		if (mCreateInfiniteLayer){

	        Plane iP;
			iP.normal = Vector3::UNIT_Y;
			iP.d = 1;
			MeshManager::getSingleton().createPlane(
			getId()+"[InfiniteCloudPlane]", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
            iP, 50, 50, 10, 10, true, 1, 50, 50, Vector3::UNIT_Z);

			mInfiniteLayer.mEntity = Level::getSingleton()->getSceneManager()->createEntity(getId()+"[InfiniteCloudPlane]", getId()+"[InfiniteCloudPlane]");
			mInfiniteLayer.mEntity->setMaterialName(this->mInfiniteLayerMaterial);
			mInfiniteLayer.mEntity->setQueryFlags(SceneQueryFlag::IMMATERIAL);

			
			mInfiniteLayer.mSceneNode = mSceneNode->createChildSceneNode();
			mInfiniteLayer.mSceneNode->attachObject(mInfiniteLayer.mEntity);
			mInfiniteLayer.mSceneNode->scale(
				mZoneSize.x,
				1, 
				mZoneSize.y
				);
			mInfiniteLayer.mSceneNode->translate(
				0,
				-0.1, 
				0,
				Node::TS_WORLD
				);
			if (mMinSpeed.x < 0){
				if (mMaxSpeed.x > 0){
					mInfiniteLayer.mSpeed.x = 0;
				}else{
					mInfiniteLayer.mSpeed.x = mMaxSpeed.x;
				}
			}else{
				mInfiniteLayer.mSpeed.x = mMinSpeed.x;
			}
			if (mMinSpeed.y < 0){
				if (mMaxSpeed.y > 0){
					mInfiniteLayer.mSpeed.y = 0;
				}else{
					mInfiniteLayer.mSpeed.y = mMaxSpeed.y;
				}
			}else{
				mInfiniteLayer.mSpeed.y = mMinSpeed.y;
			}
		}else{
			mInfiniteLayer.mSceneNode = NULL;
		}
		mPreviousPosition.x = FLT_MAX;
		mPreviousPosition.y = FLT_MAX;
	}

	void CloudZone::destroy(){


		list<CLOUD_PARTICLE>::iterator imCloudParticle;

		if (mCreateInfiniteLayer){
			Level::getSingleton()->getSceneManager()->destroyEntity(mInfiniteLayer.mEntity);
			mInfiniteLayer.mSceneNode->getParentSceneNode()->removeAndDestroyChild(mInfiniteLayer.mSceneNode->getName());
			MeshManager::getSingleton().remove(MeshManager::getSingleton().getByName(getId()+"[InfiniteCloudPlane]"));
		}

		while(mCloudParticle.size() > 0){
			CLOUD_PARTICLE p = *mCloudParticle.begin();
			p.mSceneNode->detachObject(p.mEntity);
			Level::getSingleton()->getSceneManager()->destroyEntity(p.mEntity);
			mSceneNode->removeAndDestroyChild(p.mSceneNode->getName());
			MaterialManager::getSingleton().remove(p.mMaterial->getName());
			mCloudParticle.pop_front();
		}

		MeshManager::getSingleton().remove(MeshManager::getSingleton().getByName(getId()+"[CloudPlane]"));

		Parent::destroy();
	}

	void CloudZone::show(){
		Parent::show();
		Ogre::Matrix4 t = mSceneNode->_getFullTransform();
		mPreviousPosition = t.getTrans();
		/*mPreviousPosition.x = mSceneNode->_getDerivedPosition().x;
		mPreviousPosition.y = mSceneNode->_getDerivedPosition().y;
		mPreviousPosition.z = mSceneNode->_getDerivedPosition().z;*/

		list<CLOUD_PARTICLE>::iterator it;
		int i=0;

		for (it = mCloudParticle.begin();it!=mCloudParticle.end();it++){
			(*it).mSceneNode->setScale(
				Math::RangeRandom(mMinSize.x, mMaxSize.x),
				1,
				Math::RangeRandom(mMinSize.y, mMaxSize.y)
				);
			(*it).mSceneNode->setPosition(
				Math::RangeRandom(0.0f-(mZoneSize.x/2.0f), (mZoneSize.x/2.0f)),
				Math::RangeRandom(0.0f-(mZoneSize.y/2.0f), (mZoneSize.y/2.0f)),
				Math::RangeRandom(0.0f-(mZoneSize.z/2.0f), (mZoneSize.z/2.0f))//,
				//Node::TS_WORLD
				);
			(*it).mSpeed.x = Math::RangeRandom(mMinSpeed.x, mMaxSpeed.x);
			(*it).mSpeed.y = Math::RangeRandom(mMinSpeed.y, mMaxSpeed.y);
			(*it).mSpeed.z = Math::RangeRandom(mMinSpeed.z, mMaxSpeed.z);
			if (mSpin){
				(*it).mSpin.x = Math::RangeRandom(0-Ogre::Math::PI, Ogre::Math::PI);
				(*it).mSpin.y = Math::RangeRandom(0-Ogre::Math::PI, Ogre::Math::PI);
				(*it).mSpin.z = Math::RangeRandom(0-Ogre::Math::PI, Ogre::Math::PI);
			}else{
				(*it).mSpin.z = (*it).mSpin.y = (*it).mSpin.x = 0;
			}
			i++;
		}

	}
	void CloudZone::hide(){
		Parent::hide();
	}
	

	bool CloudZone::update(double timeDelta, double parentTime){
		Vector3 frameDelta;
		Ogre::Matrix4 t = mSceneNode->_getFullTransform();
		Ogre::Vector3 snw = t.getTrans();
		
		/*mPreviousPosition.x = mSceneNode->_getDerivedPosition().x;
		mPreviousPosition.y = mSceneNode->_getDerivedPosition().y;
		mPreviousPosition.z = mSceneNode->_getDerivedPosition().z;*/


		/*float snwx = mSceneNode->_getDerivedPosition().x;
		float snwy = mSceneNode->_getDerivedPosition().y;
		float snwz = mSceneNode->_getDerivedPosition().z;*/
		/*if (snw.x != snw.x || snw.x == FLT_MAX){
//		if (frameDelta.x != frameDelta.x){
			frameDelta.x = 0;
		//}
		//if (frameDelta.y != frameDelta.y){
			frameDelta.y = 0;
			frameDelta.z = 0;
		//}
		}else{*/
			frameDelta.x = mPreviousPosition.x - snw.x;
			frameDelta.y = mPreviousPosition.y - snw.y;
			frameDelta.z = mPreviousPosition.z - snw.z;
		//}

		/*if (frameDelta.x > mZoneSize.x){
			frameDelta.x = mZoneSize.x;
		}
		if (frameDelta.x < 0-mZoneSize.x){
			frameDelta.x = 0-mZoneSize.x;
		}
		if (frameDelta.y > mZoneSize.y){
			frameDelta.y = mZoneSize.y;
		}
		if (frameDelta.y < 0-mZoneSize.y){
			frameDelta.y = 0-mZoneSize.y;
		}*/
		
		Parent::update(timeDelta, parentTime);

		/*Util::Log("Cloud Layer Position "+
			ts(mSceneNode->_getDerivedPosition().x)+","+
			ts(mSceneNode->_getDerivedPosition().y)+","+
			ts(mSceneNode->_getDerivedPosition().z)+"\n");*/
		
		if (mInfiniteLayer.mSceneNode != NULL){
			float tx = frameDelta.x+mInfiniteLayer.mSpeed.x*timeDelta;//mLevelSegment->getTimeDelta();
			float ty = frameDelta.y+mInfiniteLayer.mSpeed.y*timeDelta;
			float tz = frameDelta.z+mInfiniteLayer.mSpeed.z*timeDelta;
			
			mInfiniteLayer.mSceneNode->translate(tx, ty, tz);
			if (mInfiniteLayer.mSceneNode->getPosition().x < 0){
				mInfiniteLayer.mSceneNode->translate(mZoneSize.x,0,0);
			}
			if (mInfiniteLayer.mSceneNode->getPosition().y < 0){
				mInfiniteLayer.mSceneNode->translate(0,mZoneSize.y,0);
			}
			if (mInfiniteLayer.mSceneNode->getPosition().z < 0){
				mInfiniteLayer.mSceneNode->translate(0,0,mZoneSize.z);
			}
			if (mInfiniteLayer.mSceneNode->getPosition().x > mZoneSize.x){
				mInfiniteLayer.mSceneNode->translate(0-mZoneSize.x,0,0);
			}
			if (mInfiniteLayer.mSceneNode->getPosition().y > mZoneSize.y){
				mInfiniteLayer.mSceneNode->translate(0,0-mZoneSize.y,0);
			}
			if (mInfiniteLayer.mSceneNode->getPosition().z > mZoneSize.z){
				mInfiniteLayer.mSceneNode->translate(0,0,0-mZoneSize.z);
			}
		}

		list<CLOUD_PARTICLE>::iterator it;
		for (it=mCloudParticle.begin();it!=mCloudParticle.end();it++){
			(*it).mSceneNode->translate(frameDelta, Ogre::Node::TS_WORLD);
			(*it).mSceneNode->translate(
				(*it).mSpeed.x*timeDelta, 
				(*it).mSpeed.y*timeDelta, 
				(*it).mSpeed.z*timeDelta);
			(*it).mSceneNode->pitch((Ogre::Radian)(*it).mSpin.x*timeDelta);
			(*it).mSceneNode->roll((Ogre::Radian)(*it).mSpin.y*timeDelta);
			(*it).mSceneNode->yaw((Ogre::Radian)(*it).mSpin.z*timeDelta);
			
			if ((*it).mSceneNode->getPosition().x+(*it).mSpeed.x < 0.0f-mZoneSize.x/2.0f){
				(*it).mColour.a -= CLOUD_FADE_SPEED*timeDelta;
				if ((*it).mColour.a < 0){
					(*it).mSceneNode->translate(mZoneSize.x,0,0);
					(*it).mColour.a = 0;
				}
				if (mFade){
					(*it).mMaterialPass->setDiffuse((*it).mColour);
				}
			}else if ((*it).mSceneNode->getPosition().y+(*it).mSpeed.y < 0.0f-mZoneSize.y/2.0f){
				(*it).mColour.a -= CLOUD_FADE_SPEED*timeDelta;
				if ((*it).mColour.a < 0){
					(*it).mSceneNode->translate(0,mZoneSize.y,0);
					(*it).mColour.a = 0;
				}
				if (mFade){
					(*it).mMaterialPass->setDiffuse((*it).mColour);
				}
			}else if ((*it).mSceneNode->getPosition().z+(*it).mSpeed.z < 0.0f-mZoneSize.z/2.0f){
				(*it).mColour.a -= CLOUD_FADE_SPEED*timeDelta;
				if ((*it).mColour.a < 0){
					(*it).mSceneNode->translate(0,0,mZoneSize.z);
					(*it).mColour.a = 0;
				}
				if (mFade){
					(*it).mMaterialPass->setDiffuse((*it).mColour);
				}
			}else if ((*it).mSceneNode->getPosition().x+(*it).mSpeed.x > mZoneSize.x/2.0f){
				(*it).mColour.a -= CLOUD_FADE_SPEED*timeDelta;
				if ((*it).mColour.a < 0){
					(*it).mColour.a = 0;
					(*it).mSceneNode->translate(0-mZoneSize.x,0,0);
				}
				if (mFade){
					(*it).mMaterialPass->setDiffuse((*it).mColour);
				}
			}else if ((*it).mSceneNode->getPosition().y+(*it).mSpeed.y > mZoneSize.y/2.0f){
				(*it).mColour.a -= CLOUD_FADE_SPEED*timeDelta;
				if ((*it).mColour.a < 0){
					(*it).mSceneNode->translate(0,0-mZoneSize.y,0);
					(*it).mColour.a = 0;
				}
				if (mFade){
					(*it).mMaterialPass->setDiffuse((*it).mColour);
				}
			}else if ((*it).mSceneNode->getPosition().z+(*it).mSpeed.z > mZoneSize.z/2.0f){
				(*it).mColour.a -= CLOUD_FADE_SPEED*timeDelta;
				if ((*it).mColour.a < 0){
					(*it).mSceneNode->translate(0,0,0-mZoneSize.z);
					(*it).mColour.a = 0;
				}
				if (mFade){
					(*it).mMaterialPass->setDiffuse((*it).mColour);
				}
			}else{
				if ((*it).mColour.a < 1){
					(*it).mColour.a += CLOUD_FADE_SPEED*timeDelta;
					if ((*it).mColour.a > 1){
						(*it).mColour.a = 1;
					}
					if (mFade){
						(*it).mMaterialPass->setDiffuse((*it).mColour);
					}
				}
			}
		}
		//t = mSceneNode->_getFullTransform();
		mPreviousPosition = snw;

		return false;
	}



}