#if 0
// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___HUDAMMO_H___
#define ___DEF___HUDAMMO_H___

#include <ogre.h>
#include "hud.h"
#include "../playership.h"
namespace Shmoulette{

	class HudAmmo:public HudElement{
			float mCurrent;
			PlayerShip* mLinkedObject;
			Ogre::OverlayElement* mText;
		public:
			HudAmmo();
			bool update();
	};

}

#endif
#endif