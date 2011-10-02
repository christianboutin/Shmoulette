#include "stdafx.h"
#include "main.h"
#include "2d\text.h"
#include "util\localstr.h"
#include "util\httoi.h"
#include "util\file.h"
//21.6 worst
namespace Shmoulette{
	using namespace Ogre;


#define LETTER_BASE_ETA .5

	Letter::Letter(unsigned char letter, string id, FontStyle* style){
		mBaseAlpha = 1;
		mFontStyle = style;
		mLetter = letter;
		mETA = LETTER_BASE_ETA;
		mGlow = false;
		mColour.a = 1;
		mAlphaDir = -.004;
		
		Ogre::ColourValue c;
		c.r = 1;
		c.g = 1;
		c.b = 1;
		c.a = 1;
		char caption[2];
		caption[0] = letter;
		caption[1] = '\0';

		mShadowElem = static_cast<TextAreaOverlayElement*>(
			Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", (string)(id).c_str()));
		mShadowElem->setMetricsMode(Ogre::GMM_PIXELS);
		mShadowElem->setPosition(0, 0);
		mShadowElem->setColourTop(mFontStyle->getColour(FONT_COLOUR_SECONDARY_TOP));
		mShadowElem->setColourBottom(mFontStyle->getColour(FONT_COLOUR_SECONDARY_BOTTOM));
		mShadowElem->setDimensions(mFontStyle->getFont()->getPixelLength(letter), mFontStyle->getFont()->getPixelHeight());
		mShadowElem->setCharHeight(mFontStyle->getFont()->getPixelHeight());
		mShadowElem->setFontName(mFontStyle->getFont()->getName());
		mShadowElem->hide();
		mShadowElem->setCaption(caption); 

		mShadowDelta = mFontStyle->getFont()->getPixelHeight()/20.0f;

		mElem = static_cast<TextAreaOverlayElement*>(
			Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", (string)(id+"_shadow").c_str()));
		mElem->setMetricsMode(Ogre::GMM_PIXELS);
		mElem->setPosition(0, 0);
		mElem->setColour(mFontStyle->getColour(FONT_COLOUR_PRIMARY_TOP));
		mElem->setDimensions(mFontStyle->getFont()->getPixelLength(letter), mFontStyle->getFont()->getPixelHeight());
		mElem->setCharHeight(mFontStyle->getFont()->getPixelHeight());
		mElem->setFontName(mFontStyle->getFont()->getName());
		mElem->hide();
		mElem->setCaption(caption); 
		mFadeAway = false;
	}

	Letter::~Letter(){
		Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
		om.destroyOverlayElement(mElem);
		om.destroyOverlayElement(mShadowElem);
	}

	void Letter::pushUp(){
		dY -= mFontStyle->getFont()->getPixelHeight();
	}
	void Letter::push(float x, float y){
		setPos(mX+x, mY+y,true);
	}

	void Letter::setGlow(bool glow){
		mGlow = glow;
		if (glow){
			mColour.a = 0;
			mAlphaDir = .04;
		}
	}
	void Letter::setPos(float x, float y, bool updateAbsolutePos){
		mElem->setPosition(x, y);
		mShadowElem->setPosition(x+mShadowDelta,y+mShadowDelta);
		if (updateAbsolutePos == true){
			mX = x;
			mY = y;
			dY = y;
		}
	}

	bool Letter::isInPlace(){
		return mETA <=0;
	}


	void Letter::hide(){
		mElem->hide();
		mShadowElem->hide();
	}

	void Letter::setAlpha(float alpha){
		Util::Log("Text Alpha : "+ts(alpha));
		mBaseAlpha = alpha;
	}

	void Letter::update(double timeDelta, double parentTime){
		if (mY > dY){
			mY-=mFontStyle->getFont()->getPixelHeight()/10;
			if (mY < dY) mY = dY;
		}
		if (mDelay > 0){
			mDelay-=timeDelta;//Level::getSingleton()->getTimeDelta();
			if (mDelay < 0){
				mETA += mDelay;
			}
		}else{
			if (!mShadowElem->isVisible()){
				mShadowElem->show();
				mElem->show();
				mColour.a = 0;
			}
			if (mETA > 0){
				mColour.a = 1-mETA;
				
				//setPos((mX-25)+mETA*25, mY, false);
				setPos((mX)-mETA*25, mY, false);
				//mElem->setPosition((mX-25)+mFrame, mY);
				//mShadowElem->setPosition((mX-23)+mFrame, mY+2);
			}else{
				mColour.a = 1;
				setPos(mX, mY, false);
			}
			if (mY < 0){
				mColour.a -= .2;
				if (mColour.a < 0) mColour.a = 0;
			}else{
				if (mGlow){
					mColour.a += mAlphaDir;
					if (mColour.a <0){
						mColour.a = 0;
						mAlphaDir = .04;
					}
					if (mColour.a > 1){
						mColour.a = 1;
						mAlphaDir = -.04;
					}
				}
			}
			if (mColour.a > 1) mColour.a = 1;
			Ogre::ColourValue c = mColour;
			c.a *= mBaseAlpha;
			mElem->setColour(c);
			//mShadowElem->setColourTop(Ogre::ColourValue(0,0,0,0));
			//mShadowElem->setColourBottom(Ogre::ColourValue(0,0,0,mColour.a));
			mETA-=timeDelta;//Level::getSingleton()->getTimeDelta();
		}
	}

	void Letter::setColour(float r, float g, float b){
		mColour.r = r;
		mColour.g = g;
		mColour.b = b;
		mColour.a = 1;

		mElem->setColour(mColour);
		//mElem->setColourBottom(Ogre::ColourValue(0, 0, 0,1));
		/*Ogre::ParameterList param;
		param = mElem->getParameters();*/
	}
	Text::Text():mOverlayManager(OverlayManager::getSingleton()){
	}

	void Text::init(){
		Parent::init("Panel", 550);
		mBox.x = 0;
		mBox.y = 0;
		mBox.w = 1;
		mBox.h = 1;
		mTextAlign = TEXT_ALIGN_LEFT;
		mTextVAlign = TEXT_ALIGN_TOP;
		mFontStyle = NULL;
		setString("nothing");
	}


	void Text::setAlign(string align){
		if (align =="left"){
			mTextAlign = TEXT_ALIGN_LEFT;
		}else if(align=="right"){
			mTextAlign = TEXT_ALIGN_RIGHT;
		}else{
			mTextAlign = TEXT_ALIGN_CENTER;
		}
	}

	void Text::setVAlign(string align){
		if (align =="center"){
			mTextVAlign = TEXT_ALIGN_CENTER;
		}else if(align=="bottom"){
			mTextVAlign = TEXT_ALIGN_BOTTOM;
		}else{
			mTextVAlign = TEXT_ALIGN_TOP;
		}
	}

	void Text::init(XML* xml){
		Parent::init(xml, "Panel");
		string lbl = xml->getString("label");
		if (xml->getError() != XML_OK){
			setString(Util::LocalStr::getSingleton()->getString(
				xml->getString("filename")
				));
		}else{
			setString(Util::LocalStr::getSingleton()->getString(
				xml->getString("filename"),
				lbl
				));
		}
		string p = xml->getString("pos");
		if (xml->getError() == XML_OK){
			string s = xml->getString("size");
			setBox(
				vector2FromString(p).x,
				vector2FromString(p).y,
				vector2FromString(s).x,
				vector2FromString(s).y);
		}else{
			setBox(
				xml->getFloat("x"),
				xml->getFloat("y"),
				xml->getFloat("w"),
				xml->getFloat("h")
				);
		}
		setZOrder(xml->getFloat("z"));
		setAlign(xml->getString("align"));
		setVAlign(xml->getString("valign"));
	}

	RollingText::RollingText(){
	};

	void RollingText::init(XML* xml){
		Parent::init(xml);
	}

	void Text::splitString(){
		int i;
		mSplitString.empty();
		float lX=0;
		
		string str;
		int maxLettersToPop = 0;

		for (i=0;i<mString.size();i++){
			if (mString[i] != '<'){
				int a = mString[i];
				if (mString[i] != 0x0D && mString[i] != 0x0A){
					str = str+ mString[i];
					maxLettersToPop++;
					lX+=mFontStyle->getFont()->getPixelLength(mString[i]);

					if (lX > (mBox.w*Core::getSingleton()->getWindow()->getWidth())){
						int lettersToPop = 0;
						while(mString[i] != ' ' && mString[i] != '>'){
							lettersToPop++;
							if (lettersToPop >= maxLettersToPop){
								throw("Text box too narrow (at least one word must fit in the box) : "+mString);
							}
							i--;
						}
						str = str.substr(0, str.length()-lettersToPop);
						mSplitString.push_back(str);
						str="";

						lX = 0;//mOverlayContainer->getLeft();
						maxLettersToPop=0;
					}
				}
			}else{
				i++;
				if (mString[i] == 'b' && mString[i+1] == 'r'){
					mSplitString.push_back(str);
					str="";
					lX = 0;//mOverlayContainer->getLeft();
					i++;
					maxLettersToPop=0;
				}
				if (mString[i] == '#'){
					string tmp_str;
					tmp_str = mString.substr(i+1, 2);
					//r = ::_httoi((TCHAR*)tmp_str.c_str())/255;
					i+=2;
					tmp_str = mString.substr(i+1, 2);
					//g = ::_httoi((TCHAR*)tmp_str.c_str())/255;
					i+=2;
					tmp_str = mString.substr(i+1, 2);
					//b = ::_httoi((TCHAR*)tmp_str.c_str())/255;
					i+=2;
				}
				if (mString[i] == 'g'){
					//glow = true;
				}
				if (mString[i] == '/'){
					i++;
					if (mString[i] == 'g'){
						//glow = false;
					}
				}
				i++;
			}
		}
		mSplitString.push_back(str);

	}

	string Text::_wordWrap(string inputString, FontStyle* fs, RXYWH box){
		string rv;

		float x = 0;
		Font* font = fs->getFont();

		vector<string> word;

	    string restOfString = inputString;

		while (restOfString.length() > 0){
			if (restOfString.find_first_of(" \r\n\\<") == 0){ // First character is a delimiter, it gets its own thing
				if (restOfString.length() == 1){ // Just 1 character left, and it's a delimiter no need to process it
					restOfString = restOfString.substr(1);
				}else{
					if (
						(restOfString[0] == '\r' && restOfString[1] == '\n')||
						(restOfString[0] == '\\' && restOfString[1] == 'n')||
						(restOfString[0] == '\\' && restOfString[1] == 'r')
						){
						word.push_back("\n");
						restOfString = restOfString.substr(2);
					}else if (restOfString[0] == '<'){
						restOfString = restOfString.substr(restOfString.find('>')); // skip the tab
					}else{
						word.push_back(restOfString.substr(0,1));
						restOfString = restOfString.substr(1);
					}
				}
			}else{  // it's a word
				string::size_type nextDelimiter=restOfString.find_first_of(" -\r\n\\<");
				if (restOfString[nextDelimiter] == '-'){ // Hyphened word...add the hyphen to the word, and procees forward
					nextDelimiter++;
				}
				if (nextDelimiter != string::npos){
					word.push_back(restOfString.substr(0,nextDelimiter));
					restOfString = restOfString.substr(nextDelimiter);
				}else{
					word.push_back(restOfString.substr(0));
					restOfString = "";
				}
			}
		}

		vector<string>::iterator it;

		string line="";
		for (it = word.begin();it!=word.end();it++){
			if ((*it) == "\n"){
				line = "";
				rv = rv + (*it);
			}else{
					line = line + (*it);
					float length = font->getPixelLength(line);
					float boxLength = box.w*Core::getSingleton()->getWindow()->getWidth();
					if (length <= boxLength){
						rv = rv + (*it);
					}else{
						if (line == (*it)){  // Only 1 word on the line
							rv = rv + (*it)+"\n";
						}else if ((*it) == " "){
							rv.push_back('\n');
							line = "";
						}else{
							rv[rv.length()-1] = '\n';
							rv = rv + (*it);
							line = (*it);
						}
					}
			}
		}

		return rv;
	}
	void Text::_wordWrap(){
		mString = _wordWrap(mString, mFontStyle,mBox);
		
	}

	void Text::setString(string str){
		mString = str;
	}
	void Text::setBox(float x, float y, float w, float h){
		mBox.x = x;
		mBox.y = y;
		mBox.w = w;
		mBox.h = h;
	}
	void Text::setFontStyle(FontStyle* fontStyle){
		mFontStyle = fontStyle;
	}
	void RollingText::alignLine(list<Letter*> vectorLine){
		Font* font = mFontStyle->getFont();
		list<Letter*> listLetter;
		float delta;
		float width=0;
		if (mTextAlign == TEXT_ALIGN_LEFT){
			delta = 0;
		}else{
			for (list<Letter*>::iterator it= vectorLine.begin();it!=vectorLine.end();it++){
				//if ((*it)->getY() == y){
					listLetter.push_back(*it);
				//}
			}

			for (list<Letter*>::iterator it=listLetter.begin();it != listLetter.end();it++){
				width+=font->getPixelLength((*it)->getLetter());
			}
			if (mTextAlign == TEXT_ALIGN_CENTER){
				delta = ((mBox.w*Core::getSingleton()->getWindow()->getWidth())-width)/2;
			}else{
				delta = (mBox.w*Core::getSingleton()->getWindow()->getWidth())-width;
			}
		}
		for (list<Letter*>::iterator it=listLetter.begin();it != listLetter.end();it++){
			(*it)->push(/*(mBox.x*Core::getSingleton()->getWindow()->getWidth())+*/delta, 0);
			width+=font->getPixelLength((*it)->getLetter());
		}
	}
	void RollingText::build(){
		setId("Text", true);
		string backupString = mString;
		setString("");
		Parent::build();
		setString(backupString);
		_wordWrap();
		int i;
		float r, g, b;
		bool glow = false;
		float lX, lY;

		mOverlayContainer->setPosition(mBox.x, mBox.y);
		mOverlayContainer->setDimensions(mBox.w, mBox.h);
		
		lX=0;
		lY=0;

		r=mFontStyle->getColour(FONT_COLOUR_PRIMARY_TOP).r;
		g=mFontStyle->getColour(FONT_COLOUR_PRIMARY_TOP).g;
		b=mFontStyle->getColour(FONT_COLOUR_PRIMARY_TOP).b;

		float delay=0;
		//int maxLettersToPop = 0;

		list<Letter*> vectorLetter;
		for (i=0;i<mString.size();i++){
			if (mString[i] != '<' && mString[i] != '\n'){
				int a = mString[i];
				if (mString[i] != 0x0D && mString[i] != 0x0A){
					Letter* tmpLetter = new Letter(mString[i], getId()+ts(i), mFontStyle);
					tmpLetter->setColour(r, g, b);
					tmpLetter->setDelay(delay);
					switch (mString[i]){
						case ',':
							delay+=1;
						break;
						case '.':
							if (mString[i+1] == ' '){
								delay+=2;
							}
						break;
						default:
							delay+=.075;
						break;
					}
					//maxLettersToPop++;
					tmpLetter->setPos(lX, lY);
					if (glow) tmpLetter->setGlow(true);
					lX+=mFontStyle->getFont()->getPixelLength(mString[i]);
					mOverlayContainer->addChild(tmpLetter->getShadowElem());
					mOverlayContainer->addChild(tmpLetter->getElem());
					
					vectorLetter.push_back(tmpLetter);
					
					//mOverlayContainer->getWidth()*
					/*if (lX > (mBox.w*Core::getSingleton()->getWindow()->getWidth())){
						int lettersToPop = 0;
						while(mString[i] != ' ' && mString[i] != '>'){
							lettersToPop++;
							if (lettersToPop >= maxLettersToPop){
								throw("Text box too narrow (at least one word must fit in the box) : "+mString);
							}
							i--;
						}
						for (int k=0;k<lettersToPop;k++){
							list<Letter*>::iterator it;
							Letter* ptr;
							it = mVectorLetter.end();
							it--;
							ptr = *it;
							mOverlayContainer->removeChild(ptr->getElem()->getName());
							mOverlayContainer->removeChild(ptr->getShadowElem()->getName());
							delete ptr;
							mVectorLetter.pop_back();
						}


						lX = 0;//mOverlayContainer->getLeft();
						alignLine(lY);
						lY += mFontStyle->getFont()->getPixelHeight();
						maxLettersToPop=0;
					}*/
				}
			}else{
				if (mString[i] == '\n'){
					alignLine(vectorLetter);
					mVectorLine.push_back(vectorLetter);
					vectorLetter.clear();
					lX = 0;//mOverlayContainer->getLeft();
					lY += mFontStyle->getFont()->getPixelHeight();
					if (i > 0){
						if (mString[i-1] == '.' || mString[i-1] == '\n'){
							delay=2;
						}else if (mString[i-1] == ','){
							delay=1;
						}else{
							delay=.075;
						}
					}else{
						delay = 0;
					}
				}else{
					i++;
					/*if (mString[i] == 'b' && mString[i+1] == 'r'){
						lX = 0;//mOverlayContainer->getLeft();
						alignLine(vectorLetter);
						mVectorLine.push_back(vectorLetter);
						vectorLetter.clear();
						lY += mFontStyle->getFont()->getPixelHeight();
						i++;
						delay=.25;
						//maxLettersToPop=0;
					}*/
					if (mString[i] == '#'){
						string tmp_str;
						tmp_str = mString.substr(i+1, 2);
						r = ::_httoi((TCHAR*)tmp_str.c_str())/255;
						i+=2;
						tmp_str = mString.substr(i+1, 2);
						g = ::_httoi((TCHAR*)tmp_str.c_str())/255;
						i+=2;
						tmp_str = mString.substr(i+1, 2);
						b = ::_httoi((TCHAR*)tmp_str.c_str())/255;
						i+=2;
					}
					if (mString[i] == 'g'){
						glow = true;
					}
					if (mString[i] == '/'){
						i++;
						if (mString[i] == 'g'){
							glow = false;
						}
					}
					i++;
				}
			}
		}
		alignLine(vectorLetter);
		mVectorLine.push_back(vectorLetter);
		vectorLetter.clear();

		mWritingLine = mVectorLine.begin();
		mMaintenanceLine = mVectorLine.begin();

		lY += mFontStyle->getFont()->getPixelHeight();
		float bHeight = mBox.h/**Core::getSingleton()->getWindow()->getHeight()*/;
		if ((lY/Core::getSingleton()->getWindow()->getHeight()) < bHeight){
			if (mTextVAlign == TEXT_ALIGN_CENTER){
				float motherFuckingDelta = bHeight-(lY/Core::getSingleton()->getWindow()->getHeight());
				for (list< list<Letter*> >::iterator line = mVectorLine.begin(); line != mVectorLine.end();line++){
					list<Letter*> vectorLine = (*line);
					for (list<Letter*>::iterator it=vectorLetter.begin();it != vectorLetter.end();it++){
						(*it)->push(0, motherFuckingDelta*Core::getSingleton()->getWindow()->getHeight());
					}
				}
			}
		}
		mOverlayContainer->show();
		staticTextPtr = 0;
	}
	void RollingText::destroy(){
		// Brought that back to ::destroy as it should.  As I now destroy things as I go.  This is just maintenance, in case
		//// Keep most of the destructive work in the actual destructor.  These things take like forever
		//// To be clean, this should really happen in the ::destroy method.  But it takes way too long
		list<Letter*>::iterator it;
		for (list< list<Letter*> >::iterator line = mVectorLine.begin();line!=mVectorLine.end();line++){
			for (it = (*line).begin();it!=(*line).end();it++){
				mOverlayContainer->_removeChild((*it)->getElem());
				mOverlayContainer->_removeChild((*it)->getShadowElem());
				delete (*it);
			}
		}
		mVectorLine.clear();
		
		hide();
		Parent::destroy();
	}

	void RollingText::show(){
		Parent::show();
	}

	void RollingText::hide(){
		mOverlayContainer->hide();
		/*for (int i=0;i<mVectorLetter.size();i++){
			mVectorLetter[i]->hide();
		}*/
		Parent::hide();
	}

	void RollingText::setAlpha(float a){
		list<Letter*>::iterator it;
		for (list< list<Letter*> >::iterator line = mVectorLine.begin();line!=mVectorLine.end();line++){
			for (it = (*line).begin();it!=(*line).end();it++){
				(*it)->setAlpha(a);
			}
		}
		Parent::setAlpha(a);
	}

	RollingText::~RollingText(){
	}
	bool RollingText::update(double timeDelta, double parentTime){
		Ogre::Real top = mOverlayContainer->_getTop();
		Ogre::Real left = mOverlayContainer->_getLeft();

		Parent::update(timeDelta, parentTime);
		if (mIsBuild == true){
			if (mIsOn == true){
				if (mWritingLine != mVectorLine.end()){
						list<Letter*>::iterator it;
						bool skipWritingLineToNextLine = true;
						for (it = (*mWritingLine).begin();it!=(*mWritingLine).end();it++){
							(*it)->update(timeDelta, parentTime);
							if ((*it)->isVisible()){
								// Pretty sure I'll have to redo this :-(
								/*if ((*it)->getDy() > mOverlayContainer->getHeight()*Core::getSingleton()->getWindow()->getHeight()){//mOverlayContainer->getTop()+mOverlayContainer->getHeight()){
									list<Letter*>::iterator jit;
									for (jit = (*mWritingLine).begin();jit!=(*mWritingLine).end();jit++){
										(*jit)->pushUp();
									}
								}*/
							}else{
								skipWritingLineToNextLine = false;
							}
						}
						if (skipWritingLineToNextLine){
							mWritingLine++;
						}
				}
				if (mMaintenanceLine != mVectorLine.end()){
						list<Letter*>::iterator it;
						bool skipMaintenanceLineToNextLine = true;
						for (it = (*mMaintenanceLine).begin();it!=(*mMaintenanceLine).end();it++){
							(*it)->update(timeDelta, parentTime);
							if (!(*it)->isInPlace()){
								skipMaintenanceLineToNextLine = false;
							}
						}
						if (skipMaintenanceLineToNextLine){
							for (it = (*mMaintenanceLine).begin();it!=(*mMaintenanceLine).end();it++){
								mOverlayContainer->_removeChild((*it)->getElem());
								mOverlayContainer->_removeChild((*it)->getShadowElem());
								delete (*it);
							}
							string subStr = mString.substr(staticTextPtr);
							int subStrDel = subStr.find("\n");
							if (subStrDel != string::npos){
								staticTextPtr += subStrDel;
								staticTextPtr++;
							}else{
								staticTextPtr = mString.length();
							}
							
							mElem->setCaption(mString.substr(0,staticTextPtr));
							mShadowElem->setCaption(mString.substr(0,staticTextPtr));
							(*mMaintenanceLine).clear();
							mMaintenanceLine++;
						}
				}
			}
		}
		top = mOverlayContainer->_getTop();
		left = mOverlayContainer->_getLeft();

		return false;
	}

	//StaticText::StaticText():Parent(){};

	void StaticText::init(){
		Parent::init();
	}

	void StaticText::setString(string str){
		Parent::setString(str);
		if (isOn()){
			mElem->setCaption(str);
			mShadowElem->setCaption(str);
		}
	}


	void StaticText::init(XML* xml){
		Parent::init(xml);
	}

	StaticText::~StaticText(){
	};

	void StaticText::build(){
		Parent::build();
		//	splitString();
		//mString.replace("\\n", "\n");

		mString = mString;
		string::size_type st;
		while ((st = mString.find('\\')) != string::npos){
			mString[st] = '\r';
			mString[st+1] = '\n';
		}
		while ((st = mString.find("<br>")) != string::npos){
			mString[st] = '\r';
			mString[st+1] = '\n';
			mString.erase(st+2,2);
		}

		_wordWrap();

		Ogre::ColourValue c;
		mOverlayContainer->setPosition(mBox.x, mBox.y);
		mOverlayContainer->setDimensions(mBox.w, mBox.h);
		//mOverlayContainer->setMaterialName("PureWhite");

		mElem = static_cast<TextAreaOverlayElement*>(
			Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", (string)"b"+(mId).c_str()));

		mShadowElem = static_cast<TextAreaOverlayElement*>(
			Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", (string)"a"+(mId+"_shadow").c_str()));

		
		c = mFontStyle->getColour(FONT_COLOUR_PRIMARY_TOP);
		mElem->setColour(c);
		mElem->setDimensions(mBox.w,mBox.h);

		mElem->setFontName(mFontStyle->getFont()->getName());
		mElem->setCaption(mString); 

		float height = mFontStyle->getFont()->getPixelHeight()/Core::getSingleton()->getWindow()->getHeight();
		mElem->setCharHeight(height);
		float x,y;

		switch (mTextVAlign){
			case TEXT_ALIGN_CENTER:
				y = (mBox.h/2)-((mElem->getCharHeight()*(float)Util::stringNbLines(mString))/2);
			break;
			case TEXT_ALIGN_BOTTOM:
				y = mBox.h - mElem->getCharHeight()*(float)Util::stringNbLines(mString);//*mString.get>getHeight();//0;//mBox.h-height;
			break;
			case TEXT_ALIGN_TOP:
				y = 0;
			break;
		};

		switch (mTextAlign){
			case TEXT_ALIGN_CENTER:
				x = mBox.w/2;
				mElem->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Center);
			break;
			case TEXT_ALIGN_LEFT:
				x = 0;
				mElem->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Left);
			break;
			case TEXT_ALIGN_RIGHT:
				x = mBox.w;
				mElem->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Right);
			break;
		};
		mElem->setPosition(x,y);

		c = Ogre::ColourValue(0,0,0,.6);
		mShadowElem->setColour(c);
		mShadowElem->setDimensions(mBox.w,mBox.h);
		mShadowElem->setFontName(mFontStyle->getFont()->getName());
		mShadowElem->setCaption(mString); 
		mShadowElem->setCharHeight(height);
		mShadowElem->setPosition(x+height/20,y+height/20);
		mShadowElem->setAlignment(mElem->getAlignment());

		mOverlayContainer->addChild(mShadowElem);
		mOverlayContainer->addChild(mElem);
	}

	void StaticText::show(){
		Parent::show();
		mElem->show();
		mShadowElem->show();
	}

	void StaticText::hide(){
		Parent::hide();
		mElem->hide();
		mShadowElem->hide();
	}

	void StaticText::destroy(){
		Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
		om.destroyOverlayElement(mElem);
		om.destroyOverlayElement(mShadowElem);
		Parent::destroy();
	}

	void StaticText::setAlpha(float a){
		Ogre::ColourValue c;
		
		c = mElem->getColour();
		c.a = a;

		mElem->setColour(c);

		c = mShadowElem->getColour();
		c.a = a*.6;

		mShadowElem->setColour(c);
	}

	void Text::setAlpha(float a){

	};

	Text::~Text(){
	};

	void Text::build(){
		if (mFontStyle == NULL) throw("Text build without font ("+mId+")");
		Parent::build();
	};

	void Text::destroy(){
		Parent::destroy();
	};

	void Text::show(){
		Parent::show();
	};

	void Text::hide(){
		Parent::hide();
	};

	bool Text::update(double timeDelta, double parentTime){return Parent::update(timeDelta, parentTime);};


}
