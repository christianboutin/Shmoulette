// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___GROUNDCLAMP_H___
#define ___DEF___GROUNDCLAMP_H___

#include "goanims\goanim.h"

//#define DEBUG_GROUNDCLAMP

namespace Shmoulette{
	namespace GoAnim{
		class _ShmouletteExport GroundClamp:public GoAnim{
			typedef GoAnim Parent;

#ifdef DEBUG_GROUNDCLAMP
			Ogre::SceneNode* mPreGoAnimBallSceneNode[4];
			Ogre::Entity* mPreGoAnimBallEntity[4];
			Ogre::SceneNode* mPostGoAnimBallSceneNode[4];
			Ogre::Entity* mPostGoAnimBallEntity[4];
			bool mDoIt;
#endif

			float mAltitude;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				
				void update(float time);

		};
	}

}
#endif