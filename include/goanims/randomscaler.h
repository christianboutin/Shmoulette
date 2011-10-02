// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___RANDOMSCALER_H___
#define ___DEF___RANDOMSCALER_H___

#include "goanims\goanim.h"

namespace Shmoulette{

	namespace GoAnim{
		class _ShmouletteExport RandomScaler:public GoAnim{
			typedef GoAnim Parent;
			Ogre::Vector3 mMin;
			Ogre::Vector3 mMax;


			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);

				
				void update(float time);

		};
	}
}
#endif