#if 0
// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___HUDSCORE_H___
#define ___DEF___HUDSCORE_H___

#include <ogre.h>
#include "hud.h"
#include "../playership.h"

namespace Shmoulette{

	class HudScore:public HudElement{
			float mCurrentScore;
			//float mTargetScore;
			PlayerShip* mLinkedObject;
			Ogre::OverlayElement* mScoreText;
		public:
			HudScore();
			bool update();
	};

}

#endif
#endif