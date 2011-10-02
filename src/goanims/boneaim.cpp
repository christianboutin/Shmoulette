#include "stdafx.h"
#include "goanims\boneaim.h"

namespace Shmoulette{
	namespace GoAnim{
		void BoneAim::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
		}

		
		void BoneAim::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);

			if (PyList_Size(localObj) < 2){
				throw("BoneAim Requires 2, 3, or 4 parameters in the list through python, change code as needed.");
			}

			if (PyList_Size(localObj) >= 2){
				mBoneName = PyString_AsString(PyList_GetItem(localObj, 0));
				mTargetName = PyString_AsString(PyList_GetItem(localObj, 1));
			}
			if (PyList_Size(localObj) >= 3){
				string s = PyString_AsString(PyList_GetItem(localObj, 0));
				if (s == "x"){
					mAxis = AXIS_X;
				}else if (s == "y"){
					mAxis = AXIS_Y;
				}else if (s == "z"){
					mAxis = AXIS_Z;
				}else{
					mAxis = AXIS_ALL;
				}
			}else{
				mAxis = AXIS_ALL;
			}
			mBone = NULL;
			mTarget = NULL;
		}
		
		void BoneAim::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string s = xml->getString("axis");
			if (xml->getError() != XML_OK){
				s = "ALL";//throw("BoneAim requires axis");
			}
			if (s == "x"){
				mAxis = AXIS_X;
			}else if (s == "y"){
				mAxis = AXIS_Y;
			}else if (s == "z"){
				mAxis = AXIS_Z;
			}else{
				mAxis = AXIS_ALL;
			}

			mBoneName = xml->getString("bone");
			if (xml->getError() != XML_OK){
				throw("BoneAim requires bone");
			}
			mBone = NULL;

			mTargetName = xml->getString("target");
			if (xml->getError() != XML_OK){
				throw("BoneAim requires target");
			}

			mStrength = xml->getFloat("strength");
			if (xml->getError() != XML_OK){
				mStrength = 5;
			}


			mTarget = NULL;
		}

		void BoneAim::update(float time){
			if (mBone == NULL){
				mBone = mGo->getEntity()->getSkeleton()->getBone(mBoneName);
				mTarget = Level::getSingleton()->getCurrentSegment()->getObjectByName(mTargetName);
				if (mTarget == NULL){
					throw("No such object : "+mTargetName);
				}
			}
			//Ogre::Entity* entity;
			//mTarget = Level::getSingleton()->getPlayerShip();
			
			
			Ogre::Matrix4 matrixA = mGo->getNode()->_getFullTransform()*mBone->_getFullTransform();
			
			Ogre::Matrix4 matrix = matrixA;
			Ogre::Matrix4 transform = matrix.inverse()*mTarget->getNode()->_getFullTransform();
			Ogre::Vector3 v = transform.getTrans();
			/*Util::Log(
					"Bone:"+ts(matrixA.getTrans())+
					" Ship:"+ts(mTarget->getNode()->_getFullTransform().getTrans())+
					" Diff:"+ts(v)
					,0
				);*/


			//targetMatrix = Ogre::Matrix4::IDENTITY;
			//matrix.setTrans(mTarget->getNode()->_getFullTransform());
			

			//Ogre::Quaternion q = transform.extractQuaternion();
			Ogre::Quaternion q;

			switch (mAxis){
				case AXIS_X:
					q.FromAngleAxis(Ogre::Math::ATan2(v.y, v.z),Ogre::Vector3(-1,0,0));
				break;
				case AXIS_Y:
					q.FromAngleAxis(Ogre::Math::ATan2(v.x, v.z),Ogre::Vector3(0,1,0));
				break;
				case AXIS_Z:
					q.FromAngleAxis(Ogre::Math::ATan2(v.x, v.y),Ogre::Vector3(0,0,1));
				break;
				case AXIS_ALL:
					//q.FromAngleAxis(Ogre::Radian(0),v);
					q = transform.extractQuaternion();// - matrixA.extractQuaternion();
					//q = transform.extractQuaternion() - matrixA.extractQuaternion();
				break;
			}

			mBone->setManuallyControlled(true);
			
			//mBone->setOrientation(q);
			float slerp = time*mStrength;
			if (slerp > 1){
				slerp = 1;
			}
			Ogre::Quaternion r = Quaternion::Slerp(slerp, Ogre::Quaternion::IDENTITY, q); //Level::getSingleton()->getTimeDelta()
			
			mBone->rotate(r);
			/*mBone->yaw(Ogre::Radian(Ogre::Math::RangeRandom(-Math::PI, Math::PI)));
			mBone->pitch(Ogre::Radian(Ogre::Math::RangeRandom(-Math::PI, Math::PI)));
			mBone->roll(Ogre::Radian(Ogre::Math::RangeRandom(-Math::PI, Math::PI)));*/
			//mBone->_update(true, true);



			Parent::update(time);
			//mSpeed += .6*time; 
			//mGo->getNode()->translate(0, 0-mSpeed, 0);*/
		}
	}
}

