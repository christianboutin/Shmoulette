// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___BLINKER_H___
#define ___DEF___BLINKER_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{
		typedef struct TAG_BLINKER_DELAY{
			float mTimeOff;
			float mTimeOn;
		}BLINKER_DELAY;

		class BlinkerParams{
			vector<BLINKER_DELAY>mBlinkerDelay;
		};

		class _ShmouletteExport Blinker:public GoAnim{
			typedef GoAnim Parent;
			vector<BLINKER_DELAY>mBlinkerDelay;
			int mCurrentSegment;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);
				
				void update(float time);

		};
	}

}
#endif