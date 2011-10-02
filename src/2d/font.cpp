#include "stdafx.h"
#include "main.h"
#include "2d\text.h"
#include "2d\font.h"
#include "util\localstr.h"
#include "util\httoi.h"
#include "util\file.h"

namespace Shmoulette{
	//using namespace Ogre;
	Font::Font():mFontMgr(FontManager::getSingleton()){
		mId = "Font"+ts(GameElement::getNextIdCounter());
		mFilename = "";
	};

	void Font::setFilename(string filename){
		mFilename = filename;
	}

	Font::Font(string id):mFontMgr(FontManager::getSingleton()){
		mId = id;
	}


	void Font::load(){
		float size = mSize*(float)(Core::getSingleton()->getWindow()->getWidth()*2.0f)/1024;
		mFontRsc = mFontMgr.create(mId,"General");
		mFontRsc->setParameter("type","truetype");
		mFontRsc->setParameter("source",mFilename+".ttf");
		mFontRsc->setParameter("size", ts(size*4.0f));
		mFontRsc->setParameter("resolution","72");//ts(96.0f*(float)(Shmoulette::Core::getSingleton()->getWindow()->getWidth())/1024.0f));
		mFontRsc->load();
		try{
			mFont = FontManager::getSingleton().getByName(mId);
		}catch( ... ) {
			throw("Font not found : "+mId);
		}
		if (mFont.isNull()){
			throw("Font not found : "+mId);
		}
		//mHeight = Ogre::StringConverter::parseReal(mFontRsc->getParameter("size"));//StringConverter::parseReal(mFontRsc->getParameter("char_height"));
		//string strHeight = mFont->getTrueTypeSize->getS>getHeigh>getParameter("char_height");
		mHeight = size;//Ogre::StringConverter::parseReal(strHeight);//StringConverter::parseReal(mFontRsc->getParameter("char_height"));

		//mFont->setTrueTypeResolution(72);
		//mFont->setTrueTypeSize(mHeight);
		//float gluglu = font->getGlyphAspectRatio('M')*mHeight;
		//float gogo = getPixelLength("gluglu");
	}

	float Font::getPixelLength(string str){
		float width = 0;
		//string::iterator it;
		int i;
		for (i=0;i<str.length();i++){
			width += getPixelLength(str[i]);
		}
		return width;
	}

	float Font::getPixelLength(char chr){
		if (chr != 32){
			//return (mFont->getGlyphAspectRatio(chr)*mHeight)*2*(768/1024);
			//return (mFont->getGlyphAspectRatio(chr)*mHeight)*1.1;
			return ((mFont->getGlyphAspectRatio(chr)*mHeight));//*.75)*800/1024;
		}else{
			return (mFont->getGlyphAspectRatio(0x30)*mHeight);
		}
	}

	FontStyleManager* FontStyleManager::mFontStyleManager=NULL;

	FontStyleManager::FontStyleManager(){};

	FontStyle* FontStyleManager::requestFontStyle(string style){
		list <FontStyle*>::iterator it;
		for (it = mFontStyle.begin();it != mFontStyle.end();it++){
			if ((*it)->getName() == style){
				return (*it);
			}
		}
		/*File f(style+".xml", "media\\fonts");
		if (f.error() != FILE_OK){
			throw ("Font Style Not Found : "+style);
		}
		XML* xml = new XML(f.getStrContent());*/
		XML* xml = new XML(style+".xml", "media\\fonts");
		XMLIterator xIt(xml);
		xIt.setElemName("fontstyle");
		xIt.gotoZero();
		xIt.gotoNext();
		FontStyle* fs = new FontStyle(style, xIt.getElem().getString("font"), xIt.getElem().getFloat("size"));
		Ogre::ColourValue prim = colourValueFromString(xIt.getElem().getString("colour"));
		Ogre::ColourValue sec = colourValueFromString(xIt.getElem().getString("shadow"));
		fs->setColour(prim, sec);
		delete xml;
		mFontStyle.push_back(fs);
		return fs;
		
	}

	FontStyle::FontStyle(string name, string fontName, float size){
		mName = name;
		mIdN=0;
		mFont = new Shmoulette::Font(fontName+"_"+ts(size));
		mFont->setFilename(fontName);
		mFont->setSize(size);
		mFont->load();
		mDeleteFont = true;
	}

	FontStyle::FontStyle(string fontName){
		mIdN=0;
		mFont = new Font();
		mFont->setFilename(fontName);
		mFont->setSize(10);
		mFont->load();
		
		mDeleteFont = true;
	}

	FontStyle::FontStyle(Font* fnt){
		mIdN=0;
		mFont = fnt;
		mDeleteFont = false;
	}


	Letter* FontStyle::spawnLetter(char letter){
		
		string tmp;
		
		tmp.push_back(letter);
		return new Letter(letter, "letter_"+tmp+"_"+ts(mIdN++), this);
	}


}
