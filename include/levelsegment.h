// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___LEVELSEGMENT_H___
#define ___DEF___LEVELSEGMENT_H___

//#include "main.h"
#include "types.h"
#include "util/xml.h"
#include "util/profiler.h"

//#include "gameobject.h"
//#include "level.h"

namespace Shmoulette{

typedef struct TAG_FOG_INFO{
	Ogre::FogMode mode;
	Ogre::ColourValue colour;
	Ogre::Real expValue;
	Ogre::Real linearStart;
	Ogre::Real linearEnd;
}FOG_INFO;

/*#define ZONE_X 96
#define ZONE_Y 64
#define ZONE_Z 96*/
class ZoneInfo{
public:
	struct{
		Ogre::Vector3 current;
		Ogre::Vector3 wanted;
		Ogre::Vector3 delta;
	}mSize, mAmmoSize;
	struct{
		Ogre::Vector2 current;
		Ogre::Vector2 wanted;
		Ogre::Vector2 delta;
	}mCamRange, mCamTilt;
	double eta;
};


using namespace Shmoulette;


class _ShmouletteExport LevelSegment{
	private:
		void _preloadMaterial(string materialName);
		void _moveAddOnsToObj();
		void _destroyEverything();


	protected:


		struct{
			Profiler::ProfilerEntry* update;
			Profiler::ProfilerEntry* postFrame;
			
			// These all occur in postFrame
			Profiler::ProfilerEntry* collision;
			Profiler::ProfilerEntry* objects;

		}mProfiler;
		RAIL_TYPE mRailType;
		double mTimeAbsolute; // That's the actual time since mission start.  No trigger can change it, it is absolute
		double mTimeTrack; // That's the time rail objects and enemy spawn will occur.  May be affected by triggers
		double mTimeDelta; // Time since last frame
		double mLength; // Length of the level, when Timetrack reaches Length, the segment is over
		class RailHook* mCurrentHook;

		//vector<Terrain*>vectorTerrain;

		// The bool represents permanency.  If it is true, when a level is reset that object remains (scenery elements, mostly).  If false, object is removed from vector (ammos, explosions etc.)
		vector<pair<class GameObject*, bool>>mVectorGameObj;
		vector<pair<class GameObject*, bool>>mVectorGameObjAddon;
		vector<pair<class GameElement*, bool>>mVectorGameElement;

		vector<class RailHook*>mVectorRailHook; // Rails get added to the vectorRail IN ADDITION to the GameObjRail
		vector<class Trigger*>mVectorTrigger;
	//	vector<TalkWindow*>vectorTalkWindow;
		vector<class DroneWave*>mVectorDroneWave;
		vector<class GameObject*>mVectorCollision; // List of items that require collision detection this frame
		class Level* mLevel;

		ZoneInfo mZoneInfo;		
		
		class World* mWorld;
		Ogre::SceneNode* mContainer;

		vector<class GameObject*>target[2];

		list<Ogre::Entity*> mPreloadEntityBuffer;
		list<Ogre::ParticleSystem*> mPreloadParticleBuffer;
		//vector<StaticObj*>vectorStaticObj;
		//vector<DynamicObj*>vectorDynamicObj;
		//vector<class Camera*>vectorCameraObj;

		Ogre::SceneManager* mSceneMgr;

		class Camera* mActiveCamera;
		string mName;

		void hookContainerToFirstAvailableRail();

		list<string>mResourceLocation;
		
		bool mForceSecondUpdate;

	public:
		Ogre::Entity* addPreloadEntity(string s);
		void addPreloadParticleSystem(string s); // Can't clone a particle system, so it's useless to return anything.
		list<Ogre::Entity*> getEntityList();
		void fogChange(FOG_INFO newFog, Ogre::Real timer);
		void addCollisionTest(GameObject* object);
		void removeCollisionTest(GameObject* object);
		void buildStartObjects();
		void viewerCamControl(double timeDelta);
		void Pause();
		void UnPause();
		void reset();
		GameObject* getObjectBySceneNode(Ogre::SceneNode* node);

		list<class Drone*> getActiveDronesList();
		//void destroyViewport();

		ZoneInfo* getZoneInfo(){return &mZoneInfo;}

		void setZoneInfo(Ogre::Vector3 size, Ogre::Vector3 ammoSize, Ogre::Vector2 camSize,Ogre::Vector2 camTilt,double eta);

		Ogre::Vector3 getZoneSize(){ return mZoneInfo.mSize.current; }
		Level* getLevel(){
			return mLevel;
		}
		void forceSecondUpdate(){mForceSecondUpdate=true;}
		string getName(){return mName;}
		//void setTimes(GameElement* elem, char* sendString, XML* xml);

		static bool isTimeInFrame(double time, double endTime,double timeDelta){
			if (timeDelta > 0){
				if (time <= endTime && time >= endTime-timeDelta){
					return true;
				}
			}else if (timeDelta < 0){
				if (time >= endTime && time <= endTime-timeDelta){
					return true;
				}
			}else{
				if (endTime == time) return true;
			}
			return false;
		}
		bool isTimeInFrame(double time){
			if (mTimeDelta > 0){
				if (time <= mTimeTrack && time >= mTimeTrack-mTimeDelta){
					return true;
				}
			}else if (mTimeDelta < 0){
				if (time >= mTimeTrack && time <= mTimeTrack-mTimeDelta){
					return true;
				}
			}else{
				if (mTimeAbsolute == time) return true;
			}
			return false;
		}
		Ogre::SceneManager* getSceneManager(){return mSceneMgr;}
		void setSceneManager(Ogre::SceneManager* sm){mSceneMgr = sm;}

		double getTimeAbsolute(){
			return mTimeAbsolute;
		}
		double getTimeDelta(){
			return mTimeDelta;
		}
		double getTimeTrack(){
			return mTimeTrack;
		}
		void setTimeAbsolute(double t){
			update(t-mTimeAbsolute);
		}
		void setTimeTrack(double t){
			mTimeTrack = t;
		}
		void breakCurrentLoop();
		GameObject* getObjectByName(string name);
		Ogre::SceneNode* getContainer();
		void addGameObj(GameObject* obj, bool permanent = false);
		void addGameElement(GameElement* obj, bool permanent = false){
			pair<GameElement*, bool> a;
			a.first = obj;
			a.second = permanent;
			mVectorGameElement.push_back(a);
		}
		GameObject* getCurrentHook();

		bool hasHooks(){
			return true;
			//return mVectorRailHook.size() > 0;
		}

		//COLLISION_RESULT collisionTest(GameObject* obj, IFF iff);
		
		LevelSegment();
		void build(XML* xml, string name, double length);
		void append(XML* xml, string name, double length);
		void pushObjects(XML* xml, double deltaTime = 0);
		void load();
		void unload();
		void destroy();
		~LevelSegment();
		//void init();
		double update(double time); // If non-zero then it is the number of seconds that weren't computed, they should be passed on to the next segment
		void postFrameUpdate();
		//void trigger(TRIGGER_TYPE type);

		//void buildFromXML();

		class Camera* getActiveCamera();
		void setActiveCamera(Camera* c);
		


		void setRailType(RAIL_TYPE type){
			mRailType = type;
		}
		RAIL_TYPE getRailType(){
			return mRailType;
		}
		World* getWorld(){return mWorld;}
		Ogre::Vector3 leadDelta(int time);
};

}

#endif
