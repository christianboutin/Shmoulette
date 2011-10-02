#include "stdafx.h"

#include "2d\hudhealth.h"
#include "2d\hudshield.h"
#include "2d\hudscore.h"
#include "2d\hudammo.h"
#include "2d\hudenergy.h"
#include "2d\hud.h"

namespace Shmoulette{
	Hud::Hud(){
		/*addElement(new HudScore());
		addElement(new HudHealth());
		addElement(new HudShield());
		addElement(new HudAmmo());
		addElement(new HudEnergy());*/
	}


	Hud::~Hud(){
		std::list<HudElement*>::iterator it;
		for (it = mElement.begin();it!=mElement.end();it++){
			delete(*it);
		}
		mElement.clear();
	}
	void Hud::addElement(HudElement* he){
		mElement.push_back(he);
	}
	void Hud::removeElement(HudElement* he){
		std::list<HudElement*>::iterator it;
		for (it = mElement.begin();it!=mElement.end();it++){
			if ((*it)==he){
				mElement.erase(it);
			}
		}
		delete he;

	}

	bool Hud::update(double timeDelta, double parentTime){
		std::list<HudElement*>::iterator it;
		for (it = mElement.begin();it!=mElement.end();it++){
			(*it)->update(timeDelta, parentTime);
		}

		return false;
	}
	void Hud::show(){
		std::list<HudElement*>::iterator it;
		for (it = mElement.begin();it!=mElement.end();it++){
			(*it)->show();
		}
	};
	void Hud::hide(){
		std::list<HudElement*>::iterator it;
		for (it = mElement.begin();it!=mElement.end();it++){
			(*it)->hide();
		}
	};
}