#include "stdafx.h"
#include "goanims\materialchain.h"

namespace Shmoulette{
	namespace GoAnim{
		void MaterialChain::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
		}

		void MaterialChain::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			/*PyObject* localObj = PyList_GetItem(po, 1);
			if (PyList_Size(localObj) == 2){
				if (PyList_Check(PyList_GetItem(localObj,0)) == true){
					mMin.x = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(localObj, 0), 0));
					mMin.y = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(localObj, 0), 1));
					mMin.z = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(localObj, 0), 2));
				}else{
					mMin.x = PyFloat_AsDouble(PyList_GetItem(localObj, 0));
					mMin.y = mMin.x;
					mMin.z = mMin.x;
				}
				if (PyList_Check(PyList_GetItem(localObj,1)) == true){
					mMax.x = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(localObj, 1), 0));
					mMax.y = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(localObj, 1), 1));
					mMax.z = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(localObj, 1), 2));
				}else{
					mMax.x = PyFloat_AsDouble(PyList_GetItem(localObj, 1));
					mMax.y = mMax.x;
					mMax.z = mMax.x;
				}
			}else{
				throw("Random Scaler requires 2 params");
			}*/
			throw("Material Chain has no python bindings");
		}


		void MaterialChain::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			Ogre::Entity* entity = go->getEntity();
			mSource = xml->getString("source");

			string chainString = xml->getString("chain");

			Tokenize(chainString, mMaterialChain, ",");
			mIterator = mMaterialChain.begin();

			mDelay = xml->getDouble("delay");
			mTimeLeft = mDelay;
		}
		void MaterialChain::build(){
			Parent::build();
			Ogre::Entity* entity = mGo->getEntity();
			int j=entity->getNumManualLodLevels();

			for (int i=-1;i<j;i++){
				Ogre::Entity* lodEnt;
				if (i > -1){
					lodEnt  = entity->getManualLodLevel(i);
				}else{
					lodEnt  = entity;
				}
				for (int k=0;k<lodEnt->getNumSubEntities();k++){
					Ogre::SubEntity* subEnt = lodEnt->getSubEntity(k);
					if (subEnt->getMaterialName() == mSource){
						mEntityToReplace.push_back(subEnt);
					}
				}
			}
		}

		void MaterialChain::update(float time){
			Parent::update(time);

			if (isBuild()){
				mTimeLeft -= time;//Level::getSingleton()->getTimeDelta();
				if (mTimeLeft <= 0){
					for (vector<Ogre::SubEntity*>::iterator it = mEntityToReplace.begin(); it != mEntityToReplace.end();it++){
						(*it)->setMaterialName((*mIterator));
					}
					mIterator++;
					if (mIterator == mMaterialChain.end()){
						mIterator = mMaterialChain.begin();
					}
					mTimeLeft += mDelay;
				}
			}
		}
	}
}