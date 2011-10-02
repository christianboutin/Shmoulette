#include "stdafx.h"
#include "goanims\jump.h"

namespace Shmoulette{
	namespace GoAnim{
		void JumpOut::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
		}

		void JumpOut::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			mFX = new FX();
			PyObject* localObj = PyList_GetItem(po, 1);
			PyObject* fx = PyList_GetItem(localObj, 0);
			mFX->init(PyString_AsString(fx));
			//mSpeed = 0;
		}
		
		void JumpOut::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string fxfn = xml->getString("fx");
			if (xml->getError() != XML_OK){
				throw ("Jumpout goanim requires fx tag");
			}
			mFX = new FX();
			mFX->init(fxfn);
			//mSpeed = 0;
		}
		void JumpOut::build(){
			Ogre::Entity* goEntity = mGo->getEntity();

			Ogre::SceneNode* psn = goEntity->getParentSceneNode();

			mSceneNode = psn->getParentSceneNode()->createChildSceneNode(psn->getPosition(),psn->getOrientation());
			mEntity = goEntity->clone(goEntity->getName()+"_jumpclone");
	
			mMaterial = static_cast<Ogre::MaterialPtr>(
				MaterialManager::getSingleton().getByName("jumpGhost")
				)->clone("jumpGhost_clone_"+ts(GameElement::getNextIdCounter()));

			mEntity->setMaterial(mMaterial);
			
			
			mSceneNode->attachObject(mEntity);
			mSceneNode->setVisible(true);
			mEntity->setVisible(true);
			mDistance = 0;
			mIntensity = 1;
			mFX->setParent(mSceneNode);
			mFX->build();
			mFX->activate();
			mFX->show();
			mFX->getSceneNode()->setPosition(mSceneNode->getPosition());
			Parent::build();
		}

		void JumpOut::destroy(){
			mSceneNode->detachObject(mEntity);
			Level::getSingleton()->getSceneManager()->destroyEntity(mEntity);
			mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
			MaterialManager::getSingleton().remove(mMaterial->getName());
			mFX->deactivate();
			mFX->hide();
			mFX->destroy();
			Parent::destroy();
		}

		void JumpOut::update(float time){
			Parent::update(time);
			mDistance += time*2000;
			mGo->getSceneNode()->translate(0,0,mDistance, Ogre::Node::TS_LOCAL);
			mSceneNode->translate(0,0,time*100, Ogre::Node::TS_LOCAL);
			mIntensity -= time;
			if (mIntensity < 0) mIntensity = 0;

			for (int i=0;i<mEntity->getNumSubEntities();i++){
				Ogre::MaterialPtr mat = mEntity->getSubEntity(i)->getMaterial();
				for (int j=0;j<mat->getNumTechniques();j++){
					Ogre::Technique* tech = mat->getTechnique(j);
					for (int k=0;k<tech->getNumPasses();k++){
						Ogre::Pass* pass = tech->getPass(k);
						pass->setSelfIllumination(Ogre::ColourValue(mIntensity, mIntensity, mIntensity));
					}
				}
			}
			//mSpeed += .6*time; 
			//mGo->getNode()->translate(0, 0-mSpeed, 0);
		}

		void JumpIn::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
		}

		void JumpIn::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			mFX = new FX();
			mFX->init("JumpOutSmall_fx");
			//mSpeed = 0;
		}
		
		void JumpIn::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string fxfn = xml->getString("fx");
			if (xml->getError() != XML_OK){
				throw ("Jumpout goanim requires fx tag");
			}
			mFX = new FX();
			mFX->init(fxfn);
			//mSpeed = 0;
		}
		void JumpIn::build(){
			Ogre::Entity* goEntity = mGo->getEntity();

			mSceneNode = goEntity->getParentSceneNode()->getParentSceneNode()->createChildSceneNode(goEntity->getParentSceneNode()->getPosition(),goEntity->getParentSceneNode()->getOrientation());
			mEntity = goEntity->clone(goEntity->getName()+"_jumpclone");
	
			Ogre::MaterialPtr mat = static_cast<Ogre::MaterialPtr>(
				MaterialManager::getSingleton().getByName("jumpGhost")
				)->clone("jumpGhost_clone_"+ts(GameElement::getNextIdCounter()));

			mEntity->setMaterial(mat);
			
			//mGo->getEntity()->setVisible(false);
			
			mFX->getSceneNode()->setPosition(mSceneNode->getPosition());
			
			mSceneNode->attachObject(mEntity);
			mSceneNode->setVisible(true);
			mEntity->setVisible(true);
			mDistance = -2000;
			mSceneNode->translate(0,0,-200, Ogre::Node::TS_LOCAL);
			mIntensity = 0;
			mFX->setParent(mSceneNode);
			mFX->build();
			mFX->activate();
			mFX->show();
			Parent::build();
		}

		void JumpIn::destroy(){
			mSceneNode->detachObject(mEntity);
			Level::getSingleton()->getSceneManager()->destroyEntity(mEntity);
			mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
			//MaterialManager::getSingleton().remove(mMaterial->getName());
			mFX->deactivate();
			mFX->hide();
			mFX->destroy();
			Parent::destroy();
		}

		void JumpIn::update(float time){
			Parent::update(time);
			mDistance += time*200;
			mSceneNode->translate(0,0,time*200, Ogre::Node::TS_LOCAL);
			if (mDistance < 0){
				mDistance += time*1800;
				mGo->getSceneNode()->translate(0,0,mDistance, Ogre::Node::TS_LOCAL);
				mIntensity += time;
				if (mIntensity > 1) mIntensity = 1;
			}else{
				//mGo->getEntity()->setVisible(true);
				mIntensity -= time;
				if (mIntensity < 0) mIntensity = 0;

			}
			for (int i=0;i<mEntity->getNumSubEntities();i++){
				Ogre::MaterialPtr mat = mEntity->getSubEntity(i)->getMaterial();
				for (int j=0;j<mat->getNumTechniques();j++){
					Ogre::Technique* tech = mat->getTechnique(j);
					for (int k=0;k<tech->getNumPasses();k++){
						Ogre::Pass* pass = tech->getPass(k);
						pass->setSelfIllumination(Ogre::ColourValue(mIntensity, mIntensity, mIntensity));
					}
				}
			}
			//mSpeed += .6*time; 
			//mGo->getNode()->translate(0, 0-mSpeed, 0);
		}
	}
}

