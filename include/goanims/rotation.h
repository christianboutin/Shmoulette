// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___ROTATION_H___
#define ___DEF___ROTATION_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{
		class RotateParams:public GoAnimParams{
			public:
				float mSpeed;
		};

		class _ShmouletteExport Yaw:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Radian r;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);

				void update(float time);

		};

		class _ShmouletteExport Pitch:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Radian r;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				void update(float time);

		};

		class _ShmouletteExport Roll:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Radian r;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);

				void update(float time);

		};

		class _ShmouletteExport Aim:public GoAnim{
			typedef GoAnim Parent;
			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				void update(float time);

		};


	}
}
#endif