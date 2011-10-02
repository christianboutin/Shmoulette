// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___DRONEWAVE_H___
#define ___DEF___DRONEWAVE_H___

#include "elements\gameelement.h"
#include "drone\drones.h"

namespace Shmoulette{

	class DroneWaveEntry{
		Drone* mMainDrone;
		vector<DroneWaveEntry> mDependencies;
	};
	
	class _ShmouletteExport DroneWave:public GameObject{
		typedef GameObject Parent;
		//Drone* mRootUnit; // The Root Unit will bring down the entire wave with it if it is destroyed.
		vector<Drone*>mVectorDrone;
		vector<Drone*>mVectorDroneStandby; // Drones, before they go in, after they leave
		list<Trigger*>mPostDeathTrigger;
		//bool mIsRailed;
		int mBonus;
		bool mBonusAllowed;

		//virtual bool _rootAlive();

		public:
			virtual void addChild(XML& xml);
			//DroneWave();
			virtual void init(XML* xml);
			virtual ~DroneWave();

			virtual void build();
			virtual void destroy();
			virtual void show();
			virtual void hide();
			virtual bool update(double timeDelta, double parentTime);
			virtual bool updateIfOn(double timeDelta, double parentTime);
			virtual bool postFrameUpdate(double timeDelta, double parentTime);
			virtual void reset();
			
			virtual COLLISION_RESULT collisionTest(GameObject* obj);
			virtual Drone* getDrone(int n);
			list<Drone*> getActiveDroneList();


			//virtual string dataToXML();
	};

}
#endif
