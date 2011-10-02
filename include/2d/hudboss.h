#if 0
// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___HUDHEALTH_H___
#define ___DEF___HUDHEALTH_H___

#include <ogre.h>
#include "hud.h"
//#include "../playership.h"
namespace Shmoulette{

	class _ShmouletteExport HudBoss:public HudElement{
			GameObject* mLinkedObject;
			Ogre::OverlayElement* mHealthText;
		public:
			HudBoss();
			void setLinkedObject(GameObject* go){mLinkedObject = go;}
			bool update();
	};

}



#endif
#endif