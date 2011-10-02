#include "stdafx.h"

#include "gui\cinemabars.h"
using namespace Ogre;

namespace Gui{
	#define CINEMABAR_PERCENTHEIGHT 0.125  

	CinemaBars::CinemaBars(int zOrder){
		
		TexturePtr ptex;
		HardwarePixelBufferSharedPtr buffer;

		ptex = TextureManager::getSingleton().createManual(
		"CinemaBarsBlack","General", TEX_TYPE_2D, 1, 1, 0, PF_A8R8G8B8, 
		TU_DYNAMIC_WRITE_ONLY);
		buffer = ptex->getBuffer(0, 0);
		buffer->lock(HardwareBuffer::HBL_DISCARD);
		const PixelBox &pb = buffer->getCurrentLock();
		uint32 *data = static_cast<uint32*>(pb.data);
		data[0] = 0xFF000000;
		buffer->unlock();

		mOverlay = OverlayManager::getSingleton().create("cinemaBars");
		for (int i=0;i<2;i++){
			mBar[i] = static_cast<OverlayContainer*>(
				OverlayManager::getSingleton().createOverlayElement("Panel", "cinemaBar"+StringConverter::toString(i)));
			mBar[i]->setMetricsMode(Ogre::GMM_RELATIVE);
			mBar[i]->setPosition(0, 0);
			mBar[i]->setDimensions(1, CINEMABAR_PERCENTHEIGHT);
			mBar[i]->setMaterialName("CinemaBar");
			mBar[i]->show();
			//mBar[i]->setColour(Ogre::ColourValue(1, 0, 0, .5));
			mOverlay->add2D(mBar[i]);
		}
		mBar[1]->setPosition(0, 1.0f-CINEMABAR_PERCENTHEIGHT);
		mOverlay->setZOrder(zOrder);
		mOverlay->show();
	}
	void CinemaBars::activate(){
		mIsOn = true;
	}
	void CinemaBars::deactivate(){
		mIsOn = false;
	}
	bool CinemaBars::update(float timeDelta){
		if (mIsOn){
			if (mBar[0]->getTop() < 0){
				mBar[0]->setPosition(0, mBar[0]->getTop()+(CINEMABAR_PERCENTHEIGHT*timeDelta));
				mBar[1]->setPosition(0, mBar[1]->getTop()-(CINEMABAR_PERCENTHEIGHT*timeDelta));

				if (mBar[0]->getTop() > 0){
					mBar[0]->setPosition(0, 0);
					mBar[1]->setPosition(0, 1-CINEMABAR_PERCENTHEIGHT);
				}
			}
		}else{
			if (mBar[1]->getTop() < 1){
				mBar[0]->setPosition(0, mBar[0]->getTop()-(CINEMABAR_PERCENTHEIGHT*timeDelta));
				mBar[1]->setPosition(0, mBar[1]->getTop()+(CINEMABAR_PERCENTHEIGHT*timeDelta));

				if (mBar[1]->getTop() > 1){
					mBar[0]->setPosition(0, 0-CINEMABAR_PERCENTHEIGHT);
					mBar[1]->setPosition(0, 1);
				}
			}
		}
		return false;
	}
	
}