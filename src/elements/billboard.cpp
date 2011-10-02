#include "stdafx.h"

#include "elements\billboard.h"


namespace Shmoulette{
	using namespace Ogre;
	using namespace std;

	/*Billboard::Billboard(vector<string>& stringParams):DynamicObj(stringParams){
		
	}*/
	/*void Billboard::setCollisionGroup(COLLGROUP cg, bool children){
		Parent::setCollisionGroup(cg, children);
		//setQueryFlag(Shmoulette::SceneQueryFlag::IMMATERIAL);
		Parent::setCollisionGroup(COLLGROUP_ALL, false);
	}*/

	void Billboard::init(PyObject* pyObject){
		Parent::init(pyObject);
		mType = OBJ_TYPE_NOMESH;
		if (PyList_Check(pyObject) == true && PyList_Size(pyObject) > 3){
			PyObject* o = PyList_GetItem(pyObject, 3);
			if (PyList_Check(o) == false){
				throw("Billboard : o should be a list.");
			}
			// Possible combinations
			// 2 parameters : Material, size
			// 3 parameters : Material, sizex, sizey
			int listSize = PyList_Size(o);
			if (listSize >= 2){
				mFilename = PyString_AsString(PyList_GetItem(o, 0));
				for (int i=1; i<listSize;i++){
					PyObject* pyElem = PyList_GetItem(o, i);
					if (PyList_Size(pyElem) < 2){
						throw("Billboard Elem : each elem require at least 2 elements ex: [\"Glow/MissileExhaust\",[[posx,posy,posz],[sizex, sizey]]]");
					}
					BILLBOARD_ELEMENT b;
					b.mPos.x = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 0), 0));
					b.mPos.y = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 0), 1));
					b.mPos.z = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 0), 2));

					if (PyList_Check(PyList_GetItem(pyElem, 1)) == false){
						b.mSize.x = PyFloat_AsDouble(PyList_GetItem(pyElem, 1));
						b.mSize.y = b.mSize.x;
					}else{
						b.mSize.x = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 1), 0));
						b.mSize.y = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 1), 1));
					}
					
					if (PyList_Size(pyElem) == 3){
						if (PyList_Size(PyList_GetItem(pyElem, 2)) < 3){
							b.mColour = Ogre::ColourValue::White;
						}else{
							b.mColour.r = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 2), 0));
							b.mColour.g = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 2), 1));
							b.mColour.b = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 2), 2));
							if (PyList_Size(PyList_GetItem(pyElem, 2)) == 4){
								b.mColour.a = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pyElem, 2), 3));
							}else{
								b.mColour.a = 1;
							}
						}
					}else{
						b.mColour = Ogre::ColourValue::White;
					}

					mBillboardElement.push_back(b);
				}
				int i=0;
				/*double size = PyFloat_AsDouble(PyList_GetItem(o, 1));
				mSize.x = size;
				mSize.y = size;*/
			}else{
				throw("Billboard : o should be an array of either size 2 or 3");
			}			
		}else{
			throw("Billboard : PyObject should be an array of at least size 4");
		}
	}

	void Billboard::init(XML* xml){
		GameObject::init(xml);
		mType = OBJ_TYPE_NOMESH;
		mFilename = xml->getString("material");
		BILLBOARD_ELEMENT b;
		b.mPos = Ogre::Vector3(0,0,0);
		
		b.mSize.x = xml->getFloat("sizex");
		b.mSize.y = xml->getFloat("sizey");
		mBillboardElement.push_back(b);

		mBillboardSet = NULL;
	}


	Billboard::~Billboard(){
	}

	void Billboard::build(){
			mBillboardSet = Level::getSingleton()->getSceneManager()->createBillboardSet(mFilename+ts(GameElement::getNextIdCounter()));
			mBillboardSet->setMaterialName(mFilename);
			mBillboardSet->setCullIndividually(true);
			
			//mBillboardSet->setPointRenderingEnabled(true);
			//mBillboardSet->setBillboardsInWorldSpace(true);
			//mBillboardSet->setBillboardType(Ogre::BillboardType::BBT_PERPENDICULAR_COMMON);
			if (mParentConnector.mGameObject == NULL){
				mSceneNode->attachObject(mBillboardSet);
			}else{
				//Ogre::Vector3 scale = mSceneNode->getScale();
				if (mParentConnector.mBone != ""){
					mParentConnector.mGameObject->getEntity()->attachObjectToBone(mParentConnector.mBone, mBillboardSet);
					getNode()->setPosition(mSceneNode->getPosition());
					getNode()->setOrientation(mSceneNode->getOrientation());
				}else{
					throw("Billboard object requires to be attached to a parent's bone.  No bone, no go");
				}


				//mEntity->getParentNode()->setScale(scale);
				//mBoneNodeMode =true;
			}


			
			//attachObject(mBillboardSet);
			
			Ogre::AxisAlignedBox box(Ogre::Vector3(0,0,0), Ogre::Vector3(0,0,0));
			for (list<BILLBOARD_ELEMENT>::iterator it = mBillboardElement.begin();it!=mBillboardElement.end();it++){
				Ogre::Billboard* bb = mBillboardSet->createBillboard((*it).mPos, (*it).mColour);
				bb->setDimensions((*it).mSize.x,(*it).mSize.y);

				Ogre::Vector3 np1 = (*it).mPos + Ogre::Vector3((*it).mSize.x, (*it).mSize.y, (*it).mSize.x);
				Ogre::Vector3 np2 = (*it).mPos - Ogre::Vector3((*it).mSize.x, (*it).mSize.y, (*it).mSize.x);
				Ogre::AxisAlignedBox box2(np2, np1);
				box.merge(box2);
			}
			mBillboardSet->setBounds(box, (*mBillboardElement.begin()).mSize.x);
		


		/*mHaloSet = mSceneMgr->createBillboardSet("halo");
	mHaloSet->setMaterialName("lensflare/halo");
	mHaloSet->setCullIndividually(true);
	mHaloSet->setQueryFlags(VOIDOBJECT);	// They should not be detected by rays.*/
		Parent::build();
		//attachObject(mCamera);
		mBillboardSet->setQueryFlags(SceneQueryFlag::IMMATERIAL);
		mBillboardSet->setRenderQueueGroup(mLevelSegment->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+getRenderQueueDelta());
		
	}
	void Billboard::destroy(){
		Parent::destroy();
		//detachObject(mCamera);
	}

	void Billboard::show(){
		Parent::show();
	}
	void Billboard::setVisible(bool vis){
		if (mBillboardSet != NULL){
			mBillboardSet->setVisible(vis);
		}
	}


	bool Billboard::update(double timeDelta, double parentTime){
		return Parent::update(timeDelta, parentTime);
		//return false;
		/*Ogre::TransformKeyFrame tmp = mTrack->getInterpolatedKeyFrame(mLevelSegment->getTimeTrack()-getTimeOn());
		Degree d;
		d = tmp.getScale().x;
		Radian r = d;
		mCamera->setFOVy(r);

		mShake.z += mShakeSpeed.z*mLevelSegment->getTimeDelta();
		mShakeSpeed.z -= mLevelSegment->getTimeDelta()*(mShake.z*30);
		mShakeSpeed.z /= 1+(mLevelSegment->getTimeDelta()*5);
			
		if (mShake.z < -.2){
			mShake.z = -.2;
			mShakeSpeed.z = 0-mShakeSpeed.z;
		}else{
			if (mShake.z > .2){
				mShake.z = .2;
				mShakeSpeed.z = 0-mShakeSpeed.z;
			}
		}
		mSceneNode->roll((Radian)mShake.z);
		mSceneNode->yaw((Radian)mShake.z/10);
		mSceneNode->pitch((Radian)mShake.z/10);


		mSceneNode->translate(mDebugDelta,Ogre::Node::TS_LOCAL);
		mSceneNode->yaw((Radian)((Degree)(mDebugRotDelta.y)).valueRadians(),Ogre::Node::TS_LOCAL);
		//mOgreCam->lookAt(0,0,0);*/
	}

	/*void Billboard::addFOVToKeyframe(float fov){
		Ogre::Vector3 v;
		v.x = fov;
		v.y=1;
		v.z=1;
		mTempKeyFrame->setScale(v);
		//updateRefinedTrack();
	}*/

	void Billboard::attachToBone(Ogre::Entity* ent, string bone){
		Ogre::Vector3 scale = mSceneNode->getScale();
		//scale.x = 20;
		//scale.y = 20;
		//scale.z = 20;
		
		if (mBillboardSet->getParentSceneNode() != NULL){
			mBillboardSet->getParentSceneNode()->detachObject(mBillboardSet);
		}
		
		Ogre::Quaternion q((Ogre::Radian)(Ogre::Math::PI/2), Vector3(1,0,0));

		//Ogre::Bone* rbone = ent->getSkeleton()->getBone(bone);
		/*Ogre::Bone* tbone = rbone->createChild(ent->getSkeleton()->getNumBones());
		tbone->setScale(scale);*/

		ent->attachObjectToBone(bone, mBillboardSet,q);
		//ent->getSkeleton()->getBone(bone)->getParent()->setScale(scale);
		mBillboardSet->getParentNode()->setScale(scale);
		
		if (mSceneNode->getParentSceneNode() != NULL){
			mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
		}
		// WARNING : Scene Node remains floating around... for no reason

		
		//mLevelSegment->getSceneManager()->destroySceneNode(mSceneNode->getName());


		//mSceneNode = mEntity->getParentNode();
		//mEntity->getParentNode()->setScale(2,2,2);
	//	mSceneNode = NULL;


		

		
		//mEntity->attachToBone(ent, bone);
	}

}
