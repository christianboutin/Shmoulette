// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___TALKWINDOW_H___
#define ___DEF___TALKWINDOW_H___

namespace Game{
typedef enum TAG_PHOTO_POS{
	PHOTO_LEFT,
	PHOTO_RIGHT
}PHOTO_POS;

typedef enum TAG_WINDOW_POS{
	WINDOW_TOP,
	WINDOW_BOTTOM
}WINDOW_POS;

class TalkWindow:public GameElement{
	typedef GameElement Parent;
	static int mImage;
	int mFaceImage;
	int mWindowSprite;
	int mFaceSprite;
	string mPhotoFilename;
	string mString;
	Text* mText;
	string mStringFilename;

	PHOTO_POS mPhotoPos;
	WINDOW_POS mWindowPos;

	Ogre::Overlay* mOverlay;
	Ogre::OverlayContainer* mWindow;
	Ogre::OverlayElement* mPhoto;
	Ogre::OverlayManager & mOverlayManager;


	public:
		TalkWindow(XML* xml);
		virtual bool update();
		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual void hide();
		virtual void setPhotoPos(PHOTO_POS pp){
			mPhotoPos = pp;
		}
		virtual void setWindowPos(WINDOW_POS wp){
			mWindowPos = wp;
		}
		virtual void setPhoto(string photoFilename){
			mPhotoFilename = photoFilename;
		}

		virtual void setString(string str){
			mString = str;
		}
};

}

#endif
