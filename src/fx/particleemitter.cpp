#include "stdafx.h"
#include "fx\particleemitter.h"

namespace Shmoulette{

	void ParticleEmitter::init(PyObject* pyObject){
		Parent::init(pyObject);
		mType = OBJ_TYPE_PARTICLE;
		
		if (PyList_Check(pyObject) == true && PyList_Size(pyObject) > 3){
			PyObject* o = PyList_GetItem(pyObject, 3);
			if (PyList_Check(o) == true){
				mParticleName = PyString_AsString(PyList_GetItem(o,0));
			}else{
				mParticleName = PyString_AsString(o);
			}

		}else{
			throw("ParticleEmitter : PyObject should be an array of at least size 4");
		}
	}

	void ParticleEmitter::init(XML* xml){
		Parent::init(xml);
		mParticleName = xml->getString("particle");
		mType = OBJ_TYPE_PARTICLE;
	}
	void ParticleEmitter::show(){
		for (int i=0;i<mParticleSystem->getNumEmitters();i++){
			mParticleSystem->getEmitter(i)->setEmissionRate(mEmissionRate[i]);
		}
		Parent::show();
	}
	void ParticleEmitter::hide(){
		for (int i=0;i<mParticleSystem->getNumEmitters();i++){
			mParticleSystem->getEmitter(i)->setEmissionRate(0);
		}
		Parent::hide();
	}
	void ParticleEmitter::build(){
		Parent::build();
		if (Level::getSingleton()->getSceneManager()->hasParticleSystem(getId()+"_particles")){
			int g =0;
		}
		mParticleSystem = Level::getSingleton()->getSceneManager()->createParticleSystem(getId()+"_particles",mParticleName);
		
		//mParticleSystem->setKeepParticlesInLocalSpace(true);
		if (mParentConnector.mGameObject == NULL){
			mSceneNode->attachObject(mParticleSystem);
		}else{
			//Ogre::Vector3 scale = mSceneNode->getScale();
			if (mParentConnector.mBone != ""){
				mParentConnector.mGameObject->getEntity()->attachObjectToBone(mParentConnector.mBone, mParticleSystem);
			}else{
				if (mParentConnector.mGameObject->getEntity() != NULL){
					mParentConnector.mGameObject->getEntity()->getParentSceneNode()->attachObject(mParticleSystem);
				}else{
					mParentConnector.mGameObject->getSceneNode()->attachObject(mParticleSystem);
				}
			}
			//mEntity->getParentNode()->setScale(scale);
			//mBoneNodeMode =true;
		}
		//mSceneNode->attachObject(mParticleSystem);
		for (int i=0;i<mParticleSystem->getNumEmitters();i++){
			mEmissionRate.push_back(mParticleSystem->getEmitter(0)->getEmissionRate());
		}
		mParticleSystem->getEmitter(0)->setEmissionRate(0);
		mSceneNode->setVisible(true);
		/*mMaxSpeed = mParticleSystem->getEmitter(0)->getMaxParticleVelocity();
		mMinSpeed = mParticleSystem->getEmitter(0)->getMinParticleVelocity();
		mDecayEmit = mEmissionRate / mDecay;
		mDecayMaxSpeed = mMaxSpeed / mDecay;
		mDecayMinSpeed = mMinSpeed / mDecay;*/
	}
	void ParticleEmitter::destroy(){
		if (mParentConnector.mGameObject == NULL){
			mSceneNode->detachObject(mParticleSystem);
		}else{
			//Ogre::Vector3 scale = mSceneNode->getScale();
			
			if (mParentConnector.mGameObject->getEntity() != NULL){
				mParentConnector.mGameObject->getEntity()->detachObjectFromBone(mParticleSystem);
			}else{
				mParentConnector.mGameObject->getSceneNode()->detachObject(mParticleSystem);
			}
			//mEntity->getParentNode()->setScale(scale);
			//mBoneNodeMode =true;
		}
		Level::getSingleton()->getSceneManager()->destroyParticleSystem(mParticleSystem);
		if (Level::getSingleton()->getSceneManager()->hasParticleSystem(getId()+"_particles")){
			int g =0;
		}

		Parent::destroy();
	}
	bool ParticleEmitter::update(double timeDelta, double parentTime){
		return Parent::update(timeDelta, parentTime);
	}

	void ParticleEmitter::attachToBone(Ogre::Entity* ent, string bone){
		Ogre::Vector3 scale = mSceneNode->getScale();
		//scale.x = 20;
		//scale.y = 20;
		//scale.z = 20;
		if (mParticleSystem->getParentSceneNode() != NULL){
			mParticleSystem->getParentSceneNode()->detachObject(mParticleSystem);
		}
		
		//Ogre::Quaternion q((Ogre::Radian)(0-(Ogre::Math::PI/2)), Vector3(1,0,0));
		
		ent->attachObjectToBone(bone, mParticleSystem);
		mParticleSystem->getParentNode()->setScale(scale);

		if (mSceneNode->getParentSceneNode() != NULL){
			mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
			//Level::getSingleton()->getSceneManager()->destroySceneNode(mSceneNode->getName());
			//mSceneNode = NULL;
			// Leave the node floating.  It is assumed a GameObject has a scenenode, even if it floats in nothingness2
		}
		//mBoneNodeMode =true;
	}



}

