// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___CAMERA_H___
#define ___DEF___CAMERA_H___

//#pragma message("CAMERA_H")


//#define USING_GLOBAL_CAM_VP
#include "gameobject.h"

//#include "dynamicobj.h"
namespace Shmoulette{

class _ShmouletteExport Camera:public GameObject{
	typedef GameObject Parent;
	protected:
		Coordinator::CameraCoordinator* mCameraCoordinator;

		//SOPCameraTransformTrack* mCameraTrack;
		//static Ogre::Camera* mOgreCam;
	//float mCurrentFOV;
		Ogre::Vector3 mShake;
		Ogre::Vector3 mShakeSpeed;
		//Ogre::SceneNode* mOneHundredAndEightyDegreesNode;
		RefinedNumericTrack* mFOV;
		bool mFlipCam;
#ifndef USING_GLOBAL_CAM_VP
	Ogre::Camera* mCamera;
	//Ogre::Viewport* mViewport;
	//int mViewportZ;
	struct{
		float x, y, w, h;
	}viewport;
#endif

	bool mDebugMode;
	Ogre::Vector3 mDebugDelta;
	Ogre::Vector3 mDebugRotDelta;

	float mNearClip;
	float mFarClip;

	public:
		/*static void setOgreCam(Ogre::Camera* ogreCam){
			mOgreCam = ogreCam;
		}*/
#if 0
		void setKeyFrame();
#endif
		void debugPush(float x, float y, float z);
		void debugRotPush(float x, float y, float z);
		virtual void init(XML* xml);
		//Camera();
		virtual ~Camera();
		void setView(float x, float y, float w, float h);
		
		//virtual void addFOVToKeyframe(float fov);
#ifndef USING_GLOBAL_CAM_VP
		Ogre::Camera* getCamera(){return mCamera;}
		//virtual void setViewportZ(int z){mViewportZ = z;}
		//virtual Ogre::Viewport* getViewport(){return mViewport;}
#else
		Ogre::Camera* getCamera(){return Level::getSingleton()->getCamera();}
		virtual void setViewportZ(int z){}
		virtual Ogre::Viewport* getViewport(){return Level::getSingleton()->getViewport();}
#endif


		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual void hide();
		virtual void updateFromCoordinator(bool force = false);
		virtual bool update(double timeDelta, double parentTime);
#if 0
		virtual void setFOV(float val, bool maintenance = true);
		virtual void moveFOV(float val, bool maintenance = true);
#endif
		//virtual float getFOV(int time=GameElement::getFrameNb()){ return mFOV.getPoint(time);*/ }
#if 0
		virtual float getFOV(float time=FLT_MAX){ 
			if (time == FLT_MAX) mLevelSegment->getTimeTrack();
			return mTrack->getInterpolatedKeyFrame(time).getScale().x; 
		}
#endif
		virtual void shake(float strength = 5);
		//virtual void setPoint(int time, float x, float y, float z, float tx, float ty, float tz, float roll, float fov);
		virtual void setHardYaw(float yaw);
		virtual void setHardPitch(float pitch);
		virtual void setHardPos(float x, float y);
		virtual Ogre::MovableObject* getMovableObject(){return mCamera;}

};
}


#endif
