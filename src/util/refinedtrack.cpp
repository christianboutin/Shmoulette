
#include "main.h"
#include "level.h"
#include "util\RefinedTrack.h"
#include "util\file.h"
#include "util\profiler.h"

using namespace Ogre;

void RefinedNumericTrack::createKeyFrame(double time, Ogre::Real value, Ogre::Real cp){
	mBezier.setValue(time, value, cp);
}

void RefinedNumericTrack::setKeyFrameValue(double time, Ogre::Real value, Ogre::Real cp){
	mBezier.setValue(time, value, cp);
}

int RefinedNumericTrack::getNumKeyFrames(){
	return mBezier.getNbKeyFrames();
}

Ogre::Real RefinedNumericTrack::getValue(double time){
	return mBezier.getValue(time);
}

RefinedNumericTrack::RefinedNumericTrack(std::string id, double length):Parent(id, length){
	
	commonConstructor(id, length);

}

void RefinedNumericTrack::commonConstructor(std::string id, double length){
	Parent::commonConstructor(id, length);
	mNKF = NULL;
}

// Quaternion

void QuaternionBezier::setValue(double time, Ogre::Quaternion q, float prevControl, float nextControl){
	list<QUAT_BEZ_POINT>::iterator it;
	for (it = mBezierPoint.begin();it != mBezierPoint.end();it++){
		if (Util::aboutEqual((*it).time,time)){
			(*it).value = q;
			break;
		}else if ((*it).time > time){
			QUAT_BEZ_POINT p;
			p.time = time;
			p.value = q;
			mBezierPoint.insert(it, p);
			break;//(*it).value = q;
		}
	}
	QUAT_BEZ_POINT p;
	p.time = time;
	p.value = q;
	mBezierPoint.push_back(p);

}
Ogre::Quaternion QuaternionBezier::getValue(double time){
	//Util::Log("Asking Quaternion : "+ts(time),6);

	Ogre::Quaternion rv;
	bool treated = false;
	if (mBezierPoint.size() == 0){
		rv = Quaternion::IDENTITY;	
		treated = true;
	}else{
		list<QUAT_BEZ_POINT>::iterator it;
			QUAT_BEZ_POINT* prevPoint = NULL;

			for (it = mBezierPoint.begin();it!= mBezierPoint.end();it++){
				if ((*it).time == time){
					// Value is straight on a control point.  Return it's value
					rv = (*it).value;
					treated = true;
					break;
				}else if ((*it).time > time){
					if (prevPoint == NULL){
						return (*it).value;
					}
					double delta_time = (*it).time - prevPoint->time;
					float a=((time)-prevPoint->time)/delta_time;
					rv = getValue(prevPoint, &(*it), a);
					treated = true;
					break;
				}
				prevPoint = &(*it);
			}
			if (!treated){
				if (prevPoint == NULL){
					rv = (*mBezierPoint.begin()).value;
				}else{
					rv = prevPoint->value;
				}
			}
	}
	//Util::Log("Returning Quaternion : "+ts(rv),6);
	return rv;
}
Ogre::Quaternion QuaternionBezier::getValue(QUAT_BEZ_POINT* p1, QUAT_BEZ_POINT* p2, float a){
		//return p1->value;
		return Quaternion::Slerp(a, p1->value, p2->value, true);
}
Ogre::Quaternion QuaternionBezier::getValue(int i1, int i2, float a){
		return Ogre::Quaternion::IDENTITY;
}

void QuaternionBezier::moveKeyFrameTime(int i, double time){
	list<QUAT_BEZ_POINT>::iterator it = mBezierPoint.begin();
	for (int j=0;j<i;j++){
		it++;
	}
	(*it).time = time;
}
void QuaternionBezier::removeKeyFrame(int i){
		list<QUAT_BEZ_POINT>::iterator it = mBezierPoint.begin();

		for (int j=0;j<i;j++){
			it++;
		}
		mBezierPoint.erase(it);
}
Ogre::Quaternion QuaternionBezier::getKeyFrameValue(int i){
	list<QUAT_BEZ_POINT>::iterator it = mBezierPoint.begin();

	for (int j=0;j<i;j++){
		it++;
	}
	return (*it).value;
}


QuaternionBezier::QuaternionBezier(){}

// Base

BaseRefinedTrack::BaseRefinedTrack(){
}
double BaseRefinedTrack::truncateTime(double time){
	return time;
}

void BaseRefinedTrack::commonConstructor(std::string id, double length){
	mId = id;
	mLength = length;

}
BaseRefinedTrack::BaseRefinedTrack(std::string id, double length){
}

// Colour

RefinedNumericTrack::RefinedNumericTrack(XML* xml, string tag){
	XMLIterator it(xml);
	it.setElemName("##anything##>point");
	it.gotoZero();
	while(it.gotoNext() != false){
		XML tempXml = it.getElem();
		double t = tempXml.getDouble("time");
		Ogre::Real c = tempXml.getFloat(tag);
		if (tempXml.getError() == XML_OK){
			Ogre::Real cp = tempXml.extractElem("point>control").getFloat(tag);
		//if (cp == FLT_MAX) cp = c;
			setKeyFrameValue(t,c,cp);
		}
	}
}