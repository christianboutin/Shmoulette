// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___CELL_H___
#define ___DEF___CELL_H___

#include "..\elements\gameelement.h"
#include "..\util\refinedtrack.h"
#include "..\util\bezier.h"

namespace Shmoulette{
	class _ShmouletteExport CellElement:public GameElement{
	protected:
		typedef GameElement Parent;
		void spawnOverlayAndContainer(string type);
		Ogre::OverlayContainer* mDeltaContainer;
		Ogre::Overlay* mOverlay;
		Ogre::OverlayContainer* mOverlayContainer;
		bool mOverlayOverwritten;
	public:
		virtual void init(XML* xml,string type);
		virtual void init();
		
		virtual void init(string type, float zOrder);
		virtual ~CellElement();
		void spawnContainer();
		void setDelta(Ogre::Vector2 pos);
		virtual void setAlpha(float a){};
		virtual void setScale(float sx, float sy);
		virtual void setAngle(float angle);
		virtual void setSize(float percent);
		virtual void setZOrder(float zOrder){mOverlay->setZOrder(zOrder);}
		void setParentOverlay(Ogre::Overlay* overlay);
	};

	class Cell:public GameElement{
		typedef GameElement Parent;
		list<CellElement*>mElement;
		/*list<Ogre::OverlayContainer*>mOverlayContainer;
		Ogre::OverlayManager & mOverlayManager;*/
		Ogre::Overlay* mOverlay;
		//float zOrder;

		Ogre::Vector2 mPos;
		
		//RefinedTrack* mTrack;
		RefinedNumericTrack* mX;
		RefinedNumericTrack* mY;
		RefinedNumericTrack* mAlpha;
		RefinedNumericTrack* mScaleX;
		RefinedNumericTrack* mScaleY;
		//RefinedNumericTrack* mAngle;
		void createFromXml(XML* xml);

		public:
			virtual void init(XML *xml);
			virtual void init();
			void addCellElement(CellElement* element){mElement.push_back(element);}
			CellElement* getCellElement(int index);
			~Cell();
			void destroy();
			bool update(double timeDelta, double parentTime);
			virtual void moveTimes(double time);

	};
}

#endif
