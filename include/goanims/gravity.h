// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___GRAVITY_H___
#define ___DEF___GRAVITY_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{

		
		class _ShmouletteExport Gravity:public GoAnim{
			typedef GoAnim Parent;

			float mSpeed;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				void update(float time);

		};
	}
}
#endif