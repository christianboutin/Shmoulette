// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___JUMP_H___
#define ___DEF___JUMP_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{

		
		class _ShmouletteExport JumpOut:public GoAnim{
			typedef GoAnim Parent;
			Ogre::SceneNode* mSceneNode;
			Ogre::Entity* mEntity;
			Ogre::MaterialPtr mMaterial;
			float mDistance;
			float mIntensity;
			FX* mFX;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);

				virtual void update(float time);
				virtual void build();
				virtual void destroy();

		};

		class _ShmouletteExport JumpIn:public GoAnim{
			typedef GoAnim Parent;
			Ogre::SceneNode* mSceneNode;
			Ogre::Entity* mEntity;
			Ogre::Material* mMaterial;
			float mDistance;
			float mIntensity;
			FX* mFX;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				virtual void update(float time);
				virtual void build();
				virtual void destroy();

		};

	}
}
#endif