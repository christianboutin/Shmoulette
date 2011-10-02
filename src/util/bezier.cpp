#include "util\bezier.h"


namespace Util{
	using namespace Ogre;



	Bezier::Bezier(){
		mDirty = false;
	}

#ifndef QUADRATIC_BEZIER
	void Bezier::setValue(double time, float value, float prevControl, float nextControl){
		list<BEZIER_POINT>::iterator it;

		BEZIER_POINT* prevPoint = NULL;
		
		for (it = mBezierPoint.begin();it!= mBezierPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				// Value is straight on a control point.  Return it's value
				(*it).value = value;
				(*it).relativeCP.prev = prevControl;
				(*it).relativeCP.next = nextControl;
				return;
			}else if ((*it).time > time){
				BEZIER_POINT bp;
				bp.time = time;
				bp.value = value;
				bp.relativeCP.next = nextControl;
				bp.relativeCP.prev = prevControl;
				mBezierPoint.insert(it, bp);
				mDirty = true;
				return;
			}
			prevPoint = &(*it);
		}
		BEZIER_POINT bp;
		bp.time = time;
		bp.value = value;
		bp.relativeCP.next = nextControl;
		bp.relativeCP.prev = prevControl;
		mBezierPoint.push_back(bp);
		mDirty = true;
	}
#else
	void Bezier::setValue(double time, float value, float cp){
		list<BezierPoint>::iterator it;
		if (cp == FLT_MAX) 
			mDirty = true;
		//BEZIER_POINT* prevPoint = NULL;
		
		for (it = mBezierPoint.begin();it!= mBezierPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				(*it).value = value;
				if (cp != FLT_MAX) (*it).cp = cp;
				return;
			}else if ((*it).time > time){
				BezierPoint bp;
				bp.time = time;
				bp.value = value;
				bp.cp = cp;
				mBezierPoint.insert(it, bp);
				return;
			}
			//prevPoint = &(*it);
		}
		BezierPoint bp;
		bp.time = time;
		bp.value = value;
		bp.cp = cp;
		mBezierPoint.push_back(bp);
	}

	void Bezier::autocalcCPs(){
		//Util::Log("Entering autocalcCPs", 7);
		list<BezierPoint>::iterator it;
		list<BezierPoint>::iterator nextIt;
		it = mBezierPoint.begin();
		nextIt = mBezierPoint.begin();
		nextIt++;

		int removeMe = mBezierPoint.size();


		for (;nextIt != mBezierPoint.end();it++,nextIt++){
			if ((*it).cp == FLT_MAX){
				float thisValue = (*it).value;
				float nextValue = (*nextIt).value;
				(*it).cp = ((nextValue - thisValue) /2)+thisValue;
			}
		}
		(*it).cp = (*it).value;
		mDirty = false;
		//Util::Log("Leaving autocalcCPs", 7);
	}

	void Bezier::setCP(double time, float value){
		list<BezierPoint>::iterator it;

		BezierPoint* prevPoint = NULL;
		
		for (it = mBezierPoint.begin();it!= mBezierPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				(*it).cp = value;
				return;
			}else if ((*it).time > time){
				BezierPoint bp;
				bp.time = time;
				bp.cp = value;
				mBezierPoint.insert(it, bp);
				return;
			}
			prevPoint = &(*it);
		}
		BezierPoint bp;
		bp.time = time;
		bp.cp = value;
		mBezierPoint.push_back(bp);
	}


#endif
	float Bezier::getValue(BezierPoint* p1, BezierPoint* p2, float b){

		float rv;
		float a=1.0-b;
#ifdef QUADRATIC_BEZIER
		float Ax = p1->value;
		float Bx = p1->cp;
		float Cx = p2->value;
#else
		float Ax = p1->value;
		float Bx = p1->absoluteCP.next;
		float Cx = p2->absoluteCP.prev;
		float Dx = p2->value;
#endif

		float* val;
//		X(a) = Ax·a² + Bx·2·a·b + Cx·b²
#ifdef QUADRATIC_BEZIER
		rv = Ax*a*a + Bx*2*a*b + Cx*b*b;
#else
		rv = Ax*a*a*a + Bx*3*a*a*b + Cx*3*a*b*b + Dx*b*b*b;
#endif
		return rv;
	}
	float Bezier::getValue(double time){
		//Util::Log("Entering getValue", 7);
#ifndef QUADRATIC_BEZIER
		if (mDirty)	recalcAbsoluteCPs();
#else
		if (mDirty)	autocalcCPs();
;
#endif
		if (mBezierPoint.size() == 0){
			//Util::Log("Bezier DOES NOT Exist", 7);
			return 1.0f;
		}
		//Util::Log("Bezier Exists : "+ts(mBezierPoint.size()), 7);
		float rv=FLT_MAX;
		// Step 1, find between which and which points the time is in

		list<BezierPoint>::iterator it;
		BezierPoint* prevPoint = NULL;

		if (time < 0){
			return (*mBezierPoint.begin()).value;
		}

		for (it = mBezierPoint.begin();it!= mBezierPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				// Value is straight on a control point.  Return it's value
				rv = (*it).value;
				break;
			}else if ((*it).time > time){
				if (prevPoint == NULL){
					return (*it).value;
				}
				double delta_time = (*it).time - prevPoint->time;
				float a=((time)-prevPoint->time)/delta_time;
				rv = getValue(prevPoint, &(*it), a);
				break;
			}
			prevPoint = &(*it);
			//Util::Log("prevPoint : "+ts((*it).value), 7);

		}
		if (rv == FLT_MAX){
			if (prevPoint == NULL){
				//Util::Log("forcing 1 : "+ts((*mBezierPoint.begin()).value), 7);
				//return 1.0f;
				rv = (*mBezierPoint.begin()).value;
			}else{
				//Util::Log("forcing 1 : "+ts(prevPoint->value), 7);
				//return 1.0f;
				rv = prevPoint->value;
			}
		}
		//Util::Log("Returning : "+ts(rv), 7);
		return rv;

	}
#ifndef QUADRATIC_BEZIER
	void Bezier::recalcAbsoluteCPs(){
		if (mBezierPoint.size() > 2){
			list<BEZIER_POINT>::iterator it;
			BEZIER_POINT* prevPoint = NULL;
			list<BEZIER_POINT>::iterator nextIt = mBezierPoint.begin();
			list<BEZIER_POINT>::iterator nextNextIt = mBezierPoint.begin();
			nextIt++;
			nextNextIt++;
			nextNextIt++;
			(*mBezierPoint.begin()).absoluteCP.next = ((*nextIt).value + (*nextNextIt).value)/2;
			for (it = mBezierPoint.begin();nextNextIt != mBezierPoint.end();it++,nextIt++,nextNextIt++){
					(*nextIt).absoluteCP.next = ((*it).value + (*nextIt).value + (*nextNextIt).value)/3;
					(*nextIt).absoluteCP.prev = ((*it).value + (*nextIt).value + (*nextNextIt).value)/3;
					

					/*float delta = (*nextIt).value-(*it).value;
					float forwardDelta = (delta*(*it).relativeCP.next);
					(*it).absoluteCP.next = (*it).value + forwardDelta;
					float backwardDelta = (delta*(*nextIt).relativeCP.prev);
					(*nextIt).absoluteCP.prev = (*nextIt).value - backwardDelta;*/
				//}
			}
			(*nextIt).absoluteCP.prev = ((*it).value + (*nextIt).value)/2;

		}
		mDirty = false;
	}
#endif
	void Bezier::removeKeyFrame(int i){
		list<BezierPoint>::iterator it = mBezierPoint.begin();

		for (int j=0;j<i;j++){
			it++;
		}
		mBezierPoint.erase(it);
		//return (*it).time;
	}
	int	Bezier::getKeyFrameIndex(double time){
		list<BezierPoint>::iterator it;

		int i=0;
		for (it = mBezierPoint.begin();it!= mBezierPoint.end();it++,i++){
			if (Util::aboutEqual((*it).time,time)){
				return i;
			}
		}
		return -1;
	}
	bool Bezier::isKeyFrame(double time){
		return getKeyFrameIndex(time) != -1;
	}

	//////////////////////////
/*	void LinearTrack::setValue(double time, float value){
		list<LINEAR_POINT>::iterator it;

		for (it = mPoint.begin();it!= mPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				(*it).value = value;
				return;
			}else if ((*it).time > time){
				LINEAR_POINT bp;
				bp.time = time;
				bp.value = value;
				mPoint.insert(it, bp);
				return;
			}
			//prevPoint = &(*it);
		}
		LINEAR_POINT bp;
		bp.time = time;
		bp.value = value;
		mPoint.push_back(bp);
	}

	float LinearTrack::getValue(LINEAR_POINT* p1, LINEAR_POINT* p2, float a){
		return p1->value+((p2->value - p1->value) * a);
	}

	float LinearTrack::getValue(double time){
		if (mPoint.size() == 0){
			Util::Log("Bezier DOES NOT Exist", 7);
			return 1.0f;
		}
		float rv=FLT_MAX;
		// Step 1, find between which and which points the time is in

		list<LINEAR_POINT>::iterator it;
		LINEAR_POINT* prevPoint = NULL;

		if (time < 0){
			return (*mPoint.begin()).value;
		}

		for (it = mPoint.begin();it!= mPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				// Value is straight on a control point.  Return it's value
				rv = (*it).value;
				break;
			}else if ((*it).time > time){
				double delta_time = (*it).time - prevPoint->time;
				float a=((time)-prevPoint->time)/delta_time;
				rv = getValue(prevPoint, &(*it), a);
				break;
			}
			prevPoint = &(*it);
		}
		if (rv == FLT_MAX){
			if (prevPoint == NULL){
				rv = (*mPoint.begin()).value;
			}else{
				rv = prevPoint->value;
			}
		}
		Util::Log("Returning : "+ts(rv), 7);
		return rv;
	}
	double LinearTrack::getPrevKeyFrameTime(double time){
		return 0.0f;
	}
	double LinearTrack::getNextKeyFrameTime(double time){
		return 0.0f;
	}
	double LinearTrack::getKeyFrameTime(int i){
		list<LINEAR_POINT>::iterator it = mPoint.begin();

		for (int j=0;j<i;j++){
			it++;
		}
		return (*it).time;
	}
	void LinearTrack::setKeyFrameTime(int i, double time){}
	float LinearTrack::getKeyFrameValue(int i){return 0.0f;}

	void LinearTrack::moveKeyFrameTime(int i, double timeDelta){}

	LinearTrack::LinearTrack(){}
*/
	void OneDimensionTrack::appendValue(double time, float value){
		TrackPoint bp;
		bp.time = time;
		bp.value = value;
		mPoint.push_back(bp);
	}
	void OneDimensionTrack::setValue(double time, float value){
		list<TrackPoint>::iterator it;

		for (it = mPoint.begin();it!= mPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				(*it).value = value;
				return;
			}else if ((*it).time > time){
				TrackPoint bp;
				bp.time = time;
				bp.value = value;
				mPoint.insert(it, bp);
				return;
			}
			//prevPoint = &(*it);
		}
		TrackPoint bp;
		bp.time = time;
		bp.value = value;
		mPoint.push_back(bp);
	}
	float OneDimensionTrack::getValue(TrackPoint* p1, TrackPoint* p2, float a){
		return p1->value+((p2->value - p1->value) * a);
	}
	float OneDimensionTrack::getValue(double time){
		if (mPoint.size() == 0){
			//Util::Log("Bezier DOES NOT Exist", 7);
			return 1.0f;
		}
		float rv=FLT_MAX;
		// Step 1, find between which and which points the time is in

		list<TrackPoint>::iterator it;
		TrackPoint* prevPoint = NULL;

		if (time < 0){
			return (*mPoint.begin()).value;
		}

		for (it = mPoint.begin();it!= mPoint.end();it++){
			if (Util::aboutEqual((*it).time,time)){
				// Value is straight on a control point.  Return it's value
				rv = (*it).value;
				break;
			}else if ((*it).time > time){
				if (prevPoint == NULL){
					rv = (*it).value;
				}else{
					double delta_time = (*it).time - prevPoint->time;
					float a=((time)-prevPoint->time)/delta_time;
					rv = getValue(prevPoint, &(*it), a);
				}
				break;
			}
			prevPoint = &(*it);
		}
		if (rv == FLT_MAX){
			if (prevPoint == NULL){
				rv = (*mPoint.begin()).value;
			}else{
				rv = prevPoint->value;
			}
		}
		//Util::Log("Returning : "+ts(rv), 7);
		return rv;
	}


}
