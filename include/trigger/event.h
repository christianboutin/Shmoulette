// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___EVENT_H___
#define ___DEF___EVENT_H___

#ifndef EVENT_H
#define EVENT_H

//#include "..\levelsegment.h"
//#include "world\world.h"
#include "2d\cell.h"
#include "2d\timer.h"
#include "python.h"
#include "world/world.h"


namespace Shmoulette{
	namespace EventList{
		class Event{
			public:
				Event();
				virtual void trigger()=0;
				virtual void moveTimes(double delta){};
		};

		class Goto:public Event{
			float destination;
			public:
				Goto(XML* xml);
				virtual void trigger();
		};
		class EndSegment:public Event{
			public: virtual void trigger();
		};

		class Talk:public Event{
			public:
				Talk(XML* xml);
				virtual void trigger();
		};

		class CinemaOn:public Event{
			public:	virtual void trigger();
		};
		class CinemaOff:public Event{
			public:	virtual void trigger();
		};
		class PlaneDepth:public Event{
			public:	virtual void trigger();
		};
		class PlaneVertical:public Event{
			public:	virtual void trigger();
		};
		class PlaneHorizontal:public Event{
			public:	virtual void trigger();
		};
		class WeaponsHold:public Event{
			public:	virtual void trigger();
		};
		class WeaponsFree:public Event{
			public:	virtual void trigger();
		};
		class CameraShake:public Event{
			float mStrength;
			public:	
				virtual void trigger();
				CameraShake(XML* xml);
		};
		class MusicStart:public Event{
			string mFilename;
			bool mRepeat;
			public:	
				MusicStart(XML* xml);
				virtual void trigger();
				
		};
		class MusicFadeOut:public Event{
			double mLength;
			public:	
				MusicFadeOut(XML* xml);
				virtual void trigger();
		};

		class Sound:public Event{
			Shmoulette::Sound* mSound;
			public:	
				Sound(XML* xml);
				Sound(string filename);
				~Sound();
				virtual void trigger();
		};


		class Subtitle:public Event{
			Shmoulette::Cell* mCell;
			double mLife;
			//string mString;
			public:	
				void _constructor(string filename, string label, double timeOn, double life);
				Subtitle(XML* xml);
				Subtitle(string filename, string label, double timeOn, double life);
				virtual void moveTimes(double delta);
				virtual void trigger();
				virtual void trigger(double timeDelta);
		};

		class Speech:public Event{
			bool mIsSub;
			Shmoulette::Sound* mSound;
			list<Subtitle*>mSub;
			list<double>mTimeDelta;
			public:	
				Speech(XML* xml);
				~Speech();
				virtual void trigger();
				virtual void moveTimes(double delta);

		};

		class FogStateChange:public Event{
			FOG_INFO mFogInfo;
			//Ogre::ColourValue mColour;
			//Ogre::Real mLinearStart;
			//Ogre::Real mLinearEnd;
			Ogre::Real mTimer;
			public:	
				FogStateChange(XML* xml);
				virtual void trigger();
				
		};

		class TimeSpeed:public Event{
			float mWanted;
			float mAcceleration;
			public:	
				TimeSpeed(XML* xml);
				virtual void trigger();
				
		};

		class BreakLoop:public Event{
			public:
				BreakLoop(XML* xml);
				virtual void trigger();
		};

		class Timer:public Event{
			class Shmoulette::GameTimer* mTimer;
			double mTime;
			public:
				Timer(XML* xml);
				~Timer();
				virtual void trigger();

		};
		class VoidTimer:public Event{
			public:	virtual void trigger();
		};
		class PlayerEvent:public Event{
			string mEvent;
			public:	virtual void trigger();
			PlayerEvent(XML* xml);
		};

		class AddCompositor:public Event{
			string mType;
			string mParam;
			public:
				virtual void trigger();
				AddCompositor(XML* xml);
		};

		class Script:public Event{
			PyObject* mName;
			public:
				virtual void trigger();
				Script(XML* xml);
		};

		class Zone:public Event{
			Ogre::Vector3 mSize,mAmmoSize;
			Ogre::Vector2 mCamSize;
			Ogre::Vector2 mCamTilt;
			double mTransitTime;
			RAIL_TYPE mType;
			public:	virtual void trigger();
			Zone(XML* xml);
		};
		class SceneShadows:public Event{
				Ogre::ShadowTechnique mTechnique;
			public:	
				virtual void trigger();
				SceneShadows(XML* xml);
		};
		class AmbientChange:public Event{
			Ogre::ColourValue mNewColor;
			double mTimer;
			public:	
				virtual void trigger();
				AmbientChange(XML* xml);
		};

		class SetSky:public Event{
			SKY_DEF mSkyDef;
			public:	
				virtual void trigger();
				SetSky(XML* xml);

		};


	}
}

#endif

#endif
