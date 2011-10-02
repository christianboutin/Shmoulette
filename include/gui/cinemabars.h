// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___CINEMABARS_H___
#define ___DEF___CINEMABARS_H___

#ifndef CINEMABARS_H
#define CINEMABARS_H

#include "ogre.h"

namespace Gui{
	class CinemaBars{
		private:
			Ogre::OverlayContainer* mBar[2];
			Ogre::Overlay* mOverlay;
			bool mIsOn;
		public:
			CinemaBars(int zOrder = 0);
			void activate();
			void deactivate();
			bool update(float);


	};



}

#endif

#endif
