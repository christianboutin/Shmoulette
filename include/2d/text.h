// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___TEXT_H___
#define ___DEF___TEXT_H___

#include <OgreTextAreaOverlayElement.h>
#include <OgreFontManager.h> 
#include "cell.h"
#include "coord.h"
#include "font.h"
namespace Shmoulette{
	typedef enum{
		TEXT_ALIGN_TOP,
		TEXT_ALIGN_BOTTOM,
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_RIGHT,
		TEXT_ALIGN_CENTER
	}TEXT_ALIGN;

	class Letter{
		Ogre::TextAreaOverlayElement* mElem;
		Ogre::TextAreaOverlayElement* mShadowElem;
		Ogre::ColourValue mColour;
		float mAlphaDir;
		float mBaseAlpha;

		unsigned char mLetter;
		float mETA;
		float mDelay;
		float mX;
		float mY;
		bool mGlow;
		float dY;
		FontStyle* mFontStyle;
		bool mFadeAway;
		float mShadowDelta;

		public:
			Ogre::TextAreaOverlayElement* getElem(){return mElem;}
			Ogre::TextAreaOverlayElement* getShadowElem(){return mShadowElem;}
			Letter(unsigned char letter, string id, FontStyle* style);
			~Letter();
			float getDy(){return dY;}
			void setDelay(float delay){mDelay=delay;}
			void setPos(float x, float y, bool updateAbsolutePos = true);
			void hide();
			void update(double timeDelta, double parentTime);
			void setGlow(bool glow);
			void setAlpha(float alpha);
			bool isInPlace();
			char getLetter(){return mLetter;}
			bool isVisible(){return mElem->isVisible();}
			void pushUp();
			float getY(){return mY;}
			void push(float x, float y);
			virtual void setColour(float r, float g, float b);
	};

	class _ShmouletteExport Text:public CellElement{

		typedef CellElement Parent;
		protected:
		string mString;
		list<string>mSplitString;
		//Ogre::Overlay* mOverlay;
		Ogre::OverlayManager & mOverlayManager;
		//Ogre::OverlayContainer* mPanel;

		FontStyle* mFontStyle;
		int mZOrder;
		
		RXYWH mBox;
		TEXT_ALIGN mTextAlign;
		TEXT_ALIGN mTextVAlign;
		//Ogre::ColourValue mStartingColour;
		//Ogre::ColourValue mShadowColour;
		void alignLine(float y);

		static string _wordWrap(string string, FontStyle* fs, RXYWH box);
		void _wordWrap(); // A service call that adds \r \n to word-wrap the string.  Requires valid mBox and FontStyle.  Alters original string.

		public:
			Text();
		virtual void init();
		virtual void init(XML* xml);
		~Text();
		void splitString();
		void setString(string str);
		string getString(){return mString;}
		void setBox(float x, float y, float w, float h);
		void setFontStyle(FontStyle* font);
		void setAlign(string align);
		void setVAlign(string align);
		virtual void setAlpha(float a);
		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual void hide();
		virtual bool update(double timeDelta, double parentTime);
	};

	class _ShmouletteExport StaticText:public Text{
		typedef Text Parent;
		
		protected:		
		Ogre::TextAreaOverlayElement* mElem;
		Ogre::TextAreaOverlayElement* mShadowElem;
		Ogre::ColourValue mColour;

		public:
		virtual void init(XML* xml);
		virtual void init();
		~StaticText();
		StaticText(){};
		void setAlpha(float a);
		void setString(string s);
		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual void hide();
	};

	class _ShmouletteExport RollingText:public StaticText{
		typedef StaticText Parent;

		//float mDeltaPile;
		list< list<Letter*> > mVectorLine;
		list< list<Letter*> >::iterator mWritingLine; // This is the line I'm currently writing
		list< list<Letter*> >::iterator mMaintenanceLine; // This is the line I'm scanning to see if it's done
		int staticTextPtr;

		protected:


		public:
		virtual void init(XML* xml);
		~RollingText();
		RollingText();
		
		void alignLine(list<Letter*> line);
		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual void hide();
		virtual bool update(double timeDelta, double parentTime);
		virtual void setAlpha(float a);


	};

	

}


#endif
