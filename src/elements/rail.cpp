#include "stdafx.h"
#include "main.h"
#include "elements\rail.h"
namespace Shmoulette{

	void RailHook::build(){
		destroyCollisionStructure();
		Parent::build();
		/*mRX.Draw(dbRgb(255,0,0));
		mRY.Draw(dbRgb(0,255,0));
		mRZ.Draw(dbRgb(0,0,255));*/
	}

	void RailHook::init(XML* xml){
		Parent::init(xml);
		//setCollisionGroup(COLLGROUP_ALL,false);
		mType = OBJ_TYPE_RAIL;
	}


	bool RailHook::update(double timeDelta, double parentTime){
		Parent::update(timeDelta, parentTime);
		//mRX.DrawSpriteMove(GameElement::getFrameNb());
		//mRX.DrawClear();
		return false;
	}


}