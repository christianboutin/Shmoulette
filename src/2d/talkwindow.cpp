#include "stdafx.h"
#include "main.h"
#include "2d\text.h"
#include "2d\talkwindow.h"
#include "util\localstr.h"
#include "sound.h"

namespace Shmoulette{
using namespace Ogre;

TalkWindow::TalkWindow():mOverlayManager(OverlayManager::getSingleton()){
}

void TalkWindow::init(XML* xml){
	Parent::init(xml);
	setId(mId);
	setPhoto(xml->getString("photo"));
	setPhotoPos(xml->getString("photopos") == "right"?PHOTO_RIGHT:PHOTO_LEFT);
	setString("");
	mStringFilename = xml->getString("stringfilename");
	mStringEntry = xml->getString("stringentry");
	setTimeBuild(xml->getFloat("time"));
	setTimeOn(xml->getFloat("time"));
	setTimeOff(xml->getFloat("time")+xml->getFloat("life"));
	setTimeDestroy(xml->getFloat("time")+xml->getFloat("life"));
	mText = NULL;
	string soundString = xml->getString("sound");
	if (xml->getError() != XML_OK){
		mSound = NULL;
	}else{
		mSound = SoundManager::getSingleton()->createSound(soundString+".ogg", true);	
	}
	
}
void TalkWindow::build(){
	//setId("Overlay", true);
	Parent::build();

using namespace Util;
	setString(LocalStr::getSingleton()->getString(mStringFilename,mStringEntry));
using namespace Ogre;

	mOverlay = mOverlayManager.create(getId());
	mOverlay->setZOrder(501);
	
	mWindow = static_cast<OverlayContainer*>(
	mOverlayManager.createOverlayElement("Panel", getId()));
	mWindow->setMetricsMode(Ogre::GMM_RELATIVE);
	mWindow->setPosition(0, 0);
	mWindow->setDimensions(1, .208);
	mWindow->setMaterialName("Faces/Window");
	mWindow->show();

	mPhoto = static_cast<OverlayContainer*>(
	mOverlayManager.createOverlayElement("Panel", getId()+"_photo"));
	mPhoto->setMetricsMode(Ogre::GMM_RELATIVE);
	
	mText = new Text();
	mText->setZOrder(502);
	mText->setString(mString);

	if (mPhotoPos == PHOTO_RIGHT){
		mPhoto->setPosition(.86, .025);
		mText->setBox(.02,.02,.8,.16);
	}else{
		mPhoto->setPosition(.015, .025);
		mText->setBox(.16,.02,.8,.16);
	}
	mText->setFontStyle(FontStyleManager::getSingleton()->requestFontStyle("TalkWindow"));
	mText->setDelta(Ogre::Vector2(0,0));


	mPhoto->setDimensions(.125, .16);
	mPhoto->setMaterialName(mPhotoFilename);
	mPhoto->show();
	mWindow->addChild(mPhoto);



	mOverlay->add2D(mWindow);
	mOverlay->show();

	mText->setTimeOn(getTimeOn());
	mText->setTimeOff(getTimeOff());
	mText->setTimeBuild(getTimeBuild());
	mText->setTimeDestroy(getTimeDestroy());


	mText->build();
}

void TalkWindow::destroy(){
	mWindow->removeChild(mPhoto->getName());
	Ogre::OverlayManager::getSingleton().destroyOverlayElement(mPhoto);
	mOverlay->remove2D(mWindow);
	Ogre::OverlayManager::getSingleton().destroyOverlayElement(mWindow);
	mOverlayManager.destroy(getId());
	if (mText->isOn()) mText->hide();
	if (mText->isBuild()) mText->destroy();
	delete mText;
	mText = NULL;

	Parent::destroy();
}

void TalkWindow::show(){
	if (mSound != NULL){
		mSound->play();
	}
	Parent::show();
}

void TalkWindow::hide(){
	if (mSound != NULL){
		mSound->stop();
	}
	Parent::hide();
}

bool TalkWindow::update(double timeDelta, double parentTime){
	if (mText != NULL){
		mText->updateIfOn(timeDelta, parentTime);
	}
	Parent::update(timeDelta, parentTime);
	return false;
}

}
