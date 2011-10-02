// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___GAMEOBJECT_H___
#define ___DEF___GAMEOBJECT_H___

#include "gameelement.h"
#include "types.h"
#include "collisionelement.h"
#include "goanims\goanim.h"
#include "python.h"
#include "weaponsupply.h"
#include "util\refinedtrack.h"
#include "coordinator\coordinator.h"

//#include "OgreOpcode.h"

/*CLUSAPI_READ_ACCESS 0x00000001L
#define CLUSAPI_CHANGE_ACCESS   0x00000002L
#define CLUSAPI_NO_ACCESS       0x00000004L
#define CLUSAPI_ALL_ACCESS (CLUSAPI_READ_ACCESS | CLUSAPI_CHANGE_ACCESS)*/


namespace Shmoulette{

	/*typedef enum{


	}GoRequestType;

	class GORequest{
		GoRequestType type;
	};*/
	/*typedef unsigned int COLLGROUP;

	// NONE collides with everything, ALL with nothing
	// Objects in the same group DON'T collide with one-another

	#define	COLLGROUP_NONE		0x00000000
	#define	COLLGROUP_SCENERY	0x00000001
	#define	COLLGROUP_FRIENDLY	0x00000002
	#define	COLLGROUP_ENEMY		0x00000004
	#define	COLLGROUP_BULLET	0x80000000
	#define	COLLGROUP_ALL		0xFFFFFFFF*/

	// Remember that Queries are valid as soon as the result is non-zero.  AMMO = AMMO_FRIENDLY|AMMO_ENEMY and not AMMO_FRIENDLY = AMMO|FRIENDLY.
	namespace SceneQueryFlag{
		enum SceneQueryFlag{
			IMMATERIAL = 0xFFFFFFFF,
			INANIMATE = 1<<0,
			AMMO_FRIENDLY = 1<<1,
			AMMO_ENEMY = 1<<2,
			AMMO = AMMO_FRIENDLY|AMMO_ENEMY,
			DRONE_FRIENDLY = 1<<3,
			DRONE_ENEMY = 1<<4,
			DRONE = DRONE_FRIENDLY|DRONE_ENEMY,
			PLAYER_SHIP = 1<<5,
			ALL_NPC = AMMO|DRONE|INANIMATE,
			ALL_MATERIAL = PLAYER_SHIP|AMMO|DRONE|INANIMATE
		};
	}


typedef enum TAG_DYN_OBJ_ANIM_TYPE{
	PLAY_SPEED,
	GOTO_AND_STOP,
	GOTO_AND_PLAY,
	REVERSE_PLAY,
	STOP,
	PLAY,
	DISABLE_LOOP,
	ENABLE_LOOP,
	DISABLE,
	ENABLE
}DYN_OBJ_ANIM_TYPE;

typedef enum TAG_DYN_OBJ_ANIM_STATUS{
	PLAYING,
	STOPPED,
	PLAYING_REVERSE,
	STOPPED_REVERSE
}DYN_OBJ_ANIM_STATUS;


typedef struct TAG_DYNAMIC_OBJ_ANIM{
	string animName;
	Ogre::AnimationState* animState;
	DYN_OBJ_ANIM_STATUS status;
	float speedMultiplier;
}DYNAMIC_OBJ_ANIM;


typedef struct TAG_DYNAMIC_OBJ_ANIM_TRIG{
	DYN_OBJ_ANIM_TYPE type;
	float time;
	float dest;
	DYNAMIC_OBJ_ANIM* anim;
	bool play;
}DYNAMIC_OBJ_ANIM_TRIG;


	typedef enum TAG_OBJ_TYPE{
		OBJ_TYPE_BOX,
		OBJ_TYPE_CLOUDZONE,
		OBJ_TYPE_CUBE,
		OBJ_TYPE_GROUNDPLANE,
		OBJ_TYPE_LIGHT,
		OBJ_TYPE_CONTAINER,
		OBJ_TYPE_RAIL,
		OBJ_TYPE_LOADED_MESH,
		OBJ_TYPE_INSTANCE,
		OBJ_TYPE_CLONE,
		OBJ_TYPE_NOMESH,
		OBJ_TYPE_PARTICLE,
		OBJ_TYPE_BILLBOARD,
		OBJ_TYPE_WATERZONE,
		OBJ_TYPE_EMPTY,
		OBJ_TYPE_CAMERA
	}OBJ_TYPE;

	typedef enum TAG_DAMAGE_TYPE{
		DAM_TYPE_CONCUSSION,
		DAM_TYPE_CUT,
		DAM_TYPE_PIERCE,
		DAM_TYPE_FIRE,
		DAM_TYPE_WATER,
		DAM_TYPE_ELECTRICITY,
		DAM_TYPE_FROST,
		DAM_TYPE_EMP
	}DAMAGE_TYPE;

	typedef enum TAG_DAMAGE_TOTAL{
		DAM_TOTAL_ADD,
		DAM_TOTAL_HIGHEST
	}DAMAGE_TOTAL;
	
	typedef enum TAG_DAMAGE_CYCLE{
		DAM_CYCLE_INSTANT,
		DAM_CYCLE_CONTINUOUS
	}DAMAGE_CYCLE;

	typedef struct TAG_DAMAGE{
		float amount;
		DAMAGE_TYPE type;
	}DAMAGE;

	class _ShmouletteExport DamagePackage{
		private:
			DAMAGE_TOTAL mDamageTotal;
			vector<DAMAGE>mDamage;
			DAMAGE_CYCLE mDamageCycle;
		public:
			DamagePackage();
			DamagePackage(PyObject* po);
			void addFromXml(XML* xml);
			void addFromDamagePackage(DamagePackage& pack);
			void copyFromDamagePackage(DamagePackage& pack);
			void addDamage(DAMAGE_TYPE type, float damage);
			void setDamageTotal(DAMAGE_TOTAL tot){mDamageTotal = tot;}
			void setDamageCycle(DAMAGE_CYCLE cyc){mDamageCycle = cyc;}
			DAMAGE_CYCLE getDamageCycle(){return mDamageCycle;}
			float getTotalDamage();
			vector<DAMAGE> getDamageVector();
	};

	typedef struct TAG_GAME_OBJECT_CHILD{
		string bone;
		GameObject* gameObject;
	}GAME_OBJECT_CHILD;

	typedef struct TAG_GAME_OBJECT_MAT_REPLACEMENTS{
		double time;
		string replace;
		string with;
	}GAME_OBJECT_MAT_REPLACEMENTS;
	
	typedef struct TAG_PYTHON_INSTANCE{
		PyObject* pythonInstance;
		bool hasUpdate;
		bool hasBuild;
		bool hasDestroy;
		bool hasShow;
		bool hasHide;
		PyObject* updateString;
		PyObject* buildString;
		PyObject* destroyString;
		PyObject* showString;
		PyObject* hideString;
	}PYTHON_INSTANCE;

	class ParentConnector{
		public:
			GameObject* mGameObject;
			//Ogre::Entity* mEntity;
			string mBone;
			bool mTransferPaintOver;
	};

	class LodInfo{
	public:
		string meshName;
		float distance;
	};
	class _ShmouletteExport GameObject:public GameElement{
		private:
			typedef GameElement Parent;
			//COLLGROUP mCollGroup;
			bool mTestCollisions;
			void initDefaultVariables();
			void materialReplace(string from, string to);
			//void setCollGroup(string str);
			Ogre::Entity* entityFromLod(int ndx);
			int mRenderQueueDelta;
		protected:


			Coordinator::TransformCoordinator* mTransformCoordinator;

			vector<class Weapon*>mWeapon;

			bool mSkipHook;
			DamagePackage mDamagePackage;
			Ogre::SceneNode* mSceneNode;
			void setRenderQueueGroup(int group, bool includeChildren=true);
			void setRenderQueueDelta(int delta){
				mRenderQueueDelta = delta;
			}
			int getRenderQueueDelta(){
				return mRenderQueueDelta;
			}
			
		string mFilename;
		Ogre::Entity* mRootEntity; // Clone that on build
		float mLife;
		void checkForChildren();
		void checkForInitScript();
		void checkForDamagePackage();

		int currentLod;
		vector<LodInfo> mLodInfo;

		Ogre::uint32 mSceneQueryFlags;
		Ogre::Entity* mEntity;

		//SceneQueryFlag::SceneQueryFlag mCollisionQueryFlags;
		string mChildrenFilename;
		string mInitParams;
		//string mResourcePath;
		bool mAllowPaintOver;
		//bool mBoneNodeMode;
		list<DYNAMIC_OBJ_ANIM>mAnim;
		list<DYNAMIC_OBJ_ANIM_TRIG>mAnimTrig; 
		list<GoAnim::GoAnim*>mGoAnim;

		/*virtual bool prequeueMovableObjectLodChanged(const Ogre::MovableObjectLodChangedEvent& evt);
        virtual void postqueueMovableObjectLodChanged(const Ogre::MovableObjectLodChangedEvent& evt);
        virtual void entityMeshLodChanged(const Ogre::EntityMeshLodChangedEvent& evt);
        virtual void entityMaterialLodChanged(const Ogre::EntityMaterialLodChangedEvent& evt);*/
        

		//IFF mIFF;

		vector<Ogre::AxisAlignedBox>mSectionBox;
		vector<Ogre::Sphere>mSectionSphere;

		//bool mStreamBuild;

		PYTHON_INSTANCE	mPythonInstance;

		CollisionStructure* mCollisionStructure;
		Shmoulette::GameObject* mCollisionForward; // If "this" is hit, then the hit will be forwarded to the mCollisionBypass object.  Like if the turret on a cruiser is hit, it may be forwarded to the cruiser.
		//list<CollisionElement> mCollisionElement;

		//OgreOpcode::CollisionContext* mCollideContext;
		//OgreOpcode::CollisionObject* mCollObj;

		Ogre::Vector3 mBBMin, mBBMax;

		Ogre::Bone* mRootBone;// If an entity has a bone called "root", the bounding box will follow that bone.  If it's null, bounding box will follow the sceneNode.

		Ogre::Vector3 mInertia;
		Ogre::Vector3 mOldPos;
		
		/*list<Ogre::Entity*> mChildEntity;
		list<Ogre::SceneNode*> mChildSceneNode;*/
		vector<GAME_OBJECT_MAT_REPLACEMENTS> mMatReplace;
		list<GAME_OBJECT_CHILD> mChild;
		list<GAME_OBJECT_CHILD> mChildStandBy; // This contains children that were destroyed before their time, they're put there so they don't get rebuild on the next frame, but kept in case the level resets.
		list<GAME_OBJECT_CHILD> mChildAttachedToBones;

		ParentConnector mParentConnector;

		//GameObject* mDependent; // If an object is dependent, for collision and damage purposes, on it's parent Gameobject, then the parent is in this variable.
		//bool mDoNotHook;

		bool mIsPaintedOver;
		list<Ogre::MaterialPtr> mBackupMaterial;

		OBJ_TYPE mType;
		OBJ_TYPE getType(){return mType;}

		//float mSize[3];

		bool mHooked;
		//class RailHook* mHook;
		void attachObject(Ogre::MovableObject* obj){
			mSceneNode->attachObject(obj);
		}
		void detachObject(Ogre::MovableObject* obj){
			mSceneNode->detachObject(obj);
		}

		GameObject* mOwner; // If the object was launched by some other object, that is it.  Mainly used for scoring.

		bool mCastShadows;
		string mSkin;
		bool mForcedUpdate; // Was the scene node of this objected forced to update this frame?
		
		public:
			void loadCollisionData(string filename="");
			LevelSegment* getLevelSegment(){return mLevelSegment;}
			static SceneQueryFlag::SceneQueryFlag queryFlagsFromIff(string iff);
			
			//Python Bindings
			static PyObject* setVisible(PyObject *self, PyObject* args);
			static PyObject* getChild(PyObject *self, PyObject* args);
			static PyObject* timeAlive(PyObject *self, PyObject* args);
			static PyObject* movePos(PyObject *self, PyObject* args);
			static PyObject* moveRot(PyObject *self, PyObject* args);
			static PyObject* moveScl(PyObject *self, PyObject* args);
			static PyObject* setPos(PyObject *self, PyObject* args);
			static PyObject* setRot(PyObject *self, PyObject* args);
			static PyObject* setScl(PyObject *self, PyObject* args);
			static PyObject* getPos(PyObject *self, PyObject* args);
			static PyObject* getRot(PyObject *self, PyObject* args);
			static PyObject* getScl(PyObject *self, PyObject* args);
			static PyObject* setColour(PyObject *self, PyObject* args);
			static PyObject* cloneMaterials(PyObject *self, PyObject* args);
			static PyObject* animTrigger(PyObject *self, PyObject* args);
			static PyObject* takeDamage(PyObject *self, PyObject* args);
			static PyObject* railToOrbit(PyObject *self, PyObject* args);
			static PyObject* attach(PyObject *self, PyObject* args);
			static PyObject* attachHook(PyObject *self, PyObject* args);
			static PyObject* createChild(PyObject *self, PyObject* args);
			static PyObject* addWeapon(PyObject *self, PyObject* args);
			static PyObject* addGoAnim(PyObject *self, PyObject* args);
			static PyObject* addAnim(PyObject *self, PyObject* args);
			static PyObject* addMaterialReplace(PyObject *self, PyObject* args);
			//static PyObject* setCollGroup(PyObject *self, PyObject* args);
			static PyObject* setShadows(PyObject *self, PyObject* args);
			static PyObject* getLife(PyObject *self, PyObject* args);
			static PyObject* setLife(PyObject *self, PyObject* args);
			static PyObject* getChildren(PyObject *self, PyObject* args);
			static PyObject* setTimes(PyObject *self, PyObject* args);
			static PyObject* setRenderQueue(PyObject *self, PyObject* args);
			static PyObject* fire(PyObject *self, PyObject* args);
			static PyObject* ceaseFire(PyObject *self, PyObject* args);

			static PyObject* activate(PyObject *self, PyObject* args); // Only for FX, but needs to be here
			//static PyObject* setAlpha(PyObject *self, PyObject* args);
			static void registerPython();
			void setPythonCallback(string s);
			//Python Bindings end
			void setTransformCoordinator(Coordinator::TransformCoordinator* newCoord);
			void setTransformCoordinator(string s);
			Ogre::Vector3 getInertia(){return mInertia;}
			void addMaterialReplace(string from, string to, float time=0);
			void addDamage(DAMAGE_TYPE type, float dmg);
			DamagePackage& getDamagePackage(){return mDamagePackage;}
			virtual double timeAlive(){return Parent::timeAlive();}
			//void addChildren(char* binaryContent);
			void addChildren(XML* xml);
			virtual void addChild(XML& xml);
			virtual void addChild(GAME_OBJECT_CHILD c);
			GameObject* createChild(string type, string bone, string label, string params);
			DYN_OBJ_ANIM_TYPE animTypeFromString(string s);
			DYNAMIC_OBJ_ANIM* animFromString(string s);
			void addAnim(XML* xml);
			
			void addGoAnim(XML* xml);
			//void addGoAnim(string goanim, string params="");
			void addGoAnim(string goanim, GoAnimParams& p);

			void animTrigger(DYNAMIC_OBJ_ANIM_TRIG& it);
			//void setStreamBuild(bool sb){mStreamBuild = sb;}
			//void setCollisionQueryFlags(SceneQueryFlag::SceneQueryFlag flag);// What does that thing collides with?
			//SceneQueryFlag::SceneQueryFlag getCollisionQueryFlags(){return mCollisionQueryFlags;}// What does that thing collides with?
			void setQueryFlags(SceneQueryFlag::SceneQueryFlag flag);
			SceneQueryFlag::SceneQueryFlag getQueryFlags(){
				if (mEntity){
					return (SceneQueryFlag::SceneQueryFlag)mEntity->getQueryFlags();
				}else{
					return SceneQueryFlag::IMMATERIAL;
				}
			}
			void setParentConnector(GameObject* gameObject, string bone, bool transferPaintOver){
				mParentConnector.mGameObject = gameObject;
				mParentConnector.mBone = bone;
				mParentConnector.mTransferPaintOver = transferPaintOver;
			};
			ParentConnector getParentConnector(){return mParentConnector;}

			virtual void setLevelSegment(LevelSegment* ls){
				Parent::setLevelSegment(ls);
				list<GAME_OBJECT_CHILD>::iterator it;
				for (it = mChild.begin();it != mChild.end();it++){
					(*it).gameObject->setLevelSegment(ls);
				}
			}
			//void setDoNotHook(bool set){mDoNotHook = set;}
		virtual void setLife(float life){mLife = life;}
		virtual bool isAlive(){return mLife > 0;}
		virtual float getLife(){return mLife;}

		//virtual void setCollisionGroup(COLLGROUP cg, bool setAllChildren=true);
		//COLLGROUP getCollisionGroup(bool includeChildren=false,bool ANDChildren=false );
		void destroyCollisionStructure(){
			if (mCollisionStructure){
				delete mCollisionStructure;
				mCollisionStructure = false;
			}
		}

		void forceUpdate();// Forces internal variables to update.  Required if the vehicle will launch a child or something of that nature.
		void disableCollisionTest(){mTestCollisions = false;}
		void enableCollisionTest(){mTestCollisions = true;}
		GameObject* getChild(string str);
		GameObject* getOwner(){return mOwner;}
		//void setDependent(GameObject* obj);
		//GameObject* getDependent(){return mDependent;}
		void setOwner(GameObject* owner){mOwner = owner;}
		Ogre::SceneNode* getNewChildNode(string _name);
		Ogre::SceneNode* getSceneNode(){return mSceneNode;}
		virtual void overrideQueryFlags();

		CollisionStructure* getCollisionStructure(){return mCollisionStructure;}
		void setCollisionForward(Shmoulette::GameObject* go){mCollisionForward = go;}

		Ogre::Entity* getEntity();
		virtual Ogre::MovableObject* getMovableObject(){return mEntity;}
		std::string getFilename(){return mFilename;}
		virtual void attachToBone(Ogre::Entity* ent, string bone);

		void setPaintOver(string mat);
		void setPaintOver(Ogre::Material* mat);
		void removePaintOver();
		bool isPaintedOver(){return mIsPaintedOver;}
		void setSkin(string skin){mSkin = skin;}

		virtual Weapon* addWeapon(string type, string bone, WeaponSupply* ws = NULL);
		virtual void fire(int type=0, Ogre::Vector3 inertia = Ogre::Vector3(0,0,0));
		virtual void ceaseFire(int type=0);

		void moveTimes(double time);
		void capTimes(double time);

		virtual void init(string id, string mesh, bool append_number = false);//,Ogre::SceneNode* parentSceneNode = Level::getSingleton()->getSceneManager()->getRootSceneNode());
		virtual void init(string id, bool append_number = false);//,Ogre::SceneNode* parentSceneNode = Level::getSingleton()->getSceneManager()->getRootSceneNode());
		virtual void init(string id, SceneNode* parentNode, Ogre::Matrix4 position = Ogre::Matrix4::IDENTITY);
		//GameObject(string id, GameObject* childOf);
		virtual void init(XML* xml);
		virtual void init(PyObject* pyObject);
		//GameObject(vector<string> params);
		virtual Ogre::Vector3 getSpeed(){return Ogre::Vector3(0,0,0);}

		virtual ~GameObject();
		
		virtual void updateFromCoordinator(bool force = false);
		virtual bool update(double timeDelta, double parentTime);  // An "update" that returns true will instruct the Game do erase it from the list.  The delete will be handled by the list, not from within the class.
		virtual bool postFrameUpdate(double timeDelta, double parentTime); // Updates the collision structure

		//void addChildEntity(string mesh, Ogre::Entity** entity, Ogre::SceneNode** sceneNode, Ogre::SceneNode* parentSceneNode = NULL);

		void recreateSceneNode();

		virtual void setVisible(bool vis);//Sets the object's visibility WITHOUT affecting mIsOn.  Used for SFX
		
		virtual void notifyChildDeath(GameObject* go);

		virtual void resetTransforms();
		virtual void setPos(float x, float y, float z);
		virtual void setRot(float x, float y, float z, float deg);
		virtual void setScl(float x, float y, float z);
		virtual void setScl(Ogre::Vector3 v){setScl(v.x, v.y, v.z);}

		virtual void movePos(float x, float y, float z);
		virtual void moveRot(float x, float y, float z);
		virtual void moveScl(float x, float y, float z);

		virtual void pitch(float val, bool maintenance = true);
		virtual void turn(float val, bool maintenance = true);
		virtual void yaw(float val, bool maintenance = true){turn(val, maintenance);}
		virtual void roll(float val, bool maintenance = true);

		virtual void pitch(Ogre::Radian r);
		virtual void yaw(Ogre::Radian r);
		virtual void roll(Ogre::Radian r);

		virtual void grow(float val, bool maintenance = true);
		virtual void move(float val, bool maintenance = true);
		virtual void climb(float val, bool maintenance = true);
		virtual void strafe(float val, bool maintenance = true);

		virtual void setToObjectOrientation(GameObject* obj);
		virtual void point(GameObject* obj);
		virtual void point(float x, float y, float z);
		virtual void onMove();

		virtual void hook();
		//virtual GameObject* getHook(){ return mLevelSegment->getContainer(); }
		virtual bool isHooked(){ return mHooked; }
		//virtual void setHook(GameObject* hook, bool convertSceneNodePositionToLocal=false);
		virtual void setHooked(bool hooked, bool convertSceneNodePositionToLocal=false);

		virtual void attach(GameObject* newchild);
		virtual void attach(Ogre::SceneNode* newchild);
		virtual void detach(GameObject* newchild);
		virtual void orphan();

		virtual void setType(OBJ_TYPE type);
		virtual void setLight(int light);
		virtual void setFilename(const string& filename);
		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual void hide();

		//virtual bool request(GORequest r);


		virtual void setRotFromObj(GameObject* obj);
		virtual void setPosFromObj(GameObject* obj);
		virtual Ogre::Node* getNode();
		virtual void activate();
		virtual bool isSceneNode(){
			if (mParentConnector.mBone == ""){
				return true;
			}else{
				return false;
			}
		}


		/*bool isSolid(){return mSolid;}
		void setSolid(bool solid){mSolid = solid;}*/

		virtual void hit(DamagePackage& pack, GameObject* hitter, GameObject* forwardFrom, float distance, double time);
		virtual void takeDamage(DamagePackage& pack, double time);

		bool isHit(){
			return false;
		}
		bool isCollisionElementIntersect(CollisionElement* ce);
		virtual COLLISION_RESULT isColliding(GameObject* obj);

		//void collision(COLLISION_RESULT cr);

		virtual float getX(){return mSceneNode->getPosition().x;}
		virtual float getY(){return mSceneNode->getPosition().y;}
		virtual float getZ(){return mSceneNode->getPosition().z;}
		float getRX(){return mSceneNode->getOrientation().x;}
		float getRY(){return mSceneNode->getOrientation().y;}
		float getRZ(){return mSceneNode->getOrientation().z;}

		float getAbsoluteX();
		float getAbsoluteY();
		float getAbsoluteZ();

	};
}
#endif

