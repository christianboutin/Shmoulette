#if 0
// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___HUDSHIELD_H___
#define ___DEF___HUDSHIELD_H___

#include <ogre.h>
#include "hud.h"
#include "../playership.h"
namespace Shmoulette{

	class HudShield:public HudElement{
			float mCurrentHealth;
			PlayerShip* mLinkedObject;
			Ogre::OverlayElement* mHealthText;
		public:
			HudShield();
			bool update();
	};

}

#endif
#endif