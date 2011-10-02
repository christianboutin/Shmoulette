#include "stdafx.h"

#include "main.h"
#include "trigger\event.h"
#include "trigger\trigger.h"
#include "trigger\popup.h"
#include "level.h"

namespace Shmoulette{

void Trigger::run(){
	if (mEnabled == true){
		if (!mHasTriggered){
			mEvent->trigger();
			mHasTriggered = true;
		}
	}
}

bool Trigger::checkAndRun(float time){
	//Util::Log("Checking Trigger..."+ts(mTime));
	if (mEnabled == true){
		if (!mHasTriggered){
			if (time >= mTime){
				mEvent->trigger();
				mHasTriggered = true;
			}
		}else{
			if (time < mTime){ // In case I decide that going back in time is possible
				mHasTriggered = false;
			}
		}
	}
	//Util::Log("...done");


	return false;
}

void Trigger::moveTimes(double time){
	mTime += time;
	mEvent->moveTimes(time);
}
void Trigger::reset(){
	mHasTriggered = false;
	mEnabled = true;
}
Trigger::Trigger(XML* xml){
	
	string tType = xml->getString("type");

	Util::Log("Creating trigger of type : "+tType, 4);
	
	if (tType == "cinema_on"){
		mEvent = new EventList::CinemaOn();
	}else if (tType == "cinema_off"){
		mEvent = new EventList::CinemaOff();
	}else if (tType == "goto"){
		mEvent = new EventList::Goto(xml);
	}else if (tType == "end_segment"){
		mEvent = new EventList::EndSegment();
	}else if (tType == "plane_depth"){
		mEvent = new EventList::PlaneDepth();
	}else if (tType == "plane_vertical"){
		mEvent = new EventList::PlaneVertical();
	}else if (tType == "plane_horizontal"){
		mEvent = new EventList::PlaneHorizontal();
	}else if (tType == "talk"){
		mEvent = new EventList::Talk(xml);
	}else if (tType == "music_start"){
		mEvent = new EventList::MusicStart(xml);
	}else if (tType == "music_fade_out"){
		mEvent = new EventList::MusicFadeOut(xml);
	}else if (tType == "time_speed"){
		mEvent = new EventList::TimeSpeed(xml);
	}else if (tType == "weapons_hold"){
		mEvent = new EventList::WeaponsHold();
	}else if (tType == "weapons_free"){
		mEvent = new EventList::WeaponsFree();
	}else if (tType == "fog_state_change"){
		mEvent = new EventList::FogStateChange(xml);
	}else if (tType == "subtitle"){
		mEvent = new EventList::Subtitle(xml);
	}else if (tType == "sound"){
		mEvent = new EventList::Sound(xml);
	}else if (tType == "speech"){
		mEvent = new EventList::Speech(xml);
	}else if (tType == "popup"){
		mEvent = new EventList::Popup(xml);
	}else if (tType == "arrow"){
		mEvent = new EventList::Arrow(xml);
	}else if (tType == "camera_shake"){
		mEvent = new EventList::CameraShake(xml);
	}else if (tType == "break_loop"){
		mEvent = new EventList::BreakLoop(xml);
	}else if (tType == "timer"){
		mEvent = new EventList::Timer(xml);
	}else if (tType == "void_timer"){
		mEvent = new EventList::VoidTimer();
	}else if (tType == "player_event"){
		mEvent = new EventList::PlayerEvent(xml);
	}else if (tType == "compositor_add"){
		mEvent = new EventList::AddCompositor(xml);
	}else if (tType == "zone"){
		mEvent = new EventList::Zone(xml);
	}else if (tType == "script"){
		mEvent = new EventList::Script(xml);
	}else if (tType == "set_sky"){
		mEvent = new EventList::SetSky(xml);
	/*}else if (tType == "scene_shadows"){
		mEvent = new EventList::SceneShadows(xml);
	}else if (tType == "ambient_change"){
		mEvent = new EventList::AmbientChange(xml);*/
	}else{
		throw("Event of type : "+tType+" doesn't exist in-code");
		//mEvent = new EventList::Talk();
	}

	
	/*if (tType == "cinema_on"){
		mType = TRIGGER_CINEMA_ON;
	}else if (tType == "cinema_off"){
		mType = TRIGGER_CINEMA_OFF;
	}else if (tType == "plane_depth"){
		mType = TRIGGER_PLANE_DEPTH;
	}else if (tType == "plane_vertical"){
		mType = TRIGGER_PLANE_VERTICAL;
	}else if (tType == "plane_horizontal"){
		mType = TRIGGER_PLANE_HORIZONTAL;
	}else{
		mType = TRIGGER_IGNORE;
	}*/
	mTime = xml->getFloat("time");
	if (xml->getError() != XML_OK){
		mTime = 0;
	}
	mHasTriggered = false;
	mEnabled = true;
}

}