// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___LEVEL_H___
#define ___DEF___LEVEL_H___

#include "levelsegment.h"
#include "gui/cinemabars.h"
#include "2D/hud.h"
#include "2D/sensoroverlay.h"
#include "sound.h"
#include "weapon/weapon.h"

//#define ALPHA_SCREENSHOT

using namespace Shmoulette;
namespace Shmoulette{

	class _ShmouletteExport WeaponChain{
		public:
			WeaponChain(){};
			WeaponChain(XML& xml);
			float mTimeBetweenShots;
			list<class WeaponInfo> mWeapon;
			void fromXml(XML& xml);
	};

	class _ShmouletteExport PlayerDetails{
		public:
			PlayerDetails(){};
			PlayerDetails(XML& xml);

			std::string mMesh;
			std::string mDll;
			float mShield;
			float mShieldRegen;
			float mArmour;
			float mEnergy;
			float mEnergyRegen;
			float mSpeed;
			int mAmmo;
			vector<WeaponChain> mWeaponChain;
			list<string>mReactorNodes;
			string mPointerNode;
			
			string mDeathFX;

			void fromXml(XML& xml);
	};

	typedef enum TAG_TIMER_RESULT{
		TIMER_RESULT_FATAL,
		TIMER_RESULT_TOTAL
	}TIMER_RESULT;


class _ShmouletteExport Level{
	Shmoulette::Sound* mLoadingSound;
	void _reset();
	void _resetStdVar();
	protected:
		bool mLogKeyUp;

		list<string> mDronePreloadList;
		struct{
			Profiler::ProfilerEntry* timeWithOgre;
			Profiler::ProfilerEntry* completeFrameCycle;
			Profiler::ProfilerEntry* postFrametimeWithOgre;
			bool started;
		}mProfiler;

		list<class LevelSegment*>::iterator mSegmentIterator;

		//vector<Ogre::CompositorInstance*> mCompositor;
		//class ProgressBar* mProgressBar;
		Hud* mHud;
		class Gui::CinemaBars* mCinemaBars;
		bool mNeedPostFrameUpdate;

		class InputBridge* mInputBridge;

		double mMaxFrameTime;
		double mMinFrameTime;
		//void openLevelFile(string filename);
		//void closeLevelFile();
		//File* mFile;
		//XML* mXml;
		/*float mLoadTimeDelta; // used at loadTime
		string mLoadTimeIdPrefix; // used at loadTime*/
		

		Ogre::Viewport* mViewport;

		//Ogre::Camera* mCamera;
		Ogre::Camera* mDebugCam;
		Ogre::Viewport* mDebugViewport;
		Ogre::SceneNode* mDebugCamNode;

		bool mFastForward;
		bool mReset;
		//Ogre::CompositorInstance* mFastForwardCompositor;

		list<class LevelSegment*>mSegment;
		class LevelSegment* mCurrentSegment;

		double mTimeDelta; // Time since last frame
		double mTimeAbsolute; // That's the actual time since mission start.  No trigger can change it, it is absolute
		double mTimeCatchup; // If a frame takes more than .06 of a second, update as if it took .06 of a second and tries to catch up
		list<class PlayerShip*> mPlayerShip; // Playership transcends level segments.  It must remain.

		bool mIsCinema;
		static class Level* mSingleton;

		list<string>mResourceLocation;
		list<string>mResource;
		int mNbResGroups;
		struct{
			float wanted;
			float current;
			float acceleration;
		}mSpeedModifier;

		Level();
		list<PlayerDetails> mPlayerDetails;
		string mHudDll;

		bool mEndSegmentFlag;

		bool mPaused;
		bool mDebugCamera;

		int mLoadedSegments;

		
		double mTimer;
		TIMER_RESULT mTimerResult;
		void _endSegment(double timeOverflow = 0);

		SensorOverlay* mSensorOverlay;

	public:
#ifdef ALPHA_SCREENSHOT
		Ogre::TexturePtr mRenderTexture;
		Ogre::RenderTexture* mRenderTextureTarget;
#endif
		static PyObject* getTimeTrack(PyObject *self, PyObject* args);
		static PyObject* getTimeDelta(PyObject *self, PyObject* args);
		static PyObject* getObjectPtr(PyObject *self, PyObject* args);
		static PyObject* getPlayerShipObjectPtr(PyObject *self, PyObject* args);
		static PyObject* setSecondLifeHud(PyObject *self, PyObject* args);
		static PyObject* setSkyBox(PyObject *self, PyObject* args);
		static PyObject* addXmlBranch(PyObject *self, PyObject* args);
		SensorOverlay* getSensorOverlay(){return mSensorOverlay;}
		void resetTimeCatchup(){mTimeCatchup = 0;}

		void registerPython();
		double getMaxFrameTime(){return mMaxFrameTime;}
		double getMinFrameTime(){return mMinFrameTime;}
		bool isPaused(){
			return mPaused;
		}

		void pause();
		void unPause();

		void debugCameraToggle();

		void reset(); // Returns to the beginning of the level (for when you die)

		void DronePreloadAdd(string s);


		double getTimerTime(){return mTimer;}
		TIMER_RESULT getTimerResult(){return mTimerResult;}

		void setTimerTime(double timer){mTimer = timer;}
		void getTimerResult(TIMER_RESULT  tr){mTimerResult = tr;}

		list<Ogre::Entity*> getEntityList();


		Ogre::Camera* getCamera();
		Ogre::Viewport* getViewport(){return mViewport;}
		void setCamera(Ogre::Camera* cam);
		Hud* getHud(){return mHud;}

		void setHudDll(string dllName){mHudDll = dllName;}
		void removeAllPlayers();
		void addPlayer(PlayerDetails pd);
		void addSegment(string filename, string context, string iD, float length, bool append);
		void addSegment(XML* xml, string iD, float length, bool append);

		void initCameraAndViewport();
		void uninitCameraAndViewport();

		void endSegment(){
			mEndSegmentFlag = true;
		}
		
		void addSegment(string s);
		void addResourceContext(string context, string parentContext="", string groupName="Level");
		void addResource(string name, string type, string groupName="Level");

		Ogre::Root* getRoot(); // Because a DLL needed it.  I know it's a bad call, sue me.
		static Level* getSingleton(){
			if (mSingleton == NULL){
				mSingleton = new Level();
			}
			return mSingleton;
		}
		void setSpeedModifier(float wanted, float acceleration){
			mSpeedModifier.wanted = wanted;
			mSpeedModifier.acceleration = acceleration;
		}
		double getCurrentSpeedModifier(){
			if (isPaused()){
				return 0;
			}
			return mSpeedModifier.current;
		}
		Ogre::CompositorInstance* addCompositor(string type, string params="");
		void removeCompositor(string type);

		//void removeCompositor(Ogre::CompositorInstance* oI);


	class LevelSegment* getCurrentSegment(){return mCurrentSegment;}

	int getNbSegments(){return mSegment.size();}
	int getNbLoadedSegments(){return mSegment.size();}

	void score(int score, GameObject* scorer);
	bool isCinema(){return mIsCinema;}
	bool isFastForward(){return mFastForward;}
	void cinema(bool cinema);
	//Ogre::SceneNode* getContainer(){return mContainer;}

	//double getTimeAbsolute(){return mTimeAbsolute;}
	//double getTimeDelta(){return mTimeDelta;}
	//double getTimeTrack(){return mCurrentSegment->getTimeTrack();}


	//float getTimeDelta(){return (*mCurrentSegment)->getTimeDelta();}

	virtual void update(double timeDelta);
	virtual void postFrameUpdate();
	virtual void init(string filename);
	virtual void uninit();

	virtual void load();
	virtual void unload();
	virtual void destroy(); // Destroys all segments and the player ship, readies for another load

	Ogre::SceneManager* getSceneManager();

	PlayerShip* getPlayerShip();
};

}

#endif
