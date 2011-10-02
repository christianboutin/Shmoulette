#include "stdafx.h"
#include "main.h"
#include "elements\gameelement.h"
namespace Shmoulette{

int GameElement::mIdCounter = 0;
_ShmouletteExport LevelSegment* GameElement::sLoadTimeSegmentPlaceholder = NULL;

	void GameElementParams::setTimes(double b, double on, double off, double d){
		mTimeBuild = b;
		mTimeOn = on;
		mTimeOff = off;
		mTimeDestroy = d;
	}
	void GameElementParams::setTimes(double b, double d){
		mTimeBuild = b;
		mTimeOn = b;
		mTimeOff = d;
		mTimeDestroy = d;
	}
	GameElementParams::GameElementParams(){
		mId = "OtherElem_"+GameElement::getNextIdCounter();
		mTimeOn = 0;
		mTimeBuild = 0;
		mTimeOff = 9999;
		mTimeDestroy = 9999;
	}
	void GameElementParams::setParam(string param, string value){

	}





void GameElement::setId(){
	setId("Noname"+ts(mIdCounter++));
}

bool GameElement::shouldBeBuild(double time){
	if (time == FLT_MAX) time=mTimeOfParent;
	if (time >= mTimeBuild && time < mTimeDestroy){
		return true;
	}
	return false;
}
bool GameElement::isBuild(){//float time){
	return mIsBuild;
	/*if (time >= mTimeBuild && time <= mTimeDestroy){
		if (time == mLevelSegment->getTimeTrack()){
			if (!mIsBuild){ // Sometimes other applications will test this to see if they can interact with it BEFORE it gets updated.
				build();
				if (isOn()){
					show();
				}else{
					hide();
				}
			}
		}
		
		return true;
	}
	return false;*/
}
bool GameElement::shouldBeOn(double time){
	if (time == FLT_MAX) time=mTimeOfParent;
	if (time >= mTimeOn && time < mTimeOff){
		return true;
	}
	return false;
}
bool GameElement::isOn(){//float time){
	return mIsOn;
	/*
	if (time >= mTimeOn && time <= mTimeOff){
		if (time == mLevelSegment->getTimeTrack()){
			if (!mIsBuild){// Sometimes other applications will test this to see if they can interact with it BEFORE it gets updated.
				build();
				if (isOn()){
					show();
				}else{
					hide();
				}
			}
				
		}
		return true;
	}
	return false;*/
}

void GameElement::setDefaultVariables(){
	mMarkedForTermination = false;
	mLevelSegment = sLoadTimeSegmentPlaceholder;
	mIsBuild = false;
	mIsOn = false;
	mXml = new XML();
	mTimeBuild = 0;
	mTimeOn = 0;
	mTimeOff = 9999;
	mTimeDestroy = 9999;
	mCountdownToDeath = FLT_MAX;
	mAutoManage = true;
	mTimeOfParent = 0;
	//mTimeAlive = 0;
#ifdef PROFILER_ON
	mProfiler = NULL;
#endif
}

void GameElement::init(GameElementParams& params){
	setDefaultVariables();
	MACRO_TRANSFER_PARAM(mTimeOn);
	MACRO_TRANSFER_PARAM(mTimeOff);
	MACRO_TRANSFER_PARAM(mTimeBuild);
	MACRO_TRANSFER_PARAM(mTimeDestroy);
	MACRO_TRANSFER_PARAM(mId);
	MACRO_TRANSFER_PARAM(mAutoManage);
}
/*GameElement::GameElement(vector<string>& param){
#ifdef PROFILER_ON
	mProfiler = NULL;
#endif
}*/
// Ex : ["gluglu", true]  // Named "gluglu" with number append
// ["gluglu", true, 0, 10] // From 0 second lasting 10 seconds
// ["gluglu", true, 0, 5, 10, 15] // Built at 0 seconds, on at 5, off at 10 and destroyed at 15
void GameElement::init(PyObject* pyObject){
#ifdef PROFILER_ON
	mProfiler = NULL;
#endif
	if (PyList_Check(pyObject) == true && PyList_Size(pyObject) > 0){
		PyObject* o = PyList_GetItem(pyObject, 0);
		if (PyList_Check(o) == false){
			throw("GameElement : o should be a list.  Too few parameters?");
		}
// Possible combinations
// 2 parameters : ID and Append, then object is alive all the time during the segment
// 4 parameters : Add timeon and life, off, build and destroy are deduced by it
// 6 parameters : ID, Append, timebuild, timeon, timeoff, timedestroy
		if (PyList_Size(o) == 1){
			setDefaultVariables();
			setId(PyString_AsString(PyList_GetItem(o, 0)), true);

		}else if (PyList_Size(o) >= 2){
			PyObject* i = PyList_GetItem(o, 0);
			string s = PyString_AsString(i);
			i = PyList_GetItem(o, 1);
			bool append = i == Py_True;
			setDefaultVariables();
			setId(s, append);
		}
		if (PyList_Size(o) == 3){
			throw("GameElement : o should be a list of 1, 2, 4 or 6 elements");
		}
		
		if (PyList_Size(o) >= 4){
			setTimeBuild(PyFloat_AsDouble(PyList_GetItem(o,2)));
			setTimeOn(PyFloat_AsDouble(PyList_GetItem(o,2)));
			setTimeOff(PyFloat_AsDouble(PyList_GetItem(o,3)));
			setTimeDestroy(PyFloat_AsDouble(PyList_GetItem(o,3)));
		}

		if (PyList_Size(o) == 6){
		}
		
		//	throw("GameElement : o should be an array of either size 1, 2, 4 or 6");
		//}
	}else{
		throw("GameElement : PyObject should be an array of at least size 1");
	}
}

void GameElement::init(string id, bool append_object_number){
	setDefaultVariables();
	setId(id, append_object_number);
}


void GameElement::init(){
	setDefaultVariables();
	setId();
	mXml->setContent("");
}
void GameElement::init(XML* xml){
	setDefaultVariables();
	mXml->setContent(xml->getContent());
	
	double tmpVal;
	tmpVal = xml->getDouble("timeon");
	if (xml->getError() == XML_OK){
		setTimeOn(tmpVal);
	}else{
		setTimeOn(0);
	}

	tmpVal = xml->getDouble("timeoff");
	if (xml->getError() == XML_OK){
		setTimeOff(tmpVal);
	}else{
		double life = mTimeOn+xml->getDouble("life");
		if (xml->getError() == XML_OK){
			setTimeOff(mTimeOn+xml->getDouble("life"));
		}else{
			setTimeOff(FLT_MAX);
		}
	}

	tmpVal = xml->getDouble("timebuild");
	if (xml->getError() == XML_OK){
		setTimeBuild(tmpVal);
	}else{
		setTimeBuild(mTimeOn);
	}

	tmpVal = xml->getDouble("timedestroy");
	if (xml->getError() == XML_OK){
		setTimeDestroy(tmpVal);
	}else{
		setTimeDestroy(mTimeOff);
	}

	if (mTimeOff > mTimeDestroy){
		mTimeOff = mTimeDestroy;
	}

	if (mTimeOn < mTimeBuild){
		mTimeOn = mTimeBuild;
	}

	string tmpStr = xml->getString("id");
	if (xml->getError() == XML_OK){
		setId(tmpStr);
	}else{
		tmpStr = xml->getString("name");
		if (xml->getError() == XML_OK){
			setId(tmpStr+ts(mIdCounter++));
		}else{
			tmpStr = xml->getString("mesh");
			if (xml->getError() == XML_OK){
				setId(tmpStr,true);
			}else{
				tmpStr = xml->getString("type");
				if (xml->getError() == XML_OK){
					setId(tmpStr,true);
				}else{
					tmpStr = xml->getString("model");
					if (xml->getError() == XML_OK){
						setId(tmpStr,true);
					}else{
						if (typeid(this) == typeid(Shmoulette::Camera*)){
							setId("Unnamed Camera", true);
						}else if (typeid(this) == typeid(Shmoulette::RailHook*)){
							setId("Unnamed RailHook", true);
						}else if (typeid(this) == typeid(Shmoulette::PlayerShip*)){
							setId("Unnamed PlayerShip", true);
						}else if (typeid(this) == typeid(Shmoulette::Drone*)){
							setId("Unnamed Drone", true);
						}else if (typeid(this) == typeid(Shmoulette::DefaultDrone*)){
							setId("Unnamed StationaryDrone", true);
						/*}else if (typeid(this) == typeid(Shmoulette::StaticObj*)){
							setId("Unnamed Static", true);

							throw("DO SOMETHING");*/

						//}else if (typeid(this) == typeid(Shmoulette::DynamicObj*)){
							//setId("Unnamed Dynamic", true);
						}else{
							setId("Unnamed Type Unknown", true);
						}
					}
				}
			}
		}
	}
}
GameElement::~GameElement(){
	if (mIsOn) hide();
	if (mIsBuild) destroy();
	Util::Log("Destroying : "+mId);
}

void GameElement::moveTimes(double seconds){
	if (mTimeBuild != FLT_MAX) mTimeBuild += seconds;
	if (mTimeDestroy != FLT_MAX) mTimeDestroy += seconds;
	if (mTimeOn != FLT_MAX) mTimeOn += seconds;
	if (mTimeOff != FLT_MAX) mTimeOff += seconds;
}

void GameElement::capTimes(double seconds){
	if (mTimeBuild == FLT_MAX) mTimeBuild = seconds;
	if (mTimeDestroy == FLT_MAX) mTimeDestroy = seconds;
	if (mTimeOn == FLT_MAX) mTimeOn = seconds;
	if (mTimeOff == FLT_MAX) mTimeOff = seconds;
}

bool GameElement::updateIfOn(double timeDelta, double parentTime){
	mTimeOfParent+=timeDelta;//
	mTimeOfParent = parentTime;
//	if (parentTime != mTimeOfParent){
//		throw("Parent Time Mismatch: "+ts(parentTime)+","+ts(mTimeOfParent));
//	}
	if (isAutoManaged()){
		if (shouldBeBuild() && !mIsBuild){
			//Profiler::ProfilerEntry* pro = Profiler::ProfilerFactory::getSingleton()->getProfiler(ts(Level::getSingleton()->getTimeTrack())+"_"+mId,"BuildObjects");
			Profiler::ProfilerEntry* pro = Profiler::ProfilerFactory::getSingleton()->getProfiler(ts(parentTime)+"_"+mId,"BuildObjects");
			pro->start();
			build();
			pro->end();
			if (shouldBeOn(parentTime)){
				show();
			}else{
				hide();
			}
		}
		if (shouldBeOn() && !mIsOn){
			show();
		}
		if (!shouldBeOn() && mIsOn){
			hide();
		}
		if (!shouldBeBuild() && mIsBuild){
			Profiler::ProfilerEntry* pro = Profiler::ProfilerFactory::getSingleton()->getProfiler(mId,"DestroyObjects");
			pro->start();
			destroy();
			pro->end();
		}
	}

	if (mIsOn) return update(timeDelta, parentTime);
	return false;
}

bool GameElement::postFrameUpdate(){
	return false;
}

bool GameElement::update(double timeDelta, double parentTime){
	if (mCountdownToDeath != FLT_MAX){
		mCountdownToDeath -= timeDelta;//mLevelSegment->getTimeDelta();
		if (mCountdownToDeath <= 0){
			setTimeOff(0);
			setTimeOn(0);
			setTimeBuild(0);
			setTimeDestroy(0);
		}
	}

	return false;
};
double GameElement::timeAlive(){
	double rv = mTimeOfParent-mTimeOn;//mLevelSegment->getTimeTrack() - mTimeOn;
	return (rv < 0)?0:rv;
}

void GameElement::show(){
	mIsOn = true;
};
void GameElement::hide(){
	mIsOn = false;
};
void GameElement::build(){
	mIsBuild = true;
};
void GameElement::destroy(){
	mIsBuild = false;
}
void GameElement::setTimeOn(double time){
	mTimeOn = time;
}
void GameElement::setTimeOff(double time){
	mTimeOff = time;
}
void GameElement::setTimeBuild(double time){
	mTimeBuild = time;
}
void GameElement::setTimeDestroy(double time){
	mTimeDestroy = time;
}
void GameElement::setId(string id, bool append_number){

	mId = id;
#ifdef PROFILER_ON
	mProfiler = Shmoulette::Profiler::ProfilerFactory::getSingleton()->getProfiler(mId,"GameElement");
#endif
	if (append_number){
		mId += ts(mIdCounter++);
	}
}
string GameElement::getId(){
	return mId;
}
void GameElement::spawn(int time, bool bbuild){
	mCountdownToDeath = time;
	mTimeBuild = 0;
	mTimeOn = 0;
	mTimeOff = 9999;
	mTimeDestroy = 9999;
	if (bbuild) build();
}



}