#include "stdafx.h"
#include "goanims\gravity.h"

namespace Shmoulette{
	namespace GoAnim{
		void Gravity::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			mSpeed = 0;
		}

		void Gravity::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			mSpeed = 0;
		}
		
		void Gravity::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			mSpeed = 0;
		}

		void Gravity::update(float time){
			Parent::update(time);
			mSpeed += .6*time; 
			mGo->getNode()->translate(0, 0-mSpeed, 0);
		}
	}
}

