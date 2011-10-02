#include "stdafx.h"

#include "2d\cell.h"
#include "2d\image.h"
#include "2d\text.h"
#include <OgreTextAreaOverlayElement.h>

using namespace Ogre;

namespace Shmoulette{

	void CellElement::spawnOverlayAndContainer(string type){
		mOverlay = OverlayManager::getSingleton().create(getId());
		mDeltaContainer = static_cast<OverlayContainer*>(
			OverlayManager::getSingleton().createOverlayElement("Panel", getId()+"_DeltaContainer"));
		
		mDeltaContainer->setMetricsMode(Ogre::GMM_RELATIVE);
		mDeltaContainer->setPosition(0, 0);
		mDeltaContainer->setDimensions(1, 1);


		mOverlayContainer = static_cast<OverlayContainer*>(
			OverlayManager::getSingleton().createOverlayElement(type, getId()+"_Container"));
		
		mOverlayContainer->setMetricsMode(Ogre::GMM_RELATIVE);
		mOverlayContainer->setPosition(0, 0);
		mOverlayContainer->setDimensions(1, 1);
		mDeltaContainer->addChild(mOverlayContainer);

		//mOverlayContainer->setMaterialName("Faces/Window");
		mOverlay->show();
		mOverlay->add2D(mDeltaContainer);


		mDeltaContainer->show();
		setDelta(Ogre::Vector2(0,0));
		mOverlayOverwritten = false;

	}
	CellElement::~CellElement(){
		mOverlay->remove2D(mOverlayContainer);
		mDeltaContainer->removeChild(mOverlayContainer->getName());
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(mOverlayContainer);
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(mDeltaContainer);
		if (!mOverlayOverwritten){
			Ogre::OverlayManager::getSingleton().destroy(mOverlay);
		}
	}

	/*void CellElement::setAlpha(float f){
		mDeltaContainer->setColour(Ogre::ColourValue(1,1,1,f));
	}*/
	/*void CellElement::spawnContainer(string type){
	}*/

	void CellElement::setAngle(float angle){
		//mOverlay->setRotate((Ogre::Radian)(Ogre::Degree)angle);
	}


	void CellElement::init(){
		spawnOverlayAndContainer("Panel");
		mOverlay->setZOrder(550);
	}
	void CellElement::init(string type, float zOrder){
		Parent::init("TalkWindow_Text", true);
		spawnOverlayAndContainer(type);
		mOverlay->setZOrder(zOrder);
	}
	void CellElement::init(XML* xml, string type){
		Parent::init(xml);
		spawnOverlayAndContainer(type);
		mOverlay->setZOrder(xml->getFloat("z"));
	}
	void CellElement::setParentOverlay(Ogre::Overlay* overlay){
		mOverlay->remove2D(mDeltaContainer);
		if (!mOverlayOverwritten){
			Ogre::OverlayManager::getSingleton().destroy(mOverlay);
		}
		mOverlay = overlay;
		mOverlay->add2D(mDeltaContainer);
		mOverlayOverwritten = true;
	}
	void CellElement::setDelta(Ogre::Vector2 pos){
		mDeltaContainer->setPosition(pos.x, pos.y);
	}
	void CellElement::setScale(float sx, float sy){
		setSize(sx);
	}
	void CellElement::setSize(float percent){
	}

	CellElement* Cell::getCellElement(int index){
		list<CellElement*>::iterator it;
		it = mElement.begin();
		for (int i=0;i<index;i++,it++);
		return (*it);

	}
	void Cell::createFromXml(XML* xml){
		/*mOverlay = mOverlayManager.create(getId());
		mOverlay->setZOrder(xml->getFloat("z"));
		mOverlay->show();*/

		//mZOrder = xml->getFloat("z");

		//mTrack = new RefinedTrack(xml, false);
		mX = new RefinedNumericTrack(xml, "posx");
		mY = new RefinedNumericTrack(xml, "posy");
		mAlpha = new RefinedNumericTrack(xml, "alpha");
		mScaleX = new RefinedNumericTrack(xml, "sclx");
		mScaleY = new RefinedNumericTrack(xml, "scly");

		//mAngle = new RefinedNumericTrack(xml, "angle");

		mPos.x = xml->getFloat("posx");
		mPos.y = xml->getFloat("posy");

		XMLIterator xmlIt(xml);


		/*xmlIt.setElemName("cell");
		xmlIt.gotoZero();
		while(xmlIt.gotoNext()==true){
			mElement.push_back(new Cell(&(xmlIt.getElem())));
		}*/
		xmlIt.setElemName("cell>image");
		xmlIt.gotoZero();
		
		while(xmlIt.gotoNext()==true){
			Image* img = new Image();
			img->init(&(xmlIt.getElem()));
			mElement.push_back(img);
			img->setDelta(Ogre::Vector2(-1, -1));
			img->moveTimes(getTimeOn());
		}
		xmlIt.setElemName("cell>textzone");
		xmlIt.gotoZero();
		while(xmlIt.gotoNext()==true){
			XML xml = xmlIt.getElem();
			string txttype = xml.getString("type");
			Text* txt;
			if (txttype == "rolling"){
				txt = new RollingText();
			}else if (txttype == "static"){
				txt = new StaticText();
			}else{
				txt = new StaticText();
			}
			txt->init(&(xmlIt.getElem()));
			txt->setFontStyle(FontStyleManager::getSingleton()->requestFontStyle(xmlIt.getElem().getString("style")));
			mElement.push_back(txt);
			txt->setDelta(Ogre::Vector2(-1, -1));
			txt->moveTimes(getTimeOn());
		}
	}

	void Cell::init(){
		Parent::init("BlankCell",true);
		mX=NULL;
		mY=NULL;
		mAlpha=NULL;
		mScaleX=NULL;
		mScaleY=NULL;
	}
	void Cell::init(XML* xml){
		Parent::init(xml);
		createFromXml(xml);
	}
	void Cell::moveTimes(double time){
		Parent::moveTimes(time);
		list<CellElement*>::iterator it;
		for (it=mElement.begin();it!=mElement.end();it++){
			(*it)->moveTimes(time);
		}
	}
	bool Cell::update(double timeDelta, double parentTime){
		if (mId == "mouseblocker"){
			int g=0;
		}
		//Ogre::TransformKeyFrame kf = mTrack->getInterpolatedKeyFrame(mLevelSegment->getTimeTrack()-getTimeOn());
		list<CellElement*>::iterator it;
		for (it=mElement.begin();it!=mElement.end();it++){
			(*it)->updateIfOn(timeDelta, parentTime);
			if ((*it)->isOn()){
				//(*it)->setDelta(Ogre::Vector2(kf.getTranslate().x,kf.getTranslate().y));
				
				(*it)->setDelta(Ogre::Vector2(
					mX->getValue(parentTime-getTimeOn()),
					mY->getValue(parentTime-getTimeOn())
					));
				//(*it)->setAngle(mAngle->getValue(mLevelSegment->getTimeTrack()-getTimeOn()));

				float wantedAlpha = mAlpha->getValue(parentTime-getTimeOn());
				if (wantedAlpha < 0) wantedAlpha = 0;
				if (wantedAlpha > 1) wantedAlpha = 1;
				(*it)->setAlpha(wantedAlpha);
				(*it)->setScale(
					mScaleX->getValue(parentTime-getTimeOn()),
					mScaleY->getValue(parentTime-getTimeOn()));
			}
		}
		
		return false;
		
	}
	void Cell::destroy(){
		list<CellElement*>::iterator it;
		for (it=mElement.begin();it!=mElement.end();it++){
			if ((*it)->isBuild())
				(*it)->destroy();
		}
	}
	Cell::~Cell(){
		list<CellElement*>::iterator it;
		for (it=mElement.begin();it!=mElement.end();it++){
			delete (*it);
		}
	}

}