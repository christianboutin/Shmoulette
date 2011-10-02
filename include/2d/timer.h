#ifndef ___DEF___TIMER_H___
#define ___DEF___TIMER_H___

#include <OgreTextAreaOverlayElement.h>
#include <OgreFontManager.h> 
#include "cell.h"
#include "text.h"

namespace Shmoulette{
	class GameTimer:public CellElement{
		Cell* mTimer;
		//StaticText* mLabel;
		double mTime;
		double mVoidTime;
		string mLabel;
		TIMER_RESULT mTimerResult;

	public:
		GameTimer(double time, double void_time, string label, TIMER_RESULT result = TIMER_RESULT_FATAL);
		virtual void build();
		virtual void destroy();
		bool update(double timeDelta, double parentTime);
		~GameTimer();

	};
}

#endif