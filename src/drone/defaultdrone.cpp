#include "stdafx.h"

#include "main.h"
#include "weapon\weapon.h"

#include "drone\defaultdrone.h"
#include "util\file.h"
namespace Shmoulette{

Ogre::TransformKeyFrame DefaultDroneDBE::getKeyFrame(float time){
	Ogre::TransformKeyFrame rv(NULL, 0);
	
	Coordinator::Transform t = mTransformCoordinator->getValue(time);
	rv.setTranslate(t.mPos);
	rv.setRotation(t.mRot);
	rv.setScale(t.mScl);
	
	return rv;
}

DefaultDrone* DefaultDroneDBE::createDrone(){
	DefaultDrone* rv = new DefaultDrone();
	rv->init(this);
	if (mPythonCallback!=""){
		rv->setPythonCallback(mPythonCallback);\
	}
	return rv;
}
	//#include "drone.h"

void DefaultDroneDBE::init(XML& dronestatXml, XML& objectMovementXml){
		string coord = objectMovementXml.getString("transform_c");
		if (objectMovementXml.getError() != XML_OK){
			coord = "macro:zero_transform";
		}else{
			int g=0;
		}
		mTransformCoordinator = dynamic_cast<Coordinator::TransformCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get(coord));
			
		mMaxTime = mTransformCoordinator->getLength();

		{
			XMLIterator it(&objectMovementXml);
			it.setElemName("##anything##>goanim");
			it.gotoZero();
		
			while(it.gotoNext() != false){
				XML* goxml = new XML(it.getElem().getContent());
				mGoAnim.push_back(goxml);
			};
		}{
			XMLIterator it(&objectMovementXml);
			it.setElemName("##anything##>anim");
			it.gotoZero();
		
			while(it.gotoNext() != false){
				XML* goxml = new XML(it.getElem().getContent());
				mAnim.push_back(goxml);
			};
		}

		mMaxTime = 9999;

		Parent::init(dronestatXml,objectMovementXml);

		/*XML xml3;
		xml3 = xml->extractElem("level>dronestats");
		if (xml->getError()!=XML_OK){
			throw("WTF?");
		}*/

		{
		XMLIterator it(&dronestatXml);
		it.setElemName("##anything##>fire");
		it.gotoZero();
		FIRING_TIME ft;
		std::string g;

		while(it.gotoNext() != false){
			XML itXml = it.getElem();
			string time = itXml.getString("time");
			string mount = itXml.getString("mount");
			vector<string> t;
			vector<string> m;
			Tokenize(time,t,",");
			Tokenize(mount,m,",");

			vector<string>::iterator ti;
			vector<string>::iterator mi;
			string is = itXml.getString("inertia");
			if (itXml.getError() == XML_OK){
				if (is == "a"){ // as in actual;
					ft.inertia = Ogre::Vector3(FLT_MAX,FLT_MAX,FLT_MAX);
				}else{
					ft.inertia = vector3FromString(is);
				}
			}else{
				ft.inertia = Ogre::Vector3(0,0,0);
			}
			mi = m.begin();
			//for (mi = m.begin();mi!=m.end();mi++){
				for (ti = t.begin();ti!=t.end();ti++){
					//ft.mount = atoi((*m.begin()).c_str());
					if ((*ti).find("-") != string::npos){
						vector<string> i;
						Tokenize((*ti),i,"-");
						if (i.size() != 3){
							i.push_back("0");
							//throw("Invalid drone firing times range, requires 3 arguments ("+(*ti)+")");
						}
						float start, end, step;
						start = atof(i[0].c_str());
						end = atof(i[1].c_str());
						step = atof(i[2].c_str());

						ft.time = start;
						ft.firingOption = FIRING_START;
						for (mi = m.begin();mi!=m.end();mi++){
							ft.mount = atoi((*mi).c_str());
							mFiringTime.push_back(ft);
							ft.time +=step;
						}

						ft.time = end;
						ft.firingOption = FIRING_STOP;
						for (mi = m.begin();mi!=m.end();mi++){
							ft.mount = atoi((*mi).c_str());
							mFiringTime.push_back(ft);
							ft.time +=step;
						}

						
						/*for (float h=start;h<end;h+=step){
							ft.time = h;
							mFiringTime.push_back(ft);
							mi++;
							if (mi == m.end()) mi = m.begin();
							ft.mount = atoi((*mi).c_str());
						}*/
					}else{
						ft.mount = atoi((*m.begin()).c_str());
						ft.time = atof((*ti).c_str());
						ft.firingOption = FIRING_START;
						mFiringTime.push_back(ft);
						ft.time += .07;
						ft.firingOption = FIRING_STOP;
						mFiringTime.push_back(ft);
					}
				}
			//}
			//ft.time = itXml.getFloat("time");
			//ft.mount = itXml.getInt("mount");
			//if (itXml.getError() != XML_OK){
			//	ft.mount = 0;
			//}
			//ft.inertia = vector3FromString(itXml.getString("inertia"));
			//if (itXml.getError() != XML_OK){
				//ft.inertia = Ogre::Vector3(0,0,0);
			//}
		};

		}{

		/*XMLIterator it(xml2);
		it.setElemName("##anything##>anim");
		it.gotoZero();
		DRONE_TIMED_ANIM ft;
		while(it.gotoNext() != false){
			ft.time = it.getElem().getFloat("time");
			ft.label = it.getElem().getString("label");
			ft.loop = (it.getElem().getString("loop")=="true");
			mTimedAnim.push_back(ft);
		};*/
		
		}



}

DefaultDrone::DefaultDrone(){
	

}


void DefaultDrone::init(DroneDBE* dbe){
	Parent::init(dbe);
}

void DefaultDrone::show(){
	Parent::show();

	float f = timeAlive();
	Coordinator::Transform base;
	if (mTransformCoordinator){
		base = mTransformCoordinator->getValue(timeAlive());
	}else{
		base.mPos = Ogre::Vector3::ZERO;
		base.mRot = Ogre::Quaternion::IDENTITY;
		base.mScl = Ogre::Vector3::UNIT_SCALE;
	}
	getNode()->setPosition(base.mPos);
	getNode()->setOrientation(base.mRot);

	Ogre::TransformKeyFrame key(NULL, 0);
	key = dynamic_cast<DefaultDroneDBE*>(mDbe)->getKeyFrame(f);
	Ogre::Vector3 v = key.getTranslate();
	v=v*base.mScl;

	getNode()->translate(v);

	Ogre::Radian angle;
	Ogre::Vector3 axis;

	Ogre::Quaternion r = key.getRotation();

	if ( base.mScl.x == -1){
		r.ToAngleAxis(angle, axis);
		axis.x *= -1;
		angle = (Ogre::Radian)0-angle;
		r.FromAngleAxis(angle, axis);
	}
	mSceneNode->rotate(r);

	/*Ogre::Node* n = getNode();
	n->setPosition(mDelta+mTransformCoordinator->getValue(timeAlive()).mPos);*/
}
void DefaultDrone::updateAlive(double timeDelta, double parentTime){
	float f = timeAlive();

	if (!mDbe->isStatic()){
		Coordinator::Transform base;
		if (mTransformCoordinator){
			base = mTransformCoordinator->getValue(timeAlive());
		}else{
			base = dynamic_cast<Coordinator::TransformCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get("static_transform:0,0,0"))->getValue(0);
		}

		getNode()->setPosition(base.mPos);
		getNode()->setOrientation(base.mRot);
		Ogre::TransformKeyFrame key(NULL, 0);
		key = dynamic_cast<DefaultDroneDBE*>(mDbe)->getKeyFrame(f);
		Ogre::Vector3 v = key.getTranslate();
		v=v*base.mScl;

		getNode()->translate(v);
	
		Ogre::Radian angle;
		Ogre::Vector3 axis;
	
		Ogre::Quaternion r = key.getRotation();

		if ( base.mScl.x == -1){
			r.ToAngleAxis(angle, axis);
			axis.x *= -1;
			angle = (Ogre::Radian)0-angle;
			r.FromAngleAxis(angle, axis);
		}
		mSceneNode->rotate(r);


	}

	{
		list<GoAnim::GoAnim*>::iterator it, begin, end;
		
		begin = mGoAnim.begin();
		end = mGoAnim.end();
		for (it=begin;it!=end;it++){
			(*it)->update(timeDelta, parentTime);
		}
	}{
		list<FIRING_TIME>::iterator it, begin, end;
		
		begin = (dynamic_cast<DefaultDroneDBE*>(mDbe)->getFiringTime())->begin();
		end = (dynamic_cast<DefaultDroneDBE*>(mDbe)->getFiringTime())->end();
		for (it=begin;it!=end;it++){
			if (mLevelSegment->isTimeInFrame(getTimeOn()+(*it).time)){
				//Util::Log("IsTimeInFrame:"+ts(getTimeOn()+(*it).time));
				if ((*it).firingOption == FIRING_START){
					fire((*it).mount, (*it).inertia);
				}else{
					ceaseFire((*it).mount);
				}
			}
		}
	}{
		/*list<DRONE_TIMED_ANIM>::iterator it, begin, end;
		
		begin = (dynamic_cast<DefaultDroneDBE*>(mDbe)->getTimedAnim())->begin();
		end = (dynamic_cast<DefaultDroneDBE*>(mDbe)->getTimedAnim())->end();
		for (it=begin;it!=end;it++){
			if (mLevelSegment->isTimeInFrame(getTimeOn()+(*it).time)){
				Util::Log("IsTimeInFrame:"+ts(getTimeOn()+(*it).time));
				Ogre::AnimationState* as;
				as = mEntity->getAnimationState( (*it).label );
				as->setEnabled(true);
				as->setLoop((*it).loop);
				mAnim.push_back(as);
				
				//fire((*it).type);
			}
		}*/
	}{
		list<Ogre::AnimationState*>::iterator it, begin, end;
		
		begin = mAnim.begin();
		end = mAnim.end();
		for (it=begin;it!=end;it++){
			(*it)->addTime(timeDelta);//Level::getSingleton()->getTimeDelta());
		}
	}
}

void DefaultDrone::updateDead(double timeDelta, double parentTime){
	/*mSceneNode->roll((Radian)mDeathRoll*mLevelSegment->getTimeDelta());
	if (
		mSceneNode->getOrientation().getRoll().valueDegrees() < 90 || 
		mSceneNode->getOrientation().getRoll().valueDegrees() > 270){
		mSceneNode->pitch((Radian)1*mLevelSegment->getTimeDelta());
	}else{
		mSceneNode->pitch((Radian)1*mLevelSegment->getTimeDelta());
	}
	mSceneNode->translate(Vector3(0,0,75*mLevelSegment->getTimeDelta()), Ogre::Node::TS_LOCAL);
	mSceneNode->translate(Vector3(0,-75*mLevelSegment->getTimeDelta(), 0), Ogre::Node::TS_PARENT);*/
}

/*bool PathedDrone::update(){
	if (mDbe->isExpired(timeAlive())){
		return true;
	}
	Parent::update();
	if (mLife > 0){
		updateAlive();
	}else{
		updateDead();
	}
	return false;
}*/
}
