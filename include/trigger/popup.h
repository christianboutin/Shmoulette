// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___POPUP_H___
#define ___DEF___POPUP_H___

#include "..\elements\gameelement.h"
#include "..\util\refinedtrack.h"
#include "..\util\bezier.h"
#include "event.h"

#include "..\2d\cell.h"
namespace Shmoulette{
	namespace EventList{
		class Popup:public Event{
			Cell* mBlocker;
			Cell* mCell;
			double mLife;
			public:
			Popup(XML* xml);
			Popup(string filename, string label, double timeOn, double life);
			virtual void moveTimes(double delta);
			virtual void trigger();
		};
		class Arrow:public Event{
			Cell* mCell;
			double mLife;
			public:
				Arrow(XML* xml);
				virtual void moveTimes(double delta);
				virtual void trigger();
		};
	}
}

#endif
