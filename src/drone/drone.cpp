
#include "stdafx.h"

#include "main.h"
#include "drone\drones.h"
#include "level.h"
#include "fx\explosion.h"
#include "fx\debris.h"
#include "util\file.h"
namespace Shmoulette{
//DroneDB* Drone::mDroneDB = NULL;



/*#define PushDroneIn(TAG_TYPE)\
	mEntry.push_back(new TAG_TYPE##DB(#TAG_TYPE))*/


void DroneDB::init(){
	if (!isInitDone){

		/*PushDroneIn(ST92RollingThunder);
		PushDroneIn(ST93Mastodon);
		PushDroneIn(SCTruck);
		PushDroneIn(JetMan);*/

		for (unsigned int i=0;i<mEntry.size();i++){
			throw("This still called?");

			//mEntry[i]->init();
		}
		isInitDone = true;
	}
}

Drone* DroneDB::createDrone(XML& dronestatXml, XML& objectMovementXml){
	if (!isInitDone) init();
	
	Drone* rv = _createEntry(dronestatXml, objectMovementXml, "UNIQUEDRONEID"+GameElement::getNextIdCounter())->createDrone();
	string s = objectMovementXml.getString("iff");
	if (objectMovementXml.getError() != XML_OK){
		throw("Drones created on the levelsegment must have IFF flag");
	}
	rv->setQueryFlags(GameObject::queryFlagsFromIff(s));

/*	{
		Ogre::Vector3 dp = vector3FromString(objectMovementXml.getString("pos"));
		if (objectMovementXml.getError() == XML_OK){
			rv->deltaPush(dp);
		};
		Ogre::Quaternion ba = quaternionFromString(objectMovementXml.getString("rot"));
		if (objectMovementXml.getError() == XML_OK){
			rv->baseAngleSet(ba);
		};
		
	}*/
	
	float f = objectMovementXml.getFloat("timeon");
	if (objectMovementXml.getError() == XML_OK){
		rv->setTimeOn(f);
	}
	f = objectMovementXml.getFloat("timeoff");
	if (objectMovementXml.getError() == XML_OK){
		rv->setTimeOff(f);
	}
	f = objectMovementXml.getFloat("life");
	if (objectMovementXml.getError() == XML_OK){
		rv->setTimeOff(rv->getTimeOn()+f);
	}


	return rv;
}

Drone* DroneDB::createDrone(PyObject* po){
	if (!isInitDone) init();
	if (!PyList_Check(po)){
		throw("Drone param must be 1 item list");
	}

	PyObject* myStuff = PyList_GetItem(po,1);
	if (!PyList_Check(myStuff)){
		throw("Drone param must be 1 item list");
	}
	PyObject* tpo = PyList_GetItem(myStuff, 0);

	string type = PyString_AsString(tpo);
	Drone* rv = entry(type)->createDrone();
	rv->setQueryFlags(SceneQueryFlag::DRONE_ENEMY);

	return rv;
}


DroneDBE* DroneDB::entry(string s){
	if (!isInitDone) init();
	for (unsigned int i=0;i<mEntry.size();i++){
		if (stricmp (mEntry[i]->getId().c_str(), s.c_str()) == 0){
			return mEntry[i];
		}
	}
	DroneDBE* rv;
	/*if (s == "jetman_h_zigzag" || 
		s =="jetman_h_line"
		){
		rv = new JetManDB(s);
	}else if (s == "batos_v_cloud" || 
				s=="batos_v_loop" ||
				s=="batos_v_side" ||
				s=="batos_v_straightline" ||
				s=="p94batosswoopfromabove"
				){
		rv = new P94BatosDB(s);
	}else{*/
		/*File* f=new File(s+".xml", "media\\drones");
		if (f->error() != FILE_OK){
			throw("file io error : "+s);
		}

		XML* xml = new XML(f->getStrContent());*/
		XML* xml = new XML(s+".xml", "media\\drones");
		// TODO : Support for static... or force "drone" class?  Hmmmm....
		rv = _createEntry(xml->extractElem("level>dronestats"), xml->extractElem("level>object"),s);

		

	return rv;
/*	File* file;
	file = new File("media\\drones\\"+s+".xml");
	if (file->error() != FILE_OK){
		throw("Database entry "+s+" not found!");
	}
	XML xml;
	xml.setContent((char*)file->content());
	s = xml.getString("dronestats", "class");
	for (unsigned int i=0;i<mEntry.size();i++){
		if (stricmp (mEntry[i]->getId().c_str(), s.c_str()) == 0){
			DroneDBE* newEntry = dynamic_cast<typeid(mEntry[i])>(new DroneDBE("gluglu");
		}
	}
	delete file;*/
}
DroneDBE* DroneDB::_createEntry(XML& dronestatXml, XML& objectMovementXml, string id){ // A drone that is supplied in such a way is unique, therefore it will be created with an pseudo-random entry and not verified against the entry database
DroneDBE* rv;
		string parentType = dronestatXml.getString("cppclass");
		if (dronestatXml.getError() != XML_OK){
			throw("drone data file doesn't have an associated CPP class : "+id);
		}
		
		if (parentType == "DefaultDrone"){
			rv = new DefaultDroneDBE(id);
/*		}else if (parentType == "StationaryDrone"){
			rv = new StationaryDroneDB(id);*/
		}else if (parentType == "Drone"){
			rv = new DroneDBE(id);
		}else{
			throw("Drone Parent of Unknown Type : "+parentType);
		}

		string pc = dronestatXml.getString("python_callback");
		if (dronestatXml.getError() == XML_OK){
			rv->setPythonCallback(pc);
		}else{
			string pc = dronestatXml.getString("python");
			if (dronestatXml.getError() == XML_OK){
				rv->setPythonCallback(pc);
			}else{
				rv->setPythonCallback("");
			}
		}

		//throw("Database entry "+s+" not found, and I don't know how to create it!");
	//}
	rv->init(dronestatXml,objectMovementXml);
	mEntry.push_back(rv);
	return rv;
}

void Drone::_createFromDbe(DroneDBE* dbe){
	mRestorePaintTime = 0;
//	mDelta = Ogre::Vector3(0,0,0);
	//mRailSpawn = false;

	//mMarkedForStandby = false;
	
		

	mDbe = dbe;

	mDeathIndex = mDbe->requestDeathIndex();

	DEATH_BEHAVIOUR* detBeh = mDbe->getDeathBehaviour(mDeathIndex);
	if (detBeh != NULL){
		if (detBeh->mFX != ""){
			FXDB::getSingleton()->getEntry(detBeh->mFX)->preload(mLevelSegment);
		}
	}
	//mDbe->getDeathBehaviour()>preload();

	
	mLife = dbe->getMaxLife();

	{
		vector<DRONE_CHILD> children = dbe->getChildVector();
		vector<DRONE_CHILD>::iterator it;
		for (it = children.begin();it != children.end();it++){
			addChild((*it).mName, (*it).mBone, (*it).mLabel, (*it).mForward, (*it).mDelta);
		}
	}{
		list<XML*>goAnim = dbe->getGoAnim();
		list<XML*>::iterator it;

		for (it = goAnim.begin();it!=goAnim.end();it++){
			addGoAnim(*it);
		}
	}{
		list<XML*>goAnim = dbe->getAnim();
		list<XML*>::iterator it;

		for (it = goAnim.begin();it!=goAnim.end();it++){
			addAnim(*it);
		}
	}

	string cn = mDbe->getChildrenName();
	if (cn != ""){
		this->mChildrenFilename = cn;
		checkForChildren();
	}
	/*setFilename(mDbe->getMeshName());
	if (mFilename.size()==0){
		throw("No filename for drone");
	}*/
	list<WeaponInfo*> inf = mDbe->getWeaponInfo();
	
	
	list<WeaponInfo*>::iterator it;
	for (it = inf.begin();it != inf.end();it++){
		mWeapon.push_back((*it)->spawn(this, WeaponSupply::getDefault()));
	}
}

Drone::Drone():Parent(){
}
void Drone::init(string id){
	Parent::init(id,true);
	_createFromDbe(Core::getSingleton()->getDroneDB()->entry(id));
//	baseScaleSet(Ogre::Vector3(1,1,1));
	//baseAngleSet(Ogre::Quaternion::IDENTITY);

	/*mRestorePaintTime = 0;
	mDelta = Ogre::Vector3(0,0,0);
	mDeathIndex = 0;
	mDbe = NULL;
	mRailSpawn = false;
	setCollisionGroup(COLLGROUP_ENEMY);*/
}

Drone::~Drone(){
	vector<Weapon*>::iterator it;
	for (it = mWeapon.begin();it!=mWeapon.end();it++){
		delete (*it);
	}
}

void Drone::setTimeAlive(float timeAlive){
	setTimeOff(getTimeOn()+timeAlive);
}

void Drone::setTimeStart(float timeStart){
	float timeOff = mDbe->getMaxTime();
	setTimeOn(timeStart);
	if (getTimeOn() == 0){
		setTimeOn(0.0001);
	}
	setTimeBuild(0.0001);//timeDelta+xml->getFloat("Unit", "timestart"));
	//setTimeOff(timeDelta+xml->getFloat("Unit", "timestart")+timeOff);
	setTimeOff(9999);
	setTimeDestroy(timeStart+timeOff);
}

void Drone::init(DroneDBE* dbe){
	Parent::init(dbe->getMeshName(), true);
	_createFromDbe(dbe);
//	baseScaleSet(Ogre::Vector3(1,1,1));
//	baseAngleSet(Ogre::Quaternion::IDENTITY);
}
void Drone::destroy(){
	Parent::destroy();
}
void Drone::build(){
	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_A","BuildDrone")->start();

	mDeathType = DRONE_ALIVE;

	//Util::Log("DRONEBUILD");
	if (mDbe != NULL){
		mLife = mDbe->getMaxLife();

		//if (mDbe->getResourceContext() != mDbe->getMeshName()){
			//this->mResourcePath = mDbe->getResourceContext();
		//}
		/*setFilename(mDbe->getMeshName());
		if (mFilename==""){
			throw("No filename for drone");
		}*/
		//Weapon* wp;

		//Util::Log("DRONEBUILD:"+mFilename);
	}
	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_A","BuildDrone")->end();
	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_B1","BuildDrone")->start();

	//setCollisionGroup(COLLGROUP_ENEMY);

	//Util::Log("DRONEBUILD PARENT : "+mFilename);
	Parent::build();

	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_B1","BuildDrone")->end();
	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_B2","BuildDrone")->start();

	//if (mDbe != NULL){
	//}
	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_B2","BuildDrone")->end();
	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_C","BuildDrone")->start();


	//Util::Log("DRONEBUILD: ALMOST DONE");
		mTarget = Level::getSingleton()->getPlayerShip();
	//Util::Log("DRONEBUILD: DONE");
	Ogre::Vector3 scl = mDbe->getScale();
	setScl(scl);

	/*mSpawnPointEntity = mLevelSegment->getSceneManager()->createEntity(mId+"arrow", "arrow.mesh");;
	mSpawnPointSceneNode = mLevelSegment->getSceneManager()->getRootSceneNode()->createChildSceneNode(mId+"_arrownode");
	mSpawnPointSceneNode->attachObject(mSpawnPointEntity);

	mLocalSpawnPointEntity = mLevelSegment->getSceneManager()->createEntity(mId+"larrow", "arrow.mesh");;
	mLocalSpawnPointSceneNode = mSceneNode->getParentSceneNode()->createChildSceneNode(mId+"_larrownode");
	mLocalSpawnPointSceneNode->attachObject(mLocalSpawnPointEntity);*/
	MACRO_PROFILER_GET(ts(Level::getSingleton()->getTimeTrack())+"_"+mId+"_C","BuildDrone")->end();
}


void Drone::addChild(std::string drone, std::string bone, std::string label, bool forwardHits, Ogre::Vector3 delta, std::string DLL){
	GAME_OBJECT_CHILD c;
	Drone* d = Core::getSingleton()->getDroneDB()->createDrone(drone);
	c.gameObject = d;//Core::getSingleton()->getDroneDB()->createDrone(drone);
	if (forwardHits) dynamic_cast<Drone*>(c.gameObject)->setCollisionForward(this);
	c.bone = bone;
	c.gameObject->setParentConnector(this, c.bone, false);
//	dynamic_cast<Drone*>(c.gameObject)->deltaSet(delta);

	mChild.push_back(c);
	mDroneChild.push_back(d);
	/*GAME_OBJECT_CHILD c;
	c.gameObject = new GameObject(this->getId()+"_"+label, this);
	c.gameObject->setFilename(mesh);
	c.gameObject->setCollisionForward(this);
	//c.gameObject->disableCollisionTest();
	c.bone = bone;
	mChild.push_back(c);*/
}

list<Drone*> Drone::getActiveChildren(){
	list<Drone*> rv;
	list<Drone*>::iterator it;

	for (it = mDroneChild.begin();it!=mDroneChild.end();it++){
		list<Drone*> children = (*it)->getActiveChildren();
		if (children.size() == 0){
			rv.push_back(*it);
		}else{
			list<Drone*>::iterator cit;
			for (cit = children.begin();cit!=children.end();cit++){
				rv.push_back(*cit);
			}
		}
	}
	return rv;
}


/*void Drone::deltaSet(Ogre::Vector3 d){
	mDelta.x = d.x;
	mDelta.y = d.y;
	mDelta.z = d.z;
}*/

void Drone::die(DRONE_DEATH_TYPE deathType){
	setAutoManaged(false);
	mLife = 0;
	setQueryFlags(SceneQueryFlag::IMMATERIAL);
	mDeathType = deathType;
	DEATH_BEHAVIOUR* db;
	db = mDbe->getDeathBehaviour(mDeathIndex);
	if (db != NULL){
		/*Level::getSingleton()->getCurrentSegment()->addGameElement(
			new Explosion(getNode()->_getDerivedPosition(), db->mExplosionDecay,NULL,db->mExplosionParticles)
		);*/
		if (db->mFX != ""){
			FXObject* fxo = new FXObject();

			Ogre::Matrix4 result = mEntity->getParentNode()->_getFullTransform();

			fxo->init(db->mFX, mEntity->getParentSceneNode()->getParentSceneNode(), result);
			mLevelSegment->addGameObj(fxo);
		}

		//list<DEBRIS_DETAIL>::iterator it;
#if 0
		list<XML>::iterator it;
		for (it=db->mDebris.begin();it!=db->mDebris.end();it++){
			
			
			GameObject* debris;
			debris = new Debris();
			debris->init(&(*it));
			debris->getNode()->setPosition(getNode()->_getDerivedPosition());
			/*debris = new Debris((*it).mMesh, getNode()->_getDerivedPosition());
			debris->setTimeOff(Level::getSingleton()->getCurrentSegment()->getTimeTrack()+1);
			debris->setTimeDestroy(Level::getSingleton()->getCurrentSegment()->getTimeTrack()+1);
			if ((*it).mPython != ""){
				debris->setPythonCallback((*it).mPython);
			}*/
			Level::getSingleton()->getCurrentSegment()->addGameElement(debris);

		}
#endif
	}else{
	}

	mDeathTime = 0;
	mLife = 0;
	//setCollisionGroup(COLLGROUP_ALL);
}
void Drone::hit(DamagePackage& pack, GameObject* hitter, GameObject* forwardFrom, float distance, double time){
	if (hitter != NULL){ // Drones can not hit the ground.
		Parent::hit(pack, hitter, forwardFrom, distance, time);
		if (mLife <= 0){
			if (hitter!=NULL){
				if (hitter->getOwner() != NULL){
					Level::getSingleton()->score(mDbe->getScore(), hitter->getOwner());
				}
			}
		}
	}
}
void Drone::takeDamage(DamagePackage& pack, double time){
	//DamagePackage& pack = hitter->getDamagePackage();
	Parent::takeDamage(pack, time);
	setPaintOver("PureWhite");
	mRestorePaintTime = .07;
	if (mLife <= 0){
		die();
	}
}

bool Drone::isInZone(){
	Ogre::Vector3 zoneSize = Level::getSingleton()->getCurrentSegment()->getZoneSize();
	if (isHooked()){
		Ogre::Vector3 pos = mSceneNode->getPosition();
		if (pos.x >= 0-zoneSize.x)
			if (pos.x <= zoneSize.x)
				if (pos.y >= 0-zoneSize.y)
					if (pos.y <= zoneSize.y)
						if (pos.z >= 0-zoneSize.z)
							if (pos.z <= zoneSize.z*2) // Some further away is fine
								return true;
		return false;

	}else{
		Ogre::Vector3 railPos = Level::getSingleton()->getCurrentSegment()->getCurrentHook()->getNode()->_getDerivedPosition();
		Ogre::Vector3 thisPos = getNode()->_getDerivedPosition();
		if (railPos.squaredDistance(thisPos) 
			<=
			Level::getSingleton()->getCurrentSegment()->getZoneSize().squaredLength()*1.5
			){
			return true;
		}else{
			return false;
		}
		//throw("Support for unhooked drones soon please");
	}



}

void Drone::notifyChildDeath(GameObject* deadBaby){
	list<Drone*>::iterator it;
	for (it=mDroneChild.begin();it!=mDroneChild.end();it++){
		if ((*it) == deadBaby){
			mDroneChild.erase(it);
			break;
		}
	}
}


bool Drone::update(double timeDelta, double parentTime){

	//Parent::update();
	//fire(0);

	/*mSpawnPointSceneNode->setPosition(mSceneNode->_getDerivedPosition());
	mSpawnPointSceneNode->setOrientation(mSceneNode->_getDerivedOrientation());




	
	//Ogre::SceneNode* sn = ammo->getSceneNode();
	Ogre::Skeleton* skel = mEntity->getSkeleton();
	Ogre::Bone* bone = skel->getBone("cannon");

	mSpawnPointSceneNode->setPosition(
		mEntity->_getParentNodeFullTransform() * bone->_getDerivedPosition()
	);
	mSpawnPointSceneNode->setOrientation(mEntity->_getParentNodeFullTransform().extractQuaternion());// * bone->_getDerivedOrientation()
	
	mSpawnPointSceneNode->rotate(bone->_getDerivedOrientation(), Ogre::Node::TS_LOCAL);
	Ogre::Radian r;
	r = 0-Ogre::Math::PI;
	r = r/2;
	mSpawnPointSceneNode->pitch(r);

	mSpawnPointSceneNode->_update(true, true);
	
	mLocalSpawnPointSceneNode->getParentSceneNode()->_update(true, true);
	
	Ogre::Matrix4 matrix = mLocalSpawnPointSceneNode->getParentSceneNode()->_getFullTransform().inverse();
	Ogre::Matrix4 transform = matrix * mSpawnPointSceneNode->_getFullTransform();
	
	mLocalSpawnPointSceneNode->setPosition(
		transform.getTrans()
		);
	mLocalSpawnPointSceneNode->setOrientation(
		transform.extractQuaternion()
		);

	mLocalSpawnPointSceneNode->_update(false, false);


	mSpawnPointSceneNode->setVisible(false);

	Ogre::Vector3 crisse = mLocalSpawnPointSceneNode->_getDerivedPosition();
	Ogre::Vector3 tabarnak = mSpawnPointSceneNode->_getDerivedPosition();

	if ( crisse != tabarnak){
		
		//throw("This sucks");
		int i=0;
	}
	//mLocalSpawnPointSceneNode->roll((Ogre::Radian).001);
	//mSceneNode->setPosition(mSceneNode->_getDerivedPosition());


	/*sn->setOrientation(this->getSceneNode()->getOrientation());
	sn->setPosition(this->getSceneNode()->getPosition());
	sn->_update(true,true); // Maybe this is useless, but for the sake of testing it can't harm
	Ogre::Vector3 vec;
	vec = bone->getPosition();
	sn->translate(vec, Ogre::Node::TS_LOCAL);
	sn->_update(true,true); // Maybe this is useless, but for the sake of testing it can't harm*/

	
	//mSceneNode->setVisible(false);*/


	//Util::Log("PAINT RESTORATION",1);
	if (mRestorePaintTime > 0){
		//Util::Log("PAINT RESTORATION2",1);
		mRestorePaintTime -= mLevelSegment->getTimeDelta();
		//Util::Log("PAINT RESTORATION3",1);
		if (mRestorePaintTime <= 0){
			//Util::Log("PAINT RESTORATION4",1);
			removePaintOver();
			mRestorePaintTime =0;
		}
	}
	//Util::Log("EXPIRATION",1);

	if (mDbe->isExpired(timeAlive())){
		return true;
	}
	//Util::Log("PARENT",1);
	if (Parent::update(timeDelta, parentTime)==true){
		return true;
	};
	//Util::Log("LIFE",1);
	if (mLife > 0){
		updateAlive(timeDelta, parentTime);
	}else{
		updateDead(timeDelta, parentTime);
		mDeathTime += mLevelSegment->getTimeDelta();
		if (mDeathTime >= mDbe->getDeathTime(mDeathIndex)){
			setAutoManaged(false);
			//hide();
			//destroy();
			return true;
		}
	}
	return false;
}


bool Drone::postFrameUpdate(double timeDelta, double parentTime){
	{
		vector<Weapon*>::iterator it;
		for (it = mWeapon.begin();it!=mWeapon.end();it++){
			(*it)->postFrameUpdate();
		}
	}

	list<Drone*>::iterator it;
	for (it=mDroneChild.begin();it!=mDroneChild.end();it++){
		if (!(*it)->isAutoManaged()){
			(*it)->markForTermination(); // Drones with destroyed children can't loop, for now
		}
	}
	return Parent::postFrameUpdate(timeDelta, parentTime);
}


void Drone::show(){
	//Util::Log("SHOW",1);
	Parent::show();
	//Util::Log("SHOW2",1);
#if 0
	if (mRailSpawn){
		Ogre::SceneNode* go;
		go = mLevelSegment->getContainer()->createChildSceneNode("temp");
/*		go->translate(mDelta.x, mDelta.y, mDelta.z);
		
		mDelta.x = go->_getDerivedPosition().x;
		mDelta.y = go->_getDerivedPosition().y;
		mDelta.z = go->_getDerivedPosition().z;
		setPos(mDelta.x, mDelta.y, mDelta.z);*/
		Ogre::SceneNode* dummy = mLevelSegment->getContainer();

		Ogre::Quaternion q = mLevelSegment->getContainer()->_getDerivedOrientation();
		mSceneNode->setOrientation(q);

		mLevelSegment->getContainer()->removeAndDestroyChild("temp");
	}else{
		//getNode()->setPosition(mDelta.x, mDelta.y, mDelta.z);
	}
#endif
	//Util::Log("SHOW3",1);
	//if (mBaseAngle.x != XML_NO_VALUE){
	//getNode()->setOrientation(mBaseAngle);
	//Util::Log("SHOW4",1);
	//}
}
/*void Drone::deltaPush(Ogre::Vector3 d){
	mDelta.x += d.x;
	mDelta.y += d.y;
	mDelta.z += d.z;
}*/

}
