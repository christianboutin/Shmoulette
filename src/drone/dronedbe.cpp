#include "stdafx.h"

#include "main.h"
#include "drone\dronedbe.h"
#include "util\file.h"
namespace Shmoulette{

list<WeaponInfo*> DroneDBE::getWeaponInfo(){
	return mWeaponInfo;
}

DroneDBE::DroneDBE(string id){
		mId = id;
}
void DroneDBE::init(XML& dronestatXml, XML& objectMovementXml){
	/*File f(getId()+".xml", "media/drones");
	if (f.error() != FILE_OK){
		throw("Drone without an XML file ("+getId()+")");
	}
	XML* xml;
	xml = new XML();
	xml->setContent((char*)f.content());*/
	
	//XML* xml = new XML(getId()+".xml", "media/drones");

	mMaxTime = dronestatXml.getFloat("maxtime");

	std::string s;
	s = dronestatXml.getString("class");
	if (dronestatXml.getError() != XML_OK){
		throw("Drone class undefined ("+getId()+")");
	}


	/*File file(s+".xml", "media/drones/classes");
	if (file.error() != FILE_OK){
		throw("Drone without an XML class file ("+getId()+")");
	}
	XML* classXml;
	classXml = new XML();
	classXml->setContent((char*)file.content());*/
	XML* classXml = new XML(s+".xml", "media/drones/classes");

	mScale = vector3FromString(classXml->getString("drone", "scale"));
	if (classXml->getError() != XML_OK){
		mScale = Ogre::Vector3(1,1,1);
	}/*else{
		int g=0;
	}*/


	mMaxLife = classXml->getFloat("drone", "life");
	if (classXml->getError() != XML_OK) 
		throw("Class File Missing Argument : "+getId()+" (life)");
	mScore= classXml->getInt("drone", "score");
	if (classXml->getError() != XML_OK) 
		throw("Class File Missing Argument : "+getId()+" (score)");
	
	string dllName= classXml->getString("drone", "dll");
	if (classXml->getError() == XML_OK){
		#ifdef _DEBUG
			string name = dllName+"d";
			mDll = LoadLibrary(name.c_str());
		#else
			mDll = LoadLibrary(dllName.c_str());
		#endif
		//mDllCallback = (DLLCallback)::GetProcAddress(weaponModule, "getInstance");
	}else{
		mDll = NULL;
		//mDllCallback = NULL;
	}
	
	{
	XMLIterator it(classXml);
	it.setElemName("drone>death");
	it.gotoZero();
	while(it.gotoNext() != false){
		DEATH_BEHAVIOUR db;
		
		
		XML dbxml = it.getElem();
		db.mFX = dbxml.getString("fx");
		if (dbxml.getError() != XML_OK) db.mFX = "";
		db.mDLLCustomCode = dbxml.getString("dll");
		if (dbxml.getError() != XML_OK) db.mDLLCustomCode = "";
		db.mTime = dbxml.getFloat("time");
		if (dbxml.getError() != XML_OK) db.mTime = 0;

#if 0
		XMLIterator dit(&dbxml);
		dit.setElemName("death>debris");
		dit.gotoZero();
		while(dit.gotoNext() != false){

			db.mDebris.push_back(dit.getElem());

			/*DEBRIS_DETAIL dd;
			XML ditElem = dit.getElem();
			string dir = ditElem.getString("direction");
			if (dir == "forward"){
				dd.mDirection = DEBRIS_DIRECTION_FORWARD;
			}else if (dir == "random"){
				dd.mDirection = DEBRIS_DIRECTION_RANDOM;
			}else{
				throw("Unsupported debris direction : "+dir);
			}
			dd.mSpinMultiplier = ditElem.getFloat("spin");
			dd.mMesh = ditElem.getString("mesh");
			dd.mPython = ditElem.getString("python");
			if (ditElem.getError() != XML_OK){
				dd.mPython = "";
			}
			



			db.mDebris.push_back(dd);*/
		};
#endif
		mDeath.push_back(db);
	}
	}


	{
	XMLIterator it(classXml);
	it.setElemName("drone>child");
	it.gotoZero();
	while(it.gotoNext() != false){
		DRONE_CHILD dc;
		XML xml = it.getElem();
		dc.mName = xml.getString("drone");
		dc.mBone = xml.getString("bone");
		if (xml.getError() != XML_OK){
			dc.mBone = "";
		}
		dc.mLabel = xml.getString("label");
		int f = xml.getInt("forwardhits");
		if (f == 1){
			dc.mForward = true;
		}else{
			dc.mForward = false;
		}
		Ogre::Vector3 v = vector3FromString(xml.getString("delta"));
		if (xml.getError() != XML_OK){
			dc.mDelta = Ogre::Vector3(0,0,0);
		}else{
			dc.mDelta = v;
		}
		mChild.push_back(dc);

		
	}
	}












	//if (classXml->getError() != XML_OK) throw("Class File Missing Argument : "+getId()+" (deathtime)");

	string resource = classXml->getString("drone", "resource_path");
	/*if (classXml->getError() == XML_OK){
		this->addResourceContext(resource,"models");
	}*/


	string cn = classXml->getString("drone", "children");
	if (classXml->getError() == XML_OK){
		setChildrenName(cn);
	}else{
		setChildrenName(".");
	}
	setMeshName(classXml->getString("drone", "mesh"));
	if (classXml->getError() != XML_OK){
		throw("class without drone mesh");
	}

	mResourceContext = classXml->getString("drone", "resource_path");
	if (classXml->getError() != XML_OK){
		mResourceContext = mMeshName;
	}
	
	{
		XMLIterator it(classXml);
		it.setElemName("drone>weapon");
		it.gotoZero();
		//FIRING_TIME ft;
		while(it.gotoNext() != false){
			addWeaponInfo(&it.getElem());
		};
	}



	XMLIterator it(classXml);
	it.setElemName("drone>resource");
	it.gotoZero();
	//FIRING_TIME ft;
	while(it.gotoNext() != false){
		/*if (it.getElem().getString("type") != "meshmaterial"){
			addResource(it.getElem().getString("name"), it.getElem().getString("type"));
		}else{
			addMeshResources(it.getElem().getString("name"));
		}*/
		if (it.getElem().getError() != XML_OK){
			throw("Class File Resource Missing Argument : "+getId()+" (name)");
		}
	};

	it.setElemName("drone>resourcecontext");
	it.gotoZero();
	while(it.gotoNext() != false){
		string s = it.getElem().getString("path");
		if (it.getElem().getError() != XML_OK){
			throw("Class File Resource Content Missing Argument : "+getId()+" (path)");
		}
		//addResourceContext(s);
	};
}

void DroneDBE::setMeshName(std::string name){
	mMeshName=name;
	//addMeshResources(name);
}

/*	class WeaponInfo{
		protected:
			string mDll;
			string mXml;
			string mMount;
		public:
			WeaponInfo(XML* xml);
			Weapon* spawn(GameObject* launcher);
	};*/



void DroneDBE::addWeaponInfo(XML* xml){
	mWeaponInfo.push_back(new WeaponInfo(xml));
}

/*void DroneDBE::addMeshResources(std::string name){
	addResourceContext(name, "models");
	addResource(name, "Mesh");
	addResource(name, "Material");
	if (File::exists(name+"-armature.skeleton","models\\"+name)) 
		addResource(name+"-armature", "Skeleton");
}*/

Drone* DroneDBE::createDrone(){
	if (!mDll){
		throw("Drone class should never be instanciated without an associated DLL");
	}else{
		typedef Shmoulette::Drone* (__cdecl* DLLDroneCallback)();
		DLLDroneCallback dllFunc = (DLLDroneCallback)::GetProcAddress(mDll, "getInstance");
		Drone* d = dllFunc();
		d->init(this);
		if (mPythonCallback != ""){
			d->setPythonCallback(mPythonCallback);
		}
		return d;
	}
}

/*string DroneDBE::getBoneFromMount(int ndx){
	if (ndx >= mDroneMount.size()){
		throw("Requesting a mount that's out of range");
	}
	list<DroneMount*>::iterator it;
	it=mDroneMount.begin();
	for (int i=0;i<ndx;i++){
		it++;
	}
	return "gluglu";
}*/



}

