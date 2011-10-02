// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___WEAPONGA_H___
#define ___DEF___WEAPONGA_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{

		class _ShmouletteExport WeaponFire:public GoAnim{
			typedef GoAnim Parent;

			list<int> mWeapon;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				virtual void show();
				virtual void hide();

		};
	}
}
#endif