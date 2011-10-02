// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___TRANSLATION_H___
#define ___DEF___TRANSLATION_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{

		typedef enum TAG_TRANSLATE_BREAK{
			TRANSLATE_RESET,
			TRANSLATE_WRAP,
			TRANSLATE_DESTROY,
			TRANSLATE_BOUNCE,
			TRANSLATE_TRIGGER
		}TRANSLATE_LEAVE;

		class TranslateParams:public GoAnimParams{
		public:
			Ogre::Vector3 mSpeed;
			//Ogre::Vector3 mResetPosition;
			Ogre::AxisAlignedBox mZone;

			//TRANSLATE_LEAVE mOnBreak;
			//list<Trigger*>mTriggerList;

			TranslateParams();
		};
		
		class _ShmouletteExport Translate:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Vector3 mSpeed;
			//Ogre::Vector3 mResetPosition;
			Ogre::AxisAlignedBox mZone;

			//TRANSLATE_LEAVE mOnBreak;
			//list<Trigger*>mTriggerList;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				void update(float time);

		};
	}
}
#endif