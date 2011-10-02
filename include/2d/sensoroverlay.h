// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___OVERLAYSENSOR_H___
#define ___DEF___OVERLAYSENSOR_H___


namespace Shmoulette{
	using namespace Ogre;
	
	class SensorOverlayEntry{
			bool mIsPainted;
			Ogre::Entity* mEntity;
			Ogre::Entity* mClone;
			list<Ogre::Pass*> mPass;
			string mMaterial;
			Ogre::SceneNode* mSceneNode;
		public:
			SensorOverlayEntry(Ogre::Entity* entity, string material);
			~SensorOverlayEntry();
			Ogre::Entity* getEntity(){return mEntity;}
			void paint();
			void unpaint();
			void createSensorComponent();
			void destroySensorComponent();
	};

	class SensorOverlay{
		vector<SensorOverlayEntry*> mObjects;
		bool mActive;

		public:
			SensorOverlay();
			~SensorOverlay();

			void addEntity(Ogre::Entity* entity, string material="");
			void removeEntity(Ogre::Entity* entity);
			void update(double timeDelta, double parentTime);
			void activate();
			void deactivate();
	};
}

#endif
