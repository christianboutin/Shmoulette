// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___HUD_H___
#define ___DEF___HUD_H___

#include <list>
#include "Ogre.h"

namespace Shmoulette{

	class _ShmouletteExport HudElement{
		protected:
		Ogre::Overlay* mOverlay;
		public:
			virtual void show(){
				mOverlay->show();
			};
			virtual void hide(){
				mOverlay->hide();
			};
			virtual ~HudElement(){};
			virtual bool update(double timeDelta, double parentTime)=0;
			virtual void build()=0;
			virtual void destroy()=0;
	};

	class _ShmouletteExport Hud{
		protected:
			std::list<HudElement*>mElement;
		public:
			Hud();
			virtual ~Hud();
			virtual void show();
			virtual void hide();
			virtual bool update(double timeDelta, double parentTime);
			virtual void build(){
				std::list<HudElement*>::iterator it;
				for (it = mElement.begin();it!=mElement.end();it++){
					(*it)->build();
				}
			};
			virtual void destroy(){
				std::list<HudElement*>::iterator it;
				for (it = mElement.begin();it!=mElement.end();it++){
					(*it)->destroy();
				}
			};
			virtual void addElement(HudElement* he);
			virtual void removeElement(HudElement* he);
			virtual void setSecondLifeTarget(GameObject* go){};
			virtual void clearSecondLifeTarget(){};
	};

	class HudBlank:public Hud{
		protected:
		public:
			HudBlank(){};
			virtual void show(){};
			virtual void hide(){};
			virtual bool update(double timeDelta, double parentTime){
				return false;
			};
			virtual void build(){};
			virtual void destroy(){};
			virtual void addElement(HudElement* he){};
			virtual void removeElement(HudElement* he){};
			virtual void setSecondLifeTarget(GameObject* go){};
			virtual void clearSecondLifeTarget(){};
	};

}

#endif
