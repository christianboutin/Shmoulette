// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___IMAGE_H___
#define ___DEF___IMAGE_H___

#include "cell.h"
#include "..\util\refinedtrack.h"
namespace Shmoulette{
class Image:public CellElement{
	typedef CellElement Parent;
	//Ogre::OverlayManager* mOverlayManager;
	//Ogre::OverlayContainer* mOverlayContainer;
	Ogre::OverlayElement* mContent;
	//Ogre::Overlay* mOverlay;
	string mMaterialName;
	Ogre::Vector2 mContentDelta;
	
	float mW,mH;
	
	public:
		virtual void init(XML *xml);
		void setSize(float percent);
		void setScale(float sx, float sy);
		void setAlpha(float a);
		void setPosition(float x, float y);
		void build();
		void destroy();

};
}

#endif
