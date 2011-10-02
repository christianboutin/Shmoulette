#include "stdafx.h"

#include "2d\image.h"
#include "core.h"

namespace Shmoulette{
	using namespace Ogre;

	void Image::setScale(float sx, float sy){
		float adjustH = Core::getSingleton()->getWindow()->getHeight()*mH/768;
		float adjustW = Core::getSingleton()->getWindow()->getWidth()*mW/1024;
		
		float dw = adjustW/Core::getSingleton()->getWindow()->getWidth();
		float dh = adjustH/Core::getSingleton()->getWindow()->getHeight();

		mContent->setDimensions(dw*sx, dh*sy);
	}
	void Image::setSize(float percent){
		float adjustH = Core::getSingleton()->getWindow()->getHeight()*mH/768;
		float adjustW = Core::getSingleton()->getWindow()->getWidth()*mW/1024;
		float dw = adjustW/Core::getSingleton()->getWindow()->getWidth();
		float dh = adjustH/Core::getSingleton()->getWindow()->getHeight();
		mContent->setDimensions(dw*percent, dh*percent);
	}
	void Image::setPosition(float x, float y){
		//mOverlayContainer->setPosition(x, x);
		mContent->setPosition(x,y);
	}
	void Image::setAlpha(float a){
		//				alpha_op_ex modulate src_texture src_manual 0.5 0.5 
		Util::Log("ALPHA_"+ts(a), 10);
		mContent->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->
			setAlphaOperation(
			Ogre::LayerBlendOperationEx::LBX_SUBTRACT, 
			Ogre::LBS_TEXTURE, 
			Ogre::LBS_MANUAL,
			1-a, 1-a);
		

	}
	void Image::init(XML* xml){
		Parent::init(xml, "Panel");//,mOverlayManager(Ogre::OverlayManager::getSingleton()){
		mMaterialName = xml->getString("name");
		string s = xml->getString("pos");
		if (xml->getError() == XML_OK){
			mContentDelta = vector2FromString(s);
		}else{
			mContentDelta.x = xml->getFloat("x");
			mContentDelta.y = xml->getFloat("y");
		}
		
		//mLevelSegment->addResourceContext("images");
		
		//mOverlay->add2D(mOverlayContainer);
	}
	void Image::build(){
		Parent::build();
		mContent = static_cast<OverlayContainer*>(
			Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", getId()+"_photo"));
		mContent->setMetricsMode(Ogre::GMM_RELATIVE);
		//mContent->setPosition(.015, .025);
		//mContent->setDimensions(1, .57);
		/*mContent->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_CENTER);
		mContent->setVerticalAlignment(Ogre::GuiVerticalAlignment::GVA_CENTER);*/
		mContent->setMaterialName("Images/"+mMaterialName);
		Ogre::MaterialPtr mat = mContent->getMaterial();

		if (mat->getTechnique(0)->getPass(0)->getNumTextureUnitStates() == 0){
			throw("Materials used in cell overlays REQUIRE textures.  Don't ask, they just do.");
		}else{
			string txtName = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getFrameTextureName(0);
			Ogre::TexturePtr txt;
			txt = Ogre::TextureManager::getSingleton().getByName(txtName);
			mH = txt->getSrcHeight();
			mW = txt->getSrcWidth();
		}
		//mContentDelta.x -= (mW/1024)/2;
		//mContentDelta.y -= (mH/768)/2;

		setSize(1);
		setPosition(mContentDelta.x, mContentDelta.y);

		//float h = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureHScale();

		//Ogre::TextureManager::getSingleton().getByName("level3_toptextzone.png")->->mS->getWidth();
		mContent->show();
		mOverlayContainer->addChild(mContent);

	}
	void Image::destroy(){
		mOverlayContainer->removeChild(mContent->getName());
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(mContent);

		Parent::destroy();
	}
}
