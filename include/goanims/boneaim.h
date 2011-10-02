// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___BONEAIM_H___
#define ___DEF___BONEAIM_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{

		typedef enum TAG_AXIS{
			AXIS_X,
			AXIS_Y,
			AXIS_Z,
			AXIS_ALL
		}AXIS;

		
		class _ShmouletteExport BoneAim:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Bone* mBone;
			GameObject* mTarget;

			string mBoneName, mTargetName;
			

			AXIS mAxis;

			float mStrength;


			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);

				void update(float time);

		};
	}
}
#endif