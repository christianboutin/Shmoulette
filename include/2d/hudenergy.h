#if 0
// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___HUDENERGY_H___
#define ___DEF___HUDENERGY_H___

#include <ogre.h>
#include "hud.h"
#include "../playership.h"
namespace Shmoulette{

	class HudEnergy:public HudElement{
			float mCurrent;
			PlayerShip* mLinkedObject;
			Ogre::OverlayElement* mText;
		public:
			HudEnergy();
			bool update();
	};

}


#endif
#endif