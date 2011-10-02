#include "stdafx.h"

//#include "string"
#include "main.h"
//#include "..\util\stringutil.h"
//#include "..\xml.h"
#include "trigger\event.h"
#include "2D\text.h"
#include "2D\talkwindow.h"
#include "2D\timer.h"
#include "sound.h"
#include "util\localstr.h"
#include "elements\camera.h"

#include "level.h"
namespace Shmoulette{
extern class Level* gLevel;
	namespace EventList{
		Event::Event(){};
		
		Goto::Goto(XML* xml){
			destination = xml->getFloat("value");
			if (xml->getError() != XML_OK){
				destination = xml->getFloat("dest");
				if (xml->getError() != XML_OK){
					throw("Goto trigger without 'dest' or 'value' parameter");
				}
			}
		}
		void EndSegment::trigger(){
			Level::getSingleton()->endSegment();
		}

		void Goto::trigger(){
			Level::getSingleton()->getCurrentSegment()->setTimeTrack(destination);
		}

		Talk::Talk(XML* xml){
			TalkWindow* tw = new TalkWindow();
			tw->init(xml);
			Level::getSingleton()->getCurrentSegment()->addGameElement(tw);
		}
		void Talk::trigger(){
			// once the talkwindow is created it will take care of itself
		}

		void CinemaOn::trigger(){
			Level::getSingleton()->cinema(true);
		}

		void CinemaOff::trigger(){
			Level::getSingleton()->cinema(false);
		}
		void PlaneDepth::trigger(){
			Level::getSingleton()->getCurrentSegment()->setRailType(RAIL_DEPTH);
		}
		void PlaneVertical::trigger(){
			Level::getSingleton()->getCurrentSegment()->setRailType(RAIL_VERTICAL);
		}
		void PlaneHorizontal::trigger(){
			Level::getSingleton()->getCurrentSegment()->setRailType(RAIL_HORIZONTAL);
		}
		MusicStart::MusicStart(XML* xml){
			Level::getSingleton()->resetTimeCatchup();// Since the time catchup variable is so the game tries to remain in sync with the music, reset it on a new tune
			mFilename = "music\\"+xml->getString("filename");
			try{
				mRepeat = boolFromString(xml->getString("repeat"));
			}catch(string s){
				mRepeat = false;
			}
		}
		void MusicStart::trigger(){
			SoundManager::getSingleton()->startMusic(mFilename, mRepeat);
		}
		MusicFadeOut::MusicFadeOut(XML* xml){
			mLength = xml->getFloat("length");
			if (xml->getError() != XML_OK){
				mLength = 4;
			}
		}
		void MusicFadeOut::trigger(){
			SoundManager::getSingleton()->musicFadeOut(mLength);
		}

		TimeSpeed::TimeSpeed(XML* xml){
			mWanted = xml->getFloat("value");
			mAcceleration = xml->getFloat("acceleration");
		}
		void TimeSpeed::trigger(){
			Level::getSingleton()->setSpeedModifier(mWanted, mAcceleration);
		}
		void WeaponsHold::trigger(){
			Level::getSingleton()->getPlayerShip()->weaponsHold();
		}
		void WeaponsFree::trigger(){
			Level::getSingleton()->getPlayerShip()->weaponsFree();
		}

		void CameraShake::trigger(){
			Level::getSingleton()->getCurrentSegment()->getActiveCamera()->shake(mStrength);
		}
		PlayerEvent::PlayerEvent(XML* xml){
			mEvent = xml->getString("event");
			if (xml->getError() != XML_OK) throw("Player Event without 'event'");
		}

		void PlayerEvent::trigger(){
			Level::getSingleton()->getPlayerShip()->specialEvent(mEvent);
		}
		CameraShake::CameraShake(XML* xml){
			mStrength = xml->getFloat("strength");
			if (xml->getError() != XML_OK){
				throw("CameraShake trigger without strength");
			}
		}

		FogStateChange::FogStateChange(XML* xml){
			mTimer = xml->getFloat("timer");
			mFogInfo.colour = colourValueFromString(xml->getString("colour"));
			mFogInfo.linearStart = xml->getFloat("linearstart");
			mFogInfo.linearEnd = xml->getFloat("linearend");
		}
		void FogStateChange::trigger(){
			Level::getSingleton()->getCurrentSegment()->fogChange(mFogInfo, mTimer);
		}
		Sound::Sound(XML* xml){
			mSound = SoundManager::getSingleton()->createSound(xml->getString("filename"));
		};
		Sound::Sound(string filename){
			mSound = SoundManager::getSingleton()->createSound(filename);
		}
		void Sound::trigger(){
			mSound->play();
		}
		Sound::~Sound(){
			SoundManager::getSingleton()->destroySound(mSound);
		}
		void Subtitle::_constructor(string filename, string label, double timeOn, double length){
			string xmlString;
			
			xmlString = "<cell id=\""+filename+"_"+label+"\" ";
			xmlString += "x=\"0\" y=\"0\" timeon=\"0\" life=\""+ts(length)+"\">\n";
			xmlString += "	<textzone type=\"static\" filename=\""+filename+"\"";
			if (label != ""){
				xmlString += "label=\""+label+"\" ";
			}
			xmlString += "style=\"introtext\" timeon=\"0\" life=\""+ts(length)+"\" ";

			xmlString += "x=\".1\" y=\".8\" w=\".8\" h=\".2\" z=\"510\" align=\"center\" valign=\"center\"/>\n";
			xmlString += "	<point time=\"0\" posx=\"0\" posy=\"0\" sclx=\"1\" scly=\"1\" angle=\"0\" alpha=\"1\"/>";
			xmlString += "</cell>";
			Shmoulette::XML xml(xmlString);

			mCell = new Cell();
			mCell->init(&xml);
		}
		Subtitle::Subtitle(XML* xml){
			string filename = xml->getString("filename");
			if (filename.find(".") == string::npos){
				filename+=".txt";
			}
			string label = xml->getString("label");
			
			if (xml->getError() != XML_OK){
				label = "";
			}
			_constructor(filename, label, xml->getDouble("time"), xml->getDouble("life"));
		}
		Subtitle::Subtitle(string filename, string label, double timeOn, double length){
			_constructor(filename, label, timeOn, length);
			/*if (label != ""){
				mString = Util::LocalStr::getSingleton()->getString(filename,label);
			}else{
				mString = Util::LocalStr::getSingleton()->getString(filename);
			}*/
		}
		void Subtitle::trigger(){
			mCell->moveTimes(Level::getSingleton()->getCurrentSegment()->getTimeTrack());//addGameElement(mCell);
			Level::getSingleton()->getCurrentSegment()->addGameElement(mCell);
		}

		void Subtitle::trigger(double timeDelta){
			mCell->moveTimes(timeDelta);
			trigger();
		}
		void Subtitle::moveTimes(double delta){
			//mCell->moveTimes(delta);
		}

		Speech::Speech(XML* xml){
			string sndFilename = xml->getString("sndfilename");
			mSound = SoundManager::getSingleton()->createSound("speech\\"+sndFilename);

			string stfilename = xml->getString("stfilename");
			double time = xml->getDouble("time");
			
			XMLIterator it(xml);
			it.setElemName("trigger>subtitle");
			it.gotoZero();

			string stlabel="";
			double sttime=0;
			bool createSub = false;
			Subtitle* sub;
			while(it.gotoNext() != false){
				XML xml2 = it.getElem();
				if (createSub){
					sub = new Subtitle(
						stfilename, 
						stlabel, 
						time+sttime, 
						xml2.getDouble("time")-sttime);
					mSub.push_back(sub);
					mTimeDelta.push_back(sttime);
				}
				

				stlabel = xml2.getString("label");
				sttime = xml2.getDouble("time");
				createSub = true;
			}
			if (createSub){
				sub = new Subtitle(
					stfilename,
					stlabel,
					time+sttime, 
					mSound->getLength()-sttime);
				mSub.push_back(sub);
				mTimeDelta.push_back(sttime);
			}
		}

		void Speech::trigger(){
			if (!Level::getSingleton()->isFastForward()){
			//if (Level::getSingleton()->getTimeDelta() <= Level::getSingleton()->getMaxFrameTime()){
				mSound->play(true);//trigger();
				list<Subtitle*>::iterator it;
				list<double>::iterator tit;
				for (it = mSub.begin(),tit=mTimeDelta.begin();it!=mSub.end();it++,tit++){
					(*it)->trigger((*tit));
				}
			}
		}

		Speech::~Speech(){
			delete mSound ;
		}

		void Speech::moveTimes(double delta){
			/*list<Subtitle*>::iterator it;
			for (it = mSub.begin();it != mSub.end();it++){
				(*it)->moveTimes(delta);
			}*/
		}
		
		BreakLoop::BreakLoop(XML* xml){}

		void BreakLoop::trigger(){
			Level::getSingleton()->getCurrentSegment()->breakCurrentLoop();
		}
		
		Timer::Timer(XML* xml){
			mTime = xml->getDouble("value");
			if (xml->getError() != XML_OK) throw("Timer trigger missing time value");
			//double voidTime = xml->getDouble("void_time");
			//if (xml->getError() != XML_OK) throw("Timer trigger missing void_time value");
			string label = xml->getString("label");
			if (xml->getError() != XML_OK) throw("Timer trigger missing label value");
		}

		void Timer::trigger(){
			Level::getSingleton()->setTimerTime(mTime);
		}
		Timer::~Timer(){
		}
		void VoidTimer::trigger(){
			Level::getSingleton()->setTimerTime(0);
		}

		AddCompositor::AddCompositor(XML* xml){
			mType = xml->getString("value");
			if (xml->getError() != XML_OK) throw("Compositor trigger missing value value");
			mParam = xml->getString("params");
			if (xml->getError() != XML_OK){
				mParam="";
			}
		}

		void AddCompositor::trigger(){
			Level::getSingleton()->addCompositor(mType, mParam);
		}

		Script::Script(XML* xml){
			string name = xml->getString("name");
			mName = PyString_FromString(name.c_str());
				
			if (xml->getError() != XML_OK) throw("Script trigger missing name value");
		}

		void Script::trigger(){
			PyObject* pModule = PyImport_Import(mName);
			if (pModule == NULL){
				throw(Core::getSingleton()->getPythonException()+"\n(in loading module "+PyString_AsString(mName)+".py)");
			}
		}


		Zone::Zone(XML* xml){
			string filename = xml->getString("value")+".xml";
			if (xml->getError() != XML_OK) throw("Zone trigger missing value value");
			File f(filename, "media/levels/zones");
			if (f.error() != FILE_OK) throw("Zone file not found : "+filename);
			XML x(f.getStrContent());


			mSize = vector3FromString(x.getString("size"));
			mAmmoSize = vector3FromString(x.getString("ammo_size"));
			if (x.getError() != XML_OK){
				mAmmoSize = mSize*2;
			}
			mCamSize = vector2FromString(x.getString("cam_size"));
			mCamTilt = vector2FromString(x.getString("cam_tilt"));
			mTransitTime = xml->getDouble("transit_time");
			if (xml->getError() != XML_OK){
				mTransitTime = 0;
			}

			string d = x.getString("direction");
			if (d == "h"){
				mType = RAIL_HORIZONTAL;
				if (mSize.x != 0){
					throw("Horizontal Zones must have an X size of 0");
				}
				if (mSize.y == 0){
					throw("Horizontal Zones must NOT have an Y size of 0");
				}
				if (mSize.z == 0){
					throw("Horizontal Zones must NOT have an Z size of 0");
				}
			}else if (d == "v"){
				mType = RAIL_VERTICAL;
				if (mSize.x == 0){
					throw("Vertical Zones must NOT have an X size of 0");
				}
				if (mSize.y != 0){
					throw("Vertical Zones must have an Y size of 0");
				}
				if (mSize.z == 0){
					throw("Vertical Zones must NOT have an Z size of 0");
				}
			}else if (d == "d"){
				mType = RAIL_DEPTH;
				if (mSize.x == 0){
					throw("Depth Zones must NOT have an X size of 0");
				}
				if (mSize.y == 0){
					throw("Depth Zones must NOT have an Y size of 0");
				}
				if (mSize.z == 0){
					throw("Depth Zones must NOT have an Z size of 0");
				}
			}
		}

		void Zone::trigger(){
			Level::getSingleton()->getCurrentSegment()->setRailType(mType);
			Level::getSingleton()->getCurrentSegment()->setZoneInfo(mSize, mAmmoSize, mCamSize, mCamTilt, mTransitTime);

		}

		SceneShadows::SceneShadows(XML* xml){
		}
		void SceneShadows::trigger(){
		}
		AmbientChange::AmbientChange(XML* xml){
		}
		void AmbientChange::trigger(){
		}
		SetSky::SetSky(XML* xml){
			mSkyDef.mType = SKY_CUBE;
			mSkyDef.mMaterial = xml->getString("material");
			if (xml->getError() != XML_OK) throw("SetSky trigger missing material value");
			mSkyDef.mDistance = xml->getFloat("distance");
			if (xml->getError() != XML_OK) throw("SetSky trigger missing distance value");
		}

		void SetSky::trigger(){
			Level::getSingleton()->getSceneManager()->setSkyBox(
				true,
				mSkyDef.mMaterial, 
				mSkyDef.mDistance
				);
		}
	}
}


/*
GotoTrigger::GotoTrigger(XML* xml):Parent(xml){
	mGotoTime = 0;
	mGotoTime = xml->getFloat("value");
};
void GotoTrigger::run(){
	Util::Log("Goto : "+ts(mGotoTime));
	Level::getSingleton()->setTimeTrack(mGotoTime);
}

string GotoTrigger::dataToXML(bool raw){
	string rv = Parent::dataToXML(true);
	return rv+" value=\""+ts(mGotoTime)+"\"/>\n";
};

*/
