// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___TRIGGER_H___
#define ___DEF___TRIGGER_H___

#include "trigger/event.h"

namespace Shmoulette{

/*typedef enum TAG_TRIGGER_TYPE{
	TRIGGER_CINEMA_ON,
	TRIGGER_CINEMA_OFF,
	TRIGGER_GOTO_FRAME,
	TRIGGER_LEVEL_COMPLETE,
	TRIGGER_PLANE_VERTICAL,
	TRIGGER_PLANE_HORIZONTAL,
	TRIGGER_PLANE_DEPTH,
	TRIGGER_PLANE_TURRET, // Maybe?
	TRIGGER_SET_TIME_SPEED,
	TRIGGER_IGNORE
}TRIGGER_TYPE;*/

class _ShmouletteExport Trigger{
	float mTime;
	//TRIGGER_TYPE mType;
	class EventList::Event* mEvent;
	bool mHasTriggered;
	bool mEnabled;
	protected:


	public:
		Trigger(XML* xml);
		virtual bool checkAndRun(float time);
		virtual void run();
		virtual void disable(){ mEnabled = false;}
		virtual void moveTimes(double time);
		virtual void reset();
		virtual EventList::Event* getEvent(){return mEvent;}
		virtual float getTime(){return mTime;}
};

}

#endif
