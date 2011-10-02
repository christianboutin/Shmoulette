#include "stdafx.h"

#include "2d/timer.h"
namespace Shmoulette{

	GameTimer::GameTimer(double time, double void_time, string label, TIMER_RESULT result){
			double mTime = time;
			double mVoidTime = void_time;
			string mLabel = label;
			TIMER_RESULT mTimerResult = result;
		{
			TiXmlElement cell( "cell" );
			cell.SetAttribute( "x", "0" );
			cell.SetAttribute( "y", "0" );

			TiXmlElement tz1( "textzone" );
			tz1.SetAttribute( "z", "32" );
			tz1.SetAttribute( "type", "static" );
			tz1.SetAttribute( "filename", "timer" );
			//tz1.SetAttribute( "label", label );
			tz1.SetAttribute( "style", "tutorial" );
			tz1.SetAttribute( "x", ".1" );
			tz1.SetAttribute( "y", ".90" );
			tz1.SetAttribute( "w", ".02" );
			tz1.SetAttribute( "h", ".05" );
			tz1.SetAttribute( "align", "center" );
			tz1.SetAttribute( "valign", "center" );

			TiXmlElement point( "point" );
			point.SetAttribute( "time", "0" );
			point.SetAttribute( "posx", "0" );
			point.SetAttribute( "posy", "0" );
			point.SetAttribute( "sclx", "1" );
			point.SetAttribute( "scly", "1" );
			point.SetAttribute( "angle", "0" );
			point.SetAttribute( "alpha", "1" );
			
			cell.InsertEndChild(tz1);
			cell.InsertEndChild(point);

			XML cellXml(&cell);
			string s= cellXml.getContent();

			mTimer = new Cell();
			mTimer->init(&cellXml);
		}
	}

	GameTimer::~GameTimer(){
		if (mTimer != NULL){
			delete mTimer;
		}
	}
	void GameTimer::build(){
		Parent::build();
		if (mTimer != NULL){
			mTimer->build();
		}
	}
	void GameTimer::destroy(){
		Parent::destroy();
		if (mTimer != NULL){
			mTimer->destroy();
		}
	}

	bool GameTimer::update(double timeDelta, double parentTime){
		mTime -= timeDelta;//Level::getSingleton()->getTimeDelta();
		
		if (parentTime > mVoidTime){ //Level::getSingleton()->getCurrentSegment()->getTimeTrack()
			if (mTimer != NULL){
				delete mTimer;
				mTimer = NULL;
			}
		}else{
			if (mTime < 0){
				throw("Timer expired.  Game over");
			}
		}

		

		if (mTimer != NULL){
			CellElement* ce = mTimer->getCellElement(0);
			StaticText* st = dynamic_cast<StaticText*>(ce);
			//st->setString("GLUGLU");
			mTimer->update(timeDelta, parentTime);
		}
		
		
		return Parent::update(timeDelta, parentTime);
	}

}