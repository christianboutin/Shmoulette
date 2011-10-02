#include "stdafx.h"

#include "elements\camera.h"

namespace Shmoulette{
	//static int zOrderIncremental = 5;
	using namespace std;
	using namespace Ogre;

	void Camera::init(XML* xml){
		Parent::init(xml);
		string pathFilename = xml->getString("path_file");
/*		if (xml->getError() != XML_OK){
			mTrack = new KeyFramedTransformTrack(xml);
			mFlipCam = false;
		}else{
			mCameraTrack = new SOPCameraTransformTrack(xml);
			mCameraTrack->readAllKeyFrames();
			mTrack = mCameraTrack;
			mFlipCam = false;
		}*/

		destroyCollisionStructure();
		
//		setCollisionGroup(COLLGROUP_ALL);

		mNearClip = xml->getFloat("nearclip");
		if (xml->getError() != XML_OK){
			mNearClip = .5;
		}
		mFarClip = xml->getFloat("farclip");
		if (xml->getError() != XML_OK){
			mFarClip = 1500000;
		}

		//zOrderIncremental--;

#ifndef USING_GLOBAL_CAM_VP
		//mViewportZ = 1;//zOrderIncremental;
		//viewport.x = 0;
		//viewport.y = 0;
		//if (mViewportZ == 11){
			//viewport.w = 1;
			//viewport.h = 1;
		//}else{
			//viewport.w = .5;
			//viewport.h = .5;
		//}
#endif
		mType = OBJ_TYPE_CAMERA;

		mShakeSpeed.x = 0;
		mShakeSpeed.y = 0;
		mShakeSpeed.z = 0;
		mShake.x = 0;
		mShake.y = 0;
		mShake.z = 0;
/*		if (mLegacyTrack){
			mFOV = new RefinedNumericTrack(xml,"fov");
			if (mFOV->getNumKeyFrames() == 0){
				mFOV->setKeyFrameValue(0,60);
			}
		}*/
		mDebugMode = false;
		mDebugDelta.x = 0;
		mDebugDelta.y = 0;
		mDebugDelta.z = 0;
		mDebugRotDelta.x=0;
		mDebugRotDelta.y=0;
		mDebugRotDelta.z=0;
#ifndef USING_GLOBAL_CAM_VP
		mCamera = mLevelSegment->getSceneManager()->createCamera(mId+"_ogrecam");
#endif
		if (getTimeOn() == 0){
			mLevelSegment->setActiveCamera(this);
		}
/*		mOneHundredAndEightyDegreesNode = mSceneNode->createChildSceneNode();
		string s = xml->getString("flipNode");
		if (xml->getError() == XML_OK){
			if(boolFromString(s)== true){
				mOneHundredAndEightyDegreesNode->setDirection(0,0,1);
			}
		}*/
		//mOneHundredAndEightyDegreesNode->yaw((Ogre::Radian)(Math::PI/2));

		string transform_c = xml->getString("camera_c");
		if (xml->getError() == XML_OK){
			mCameraCoordinator = dynamic_cast<Coordinator::CameraCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get(transform_c));
		}else{
			mCameraCoordinator = NULL;
			throw("Camera requires a camera_c (coordinator)");
			//mCameraCoordinator = dynamic_cast<Coordinator::CameraCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get("macro:zero_transform"));
		}


	}
	Camera::~Camera(){
#ifndef USING_GLOBAL_CAM_VP
		mLevelSegment->getSceneManager()->destroyCamera(mCamera);
#endif
	}
	void Camera::setView(float x, float y, float w, float h){
		
#ifndef USING_GLOBAL_CAM_VP
		/*viewport.x = x;
		viewport.y = y;
		viewport.w = w;
		viewport.h = h;
		mViewport->setDimensions(x,y,w,h);*/
#endif
		/*DGSDK
		if (mIsBuild)
			dbSetCameraView(camNb, viewport.x1, viewport.y1, viewport.x2, viewport.y2);*/
	}

	void Camera::debugPush(float x, float y, float z){
		mDebugDelta.x+=x;
		mDebugDelta.y+=y;
		mDebugDelta.z+=z;
		Util::Log("DebugDelta"+ts(mDebugDelta.x),0);
	}

	void Camera::debugRotPush(float x, float y, float z){
		mDebugRotDelta.x+=x;
		mDebugRotDelta.y+=y;
		mDebugRotDelta.z+=z;
		Util::Log("DebugDelta"+ts(mDebugDelta.x),0);
	}

	void Camera::build(){
		mShakeSpeed.x = 0;
		mShakeSpeed.y = 0;
		mShakeSpeed.z = 0;
		mShake.x = 0;
		mShake.y = 0;
		mShake.z = 0;
		Parent::build();
		//
		//mSceneNode->attachObject(mCamera);

	}
	void Camera::destroy(){
		Parent::destroy();
	}

	void Camera::show(){
		Parent::show();
#ifndef USING_GLOBAL_CAM_VP
		//mViewport = Core::getSingleton()->getWindow()->addViewport(mCamera,mViewportZ,viewport.x, viewport.y, viewport.w, viewport.h);
#endif
		

		//mCamera->setDirection(0, 0, 1);
		//mCamera->yaw((Ogre::Radian)Math::PI/2);
#ifndef USING_GLOBAL_CAM_VP
		mCamera->setPosition(0,0,0);
		mCamera->setNearClipDistance(mNearClip);
		mCamera->setFarClipDistance(mFarClip);
		mSceneNode->attachObject(mCamera);
		//mOneHundredAndEightyDegreesNode->attachObject(mCamera);
#else
		Level::getSingleton()->getCamera()->setPosition(0,0,0);
		Level::getSingleton()->getCamera()->setNearClipDistance(mNearClip);
		Level::getSingleton()->getCamera()->setFarClipDistance(mFarClip);
		Level::getSingleton()->getCamera()->setAspectRatio(
			Ogre::Real(Level::getSingleton()->getCamera()->getViewport()->getActualWidth()) / 
			Ogre::Real(Level::getSingleton()->getCamera()->getViewport()->getActualHeight())
			);
		//Level::getSingleton()->getCamera()->setAutoAspectRatio(true);

		Ogre::Camera* cam = Level::getSingleton()->getCamera();
		if (cam->getParentSceneNode() != NULL){
			Level::getSingleton()->getCurrentSegment()->getActiveCamera()->hide();
			//throw("More than one camera share the same time slot");
		}
		if (mFlipCam == true){
			cam->setDirection(0,0,1);
		}else{
			cam->setDirection(0,0,-1);
		}
		if (isSceneNode()){
			mSceneNode->attachObject(Level::getSingleton()->getCamera());
		}else{
			throw("Cameras can't be attached to bones yet");
		}
		//mOneHundredAndEightyDegreesNode->attachObject(Level::getSingleton()->getCamera());
#endif
		mLevelSegment->setActiveCamera(this);
	}
	void Camera::hide(){
		Parent::hide();
		/*if (Level::getSingleton()->getViewport()->getCamera() == mCamera){
			Level::getSingleton()->getViewport()->setCamera(NULL);
		}*/
	#ifndef USING_GLOBAL_CAM_VP
		//mOneHundredAndEightyDegreesNode->detachObject(mCamera);
		//mSceneNode->detachObject(mCamera);
		//Core::getSingleton()->getWindow()->removeViewport(mViewportZ);
#else
		mSceneNode->detachObject(Level::getSingleton()->getCamera());
#endif

	}

	void Camera::updateFromCoordinator(bool force){
		Parent::updateFromCoordinator(true);
		if (mCameraCoordinator){
			if (mCameraCoordinator->hasChanged() || force){
				mCamera->setFOVy(Ogre::Radian(mCameraCoordinator->getValue(timeAlive())));
			}
		}
	}



	bool Camera::update(double timeDelta, double parentTime){
		Parent::update(timeDelta, parentTime);
		//Parent::updateFromCoordinator(true);
		//mCamera->set

		mShake.z += mShakeSpeed.z*timeDelta;
		mShakeSpeed.z -= timeDelta*(mShake.z*30);
		mShakeSpeed.z /= 1+(timeDelta*5);
			
		if (mShake.z < -.2){
			mShake.z = -.2;
			mShakeSpeed.z = 0-mShakeSpeed.z;
		}else{
			if (mShake.z > .2){
				mShake.z = .2;
				mShakeSpeed.z = 0-mShakeSpeed.z;
			}
		}
		if (mShakeSpeed.z == 0){
			mShake.z = 0;
		}
		mSceneNode->roll((Radian)mShake.z);
		mSceneNode->yaw((Radian)mShake.z/10);
		mSceneNode->pitch((Radian)mShake.z/10);


		mSceneNode->translate(mDebugDelta,Ogre::Node::TS_LOCAL);
		mSceneNode->yaw((Radian)((Degree)(mDebugRotDelta.y)).valueRadians(),Ogre::Node::TS_LOCAL);

		//mOgreCam->lookAt(0,0,0);

		return false;
	}

	/*void Camera::addFOVToKeyframe(float fov){
		Ogre::Vector3 v;
		v.x = fov;
		v.y=1;
		v.z=1;
		mTempKeyFrame->setScale(v);
		//updateRefinedTrack();
	}*/


	void Camera::setHardYaw(float yaw){
		//mOgreCam->setDirection(0,0,-1);
		mSceneNode->yaw((Radian)yaw);
		//mSceneNode->roll((Radian)yaw);
		//mSceneNode->translate(Vector3(yaw*300, 0, 0));
	}
	void Camera::setHardPos(float x, float y){
#ifndef USING_GLOBAL_CAM_VP
		mCamera->setPosition(x, y, 0);
#else
		//mOneHundredAndEightyDegreesNode->setPosition(x,y,0);
		//Level::getSingleton()->getCamera()->setPosition(x, y, 0);
		mSceneNode->translate(x, y, 0,Ogre::Node::TS_LOCAL);
#endif
	}
	void Camera::setHardPitch(float pitch){
		mSceneNode->pitch((Radian)pitch);
	}

	void Camera::shake(float strength){
		//Util::Log("Camera Hit");
		
		if (Math::RangeRandom(0, 1) < .5){
			mShakeSpeed.z = Math::RangeRandom(-(strength*4/3), -(strength*3/4));
		}else{
			mShakeSpeed.z = Math::RangeRandom((strength*3/4), (strength*4/3));
		}
		//mShake.z = 0;

		/*if (mShakeSpeed.z < 0){
			mShakeSpeed.z -= Math::RangeRandom(0, 30);
		}else{
			mShakeSpeed.z += Math::RangeRandom(0, 30);
		}*/
		/*if (mShakeSpeed.z < -150){
			mShakeSpeed.z = -150;
		}
		if (mShakeSpeed.z > 150){
			mShakeSpeed.z = 150;
		}*/
	}
}
