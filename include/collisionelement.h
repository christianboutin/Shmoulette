// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___COLLISIONELEMENT_H___
#define ___DEF___COLLISIONELEMENT_H___

//#define DEBUG_CUBES

typedef enum{
	COLLISION_ELEMENT_BOX,
	COLLISION_ELEMENT_SPLIT_BOX,
	COLLISION_ELEMENT_RAY,
	COLLISION_ELEMENT_SPHERE
}COLLISION_ELEMENT_TYPES;

enum{
	INVULNERABLE_NONE = 0x00000,
	INVULNERABLE_SOURCE = 0x00001,
	INVULNERABLE_DESTINATION = 0x00002,
	INVULNERABLE_BOTH = INVULNERABLE_DESTINATION|INVULNERABLE_SOURCE
};

typedef struct TAG_INTERSECTS_RESULT{
	bool mIntersects;
	float mDistance;
	int mInvulnerable;
}INTERSECTS_RESULT;


namespace Shmoulette{
	class _ShmouletteExport CollisionElement{
	protected:
		// Either this
		string mBoneName;
		Ogre::Bone* mBone;
		class GameObject* mGameObject;
		// or that
		Ogre::SceneNode* mSceneNode;
		
		bool mIsBuild;
		string mId;
		bool mInvulnerable;
		COLLISION_ELEMENT_TYPES mType;
		Ogre::WireBoundingBox* mWbb;
		vector<CollisionElement*> mChildElem;

		bool mIsDirty;
	public:
		CollisionElement(SceneNode* sceneNode, XML* xml, COLLISION_ELEMENT_TYPES type);
		CollisionElement(SceneNode* sceneNode, COLLISION_ELEMENT_TYPES type);

		CollisionElement(GameObject* gameObject, XML* xml, COLLISION_ELEMENT_TYPES type);
		CollisionElement(GameObject* gameObject, COLLISION_ELEMENT_TYPES type);
		CollisionElement(GameObject* gameObject, string boneName, COLLISION_ELEMENT_TYPES type);
		virtual ~CollisionElement();
		virtual void setInvulnerable(bool inv);
		COLLISION_ELEMENT_TYPES getType(){return mType;}
		virtual void build();

		virtual INTERSECTS_RESULT intersects(class CollisionElement* ce);
		virtual bool isDirty(){return mIsDirty;}
		virtual void setDirty(bool dirty){ mIsDirty = dirty; }
		virtual void update();
		virtual bool hasChildren(){return mChildElem.size() > 0;}
	};

	class _ShmouletteExport BoxCollisionElement:public CollisionElement{
		typedef CollisionElement Parent;
		protected:
#ifdef DEBUG_CUBES
			Ogre::Entity* mDebugEntity[8];
			Ogre::SceneNode* mDebugSceneNode[8];
#endif
			Ogre::AxisAlignedBox* mBox;
			Ogre::AxisAlignedBox* mTBox;
		public:
			Ogre::AxisAlignedBox* getRootBox(){return mBox;}
			Ogre::AxisAlignedBox* getBox(){return mTBox;}
			void setBox(Ogre::AxisAlignedBox aabb);

			BoxCollisionElement(SceneNode* sceneNode, XML* xml);

			BoxCollisionElement(GameObject* gameObject, string boneName);
			BoxCollisionElement(GameObject* gameObject, XML* xml);
			~BoxCollisionElement();

			void build();
			void update();

	};

	class _ShmouletteExport AutoBoxCollisionElement:public BoxCollisionElement{
		protected:
		typedef BoxCollisionElement Parent;
		Ogre::Vector3 mScale;
		void stretchBox(Ogre::MeshPtr meshPtr, int boneHandle);
		public:
			AutoBoxCollisionElement(GameObject* gameObject, XML* xml);
			AutoBoxCollisionElement(GameObject* gameObject, string boneName);
			void build();

	};
	
	class _ShmouletteExport RayCollisionElement:public CollisionElement{
		typedef CollisionElement Parent;
		Ogre::Ray* mRay;
		Ogre::Ray* mTRay;
		Ogre::WireBoundingBox* mWDbb;
		public:
			RayCollisionElement(GameObject* gameObject, XML* xml);
			RayCollisionElement(GameObject* gameObject, string boneName);
			~RayCollisionElement();
			Ogre::Ray* getRay(){return mTRay;}
			void update();
			void build();
	};

	class _ShmouletteExport SphereCollisionElement:public CollisionElement{
		typedef CollisionElement Parent;
		Ogre::Sphere* mSphere;
		Ogre::Sphere* mTSphere;
		public:
			SphereCollisionElement(Ogre::SceneNode* sceneNode);
			SphereCollisionElement(GameObject* gameObject, XML* xml);
			//SphereCollisionElement(GameObject* gameObject, string boneName);
			~SphereCollisionElement();
			void setRadius(float radius);
			Ogre::Sphere* getSphere(){return mTSphere;}
			void update();
			void build();
	};

	class _ShmouletteExport CollisionStructure:public AutoBoxCollisionElement{
		typedef AutoBoxCollisionElement Parent;
		public:
			CollisionStructure(GameObject* gameObject, XML* xml);
			CollisionStructure(GameObject* gameObject, string boneName);
			CollisionStructure(GameObject* gameObject);

			void build();
			INTERSECTS_RESULT intersects(CollisionElement* cStruct);
			//bool isColliding(CollisionStructure* cStruct);
			void addElement(GameObject* gameObject, XML* xml);
			void addElement(GameObject* gameObject, string type);
			void addElement(GameObject* gameObject, string boneName, string type);
			void addElement(CollisionElement* ce);
			void update();
	};

	class _ShmouletteExport SplitBoxCollisionElement:public CollisionStructure{
		typedef CollisionStructure Parent;
		Ogre::Vector3 mSplit;
		bool mForceAll;
		public:
			SplitBoxCollisionElement(GameObject* gameObject, XML* xml);
			SplitBoxCollisionElement(GameObject* gameObject, string boneName, Ogre::Vector3 split);
			void build();

	};

}
#endif