#ifndef ___DEF___FX_H___
#define ___DEF___FX_H___

#include "elements\gameobject.h"

// An FX structure doesn't do anything that can't be done through standard gameobject inheritance.  It is way less versatile but is built from speed
// * No XML allowed
// * Only 1 scenenode with a series 
// * No children
// * First iteration is nothing more than a chain of particle effects.  More to come as necessary (like sound);

namespace Shmoulette{

	class DebrisInfo{
			list<string>mUnique;
			list<string>mGeneric;
			pair<int,int>mQuantity;
			bool mGravity;
			float mSpin;
			float mSpeed;
		public:
			void spawnDebris(Ogre::Vector3 v);
			void preload(LevelSegment* ls);
			DebrisInfo(XML& xml);
	};

	class FXDBE{
		public:
			string mId;

			struct{
				string mMesh;
				Ogre::Vector3 mStartScale;
				Ogre::Vector3 mStartRotation;
				Ogre::Vector3 mAnimScale;
				Ogre::Vector3 mAnimRotation;
				float mTime;
			}mCoreMesh;

			list<string>mParticleSystem;
			list<DebrisInfo>mDebrisInfo;
			string mSound;

			void preload(LevelSegment* ls);

			FXDBE(string filename);
	};

	class FXDB{
		static FXDB* mSingleton;
		list<FXDBE*> mEntry;
	private:
		FXDB();
	public:
		static FXDB* getSingleton(){
			if (mSingleton == NULL){
				mSingleton = new FXDB();
			}
			return mSingleton;
		}
		FXDBE* getEntry(string label){
			list<FXDBE*>::iterator it;
			for (it= mEntry.begin();it!=mEntry.end();it++){
				if ((*it)->mId == label){
					return (*it);
				}
			}
			FXDBE* rv = new FXDBE(label);
			mEntry.push_back(rv);
			return rv;
		}
		~FXDB(){
			list<FXDBE*>::iterator it;
			for (it= mEntry.begin();it!=mEntry.end();it++){
				delete (*it);
			}
		}
	};

	class _ShmouletteExport FX:public GameObject{
		typedef GameObject Parent;
		class FXDBE* mFXDbe;
		list<Ogre::ParticleSystem*> mParticleSystem;
		float mTimeLeft;
		Sound* mSound;
		string mBone;
		GameObject* mParentGameObject;

		bool mUseParentSceneNode;

		Ogre::SceneNode* mParentSceneNode;

		public:
			FX(){};
			virtual void init(string name);
			virtual void init(PyObject* pyObject);
			virtual Ogre::Node* getNode();
			virtual void show();
			virtual void hide();
			virtual void setParent(GameObject* go, string bone);
			virtual void setParent(Ogre::SceneNode* scenenode);
			virtual void build();
			virtual void activate(); // Resets timeline to zero, returns all particle emitter to 0 timeline
			virtual void deactivate(); // Disables all particle emitters and hides mesh.  If particle emitter has duration/loop it may reappear
			virtual void destroy();
			virtual bool update(double timeDelta, double parentTime);
			virtual void setScale(Ogre::Vector3 s);
			virtual void limitParticleZone(Ogre::AxisAlignedBox b);
			virtual bool isDone();
	};

};

#endif
