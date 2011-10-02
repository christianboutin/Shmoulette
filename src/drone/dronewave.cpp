#include "stdafx.h"
#include "main.h"
#include "drone\dronewave.h"
#include "trigger\trigger.h"
#include "util\file.h"
namespace Shmoulette{
	using namespace Ogre;

#define DroneTypeCheck(TAG_STRING,TAG_CLASS)\
	if (droneType == TAG_STRING){\
		obj = new TAG_CLASS(&waveXMLIt.getElem(),mTimeOn);\
	}

	void DroneWave::addChild(XML& xml){
		bool stdChild = false;
	
		string cBone = xml.getString("bone");
		if (xml.getError() != XML_OK){
			cBone = "";
		}

		string cType=xml.getString("type");
		GAME_OBJECT_CHILD c;
		if (cType=="drone"){
			XML bbxml = xml.extractElem("child>drone");

			Drone* obj;
			obj = Core::getSingleton()->getDroneDB()->createDrone(bbxml, bbxml);
			
			obj->setTransformCoordinator("macro:zero_transform");

			float to = xml.getFloat("timeon");
			if (xml.getError() != XML_OK){
				to=0;
			}
			obj->setTimeStart(to);
			obj->moveTimes(getTimeOn());
			obj->setParentConnector(this, "", false);

			mVectorDrone.push_back(obj);

		}else{
			Parent::addChild(xml);
		}

	}

	void DroneWave::init(XML* xml){
		Parent::init(xml);
		//mRootUnit = NULL;
		XML* waveXML;
		Ogre::Vector3 delta_push;
		Quaternion baseAngle;
		string tmp;
		mType = OBJ_TYPE_NOMESH;

		//mTimeBuild = 0;

		{
			XMLIterator it(xml);
			it.setElemName("dronewave>ondeath>trigger");
			it.gotoZero();
			while(it.gotoNext() != false){
				Trigger* trig;
				trig = new Trigger(&it.getElem());
				mPostDeathTrigger.push_back(trig);
			}
		}

		int current_pt = 1;

		/*mIsRailed = xml->getInt("dronewave", "railed")>0?true:false;
		if (xml->getError() != XML_OK){
			mIsRailed = true;
		}
		bool isRailSpawn = xml->getInt("dronewave", "railspawn")>0?true:false;
		if (xml->getError() != XML_OK){
			isRailSpawn = false;
		}*/

		//delta_push = vector3FromString(xml->getString("pos"));
		//baseAngle = quaternionFromString(xml->getString("rot"));
		//delta_push.x = xml->getInt("dronewave>pos", "x");
		//delta_push.y = xml->getInt("dronewave>pos", "y");
		//delta_push.z = xml->getInt("dronewave>pos", "z");

		/*Radian r;
		Degree d;
		Vector3 v;
		d = xml->getFloat("dronewave>rot", "a");
		r=d.valueRadians();
		v.x = xml->getFloat("dronewave>rot", "x");
		v.y = xml->getFloat("dronewave>rot", "y");
		v.z = xml->getFloat("dronewave>rot", "z");
		if (v.x != XML_NO_VALUE){
			baseAngle.FromAngleAxis(r, v);
		}else{
			baseAngle.FromAngleAxis((Radian)Ogre::Math::PI, Vector3(0,1,0));
		}*/

		int numberOfDrones=0;
		int totalScore=0;

		tmp = xml->getString("dronewave", "type")+".xml";
		if (xml->getError() == XML_OK){

			waveXML = new XML(tmp, "media\\dronewaves");
			
			XMLIterator waveXMLIt(waveXML);
			waveXMLIt.setElemName("drone>unit");

			while(waveXMLIt.gotoNext() != false){
				XML elem(waveXMLIt.getElem());
				Util::Log("Initiating Drone of type "+elem.getString("type"));
				Drone* obj;
				string droneType = elem.getString("type");


				//DroneTypeCheck("shippu", Shippu);
				//DroneTypeCheck("scflak", SCFlak);


				obj = Core::getSingleton()->getDroneDB()->createDrone(droneType);
				/*if (elem.getString("root_unit") == "1"){
					mRootUnit = obj;
				}*/
				obj->setParentConnector(this, "", false);

				obj->setQueryFlags(SceneQueryFlag::DRONE_ENEMY);
				//, &waveXMLIt.getElem(), getTimeOn()
				obj->setTimeStart(elem.getFloat("timestart")+getTimeOn());

				float timeAlive = elem.getFloat("timealive");
				if (elem.getError() == XML_OK){
					obj->setTimeAlive(timeAlive);
				}

				//Ogre::Vector3 v = vector3FromString(waveXMLIt.getElem().getString("pos"));
				//obj->deltaSet(v);
				{
					string s = elem.getString("transform_c");
					if (elem.getError() == XML_OK){
						obj->setTransformCoordinator(s);
					}
				}

				Ogre::Vector3 scale(1,1,1);
				string s = elem.getString("path_scl");
				if (elem.getError() == XML_OK){
					scale = vector3FromString(s);
				}
				//scale.y = xml->getInt("Unit>path_scl", "y");
				//scale.z = xml->getInt("Unit>path_scl", "z");*/
				//obj->baseScaleSet(scale);
				//obj->deltaPush(delta_push);

				//obj->baseAngleSet(baseAngle);
				//obj->setRailSpawn(isRailSpawn);

				mVectorDrone.push_back(obj);
				numberOfDrones++;
				totalScore+=obj->getDbe()->getScore();
			}
		}else{
			// Drones are integrated as children of the dronewave.  Individual drones will be fed through the addChild method called from GameObject
			//mIsRailed = false;
			//Coordinator::TransformCoordinator* tc = dynamic_cast<Coordinator::TransformCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get(xml->getString("transform_c")));
//			string tc = xml->getString("transform_c");
			//mTransformCoordinator = dynamic_cast<Coordinator::TransformCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get(tc));
			
			/*Ogre::Vector3 delta = tc->getValue(0).mPos;//vector3FromString(xml->getString("pos"));
			delete tc;
			if (xml->getError() != XML_OK){
				delta = Ogre::Vector3(0,0,0);
			}*/
			
			/*XML childXml = xml->extractElem("dronewave>children");
			XMLIterator it(&childXml);
			it.setElemName("children>child");
			while(it.gotoNext() != false){
					XML elem = it.getElem();
					XML xml = elem.extractElem("child>drone");
					if (elem.getError() != XML_OK){
						throw("dronwave can only have 'drone' type children.  Check source file");
					}
					Drone* obj;
					obj = Core::getSingleton()->getDroneDB()->createDrone(xml, xml);
					//obj->moveTimes(deltaTime);
					//obj->capTimes(mLength);
					float to = xml.getFloat("timeon");
					if (xml.getError() != XML_OK){
						to=0;
					}
					obj->setTimeStart(to);
					obj->moveTimes(getTimeOn());
					obj->setParentConnector(this, "", false);
					obj->setRailSpawn(false);

					mVectorDrone.push_back(obj);
					numberOfDrones++;
					totalScore+=obj->getDbe()->getScore();
			}*/
		}
		if (numberOfDrones > 1){
			mBonus = totalScore;
		}else{
			mBonus = 0;
		}

			
		/*sprintf(sendString, "drone>Unit");
		tmp = xml.getString(sendString, "type"); // Only used to trigger an error, doesn't use the info
		while (xml.getError() == XML_OK){
		
			Drone* obj;
			if (tmp == "shippu"){
				obj = new Shippu();
			}else if (tmp == "scflak"){
				obj = new SCFlak();
			}else{
				obj = new Drone();
			}

			//obj->dataFromXML(xml.extractElem(sendString), mTimeBuild, mTimeDestroy - mTimeBuild);
			obj->deltaPush(
				delta_push.x, 
				delta_push.y, 
				delta_push.z 
					);

			mVectorDrone.push_back(obj);
		}*/
		//mTimeOn = 0;
		//delete file;
		//setTimeOff(9999);
		//setTimeDestroy(9999);
	}

	void DroneWave::reset(){
		vector<Drone*>::iterator it;
		
		/*for (it=mVectorDroneStandby.begin();it!=mVectorDroneStandby.end();it++){
			mVectorDrone.push_back((*it));
		}
		mVectorDroneStandby.clear();*/

		for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
			if ((*it)->isOn()){
				(*it)->hide();
			}
			if ((*it)->isBuild()){
				(*it)->destroy();
			}
		}
	}

	DroneWave::~DroneWave(){
			vector<Drone*>::iterator it;
			for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
				delete(*it);
			}
	}

	Drone* DroneWave::getDrone(int n){
		if (n>=mVectorDrone.size()){
			return NULL;
		}else{
			vector<Drone*>::iterator it = mVectorDrone.begin();
			for (int i=0;i<n;i++){
				it++;
			}

			return (*it);
		}
	}

	void DroneWave::build(){
		Parent::build();
		mBonusAllowed = true;

		while(mVectorDroneStandby.size() > 0){
			Drone* t;
			t = *(mVectorDroneStandby.begin());
			t->setAutoManaged(true);
			if (t->isBuild()){
				int g= 0;
			}
			//t->build();

			mVectorDrone.push_back(t);
			mVectorDroneStandby.erase(mVectorDroneStandby.begin());
			
		}

		/*list<GameObject*>::iterator it;
		for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
			(*it)->build();
		}*/
		/*if (mIsRailed){
			vector<Drone*>::iterator it;
			for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
				(*it)->setHooked(true);
			}
		}*/
	}
	list<Drone*> DroneWave::getActiveDroneList(){
		list<Drone*> rv;

		vector<Drone*>::iterator it;
		
		for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
			if ((*it) != NULL){
				if ((*it)->isOn()){
					if ((*it)->isAlive()){
						if ((*it)->isAutoManaged()){
							list<Drone*> children = (*it)->getActiveChildren();
							if (children.size() == 0){
								if ((*it)->isInZone()){
									rv.push_back(*it);
								}
							}else{
								list<Drone*>::iterator cit;
								for (cit = children.begin();cit!=children.end();cit++){
									if ((*cit)->isInZone()){
										rv.push_back(*cit);
									}
								}
							}
						}
					}
				}
			}
		}
		return rv;

	}

	void DroneWave::destroy(){
		vector<Drone*>::iterator it;
		for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
			(*it)->destroy();
		}

		Parent::destroy();
	}
	void DroneWave::show(){
		Parent::show();
	}
	void DroneWave::hide(){
		Parent::hide();
	}


	/*bool DroneWave::_rootAlive(){
		if (mRootUnit == NULL){ // No root unit, so return fucking true because that's the way it is
			return true;
		}else{
			if (mRootUnit->isAlive()){
				return true;
			}else{
				return false;
			}
		}
	}*/

	bool DroneWave::update(double timeDelta, double parentTime){
		Util::Log("DroneWave Update",1);

		Parent::update(timeDelta, parentTime);
		vector<Drone*>::iterator it;
		bool empty = true;
		for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
			if ((*it) != NULL){
				Util::Log("Updating Drone",1);

				if ((*it)->isAlive()){
					empty = false;
				}/*else{
					if ((*it) == mRootUnit){
						for (vector<Drone*>::iterator sdit=mVectorDrone.begin();sdit!=mVectorDrone.end();sdit++){
							if ((*it)->isAlive()){
								(*it)->die();
							}
						}
					}
				}*/
				//if (_rootAlive() || (*it)->isBuild()){
				if ((*it)->updateIfOn(timeDelta, parentTime) == true){
					Util::Log("*Destroying Drone",1);
					mLevelSegment->removeCollisionTest(*it);
					(*it)->hide();
					(*it)->destroy();
					mVectorDroneStandby.push_back(*it);
					//delete(*it);
					(*it)=NULL;
				}
				//}
				Util::Log("*Updating Drone",1);
			}
		}
		for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
			if ((*it) == NULL){
				mVectorDrone.erase(it);
				if (mVectorDrone.size() == 0){
					break;
				}else{
					it = mVectorDrone.begin();
				}

			}
		}
		if (empty == true){
			if (mBonusAllowed == true){
				for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
					if ((*it)->getDeathType() != DRONE_DEATH_DESTRUCTION){
						mBonusAllowed = false;
						break;
					}
				}
				for (it=mVectorDroneStandby.begin();it!=mVectorDroneStandby.end();it++){
					if ((*it)->getDeathType() != DRONE_DEATH_DESTRUCTION){
						mBonusAllowed = false;
						break;
					}
				}
				if (mBonusAllowed == true){
					Level::getSingleton()->score(mBonus, NULL);
					mBonusAllowed = false;
				}
			}
		}
		if (mVectorDrone.size() == 0){
			list<Trigger*>::iterator it;
			for (it = mPostDeathTrigger.begin();it != mPostDeathTrigger.end();it++){
				(*it)->run();
			}
			//this->setTimeOff(Level::getSingleton()->getTimeDelta());
			return true;
		}
		Util::Log("*DroneWave Update",1);
		return false;
	}

	bool DroneWave::postFrameUpdate(double timeDelta, double parentTime){
		if (mIsOn){
			vector<Drone*>::iterator it;
			for (it=mVectorDrone.begin();it!=mVectorDrone.end();){
				if ((*it) != NULL){
					bool terminate = !(*it)->isAutoManaged();
					if (!terminate){
						if ((*it)->postFrameUpdate(timeDelta, parentTime) == true){
							(*it)->setAutoManaged(false);
						}
						it++;
					}else{
						mLevelSegment->removeCollisionTest(*it);
						Drone* tmp = *it;

						if (it != mVectorDrone.begin()){
							vector<Drone*>::iterator prev_it = it-1;
							mVectorDrone.erase(it);
							it = prev_it;
							it++;
						}else{
							mVectorDrone.erase(it);
							it = mVectorDrone.begin();
						}
						tmp->hide();
						tmp->destroy();

						mVectorDroneStandby.push_back(tmp);
						
						//delete tmp;
					};
				}else{
					it++;
				}
			}
		}
		return false;
	}



	bool DroneWave::updateIfOn(double timeDelta, double parentTime){
		return (Parent::updateIfOn(timeDelta, parentTime));
	}

	COLLISION_RESULT DroneWave::collisionTest(GameObject* obj){
		vector<Drone*>::iterator it;
		COLLISION_RESULT rv;
	/*	for (it=mVectorDrone.begin();it!=mVectorDrone.end();it++){
			if ((*it) != NULL){
				if ((*it) != obj){
					if ((*it)->isOn()){
						if ((*it)->isSolid() == true){
							rv = (*it)->isColliding(obj);
							if (rv.mOccured == true){
								return rv;
							}
						}
					}
				}
		
			}
		}*/
		rv.mOccured = false;
		return rv;
	}

}