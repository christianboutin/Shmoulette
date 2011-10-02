// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___GAMEELEMENT_H___
#define ___DEF___GAMEELEMENT_H___

//#pragma message("GAMEELEMENT_H")
namespace Shmoulette{
class GameElement;
}

#include "levelsegment.h"
#include "util/profiler.h"

namespace Shmoulette{

#define MACRO_TRANSFER_PARAM(_p_) _p_ = params._p_

class GameElementParams{
	public:
		string mId;
		double mTimeOn;
		double mTimeOff;
		double mTimeBuild;
		double mTimeDestroy;
		bool mAutoManage;
		void setTimes(double b, double on, double off, double d);
		void setTimes(double b, double d);
		GameElementParams();

		virtual void setParam(string param, string value);
};


class _ShmouletteExport GameElement{ // Anything that needs to be "updated" is a game element
	private:
		double mTimeOn;
		double mTimeOff;
		double mTimeBuild;
		double mTimeDestroy;
		bool mMarkedForTermination;
		void setDefaultVariables();
#ifdef PROFILER_ON
		Profiler::ProfilerEntry* mProfiler;
#endif
		bool mAutoManage; // If it's on, the element will be build, showed, hidden and destroyed based on the mTime params.  If off, it will be done manually

		//double mTimeAlive; // The time since the dude has been on
		double mTimeOfParent; // The total time track given by the parent.  Everytime and updateIfOn is called, this value is adjusted;

	protected:
		XML* mXml;
		string mId;
		static int mIdCounter;

		double mCountdownToDeath;

		bool mIsBuild;
		bool mIsOn;

		LevelSegment* mLevelSegment;
		static LevelSegment* sLoadTimeSegmentPlaceholder;

		

	public:
#ifdef PROFILER_ON
		Profiler::ProfilerEntry* getProfiler(){return mProfiler;}
#endif
		static int getNextIdCounter(){return mIdCounter++;}
		void markForTermination(){mMarkedForTermination = true;}
		void unmarkForTermination(){mMarkedForTermination = false;}
		bool isMarkedForTermination(){return mMarkedForTermination;}
		bool isBuild();//float time = Level::getSingleton()->getTimeAbsolute());
		bool isOn();//float time=Level::getSingleton()->getTimeAbsolute());
		bool shouldBeBuild(double time = FLT_MAX);
		bool shouldBeOn(double time = FLT_MAX);

		virtual bool isAutoManaged(){return mAutoManage;}
		virtual void setAutoManaged(bool v){mAutoManage = v;}
		
		static void setLoadTimeSegmentPlaceholder(LevelSegment* ls){ // God I hate that
			sLoadTimeSegmentPlaceholder = ls;
		}

		virtual void setLevelSegment(LevelSegment* ls){
			mLevelSegment = ls;
		}

		virtual void init(GameElementParams& params);
		virtual void init(XML *xml);
		virtual void init(PyObject* pyObject);
		virtual void init();

		virtual ~GameElement();
		virtual void init(string str, bool append_number = false);
		//GameElement(vector<string>& params);

		virtual bool updateIfOn(double timeDelta, double parentTime);
		virtual bool update(double timeDelta, double parentTime);
		virtual bool postFrameUpdate();
		virtual double timeAlive();
		//virtual XML* getXml(){return mXml;}

		//void timeMove(double seconds);

		virtual void show();
		virtual void hide();
		virtual void build();
		virtual void destroy();
		void setTimeOn(double time);
		void setTimeOff(double time);
		void setTimeBuild(double time);
		void setTimeDestroy(double time);

		double getTimeOn(){return mTimeOn;}
		double getTimeOff(){return mTimeOff;}
		double getTimeBuild(){return mTimeBuild;}
		double getTimeDestroy(){return mTimeDestroy;}

		void moveTimes(double time);
		void capTimes(double time);


		void setId(string id, bool append_object_number = false);
		void setId();
		string getId();
		void spawn(int time = 999999, bool build=false);
};

}

#endif
