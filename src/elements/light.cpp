#include "stdafx.h"
#include "main.h"
#include "elements\light.h"
using namespace Ogre;
namespace Shmoulette{

	void Light::build(){
		mLight = mLevelSegment->getSceneManager()->createLight(getId()+"_light");
		mLight->setType(mLightType);
		mLight->setCastShadows(mCastsShadows);
		mLight->setDiffuseColour(1, 1, 1);
		mLight->setDirection(0,0,-1);
		Parent::build();

		if (mParentConnector.mGameObject == NULL){
			mSceneNode->attachObject(mLight);
		}else{
			if (mParentConnector.mBone==""){
				mParentConnector.mGameObject->attach(this);
			}else{
				mParentConnector.mGameObject->getEntity()->attachObjectToBone(mParentConnector.mBone, mLight);
			}
		}
	}

	void Light::updateFromCoordinator(bool force){
		Parent::updateFromCoordinator(force);
		if (mLightCoordinator){
			if (mLightCoordinator->hasChanged() || force){
				Coordinator::LightInfo l = mLightCoordinator->getValue(timeAlive());
				mLight->setDiffuseColour(l.mDiffuse.r,l.mDiffuse.g,l.mDiffuse.b);
				mLight->setSpecularColour(1,1,1);
			}
		}
	}


	void Light::destroy(){
		if (mParentConnector.mGameObject == NULL){
			mSceneNode->detachObject(mLight);
		}else{
			if (mParentConnector.mBone==""){
				mParentConnector.mGameObject->detach(this);
			}else{
				mParentConnector.mGameObject->getEntity()->detachObjectFromBone(mLight);
			}
		}
		Level::getSingleton()->getSceneManager()->destroyLight(mLight);
		Parent::destroy();
	}

	void Light::init(PyObject* pyObject){
		Parent::init(pyObject);
		mType = OBJ_TYPE_LIGHT;
		if (PyList_Check(pyObject) == true && PyList_Size(pyObject) > 3){
		}else{
			throw("Light : PyObject should be an array of at least size 4");
		}
	}


	void Light::init(XML* xml){
		Parent::init(xml);
		destroyCollisionStructure();
		
		mType = OBJ_TYPE_LIGHT;
		if (xml->getString("type")=="point"){
			mLightType = Ogre::Light::LT_POINT;
			mFilename = "ShmoulettePL";
		}else if (xml->getString("type")=="directional"){
			mLightType = Ogre::Light::LT_DIRECTIONAL;
			mFilename = "ShmouletteDL";
		}else if (xml->getString("type")=="spotlight"){
			mLightType = Ogre::Light::LT_SPOTLIGHT;;
			mFilename = "ShmouletteSL";
		}else{
			throw("Light type undefined : "+xml->getString("type"));
		}
		if (xml->getInt("shadows") == 1){
			mCastsShadows = true;
		}else{
			mCastsShadows = false;
		}

		string light_c = xml->getString("light_c");
		if (xml->getError() == XML_OK){
			mLightCoordinator = dynamic_cast<Coordinator::LightCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get(light_c));
		}else{
			mLightCoordinator = NULL;
			throw("Light requires a light_c (coordinator)");
		}


	}


	bool Light::update(double timeDelta, double parentTime){

		Parent::update(timeDelta, parentTime);

		Util::Log("Light Position : ("+
			ts(mLight->getDerivedPosition().x)+
			","+
			ts(mLight->getDerivedPosition().y)+
			","+
			ts(mLight->getDerivedPosition().z),7);

		return false;
	}


}