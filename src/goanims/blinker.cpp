#include "stdafx.h"
#include "goanims\blinker.h"

namespace Shmoulette{
	namespace GoAnim{
		void Blinker::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
		}

		void Blinker::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);
			
			int listSize = PyList_Size(localObj);
			for (int i =0;i<listSize;i++){
				PyObject* itemPo = PyList_GetItem(localObj, i);
				BLINKER_DELAY bo;
				bo.mTimeOn = PyFloat_AsDouble(PyList_GetItem(itemPo, 0));
				bo.mTimeOff = PyFloat_AsDouble(PyList_GetItem(itemPo, 1));
				mBlinkerDelay.push_back(bo);
			}
			mCurrentSegment = 0;
		}
		void Blinker::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			XMLIterator it(xml);
			it.setElemName("goanim>segment");
			it.gotoZero();
			while(it.gotoNext() != false){
				BLINKER_DELAY bo;

				bo.mTimeOff = it.getElem().getFloat("timeOff");
				bo.mTimeOn = it.getElem().getFloat("timeOn");
				
				mBlinkerDelay.push_back(bo);
			}
			mCurrentSegment=0;
		}

		void Blinker::update(float time){
			Parent::update(time);

			while (mTime > mBlinkerDelay[mCurrentSegment].mTimeOn+mBlinkerDelay[mCurrentSegment].mTimeOff){
				mTime -= (mBlinkerDelay[mCurrentSegment].mTimeOn+mBlinkerDelay[mCurrentSegment].mTimeOff);
				mCurrentSegment++;
				if (mCurrentSegment >= mBlinkerDelay.size()){
					mCurrentSegment = 0;
				}
			}
			if (mTime <= mBlinkerDelay[mCurrentSegment].mTimeOff){
				mGo->setVisible(false);
			}else if (mTime-mBlinkerDelay[mCurrentSegment].mTimeOff <= mBlinkerDelay[mCurrentSegment].mTimeOn){
				mGo->setVisible(true);
			}
		}
	}
}