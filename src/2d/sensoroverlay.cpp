#include "stdafx.h"
#include "2d\sensoroverlay.h"

namespace Shmoulette{


	SensorOverlayEntry::SensorOverlayEntry(Ogre::Entity* entity, string material){
		mEntity = entity;
		mSceneNode = mEntity->getParentSceneNode();
		mMaterial = material;
		mIsPainted=false;
	}
	SensorOverlayEntry::~SensorOverlayEntry(){
		if (mIsPainted){
			unpaint();
		}
	}

	void SensorOverlayEntry::createSensorComponent(){
		/*Ogre::MaterialPtr replacementMaterial = MaterialManager::getSingleton().getByName("SensorOverlayFriendly");
		Ogre::Technique* replacemtnetTechnique = replacementMaterial->getTechnique(0);
		Ogre::Pass* repPass = replacemtnetTechnique->getPass(0);

		int j=mEntity->getNumManualLodLevels();
		for (int i=-1;i<j;i++){
			Ogre::Entity* lodEnt;
			if (i > -1){
				lodEnt  = mEntity->getManualLodLevel(i);
			}else{
				lodEnt  = mEntity;
			}
			for (int k=0;k<lodEnt->getNumSubEntities();k++){
				Ogre::SubEntity* subEnt = lodEnt->getSubEntity(k);
		
				Ogre::MaterialPtr mat = subEnt->getMaterial();

				for (int l=0;l<mat->getNumTechniques();l++){

					Ogre::Technique* tech = mat->getTechnique(l);
					bool hasPass = tech->getPass("SensorOverlayPass") != NULL;
					if (!hasPass){
						Ogre::Pass* pass = tech->createPass();
						mPass.push_back(pass);
						tech->movePass(tech->getNumPasses()-1,0);
						
						*tech->getPass(0) = *repPass;
						
						//int g=0;

					}
				}

			}
		}*/

		// TODO Do I still need those?
		
		mClone = mEntity->clone(mEntity->getName()+"_SensorOverlayComponent");
		mClone->setMaterial(Ogre::MaterialManager::getSingleton().getByName("SensorOverlayFriendly"));
		mClone->setQueryFlags(SceneQueryFlag::IMMATERIAL);

		mSceneNode->attachObject(mClone);

		mIsPainted = true;
		mClone->setRenderQueueGroup(Level::getSingleton()->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+1);
		mEntity->setRenderQueueGroup(Level::getSingleton()->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+2);
	}

	void SensorOverlayEntry::destroySensorComponent(){
		/*int j=mEntity->getNumManualLodLevels();
		for (int i=-1;i<j;i++){
			Ogre::Entity* lodEnt;
			if (i > -1){
				lodEnt  = mEntity->getManualLodLevel(i);
			}else{
				lodEnt  = mEntity;
			}
			for (int k=0;k<lodEnt->getNumSubEntities();k++){
				Ogre::SubEntity* subEnt = lodEnt->getSubEntity(k);
				Ogre::MaterialPtr mat = subEnt->getMaterial();

				for (int l=0;l<mat->getNumTechniques();l++){
					Ogre::Technique* tech = mat->getTechnique(l);
					for (list<Ogre::Pass*>::iterator pit = mPass.begin();pit != mPass.end();pit++){
						for (int m=0;m<tech->getNumPasses();m++){
							if (tech->getPass(m) == (*pit)){
								tech->removePass(m);
								break;
							}
						}
					}
				}
			}
		}
		mPass.clear();*/
		mIsPainted = false;
		Shmoulette::Level::getSingleton()->getSceneManager()->destroyEntity(mClone);
		mEntity->setRenderQueueGroup(Level::getSingleton()->getSceneManager()->getRenderQueue()->getDefaultQueueGroup());
	}
	void SensorOverlayEntry::paint(){
		if (mIsPainted == false){
			/*for (list<Ogre::Pass*>::iterator it = mPass.begin();it!=mPass.end();it++){
				(*it)->setDepthFunction(Ogre::CompareFunction::CMPF_GREATER);
			}*/
			mClone->setVisible(true);
			mSceneNode->attachObject(mClone);
			mEntity->setRenderQueueGroup(Level::getSingleton()->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+2);
			mIsPainted = true;
		}
	}
	void SensorOverlayEntry::unpaint(){
		if (mIsPainted == true){
			/*for (list<Ogre::Pass*>::iterator it = mPass.begin();it!=mPass.end();it++){
				(*it)->setDepthFunction(Ogre::CompareFunction::CMPF_ALWAYS_FAIL);
			}*/
			mClone->setVisible(false);
			mSceneNode->detachObject(mClone);
			mEntity->setRenderQueueGroup(Level::getSingleton()->getSceneManager()->getRenderQueue()->getDefaultQueueGroup());
			mIsPainted = false;
		}
	}

	SensorOverlay::SensorOverlay(){
		mActive = true;
	}
	void SensorOverlay::activate(){
		if (mActive != true){
			vector<SensorOverlayEntry*>::iterator it;
			for (it=mObjects.begin();it!=mObjects.end();it++){
				(*it)->paint();
			}
			mActive = true;
		}
	}
	void SensorOverlay::deactivate(){
		if (mActive != false){
			vector<SensorOverlayEntry*>::iterator it;
			for (it=mObjects.begin();it!=mObjects.end();it++){
				(*it)->unpaint();
			}
			mActive = false;
		}
	}

	void SensorOverlay::addEntity(Ogre::Entity* entity, string material){
		SensorOverlayEntry* entry = new SensorOverlayEntry(entity, material);
		entry->createSensorComponent();
		if (mActive){
			entry->paint();
		}else{
			entry->unpaint();
		}
		mObjects.push_back(entry);
	}
	void SensorOverlay::removeEntity(Ogre::Entity* entity){
		vector<SensorOverlayEntry*>::iterator it;
		for (it=mObjects.begin();it!=mObjects.end();it++){
			if ((*it)->getEntity() == entity){
				(*it)->destroySensorComponent();
				delete (*it);
				mObjects.erase(it);

				break;
			}
		}
	}
	void SensorOverlay::update(double timeDelta, double parentTime){
	}
}