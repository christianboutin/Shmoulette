// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___FONT_H___
#define ___DEF___FONT_H___


#include <OgreTextAreaOverlayElement.h>
#include <OgreFontManager.h> 
#include "2D\font.h"
#include "cell.h"
#include "coord.h"
namespace Shmoulette{
	typedef enum TAG_FONT_COLOUR{
		FONT_COLOUR_PRIMARY_TOP,
		FONT_COLOUR_PRIMARY_BOTTOM,
		FONT_COLOUR_SECONDARY_TOP,
		FONT_COLOUR_SECONDARY_BOTTOM,
		FONT_COLOUR_TOTAL
	}FONT_COLOUR;

	class _ShmouletteExport Font{
		string mFilename;
		Ogre::FontManager & mFontMgr;
		Ogre::ResourcePtr mFontRsc;
		Ogre::FontPtr mFont;
		float mSize;
		float mHeight;
		string mId;
		

		public:
			Font();
			Font(string id);
			void setFilename(string filename);
			void setSize(float size){mSize = size;}
			string getFilename(){return mFilename;}
			string getName(){return mId;}
			void load();
			float getPixelLength(string str);
			float getPixelLength(char chr);
			float getPixelHeight(){return mHeight;}
	};

	class _ShmouletteExport FontStyleManager{
		private:
			FontStyleManager();
			static FontStyleManager* mFontStyleManager;
			list<class FontStyle*> mFontStyle;
		public:
			static FontStyleManager* getSingleton(){
				if (mFontStyleManager == NULL){
					mFontStyleManager = new FontStyleManager();
				}
				return mFontStyleManager;
			}
			class FontStyle* requestFontStyle(string name);
	};

	class _ShmouletteExport FontStyle{
		Shmoulette::Font* mFont;
		int mIdN;
		Ogre::ColourValue mColour[FONT_COLOUR_TOTAL];
		bool mDeleteFont;
		string mName;

		public:
			string getName(){return mName;}
			FontStyle(string name, string fontName, float size);
			FontStyle(string fontName);
			FontStyle(Font* font);
			Shmoulette::Font* getFont(){return mFont;}
			void setFont(Font* f){mFont=f;}
			class Letter* spawnLetter(char letter);
			void setColour(Ogre::ColourValue primary, Ogre::ColourValue secondary){
				mColour[FONT_COLOUR_PRIMARY_TOP] = primary;
				mColour[FONT_COLOUR_PRIMARY_BOTTOM] = primary;
				mColour[FONT_COLOUR_SECONDARY_TOP] = secondary;
				mColour[FONT_COLOUR_SECONDARY_BOTTOM] = secondary;
			}
			void setColour(
				Ogre::ColourValue primary_top, Ogre::ColourValue primary_bottom, 
				Ogre::ColourValue secondary_top, Ogre::ColourValue secondary_bottom){
				mColour[FONT_COLOUR_PRIMARY_TOP] = primary_top;
				mColour[FONT_COLOUR_PRIMARY_BOTTOM] = primary_bottom;
				mColour[FONT_COLOUR_SECONDARY_TOP] = secondary_top;
				mColour[FONT_COLOUR_SECONDARY_BOTTOM] = secondary_bottom;
			}
			Ogre::ColourValue getColour(FONT_COLOUR f){
				return mColour[f];
			}

	};

	
}

#endif
