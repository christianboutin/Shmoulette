#include "stdafx.h"
#include "goanims\randomscaler.h"

namespace Shmoulette{
	namespace GoAnim{
		void RandomScaler::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
		}

		void RandomScaler::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);
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
			}

		}


		void RandomScaler::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			mMin = vector3FromString(xml->getString("min"));
			mMax = vector3FromString(xml->getString("max"));
		}

		void RandomScaler::update(float time){
			Parent::update(time);
			mGo->getEntity()->getParentNode()->setScale(
			//mGo->getSceneNode()->setScale(
				Ogre::Math::RangeRandom(mMin.x, mMax.x),
				Ogre::Math::RangeRandom(mMin.y, mMax.y),
				Ogre::Math::RangeRandom(mMin.z, mMax.z));
		}
	}
}