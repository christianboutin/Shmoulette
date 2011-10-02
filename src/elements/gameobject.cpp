#include "stdafx.h"
#include "main.h"
#include "elements\gameobject.h"
#include "elements\billboard.h"
#include "world\world.h"

#include "elements\light.h"
#include "elements\camera.h"
#include "util\file.h"
#include "util\pythonextra.h"
#include "weapon\weapondb.h"
using namespace Ogre;

#define MACRO_PYTHON_TYPE_CHECK(context, var, type){\
	if (!type(var)) throw("In context : "+(string)context+" Python Type Check Fail");\
}

#define MACRO_SWAP_ID \
	string oldId= bbxml.getString("id");\
	bool restoreId = false;\
	if (bbxml.getError() == XML_OK){\
		restoreId = true;\
		bbxml.setAttrib("id", getId()+"_"+oldId);\
	}\



#define MACRO_RESTORE_ID \
	if (restoreId)\
		bbxml.setAttrib("id", oldId);\
	else\
		c.gameObject->setId(this->getId()+"_"+c.gameObject->getId());

namespace Shmoulette{
	
	DamagePackage::DamagePackage(){
		mDamageCycle = DAM_CYCLE_CONTINUOUS;
		mDamageTotal = DAM_TOTAL_ADD;
	}
	DamagePackage::DamagePackage(PyObject* po){
		if (!PyList_Check(po)){
			throw("Damage Package second param must be a 2 elements list, now it's not even a list, dude");
		}
		if (PyList_Size(po)!= 2){
			throw("Damage Package second param must be a 2 elements list instead of having just "+ts(PyList_Size(po)));
		}
		PyObject* rootParams = PyList_GetItem(po, 0);
		mDamageCycle = (Shmoulette::DAMAGE_CYCLE)PyInt_AsLong(PyList_GetItem(rootParams,0));
		mDamageTotal = (Shmoulette::DAMAGE_TOTAL)PyInt_AsLong(PyList_GetItem(rootParams,1));

		PyObject* listOfDamages = PyList_GetItem(po, 1);
		for (int i=0;i<PyList_Size(listOfDamages);i++){
			PyObject* damParams = PyList_GetItem(listOfDamages, i);
			DAMAGE dam;
			dam.amount = PyFloat_AsDouble(PyList_GetItem(damParams, 0));
			dam.type = (Shmoulette::DAMAGE_TYPE) PyInt_AsLong(PyList_GetItem(damParams, 1));
			addDamage(dam.type, dam.amount);
		}
	}

	void DamagePackage::addFromXml(XML* xml){
		string s = xml->getString("cycle");
		if (s == "instant"){
			mDamageCycle = DAM_CYCLE_INSTANT;
		}else if (s == "continuous"){
			mDamageCycle = DAM_CYCLE_CONTINUOUS;
		}else{
			throw("Damage Cycle Unsupported : "+s);
		}

		s = xml->getString("total");
		if (s == "add"){
			mDamageTotal = DAM_TOTAL_ADD;
		}else if (s == "highest"){
			mDamageTotal = DAM_TOTAL_HIGHEST;
		}else{
			throw("Damage Total Unsupported : "+s);
		}
		XMLIterator it(xml);
		it.setElemName("damage>elem");
		it.gotoZero();
		while(it.gotoNext() != false){
			DAMAGE db;
			db.amount = it.getElem().getFloat("amount");
			s = it.getElem().getString("type");
			if (s == "concussion") db.type = DAM_TYPE_CONCUSSION;
			else if (s == "cut") db.type = DAM_TYPE_CUT;
			else if (s == "pierce") db.type = DAM_TYPE_PIERCE;
			else if (s == "fire") db.type = DAM_TYPE_FIRE;
			else if (s == "water") db.type = DAM_TYPE_WATER;
			else if (s == "electricity") db.type = DAM_TYPE_ELECTRICITY;
			else if (s == "frost") db.type = DAM_TYPE_FROST;
			else if (s == "emp") db.type = DAM_TYPE_EMP;

			mDamage.push_back(db);
		}
	}
	void DamagePackage::addFromDamagePackage(DamagePackage& pack){
		vector<DAMAGE>::iterator it;
		for (it = pack.mDamage.begin();it!= pack.mDamage.end();it++){
			mDamage.push_back((*it));
		}
	}
	void DamagePackage::copyFromDamagePackage(DamagePackage& pack){
		mDamageTotal = pack.mDamageTotal;
		mDamageCycle = pack.mDamageCycle;
		mDamage.clear();
		addFromDamagePackage(pack);
	}

	void DamagePackage::addDamage(DAMAGE_TYPE type, float amount){
		DAMAGE d;
		d.type = type;
		d.amount = amount;
		mDamage.push_back(d);
	}

	float DamagePackage::getTotalDamage(){
		float rv = 0;
		vector<DAMAGE>::iterator it;
		for (it = mDamage.begin();it!= mDamage.end();it++){
			if (mDamageTotal == DAM_TOTAL_ADD){
				rv += (*it).amount;
			}else if (mDamageTotal == DAM_TOTAL_HIGHEST){
				if ((*it).amount > rv){
					rv = (*it).amount;
				}
			}
		}
		return rv;
	}

	void GameObject::recreateSceneNode(){
		mSceneNode = mLevelSegment->getSceneManager()->getRootSceneNode()->createChildSceneNode(mId+"_node"+ts(GameElement::getNextIdCounter()));
		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			(*it).gameObject->recreateSceneNode();
		}
	}

	void GameObject::addChild(GAME_OBJECT_CHILD c){
		c.gameObject->setParentConnector(this, c.bone, false);

		mChild.push_back(c);

		if (c.gameObject->getId().substr(0,getId().length()) != getId()){
			throw("This child doesn't have proper ID");
		}
		if (isBuild()){
			c.gameObject->build();
		}
		if (isOn()){
			c.gameObject->show();
		}
	}

	void GameObject::ceaseFire(int type){
		mWeapon[type]->ceaseFire();
	}

	void GameObject::fire(int type, Ogre::Vector3 inertia){
		if ((type+1) > mWeapon.size()){
			throw("Weapon Index out of range "+ts(type));
		}
		mWeapon[type]->setInertia(inertia);
		mWeapon[type]->fire();
	}

	Weapon* GameObject::addWeapon(string type, string bone, WeaponSupply* ws){
		if (ws == NULL){
			ws = WeaponSupply::getDefault();
		}
		Weapon* weap = WeaponDB::getSingleton().get(type)->spawn(ws);
		//weap->setInertia(inertia);
		//weap->setHooked(hooked);
		
		weap->setMount(this, bone);
		//weap->fire();
		mWeapon.push_back(weap);
		return weap;
	};
	void GameObject::addChild(XML& xml){
		bool stdChild = false;
	
		string cBone = xml.getString("bone");
		if (xml.getError() != XML_OK){
			cBone = "";
		}

		string cType=xml.getString("type");
		GAME_OBJECT_CHILD c;
		if (xml.getError() != XML_OK || cType=="object" || cType=="mesh"){
			XML bbxml = xml.extractElem("child>object");
			
			MACRO_SWAP_ID;
			c.gameObject = new Shmoulette::GameObject();
			c.gameObject->init(&bbxml);
			MACRO_RESTORE_ID;
			stdChild = true;
		}else if (cType=="billboard"){
			XML bbxml = xml.extractElem("child>billboard");

			MACRO_SWAP_ID;
			c.gameObject = new Shmoulette::Billboard();
			c.gameObject->init(&bbxml);
			MACRO_RESTORE_ID;
			stdChild = true;
		}else if (cType=="group"){
			string fn = xml.getString("filename");
			string context = xml.getString("context");
			try{
				XML childXml(fn, context);
				addChildren(&childXml);
			}catch(...){
			}
			stdChild = false;
		}else if (cType=="light"){
			XML bbxml = xml.extractElem("child>light");

			MACRO_SWAP_ID;
			c.gameObject = new Shmoulette::Light();
			c.gameObject->init(&bbxml);
			MACRO_RESTORE_ID;
			stdChild = true;
		}else if (cType=="particle"){
			XML bbxml = xml.extractElem("child>particle");

			MACRO_SWAP_ID;
			c.gameObject = new Shmoulette::ParticleEmitter();
			c.gameObject->init(&bbxml);
			MACRO_RESTORE_ID;
			stdChild = true;
		}else if (cType=="camera"){
			XML bbxml = xml.extractElem("child>camera");

			MACRO_SWAP_ID;
			c.gameObject = new Shmoulette::Camera();
			c.gameObject->init(&bbxml);
			MACRO_RESTORE_ID;
			stdChild = true;
		/*}else if (cType == "cloudzone"){
			XML bbxml = xml.extractElem("child>cloudzone");

			MACRO_SWAP_ID;
			c.gameObject = new Shmoulette::CloudZone(&bbxml);
			MACRO_RESTORE_ID;*/
		}else if (cType == "weapon"){
			string wn = xml.getString("weapon");
			if (xml.getError() != XML_OK){
				throw("Weapon child requires weapon field");
			}

			Ogre::Vector3 inertia = vector3FromString(xml.getString("inertia"));
			if (xml.getError() != XML_OK){
				inertia.x = FLT_MAX;
			}
			/*string hookedstring = xml.getString("hooked");
			bool hooked;
			if (xml.getError() != XML_OK){
				hooked = false;
			}else{
				hooked = boolFromString(hookedstring);
			}*/
			if (cBone == ""){
				throw("Weapon must be hooked to a bone");
			}

			Weapon* weap = addWeapon(wn, cBone, WeaponSupply::getDefault());
			stdChild = false;
		}else{
			//stdChild = false;
			throw ("Child type : "+cType+" not supported");
		}

		if (stdChild == true){
			
			string colFor = xml.getString("collision_forward");
			if (xml.getError() == XML_OK){
				if (colFor == "1"){
					c.gameObject->setCollisionForward(this);
				}
			}

			c.bone = cBone;
			//string newId = this->getId()+"_"+c.gameObject->getId();
			//c.gameObject->setId(newId);
			//c.gameObject->setId(this->getId()+"_"+lbl);
			
			//c.gameObject->setDoNotHook(true);
			
			c.gameObject->setHooked(false);
			c.gameObject->setParentConnector(this, c.bone, false);
			//c.gameObject->setStreamBuild(mStreamBuild);
			
			
			//c.gameObject->moveTimes(getTimeOn()); // NO LONGER REQUIRED?
			addChild(c);
		}
	}
	void GameObject::addChildren(XML* xml){
		XMLIterator it(xml);
		it.gotoZero();
		it.setElemName("children>child");
		while(it.gotoNext()!=false){
			XML elem = it.getElem();
			addChild(elem);
		}
	}

	void GameObject::checkForInitScript(){
		string moduleName;
		moduleName = "meshinit_"+mFilename;//+".py";
		if (File::exists((string)moduleName+".py","python")){
			PyObject *pName,*pModule,*pDict,*pClass,*pArgs;


			/*if (params != ""){
				pArgs = PyString_FromString(params.c_str());
			}else{
				pArgs = Py_None;
			}*/

			/*std::vector<string> pca = StringUtil::split(pc, ",", 1);*/
			string fullModuleName;
			//fullModuleName += "python/meshinit/";
			fullModuleName += moduleName;
			pName = PyString_FromString(fullModuleName.c_str());
			
			/*if (pca.size() > 1){
				pArgs = PyString_FromString(pca[1].c_str());
			}*/
			//Util::Log("PYTHON A5 "+pc, 1);	
			pModule = PyImport_Import(pName);
			//Util::Log("Module", 1);	
			if (pModule == NULL){
				throw(Core::getSingleton()->getPythonException()+"\n(in loading module "+fullModuleName+".py) CHECK FOR CASE SENSITIVITY ISSUES");
			}
			pDict = PyModule_GetDict(pModule);
			if (pDict == NULL){
				throw(Core::getSingleton()->getPythonException()+"\n(in loading module dictionary "+fullModuleName+")");
			}

			/*PyObject* args = PyArg_ParseTuple(pArgs, "si", &mId, &num);*/
			pClass = PyDict_GetItemString(pDict, "init");

			if (pClass == NULL){
				throw(Core::getSingleton()->getPythonException()+"\n(in loading init module "+fullModuleName+")\n*meshinit python files must have an init method*");
			}
			//Util::Log("PYTHON D", 1);	
			PyObject* args;
			/*if (params != ""){
				args = PyTuple_New(2);
			}else{
				args = PyTuple_New(1);
			}*/
			PyObject* objAddress = PyLong_FromLong((long)this);
			
			/*PyTuple_SetItem(args, 0, objAddress);
			if (params != ""){
				PyTuple_SetItem(args, 1, pArgs);
			}*/
			
			PyObject* params = PyTuple_New(2);
			PyTuple_SetItem(params, 0, objAddress);
			if (mInitParams == ""){
				PyTuple_SetItem(params, 1, Py_None);
			}else{
				PyTuple_SetItem(params, 1, PyString_FromString(mInitParams.c_str()));
			}
			if (PyObject_CallObject(pClass, params) == NULL){
				throw("On init script of "+mId+", "+Core::getSingleton()->getPythonException());
			}
		}
	}

	void GameObject::checkForChildren(){
		XML* xmlptr = NULL;
		try{
			XML xml(mChildrenFilename, "media\\children");
			xmlptr = &xml;
		}catch(string s){
			if (s.find("parsing") != string::npos){
				throw(s);
			}
			// throw (s)
		}
		if (xmlptr != NULL){
			addChildren(xmlptr);
		}

		//Util::Log("Checking for Children of : "+mId+" ...DONE",2);
	}


	void GameObject::loadCollisionData(string filename){
		if (mCollisionStructure){
			delete mCollisionStructure;
		}

		Util::Log("Checking for Collision Data with : "+mId, 2);
		if (filename == ""){
			filename = mFilename+"_coll.xml";
		}else{
			filename = filename+".xml";
		}
		
		XML* xml;
		try{
			xml = new XML(filename, "media\\collisions");
			mCollisionStructure = new CollisionStructure(this, xml);
			delete xml;
		}catch(...){
			mCollisionStructure = new CollisionStructure(this);
		}

		Util::Log("Checking for Collision Data with : "+mId+" ...DONE", 2);
	}

	void GameObject::checkForDamagePackage(){
		Util::Log("Checking for Damage Package with : "+mId, 2);
		
		XML* xml = NULL;
		try{
			xml = new XML(mFilename+".xml", "media\\damage");
		}catch(string s){
			if (s.find("XML parsing error") != string::npos){
				throw(s);
			}
		}

		if (xml != NULL){
			mDamagePackage.addFromXml(xml);
			delete xml;
		}

		Util::Log("Checking for Damage Package with : "+mId+" ...DONE", 2);
	}

	void GameObject::initDefaultVariables(){
		mSceneQueryFlags = Shmoulette::SceneQueryFlag::IMMATERIAL;
		mRootEntity = NULL;
		mParentConnector.mGameObject = NULL;
		mParentConnector.mBone = "";
		mParentConnector.mTransferPaintOver = false;
		mSkipHook = false;
		mCastShadows = true;
		mIsPaintedOver = false;
		mType = OBJ_TYPE_LOADED_MESH;
		mEntity = NULL;
		mInertia.x = 0;
		mInertia.y = 0;
		mInertia.z = 0;
		mOldPos.x = 0;
		mOldPos.y = 0;
		mOldPos.z = 0;
		mHooked = false;
		mOwner = NULL;
		//mStreamBuild = false;
		mSkin = "";
		mTestCollisions = true;
		mCollisionStructure=NULL;
		mCollisionForward=NULL;
		mLife = FLT_MAX;
		mChildrenFilename = ".";
		mInitParams = "";
		//mResourcePath = ".";
		mAllowPaintOver = true;
		//setCollisionGroup(COLLGROUP_ALL, false); // This used to be commented, but really, I think it's better that way.  You want collisions, you ask for them.
		mRenderQueueDelta = 0;

		mPythonInstance.pythonInstance = NULL;
		mPythonInstance.hasBuild = false;
		mPythonInstance.hasDestroy = false;
		mPythonInstance.hasHide = false;
		mPythonInstance.hasShow = false;
		mPythonInstance.hasUpdate = false;
		mTransformCoordinator = NULL;
	}
	/*void GameObject::setCollGroup(string str){
		if (str == "none"){
			setCollisionGroup(COLLGROUP_NONE, false);
		}else if (str == "all"){
			setCollisionGroup(COLLGROUP_ALL, false);
		}else if (str == "friendly"){
			setCollisionGroup(COLLGROUP_FRIENDLY, false);
		}else if (str == "enemy"){
			setCollisionGroup(COLLGROUP_ENEMY, false);
		}else if (str == ""){
			setCollisionGroup(COLLGROUP_ALL, false);
		}else{
			setCollisionGroup(atoi(str.c_str()), false);
		}
	}*/


	void GameObject::init(PyObject* pyObject){
		Parent::init(pyObject);
		if (PyList_Check(pyObject) == true && PyList_Size(pyObject) > 1){
			initDefaultVariables();

			PyObject* o = PyList_GetItem(pyObject, 1);
			if (PyList_Check(o) == false){
				throw("GameObject : o should be a list.");
			}
			// Possible combinations
			// Required parameters:
			// model
			// Not used (Py_None)
			// railed/hooked boolean
			// IFF leading to QueryFlags
			// castsshadow
			// children filename
			// python_callback
			// streambuild
			// x number of materialreplace, anim and goanim

			string tempFilename;
			int h = PyList_Size(o);

			for (int i=0;i<h;i++){
				PyObject* elem = PyList_GetItem(o,i);
				if (elem == NULL){
					throw(Core::getSingleton()->getPythonException()+"\n(Error in gameobject tuple for "+mId+")");
				}
				if (elem != Py_None){
					switch (i){
						case 0:
							setType(OBJ_TYPE_LOADED_MESH);
							tempFilename = PyString_AsString(elem);
						break;
						case 1:
							//queryFlagsFromIff(PyString_AsString(elem));
						break;
						case 2:
							mHooked = (elem == Py_True);
						break;
						case 3:
							
							setQueryFlags(queryFlagsFromIff(PyString_AsString(elem)));
						break;
						case 4:
							mCastShadows = (elem == Py_True);
						break;
						case 5:
							mChildrenFilename = PyString_AsString(elem);
						break;
						case 6:
							setPythonCallback(PyString_AsString(elem));
						break;
						case 7:
							//mStreamBuild = (elem == Py_True);
						break;
						default:
							vector<string> v;
							Tokenize(PyString_AsString(elem),v,";");
							if (v[0] == "materialreplace"){
							}else if (v[0] == "anim"){
							}else if (v[0] == "goanim"){
							}else{
								throw ( "Custom gameobject tag not supported : "+v[0]);
							}
						break;
					}
				}
			}
			mSceneNode = mLevelSegment->getSceneManager()->getRootSceneNode()->createChildSceneNode(mId+"_node"+ts(GameElement::getNextIdCounter()));
			if (h!=0) setFilename(tempFilename);


		}else{
			throw("GameObject : PyObject should be an array of at least size 2");
		}
	}

	void GameObject::init(string id, Ogre::SceneNode* parentNode, Ogre::Matrix4 worldPosition){
		Parent::init(id, true);
		initDefaultVariables();
		//Ogre::SceneNode* parent = spawnInfo->getParentSceneNode();
		Ogre::SceneNode* rail = mLevelSegment->getCurrentHook()->getSceneNode();
		Ogre::SceneNode* root = mLevelSegment->getSceneManager()->getRootSceneNode();
		mSceneNode = parentNode->createChildSceneNode(mId+"_node"+ts(GameElement::getNextIdCounter()));
		Ogre::Matrix4 localPos = parentNode->_getFullTransform().inverse()*worldPosition;
		mSceneNode->setPosition(localPos.getTrans());
		mSceneNode->setOrientation(localPos.extractQuaternion());
		mSkipHook = true;
		
		checkForChildren();
		checkForInitScript();
		loadCollisionData();
		checkForDamagePackage();
		//setCollisionGroup(COLLGROUP_ALL,false);
	}

	void GameObject::init(string id, string mesh, bool append_number){
		GameElement::init(id, append_number);
		initDefaultVariables();
		mSceneNode = mLevelSegment->getSceneManager()->getRootSceneNode()->createChildSceneNode(mId+"_node"+ts(GameElement::getNextIdCounter()));
		if (mesh != ""){
			setFilename(mesh);
		}else{
			throw("GameObject requires mesh");
		}
	}
	void GameObject::init(string id, bool append_number){
		GameElement::init(id, append_number);
		MACRO_PROFILER_GET("A", "GameObjectNew")->start();

		initDefaultVariables();
		MACRO_PROFILER_GET("A", "GameObjectNew")->end();
		MACRO_PROFILER_GET("B", "GameObjectNew")->start();
		//mId = id;
		//mIFF = IFF_TARGET_EVERYTHING;
		//mDoNotHook = false;
		/*if (id.find("Player") != string::npos){
			int g=0;
		}*/
		mSceneNode = mLevelSegment->getSceneManager()->getRootSceneNode()->createChildSceneNode(mId+"_node"+ts(GameElement::getNextIdCounter()));
		if (id == "##NOMESH##"){
			mType = OBJ_TYPE_NOMESH;
		}else{
			setFilename(id);
		}
		MACRO_PROFILER_GET("B", "GameObjectNew")->end();
		//setTransformCoordinator("macro:zero_transform");

		//checkForChildren();
		//checkForCollisionData();
		//checkForDamagePackage();
		//setCollisionGroup(COLLGROUP_ALL,false);
	}

	/*GameObject::GameObject(string id, GameObject* childOf):GameElement(id, false){
		initDefaultVariables();
		//mIFF = IFF_TARGET_EVERYTHING;
		//mDoNotHook = true;
		
		mSceneNode = Level::getSingleton()->getSceneManager()->createSceneNode(mId+"_node");

		childOf->getNode()->addChild(mSceneNode);//createChildSceneNode(mId+"_node");
		setCollisionGroup(COLLGROUP_ALL,false);
		checkForChildren();
		checkForCollisionData();
	}*/

	DYN_OBJ_ANIM_TYPE GameObject::animTypeFromString(string str){
		if (str == "disable_loop"){
			return DISABLE_LOOP;
		}else if (str == "enable_loop"){
			return ENABLE_LOOP;
		}else if (str == "goto_and_stop"){
			return GOTO_AND_STOP;
		}else if (str == "goto_and_play"){
			return GOTO_AND_PLAY;
		}else if (str == "reverse_play"){
			return REVERSE_PLAY;
		}else if (str == "stop"){
			return STOP;
		}else if (str == "play"){
			return PLAY;
		}else if (str == "enable"){
			return ENABLE;
		}else if (str == "play_speed"){
			return PLAY_SPEED;
		}else if (str == "disable"){
			return DISABLE;
		}else{
			throw("Anim type "+str+" not supported");
		}

	}

	DYNAMIC_OBJ_ANIM* GameObject::animFromString(string str){
		list<DYNAMIC_OBJ_ANIM>::iterator doi;
		DYNAMIC_OBJ_ANIM* rv = NULL;

		while(rv == NULL){
			for (doi=mAnim.begin();doi!=mAnim.end();doi++){
				if ((*doi).animName == str){
					rv = &(*doi);
				}
			}
			if (rv == NULL){
				DYNAMIC_OBJ_ANIM anim;
				anim.animName = str;
				anim.animState = NULL;
				anim.speedMultiplier = 1;
				anim.status = STOPPED;
				mAnim.push_back(anim);
			}
		}
		return rv;

	}


	void GameObject::addAnim(XML* xml){
		DYNAMIC_OBJ_ANIM_TRIG trig;
		string animName = xml->getString("name");
		if (xml->getError() != XML_OK){
			throw("\"anim\" object requires a \"name\" field ("+xml->getContent()+")");
		}
		trig.anim = animFromString(animName);

		trig.dest=xml->getFloat("dest");
		if (xml->getError() != XML_OK){
			trig.dest=xml->getFloat("frame"); // Old standard
			if (xml->getError() != XML_OK){
				trig.dest=xml->getFloat("value");
				if (xml->getError() != XML_OK){
					trig.dest=0;
				}
			}
		}
		string str = xml->getString("type");
		if (xml->getError() != XML_OK){
			throw("Anim type not specified");
		}
		trig.time = xml->getDouble("time");
		if (xml->getError() != XML_OK){
			trig.time = 0;
		}
		
		trig.type = animTypeFromString(str);
		if (str == "play_speed"){
			trig.dest = xml->getFloat("value");	
		}
		mAnimTrig.push_back(trig);
	}
	SceneQueryFlag::SceneQueryFlag GameObject::queryFlagsFromIff(string iff){
		if (iff == "friend"){
			return SceneQueryFlag::DRONE_FRIENDLY;
		}else if (iff == "foe"){
			return SceneQueryFlag::DRONE_ENEMY;
		}else if (iff == "neutral"){
			return SceneQueryFlag::INANIMATE;
		}else if (iff == "none"){
			return SceneQueryFlag::IMMATERIAL;
		}
		throw ("IFF flag present but unrecognized : "+iff);
	}
	void GameObject::addGoAnim(string goanim, GoAnimParams& p){
		GoAnim::GoAnim* ga = GoAnimFactory::getSingleton()->createGoAnim(this, goanim, p);
		//ga->moveTimes(getTimeOn());
		mGoAnim.push_back(ga);
	}
	/*void GameObject::addGoAnim(string goanim, double time, string params){
		PyObject* po;
		if (params != ""){
			po = PyString_FromString(params.c_str());
		}else{
			po = Py_None;
		}
		mGoAnim.push_back(GoAnimFactory::getSingleton()->createGoAnim(this, goanim, time, po));
	}*/
	void GameObject::addGoAnim(XML* xml){
		GoAnim::GoAnim* ga = GoAnimFactory::getSingleton()->createGoAnim(this, xml);
		//ga->moveTimes(getTimeOn());
		mGoAnim.push_back(ga);
	}

	void GameObject::setPythonCallback(string pc){
			//Util::Log("PYTHON A "+pc, 1);	
			PyObject *pName,*pModule,*pDict,*pClass,*pArgs;

			//Util::Log("PYTHON A2 "+pc, 1);	
			std::vector<string> pca = StringUtil::split(pc, ",", 1);

			//Util::Log("PYTHON A3** "+pca[0], 1);	
			pName = PyString_FromString(pca[0].c_str());
			//Util::Log("PYTHON A4 "+pc, 1);	

			if (pca.size() > 1){
				pArgs = PyString_FromString(pca[1].c_str());
			}
			//Util::Log("PYTHON A5 "+pc, 1);	
			pModule = PyImport_Import(pName);
			//Util::Log("Module", 1);	

			if (pModule == NULL){
				throw(Core::getSingleton()->getPythonException()+"\n(in loading module "+pca[0]+".py)");
			}
			//Util::Log("PYTHON B", 1);	

			pDict = PyModule_GetDict(pModule);
			if (pDict == NULL){
				throw(Core::getSingleton()->getPythonException()+"\n(in loading module dictionary "+pca[0]+")");
			}
			//Util::Log("PYTHON C", 1);	

			// pFunc is also a borrowed reference 

			//mPythonInstance = PyDict_GetItemString(pDict, "update");
			
			PyObject* args;
			if (pca.size() > 1){
				args = PyTuple_New(2);
			}else{
				args = PyTuple_New(1);
			}
			PyObject* objAddress = PyLong_FromLong((long)this);
			PyTuple_SetItem(args, 0, objAddress);
			if (pca.size() > 1){
				PyTuple_SetItem(args, 1, pArgs);
			}

			string garglu = toString(args);
			/*PyObject* args = PyArg_ParseTuple(pArgs, "si", &mId, &num);*/
			pClass = PyDict_GetItemString(pDict, pca[0].c_str());

			if (pClass == NULL){
				throw(Core::getSingleton()->getPythonException()+"\n(in loading module class "+pc+")\n*REMEMBER CLASS NAMES MUST BE THE SAME AS THE PY FILENAME*");
			}
			//Util::Log("PYTHON D", 1);	
			
			mPythonInstance.pythonInstance = PyObject_CallObject(pClass, args);


			if (mPythonInstance.pythonInstance == NULL){
				throw(Core::getSingleton()->getPythonException());

				/*PyObject* type, *value, *traceback;
				PyErr_Fetch(&type, &value, &traceback);
				throw("Python Exception\n"+toString(type)+"\n"+toString(value)+"\n"+toString(traceback));*/
			}
			mPythonInstance.updateString = PyString_FromString("update");
			mPythonInstance.buildString = PyString_FromString("build");
			mPythonInstance.destroyString = PyString_FromString("destroy");
			mPythonInstance.showString = PyString_FromString("show");
			mPythonInstance.hideString = PyString_FromString("hide");
			//Util::Log("PYTHON E", 1);	
			
			PyObject* po;
			if (PyObject_HasAttr(mPythonInstance.pythonInstance, mPythonInstance.updateString)) mPythonInstance.hasUpdate = true;
			if (PyObject_HasAttr(mPythonInstance.pythonInstance, mPythonInstance.buildString)) mPythonInstance.hasBuild = true;
			if (PyObject_HasAttr(mPythonInstance.pythonInstance, mPythonInstance.destroyString)) mPythonInstance.hasDestroy = true;
			if (PyObject_HasAttr(mPythonInstance.pythonInstance, mPythonInstance.showString)) mPythonInstance.hasShow = true;
			if (PyObject_HasAttr(mPythonInstance.pythonInstance, mPythonInstance.hideString)) mPythonInstance.hasHide = true;


			//Util::Log("PYTHON F", 1);	

	}

	void GameObject::init(XML* xml){
		GameElement::init(xml);
		initDefaultVariables();
		string railme = xml->getString("railed");
		if (railme == "1"){
			mHooked = true;
		}

		mRenderQueueDelta = xml->getInt("render_queue_delta");
		if (xml->getError() != XML_OK){
			mRenderQueueDelta = 0;
		}else{
			int g=0;
		}

		//mIFF = IFF_TARGET_EVERYTHING;
		//mDoNotHook = false;

		Util::Log("Putting Hook to NULL");
		

		bool queryAssigned = false;

		string iff = xml->getString("iff");
		if (xml->getError() != XML_OK){
			setQueryFlags(SceneQueryFlag::IMMATERIAL);
		}else{
			setQueryFlags(GameObject::queryFlagsFromIff(iff));
		}

		string cg = xml->getString("collision_group");
		if (!queryAssigned){
			if (xml->getError() == XML_OK){
				if (cg == "none"){
					setQueryFlags(SceneQueryFlag::INANIMATE);
				}else if (cg == "friendly"){
					setQueryFlags(SceneQueryFlag::DRONE_FRIENDLY);
				}else if (cg == "enemy"){
					setQueryFlags(SceneQueryFlag::DRONE_ENEMY);
				}else{
					setQueryFlags(SceneQueryFlag::IMMATERIAL);
				}
			}
		}
		
		/*if (cg == "none"){
			setCollisionGroup(COLLGROUP_NONE, false);
		}else if (cg == "all"){
			setCollisionGroup(COLLGROUP_ALL, false);
		}else if (cg == "friendly"){
			setCollisionGroup(COLLGROUP_FRIENDLY, false);
		}else if (cg == "enemy"){
			setCollisionGroup(COLLGROUP_ENEMY, false);
		}else{
			setCollisionGroup(atoi(cg.c_str()), false);
		}*/
		
		/*if (xml->getString("streambuild") == "1"){
			mStreamBuild = true;
		}*/

		mSceneNode = mLevelSegment->getSceneManager()->getRootSceneNode()->createChildSceneNode(mId+"_node"+ts(GameElement::getNextIdCounter()));

		float tmpX;
		float tmpY;
		float tmpZ;
		float tmpA;

		/*string weapons = xml->getString("weapons");
		if (xml->getError() == XML_OK){
			DroneDBE* dbe = Core::getSingleton()->getDroneDB()->entry(weapons);
			list<WeaponInfo*> wl = dbe->getWeaponInfo();
			for (list<WeaponInfo*>::iterator it = wl.begin();it!=wl.end();it++){
				Weapon* w = (*it)->spawn(this, WeaponSupply::getDefault());
				mWeapon.push_back(w);
			}
		}*/

		string type = xml->getString("model");

		if (xml->getError() == XML_OK || type == ""){
			if (type == "BOX"){
				setType(OBJ_TYPE_BOX);
			}else if (type == "CUBE"){
				setType(OBJ_TYPE_CUBE);
			}else{
				mChildrenFilename = xml->getString("children");
				if (xml->getError() != XML_OK){
					mChildrenFilename = ".";
				}
				mInitParams = xml->getString("init_param");
				if (xml->getError() != XML_OK){
					mInitParams = "";
				}else{
				}
				
				/*mResourcePath = xml->getString("resource_path");
				if (xml->getError() != XML_OK){
					mResourcePath = ".";
				}*/
				setType(OBJ_TYPE_LOADED_MESH);
				setFilename(type);
			}
		}
		Util::Log("PREPYTHON A "+mId, 1);	

		string pc = xml->getString("python_callback");
		if (xml->getError() != XML_OK){
			pc = xml->getString("python");
		}
		if (xml->getError() == XML_OK){
			setPythonCallback(pc);
		}
		Util::Log("POSTPYTHON A", 1);	
		string transform_c = xml->getString("transform_c");
		if (xml->getError() == XML_OK){
			setTransformCoordinator(transform_c);
		}else{
			setTransformCoordinator("macro:zero_transform");
		}

		string dummy = xml->getString("castshadows");
		if (xml->getError() == XML_OK){
			throw("XML parameter castshadows no longer supported");
		}

		int tmpShadows = xml->getInt("shadows");
		if (xml->getError() == XML_OK){
			if (tmpShadows == 0) mCastShadows = false;
		}

		string cFBS = xml->getString("allow_paintover");
		if (xml->getError() == XML_OK){
			mAllowPaintOver = boolFromString(cFBS);
		}
		

		Shmoulette::XML childrenXML = xml->extractElem("##anything##>children");
		if (xml->getError() == XML_OK){
			addChildren(&childrenXML);
		}

		Shmoulette::XMLIterator it(xml);
		it.setElemName("##anything##>materialreplace");
		it.gotoZero();
		while(it.gotoNext()!=false){
			GAME_OBJECT_MAT_REPLACEMENTS rp;

			XML xml = it.getElem();
			rp.time = xml.getDouble("time");
			if (xml.getError() != XML_OK) rp.time = 0;

			rp.replace = xml.getString("replace");
			if (xml.getError() != XML_OK){
				throw("materialreplace tag requires 'replace' attribute.");
			}
			rp.with = xml.getString("with");
			if (xml.getError() != XML_OK){
				throw("materialreplace tag requires 'with' attribute.");
			}
			mMatReplace.push_back(rp);
		}
		{
		XMLIterator it(xml);
		it.setElemName("##anything##>anim");
		it.gotoZero();
		while(it.gotoNext() != false){
			addAnim(&it.getElem());

			//string anim_state = 
			
		}
		}
		{
			XMLIterator it(xml);
			it.setElemName("##anything##>goanim");
			it.gotoZero();
			while(it.gotoNext() != false){
				XML* childXml = &it.getElem();
				string objName = childXml->getString("object");
				if (childXml->getError() == XML_OK){
					getChild(objName)->addGoAnim(&it.getElem());
				}else{
					addGoAnim(&it.getElem());
				}
			}
		}
	}

	/*void GameObject::setCollisionGroup(COLLGROUP cg, bool setAllChildren){
		mCollGroup = cg;
		if (setAllChildren){
			list<GAME_OBJECT_CHILD>::iterator it;
			if (mEntity){
				if (cg == COLLGROUP_FRIENDLY){
					mEntity->setQueryFlags(SceneQueryFlag::DRONE_FRIENDLY);
				}else if (cg == COLLGROUP_ENEMY){
					mEntity->setQueryFlags(SceneQueryFlag::DRONE_ENEMY);
				}
			}
			for (it=mChild.begin();it!=mChild.end();it++){
				(*it).gameObject->setCollisionGroup(cg, true);
			}
		}
	}*/


	GameObject::~GameObject(){
		delete mCollisionStructure;
		{list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			delete(*it).gameObject;
		}}

		/*
			{list<Ogre::Entity*>::iterator it;
		for (it=mChildEntity.begin();it!=mChildEntity.end();it++){
			mLevelSegment->getSceneManager()->destroyEntity(*it);
		}}

		list<Ogre::SceneNode*>::iterator it;
		for (it=mChildSceneNode.begin();it!=mChildSceneNode.end();it++){
			(*it)->getParentSceneNode()->removeChild(*it);
		}*/
		if (mSceneNode != NULL){
			Ogre::SceneNode* sparent;
			sparent = mSceneNode->getParentSceneNode();
			if (sparent != NULL){
				sparent->removeChild(mSceneNode);
			}
			mLevelSegment->getSceneManager()->destroySceneNode(mSceneNode);
			mSceneNode = NULL;
		}

		/*mLevelSegment->getSceneManager()->destroySceneNode(mSceneNode->getName());
		for (it=mChildSceneNode.begin();it!=mChildSceneNode.end();it++){
			mLevelSegment->getSceneManager()->destroySceneNode((*it)->getName());
		}*/
	}



	/*COLLGROUP GameObject::getCollisionGroup(bool includeChildren, bool ANDChildren){
		return mCollGroup;
	}*/


	Ogre::SceneNode* GameObject::getNewChildNode(string _name){
		if (!mIsBuild){
			build();
		}
		return mSceneNode->createChildSceneNode(_name);
	}

	void GameObject::orphan(){
		Util::Log("Orphaning", 6);
		if (mSceneNode->getParentSceneNode() != NULL){
			mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
		}
	}
	void GameObject::attach(GameObject* newChild){
		Util::Log("Attaching to parent scene node");

		if (newChild->mParentConnector.mBone==""){
			//throw("Parent Connectors REQUIRE bones.  Don't ask.  They just do.  Now go back in Blender, put a bone in, and that's it.  Goodbye");
			Ogre::Node* sn = newChild->mSceneNode->getParent();
			if (sn != NULL){
				sn->removeChild(newChild->mSceneNode);
			}
			if (this->isSceneNode()){
				mSceneNode->addChild(newChild->mSceneNode);
				if (newChild->getType() != OBJ_TYPE_CAMERA && newChild->getType() != OBJ_TYPE_RAIL){
					newChild->mSceneNode->attachObject(newChild->getMovableObject());
				}
			}else{
				throw("If no bone parent connector, then parent itself must NOT be attached to a bone");
			}

			//mParentConnector.mGameObject->getNode()->addChild(mSceneNode);
		}else{
			Ogre::TagPoint* tp;
			string bone = newChild->getParentConnector().mBone;
			tp = getEntity()->attachObjectToBone(bone, newChild->mEntity);

			newChild->resetTransforms();
			//newChild->getNode()->setPosition(newChild->mSceneNode->getPosition());
			//newChild->getNode()->setOrientation(newChild->mSceneNode->getOrientation());
			
			
			/*if (newChild->mSceneNode != NULL){
				Ogre::Node* sn = newChild->mSceneNode->getParent();
				if (sn != NULL){
					sn->removeChild(newChild->getNode());
				}
			}*/

			/*Ogre::Node* sn = newChild->getNode()->getParent();
			if (sn != NULL){
					sn->removeChild(newChild->getNode());
			}*/
			//Level::getSingleton()->getSceneManager()->destroySceneNode(newChild->mSceneNode->getName());
			//newChild->mSceneNode = NULL;

		}
		//mEntity->getParentNode()->setScale(scale);
		//mBoneNodeMode =true;


	/*	Util::Log("Attaching...", 6);
		newchild->orphan();
		mSceneNode->addChild(newchild->getSceneNode());
		Util::Log("...Done", 6);*/
	}

	void GameObject::attach(Ogre::SceneNode* newChild){
		if (newChild->getParentSceneNode() != NULL){
			newChild->getParentSceneNode()->removeChild(newChild);
		}
		mSceneNode->addChild(newChild);
	}


	void GameObject::detach(GameObject* newchild){
		mSceneNode->removeChild(newchild->getSceneNode());
	}

	void GameObject::hook(){
		Ogre::SceneNode* sn = mSceneNode->getParentSceneNode();
		if (sn != NULL){
			mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
		}

		if (mHooked){
			mLevelSegment->getContainer()->addChild(mSceneNode);
		}else{
			mLevelSegment->getSceneManager()->getRootSceneNode()->addChild(mSceneNode);
		}
	}

	#define OLD_BONE_MODE

	void GameObject::attachToBone(Ogre::Entity* ent, string bone){
		Ogre::Vector3 scale = mSceneNode->getScale();
		//scale.x = 20;
		//scale.y = 20;
		//scale.z = 20;
		if (mEntity->getParentSceneNode() != NULL){
			mEntity->getParentSceneNode()->detachObject(mEntity);
		}
		
		//Ogre::Quaternion q((Ogre::Radian)(0-(Ogre::Math::PI/2)), Vector3(1,0,0));
		
	#ifndef OLD_BONE_MODE
		int handle = ent->getSkeleton()->getNumBones();
		Ogre::Bone* newBone = ent->getSkeleton()->getBone(bone)->createChild(handle);
		ent->attachObjectToBone(newBone->getName(), mEntity);
	#else
		ent->attachObjectToBone(bone, mEntity);
	#endif
		mEntity->getParentNode()->setScale(scale);

		if (mSceneNode->getParentSceneNode() != NULL){
			mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
			//Level::getSingleton()->getSceneManager()->destroySceneNode(mSceneNode->getName());
			//mSceneNode = NULL;
			// Leave the node floating.  It is assumed a GameObject has a scenenode, even if it floats in nothingness2
		}
	//	mBoneNodeMode =true;
	}

	void GameObject::setHooked(bool hooked, bool convertSceneNodePositionToLocal){
		using namespace Ogre;
		Util::Log("Setting Hook");
		mHooked = hooked;
		if (hooked == true){
			if (convertSceneNodePositionToLocal){
				// God I'm screwed

				Vector3 hookVector = mLevelSegment->getContainer()->_getDerivedPosition();
				Vector3 objectVector = mSceneNode->_getDerivedPosition();
				Vector3 deltaVector = objectVector-hookVector;

				//Quaternion hookOrientation = hook->getSceneNode()->_getDerivedOrientation();
				Quaternion objectOrientation = mSceneNode->_getDerivedOrientation();
				Vector3 x, y, z;
				objectOrientation.ToAxes(x, y, z);

				//Quaternion deltaOrientation = objectOrientation.Dot(hookOrientation);

				this->hook();
				mSceneNode->setPosition(0,0,0);
				mSceneNode->translate(deltaVector, Ogre::Node::TS_WORLD);
				//mSceneNode->resetOrientation();
				mSceneNode->setDirection(0-z, Ogre::Node::TS_WORLD);
				

				//mSceneNode->resetOrientation();
				//mSceneNode->setOrientation(deltaOrientation);


			}else{
				hook();
			}
		}

	};
/*	GameObject::GameObject(vector<string> params):Parent(params){
	}*/
	void GameObject::resetTransforms(){

	}

	

	GameObject* GameObject::createChild(string type, string bone, string label, string param){
		GAME_OBJECT_CHILD c;
		c.bone = bone;
		if (type=="static"){
			c.gameObject = new GameObject();
			c.gameObject->init(this->getId()+"_"+label, param, false);
			//c.gameObject->setFilename(param);
			//c.gameObject->disableCollisionTest();
			//c.gameObject->moveTimes(getTimeOn());
		}else if (type == "billboard"){
			//c.gameObject = new Shmoulette::Billboard(param);
			c.gameObject->setCollisionForward(this);
		}
		if (type == "goanim"){
			int g =0;
		}
		c.gameObject->setHooked(false);
		c.gameObject->setParentConnector(this, c.bone, false);
		c.gameObject->setCollisionForward(this);
		//c.gameObject->moveTimes(getTimeOn());
		mChild.push_back(c);
		return c.gameObject;
	}

	void GameObject::moveTimes(double time){
		Parent::moveTimes(time);
		{
			list<GAME_OBJECT_CHILD>::iterator it;
			for (it=mChild.begin();it!=mChild.end();it++){
				(*it).gameObject->moveTimes(time);
			}
		}
	}

	void GameObject::capTimes(double time){
		Parent::capTimes(time);
		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			(*it).gameObject->capTimes(time);
		}
	}

	/*void GameObject::setParentConnector(Ogre::Entity* ent, string bone){
		mParentConnector.mEntity = ent;
		mParentConnector.bone = bone;

	}*/

	void GameObject::destroy(){
		if (isOn()){
			hide();
		}

		if (mPythonInstance.hasDestroy){
			PyObject* c=PyObject_CallMethodObjArgs(mPythonInstance.pythonInstance, mPythonInstance.destroyString,NULL);
			if (c == NULL){
				throw("Python Exception on Destroy "+mId+" : "+Core::getSingleton()->getPythonException());
			}
		}

		list<GAME_OBJECT_CHILD>::iterator it;
		//Ogre::Node* n = mSceneNode->getChild(0);
		if (mParentConnector.mBone == ""){
			mSceneNode->detachAllObjects();
		}

		{
			vector<Weapon*>::iterator it;
			for (it = mWeapon.begin();it!=mWeapon.end();it++){
				(*it)->destroy();
			}
		}{
			for (it=mChild.begin();it!=mChild.end();it++){
				if ((*it).gameObject->isBuild()){
					(*it).gameObject->destroy();
				}
			}
		}{
			list<DYNAMIC_OBJ_ANIM_TRIG>::iterator it;
			for (it=mAnimTrig.begin();it!=mAnimTrig.end();it++){
				(*it).anim->animState = NULL;
			}
		}{
			list<GoAnim::GoAnim*>::iterator it;
			for (it = mGoAnim.begin();it!=mGoAnim.end();it++){
				(*it)->destroy();
			}
		}


		if (mEntity != NULL){
			//mAnim.clear();
			mLevelSegment->getSceneManager()->destroyEntity(mEntity);
			mEntity = NULL;
		}
		Parent::destroy();
	}

	void GameObject::materialReplace(string from, string to){
		
		//mEntity->get>setMaterialName(to);
		if (mEntity == NULL){
			throw ("Material Replace tag on non-mesh object WTF? : "+getId());
		}
		if (from == ""){
			mEntity->setMaterialName(to);
		}else{
			int j=mEntity->getNumManualLodLevels();
			for (int i=-1;i<j;i++){
				Ogre::Entity* lodEnt;
				if (i > -1){
					lodEnt  = mEntity->getManualLodLevel(i);
				}else{
					lodEnt  = mEntity;
				}
				for (int k=0;k<lodEnt->getNumSubEntities();k++){
					Ogre::SubEntity* subEnt = lodEnt->getSubEntity(k);
					if (subEnt->getMaterialName() == from){
						subEnt->setMaterialName(to);
					}
				}
				
				//subEnt->setMaterialName(to);
			}
		}
	}
	void GameObject::overrideQueryFlags(){

	}

	void GameObject::setQueryFlags(SceneQueryFlag::SceneQueryFlag flag){
		mSceneQueryFlags = flag;
		if (mEntity){
			mEntity->setQueryFlags(flag);
		}
		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			//if (!(*it).gameObject->isBuild()){
			if ((*it).gameObject->getQueryFlags() != SceneQueryFlag::IMMATERIAL){
				(*it).gameObject->setQueryFlags(flag);
			}
		}
	}
	void GameObject::setTransformCoordinator(Coordinator::TransformCoordinator* newCoord){
		if (mTransformCoordinator){
			delete mTransformCoordinator;
		}
		mTransformCoordinator = newCoord;
	}

	void GameObject::setTransformCoordinator(string s){
		if (mTransformCoordinator){
			delete mTransformCoordinator;
		}
		mTransformCoordinator = dynamic_cast<Coordinator::TransformCoordinator*>(Coordinator::CoordinatorFactory::getSingleton()->get(s));
	}

	void GameObject::updateFromCoordinator(bool force){
		if (mTransformCoordinator){
			if (mTransformCoordinator->hasChanged() || force){
				Coordinator::Transform t = mTransformCoordinator->getValue(timeAlive());
				getNode()->setPosition(t.mPos);
				getNode()->setOrientation(t.mRot);
			}
		}
	}

	void GameObject::build(){
		Util::Log("Building : "+mId, 1);
		/*if (mFilename.find("cap")!=string::npos){
			int h=0;
		}*/
		if (mParentConnector.mGameObject == NULL && !mSkipHook){
			hook();
		}
		//if (!mDoNotHook){
			//hook();
		//}
		switch (mType){
			case OBJ_TYPE_LOADED_MESH:{
				Util::Log("Loading Mesh : "+mId, 1);
				// If the mesh already exists, do not reload it, or you will create a "manual LOD Entity already exists"-type error
				MACRO_PROFILER_GET(mId+"_Entity","BuildGameObject")->start();
				
				MeshPtr pMesh;
				if (mLodInfo.size() == 0){
#if 0					
					if (MeshManager::getSingleton().getByName(mFilename+".mesh").isNull()){ 
						pMesh = MeshManager::getSingleton().load(mFilename+".mesh",
							ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,    
							HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
							HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
							true, true);
							unsigned short src, dest;
							//try{
							//if (!pMesh->suggestTangentVectorBuildParams(Ogre::VertexElementSemantic::VES_NORMAL, src, dest))
								//{
									//pMesh->buildTangentVectors(Ogre::VertexElementSemantic::VES_NORMAL, src, dest);
								//}
							//} catch( Ogre::Exception& e ) {
								//throw("GLUGLU");
							//}


					}
#endif
					/*if (mRootEntity != NULL){
						mEntity = mRootEntity->clone(mId);
					}else{*/
					MACRO_PROFILER_GET(mId+"_EntityPerSay","BuildGameObject")->start();
						mEntity = mLevelSegment->getSceneManager()->createEntity(mId, mFilename+".mesh");
					MACRO_PROFILER_GET(mId+"_EntityPerSay","BuildGameObject")->end();
					//}
					int g = mEntity->getRenderQueueGroup();
					int h = mLevelSegment->getSceneManager()->getRenderQueue()->getDefaultQueueGroup();

					mEntity->setRenderQueueGroup(mLevelSegment->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+mRenderQueueDelta);
				}else{
					if (MeshManager::getSingleton().getByName(mLodInfo.begin()->meshName+".mesh").isNull()){ 
						for (vector<LodInfo>::iterator it = mLodInfo.begin();it != mLodInfo.end();it++){
							if (it == mLodInfo.begin()){
								pMesh = MeshManager::getSingleton().load((*it).meshName+".mesh",
									ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,    
									HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
									HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
									true, true);
							}else{
								pMesh->createManualLodLevel((*it).distance, (*it).meshName+".mesh");
							}
						}
						unsigned short src, dest;
						//try{
							//if (!pMesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
							//{
								//pMesh->buildTangentVectors(VES_TANGENT, src, dest);
							//}
						//} catch( Ogre::Exception& e ) {
							//throw("GLUGLU");
						//}
					}
					mEntity = mLevelSegment->getSceneManager()->createEntity(mId, mLodInfo.begin()->meshName+".mesh");
					mEntity->setRenderQueueGroup(mLevelSegment->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+mRenderQueueDelta);
				}
				MACRO_PROFILER_GET(mId+"_Entity","BuildGameObject")->end();
				MACRO_PROFILER_GET(mId+"_Flag","BuildGameObject")->start();

				mEntity->setQueryFlags(mSceneQueryFlags);

				
	#if 0
				if (MeshManager::getSingleton().getByName(mFilename+".mesh").isNull()){ 
					MeshPtr pMesh = MeshManager::getSingleton().load(mFilename+".mesh",
						ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,    
						HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
						HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
						true, true);
		
					/*Ogre::Mesh::LodDistanceList l;
					l.push_back(750);
					l.push_back(1500);
					l.push_back(7500);
					l.push_back(15000);
					
					pMesh->generateLodLevels(l,ProgressiveMesh::VRQ_PROPORTIONAL, .75);*/
					
					if (mFilename.find("_HD") != std::string::npos){
						string coreName = mFilename.substr(0,mFilename.length()-3);

						pMesh->createManualLodLevel(750, coreName+"_MD.mesh");
						pMesh->createManualLodLevel(1500, coreName+"_LD.mesh");
						pMesh->createManualLodLevel(2000, coreName+"_ND.mesh");
					}
					
					// If a file of the same name but with the extra _ld exists, assume it is the same model with a lower LoD
					/*if (File::exists(mFilename+"_ld.mesh", "media\\models\\"+this->mResourcePath)){
						MeshManager::getSingleton().load(mFilename+"_ld.mesh",
							ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,    
							HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
							HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
							true, true); //so we can still read it
						pMesh->createManualLodLevel(300, mFilename+"_ld.mesh");
					}*/
					
					// Build tangent vectors, all our meshes use only 1 texture coordset 
					
					unsigned short src, dest;
					try{
						if (!pMesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
						{
							pMesh->buildTangentVectors(VES_TANGENT, src, dest);
						}
					} catch( Ogre::Exception& e ) {
					}
				}
	#endif

				Util::Log("Creating Entity : "+mFilename);

				/*if (mEntity->getNumManualLodLevels() != 0){
					mLevelSegment->getSceneManager()->addLodListener(this);
				}*/
				/*if (mEntity->getSkeleton() != NULL){
					int totalLod = mEntity->getNumManualLodLevels();
					if (totalLod > 0){
						
						mEntity->setListener(this);
						for (int i=0;i<totalLod;i++){
							mEntity->getManualLodLevel(i)->shareSkeletonInstanceWith(mEntity);//>getMesh()->setSkeletonName(mEntity->getSkeleton()->getName());
						}
					}
				}*/
				
				
				//mEntity->setQueryFlags(SceneQueryFlag::INANIMATE); // By default, every object is inanimate
				MACRO_PROFILER_GET(mId+"_Flag","BuildGameObject")->end();
				MACRO_PROFILER_GET(mId+"_Mat","BuildGameObject")->start();

				if (mSkin != ""){
					mEntity->setMaterialName(mSkin);
				}
				{
					vector<GAME_OBJECT_MAT_REPLACEMENTS>::iterator it;
					for (it = mMatReplace.begin();it != mMatReplace.end();it++){
						if ((*it).time == 0){
							materialReplace((*it).replace, (*it).with);
						}
					}
				}
				MACRO_PROFILER_GET(mId+"_Mat","BuildGameObject")->end();
				MACRO_PROFILER_GET(mId+"_Coll","BuildGameObject")->start();
				Util::Log("Checking for Collision");

				{
					Shmoulette::File file("gencoll.txt", "models\\"+mFilename);
					if (file.error() == FILE_OK){
						Shmoulette::File file2(mFilename+"_collGEN.xml", "models\\"+mFilename, FILE_WRITE);
						file2.write("<?xml version=\"1.0\" standalone=\"yes\"?>\r\n");
						file2.write("<collision>\r\n");
						{
							/*CollisionElement *ce;
							ce = new CollisionElement(this);
							ce->build();
							Ogre::AxisAlignedBox* aabb;
							aabb = ce->getRootBox();
							file2.write(
								(string)"<elem type=\"box\" "+
								"x1=\""+ts(aabb->getMinimum().x)+"\" "+
								"y1=\""+ts(aabb->getMinimum().y)+"\" "+
								"z1=\""+ts(aabb->getMinimum().z)+"\" "+
								"x2=\""+ts(aabb->getMaximum().x)+"\" "+
								"y2=\""+ts(aabb->getMaximum().y)+"\" "+
								"z2=\""+ts(aabb->getMaximum().z)+"\"/>\r\n"
								);

							delete ce;*/
						}



						
						list<CollisionElement*> cel;

						Ogre::Skeleton::BoneIterator it = mEntity->getSkeleton()->getBoneIterator();
						while(it.hasMoreElements()){
							CollisionElement *ce;
							Ogre::Bone* bone = it.getNext();
							/*ce = new CollisionElement(this, bone->getName(), "auto_box");
							ce->build();
							Ogre::AxisAlignedBox* aabb;
							aabb = ce->getRootBox();
							file2.write(
								"<elem bone=\""+
								bone->getName()+"\" type=\"box\" "+
								"x1=\""+ts(aabb->getMinimum().x)+"\" "+
								"y1=\""+ts(aabb->getMinimum().y)+"\" "+
								"z1=\""+ts(aabb->getMinimum().z)+"\" "+
								"x2=\""+ts(aabb->getMaximum().x)+"\" "+
								"y2=\""+ts(aabb->getMaximum().y)+"\" "+
								"z2=\""+ts(aabb->getMaximum().z)+"\"/>\r\n"
								);

							delete ce;*/
						}
						file2.write("</collision>");
					}
				}
				MACRO_PROFILER_GET(mId+"_Coll","BuildGameObject")->end();
				MACRO_PROFILER_GET(mId+"_BB","BuildGameObject")->start();

				Util::Log("Creating Default Bounding Box");

				mBBMin = mEntity->getBoundingBox().getMinimum();
				mBBMax = mEntity->getBoundingBox().getMaximum();

				Util::Log("Finding Root Bone");

				if (mEntity->getSkeleton() != NULL){
					try{
						mRootBone = mEntity->getSkeleton()->getBone("root");
					}catch( Ogre::Exception& e ) {
						mRootBone = NULL;
					}
				}else{
					mRootBone = NULL;
				}
				Util::Log("Setting Shadow");

				mEntity->setCastShadows(mCastShadows);
				if (mParentConnector.mGameObject == NULL){
					mSceneNode->attachObject(mEntity);
				}else{
					mParentConnector.mGameObject->attach(this);
				}
				//mSceneNode->setPosition(mPos.x, mPos.y, mPos.z);
				
				Util::Log("Checking XML for additional info");

				char XML_filename[MAX_PATH];
				sprintf(XML_filename, "%s.xml", (char*)mFilename.c_str());
				/*File* file;
				
				file = new File(XML_filename,"models\\"+mFilename);
				if (file->error() != FILE_OK){		
					delete file;
					file = NULL;
				}else{
					XML* xml;
					xml = new XML();
					xml->setContent((char*)file->content());
					mSceneNode->setScale(
						Vector3(
							xml->getFloat("pivot>scl", "x"), 
							xml->getFloat("pivot>scl", "y"), 
							xml->getFloat("pivot>scl", "z")
							));
					delete file;
				}*/

				/*try{
					XML* xml = new XML(XML_filename,"models\\"+mFilename);
				}catch(...){
				}*/
				MACRO_PROFILER_GET(mId+"_BB","BuildGameObject")->end();
			
				Util::Log("Done Building Mesh");}
			break;
			case OBJ_TYPE_RAIL:
			break;
			case OBJ_TYPE_CONTAINER:
			break;
			case OBJ_TYPE_CLOUDZONE:
			break;
			case OBJ_TYPE_CAMERA:
				if (mParentConnector.mGameObject != NULL){
					mParentConnector.mGameObject->attach(this);
				}

			break;
			case OBJ_TYPE_LIGHT:
			break;
			case OBJ_TYPE_EMPTY:
			break;
		}
		
		//Util::Log("Moving to parent");

		/*for (it=mChildSceneNode.begin();it!=mChildSceneNode.end();it++){
			(*it)->setVisible(false);
		}*/

		/*{
		Ogre::AxisAlignedBox ab;
		ab.setNull();
		for (int i=0;i<mEntity->getSkeleton()->getNumBones();i++){
			mSectionBox.push_back(ab);
		}

		Ogre::Mesh::BoneAssignmentIterator it = mEntity->getMesh()->getBoneAssignmentIterator();
		while(it.hasMoreElements()){
			Ogre::VertexBoneAssignment vbal;
			vbal = it.getNext();

			int boxIndex = vbal.boneIndex;
			Ogre::Vector3 vpos;
			//vpos = //??? (mEntity->getMesh()->get___SOMETHING?(vbal.vertexIndex));

			ab.setExtents(vpos, vpos);
			mSectionBox[boxIndex].merge(ab);
		};
		}*/

			//if (!mStreamBuild){
		
		MACRO_PROFILER_GET(mId+"_Children","BuildGameObject")->start();

		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			if (!(*it).gameObject->isBuild()){
				if ((*it).gameObject->shouldBeBuild(timeAlive())){
					(*it).gameObject->build();
				}
			}
			//(*it).gameObject->attachToBone(mEntity, (*it).bone);
		}
		
		{
			vector<Weapon*>::iterator it;
			for (it = mWeapon.begin();it!=mWeapon.end();it++){
				(*it)->build();
				//(*it)->fire();
			}
		}

		MACRO_PROFILER_GET(mId+"_Children","BuildGameObject")->end();
		//}

		if (mCollisionStructure){
			mCollisionStructure->build();
		}
		Parent::build();
		if (mPythonInstance.hasBuild){
			PyObject* c=PyObject_CallMethodObjArgs(mPythonInstance.pythonInstance, mPythonInstance.buildString,NULL);
			if (c == NULL){
				throw("Python Exception on Build "+mId+" : "+Core::getSingleton()->getPythonException());
			}
		}

		Util::Log("Exiting ::build",1);
	}

	/*bool GameObject::prequeueMovableObjectLodChanged(const Ogre::MovableObjectLodChangedEvent& evt){
		if (evt.movableObject == this->mEntity){
			int g=0;
			return true;
		}
		return false;
	}
	void GameObject::postqueueMovableObjectLodChanged(const Ogre::MovableObjectLodChangedEvent& evt){
		int g=0;
	}*/

	Ogre::Entity* GameObject::entityFromLod(int ndx){
		if (ndx == 0){
			return mEntity;
		}else{
			return mEntity->getManualLodLevel(ndx-1);
		}


	}

	void GameObject::setRenderQueueGroup(int group, bool includeChildren){
		mEntity->setRenderQueueGroup(group);
		if (includeChildren){
			for (list<GAME_OBJECT_CHILD>::iterator it = mChild.begin();it != mChild.end();it++){
				(*it).gameObject->setRenderQueueGroup(group, true);
			}
		}
	}


	/*void GameObject::entityMeshLodChanged(const Ogre::EntityMeshLodChangedEvent& evt){
		if (evt.entity == mEntity){
			list<GAME_OBJECT_CHILD>::iterator it;
			Ogre::Entity* oldEntity = entityFromLod(evt.previousLodIndex);
			Ogre::Entity* newEntity = entityFromLod(evt.newLodIndex);


			Entity::ChildObjectListIterator colit = newEntity->getAttachedObjectIterator();
			if (colit.hasMoreElements()){
				
				int g=0;
			}
			
			for (it=mChild.begin();it!=mChild.end();it++){
				Ogre::MovableObject* childEntity = (*it).gameObject->getMovableObject();
				Ogre::Node* childNode =	(*it).gameObject->getNode(); 
				if ((*it).bone != ""){
					childEntity->detatchFromParent();
					newEntity->attachObjectToBone(
						(*it).bone, 
						childEntity,
						childNode->getOrientation(),
						childNode->getPosition()
						);
					
				}
			}
		}
	}
	void GameObject::entityMaterialLodChanged(const Ogre::EntityMaterialLodChangedEvent& evt){
		int g=0;
	}*/

	void GameObject::activate(){}

	void GameObject::show(){
		if (mType != OBJ_TYPE_RAIL && mType != OBJ_TYPE_CAMERA && mType != OBJ_TYPE_PARTICLE){
			if (mSceneNode)
				mSceneNode->setVisible(true,false);

			if (mEntity)
				mEntity->setVisible(true);

			/*list<Ogre::SceneNode*>::iterator it;
			for (it=mChildSceneNode.begin();it!=mChildSceneNode.end();it++){
				(*it)->setVisible(true);
			}*/
			list<GAME_OBJECT_CHILD>::iterator it;
			for (it=mChild.begin();it!=mChild.end();it++){
				if ((*it).gameObject->isBuild()){
					if ((*it).gameObject->shouldBeOn(timeAlive())){
						(*it).gameObject->show();
					}
				}
			}

		}
		updateFromCoordinator(true);
		Parent::show();
	}

	void GameObject::setVisible(bool vis){
		if (mSceneNode)
			mSceneNode->setVisible(vis,false);
		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			(*it).gameObject->setVisible(vis);
		}
	}

	void GameObject::hide(){
		Parent::hide();
		if (mType != OBJ_TYPE_RAIL && mType != OBJ_TYPE_CAMERA && mType != OBJ_TYPE_PARTICLE){
			if (mSceneNode)
				mSceneNode->setVisible(false,false);
			
			if (mEntity)
				mEntity->setVisible(false);
			/*list<Ogre::SceneNode*>::iterator it;
			for (it=mChildSceneNode.begin();it!=mChildSceneNode.end();it++){
				(*it)->setVisible(false);
			}*/
			list<GAME_OBJECT_CHILD>::iterator it;
			for (it=mChild.begin();it!=mChild.end();it++){
				if ((*it).gameObject->isOn()){
					(*it).gameObject->hide();
				}
			}

		}
	}

	void GameObject::addMaterialReplace(string from, string to, float time){
		GAME_OBJECT_MAT_REPLACEMENTS mr;
		mr.replace = from;
		mr.with = to;
		mr.time = time;
		mMatReplace.push_back(mr);
	}


	PyObject* GameObject::addGoAnim(PyObject *self, PyObject* args){
		
		Shmoulette::PythonExtra::ValidateTuple("Go Anim Tuple Format : (game object pointer, type, time*, [[Params for GameElement], [Params for the actual GoAnim]])\n", args, "lsfx");
		GameObject* go = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));
		string type = PyString_AsString(PyTuple_GetItem(args,1));
		double time = PyFloat_AsDouble(PyTuple_GetItem(args,2));
		PyObject* params = PyTuple_GetItem(args,3);

		if (PyList_Size(params) != 2){
			throw("Go Anim Tuple Format : (game object pointer, type, time*, [[Params for GameElement], [Params for the actual GoAnim]])");
		}


		
		Shmoulette::GoAnim::GoAnim* ga = GoAnimFactory::getSingleton()->createGoAnim(go, type, time, params);
		//ga->moveTimes(go->getTimeOn());
		go->mGoAnim.push_back(ga);



		/*ameObject* ptr;
		PyArg_ParseTuple(args, "l", &ptr);
		ptr->attach(Level::getSingleton()->getCurrentSegment()->getCurrentHook());
		ptr->attach((Shmoulette::GameObject*)Level::getSingleton()->getCurrentSegment()->getActiveCamera());
		Le8vel::getSingleton()->getCurrentSegment()->forceSecondUpdate();*/
		Py_RETURN_NONE;
	}

	PyObject* GameObject::addMaterialReplace(PyObject *self, PyObject* args){
		GAME_OBJECT_MAT_REPLACEMENTS mr;
		if (PyTuple_Size(args) > 3){
			PythonExtra::ValidateTuple("MaterialReplace", args, "lssf");
		}else{
			PythonExtra::ValidateTuple("MaterialReplace", args, "lss");
		}
				
		GameObject* go = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));
		mr.replace = PyString_AsString(PyTuple_GetItem(args,1));
		mr.with = PyString_AsString(PyTuple_GetItem(args,2));
		if (PyTuple_Size(args) > 3){
			mr.time = PyFloat_AsDouble(PyTuple_GetItem(args,3));
		}else{
			mr.time = 0;
		}
		

		go->mMatReplace.push_back(mr);
		Py_RETURN_NONE;
	}



	PyObject* GameObject::railToOrbit(PyObject *self, PyObject* args){
		GameObject* ptr;
		PyArg_ParseTuple(args, "l", &ptr);
		ptr->attach(Level::getSingleton()->getCurrentSegment()->getCurrentHook());
		ptr->attach((Shmoulette::GameObject*)Level::getSingleton()->getCurrentSegment()->getActiveCamera());
		Level::getSingleton()->getCurrentSegment()->forceSecondUpdate();
		Py_RETURN_NONE;
	}

	PyObject* GameObject::setVisible(PyObject *self, PyObject* args){
		int g =0;
		string goglubin = toString(args);
		GameObject* ptr;
		bool value;
		PyArg_ParseTuple(args, "lb", &ptr, &value);
		ptr->setVisible(value);
		Py_RETURN_NONE;
	}

	PyObject* GameObject::getChildren(PyObject *self, PyObject* args){
		GameObject* ptr, *gorv;
		char* objName;
		PyArg_ParseTuple(args, "l", &ptr);

		PyObject* rv = PyList_New(0);

		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=ptr->mChild.begin();it!=ptr->mChild.end();it++){
			PyList_Append(rv, PyLong_FromLong((long)(*it).gameObject));
		}

		return rv;
	}

	PyObject* GameObject::addWeapon(PyObject *self, PyObject* args){
		if (PyTuple_Size(args) < 3){
			throw ("addWeapon requires 3 params : gameobject, type, bone");
		}
		GameObject* go = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));
		string type = PyString_AsString(PyTuple_GetItem(args,1));
		string bone = PyString_AsString(PyTuple_GetItem(args,2));
		bool hooked;
		Ogre::Vector3 inertia;
		WeaponSupply* ws;

		/*if (PyTuple_Size(args) > 3){
			hooked = PyTuple_GetItem(args,3) == Py_True;
		}else{
			hooked = false;
		}*/
		/*if (PyTuple_Size(args) > 4){
			inertia.x = PyFloat_AsDouble(PyList_GetItem(PyTuple_GetItem(args,4),0));
			inertia.y = PyFloat_AsDouble(PyList_GetItem(PyTuple_GetItem(args,4),1));
			inertia.z = PyFloat_AsDouble(PyList_GetItem(PyTuple_GetItem(args,4),2));
		}else{
			inertia.x = FLT_MAX;
		}*/

		if (PyTuple_Size(args) > 4){
			throw("Weapon Supplies not supported by python weapon creation yet");
		}else{
			ws = WeaponSupply::getDefault();
		}

		Weapon* weap = go->addWeapon(type, bone, ws);
		Py_RETURN_NONE;
	}


	PyObject* GameObject::getChild(PyObject *self, PyObject* args){
		GameObject* ptr, *gorv;
		char* objName;
		PyArg_ParseTuple(args, "ls", &ptr, &objName);
		if (ptr == (GameObject*)0xcccccccc){
			throw("sh_gameobject calls must always have the pointer to the object as first param");
		}
		gorv = ptr->getChild(objName);	
		
		PyObject* rv = PyLong_FromLong((long)gorv);

		return rv;
	}

	PyObject* GameObject::setRenderQueue(PyObject *self, PyObject* args){
		GameObject* ptr = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));
		PyObject* list = PyTuple_GetItem(args, 1);

		
		if (PyList_GetItem(list, 1) != Py_False){
			ptr->setRenderQueueGroup(PyInt_AsLong(PyList_GetItem(list, 0)),true);
		}else{
			ptr->setRenderQueueGroup(PyInt_AsLong(PyList_GetItem(list, 0)),false);
		}

		Py_RETURN_NONE;
	}

	PyObject* GameObject::setTimes(PyObject *self, PyObject* args){
		GameObject* ptr = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));

		PyObject* list = PyTuple_GetItem(args, 1);

		/*float i = PyFloat_AsDouble(PyList_GetItem(list, 0));
		float fuck = PyFloat_AsDouble(PyList_GetItem(list, 1));
		float you = PyFloat_AsDouble(PyList_GetItem(list, 2));
		float up = PyFloat_AsDouble(PyList_GetItem(list, 3));*/


		if (PyList_GetItem(list, 0) != Py_None) ptr->setTimeBuild(PyFloat_AsDouble(PyList_GetItem(list, 0)));
		if (PyList_GetItem(list, 1) != Py_None) ptr->setTimeOn(PyFloat_AsDouble(PyList_GetItem(list, 1)));
		if (PyList_GetItem(list, 2) != Py_None) ptr->setTimeOff(PyFloat_AsDouble(PyList_GetItem(list, 2)));
		if (PyList_GetItem(list, 3) != Py_None) ptr->setTimeDestroy(PyFloat_AsDouble(PyList_GetItem(list, 3)));

		Py_RETURN_NONE;
	}

	PyObject* GameObject::timeAlive(PyObject *self, PyObject* args){
		GameObject* ptr;
		PyArg_ParseTuple(args, "l", &ptr);
		
		/*string gargo = toString(args);
		long testerama = PyLong_AsLong(args);
		GameElement* ge = (GameElement*)(void*)testerama;*/
		double rv = ptr->timeAlive();
		return PyFloat_FromDouble(rv);
	}

	PyObject* GameObject::movePos(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z;
		PythonExtra::ValidateTuple("movePos", args, "lfff");
		PyArg_ParseTuple(args, "lfff", &ptr,&x, &y, &z);
		ptr->movePos(x,y,z);
		Py_RETURN_NONE;
	}
	PyObject* GameObject::moveRot(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z;
		PythonExtra::ValidateTuple("moveRot", args, "lfff");
		PyArg_ParseTuple(args, "lfff", &ptr,&x, &y, &z);
		ptr->moveRot(x,y,z);
		Py_RETURN_NONE;
	}
	PyObject* GameObject::moveScl(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z;
		PythonExtra::ValidateTuple("moveScl", args, "lfff");
		PyArg_ParseTuple(args, "lfff", &ptr,&x, &y, &z);
		ptr->moveScl(x,y,z);
		Py_RETURN_NONE;
	}

	PyObject* GameObject::setPos(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z;
		PyArg_ParseTuple(args, "lfff", &ptr,&x, &y, &z);
		ptr->setPos(x,y,z);
		Py_RETURN_NONE;
	}
	PyObject* GameObject::setRot(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z,w;
		PyArg_ParseTuple(args, "lffff", &ptr,&x, &y, &z,&w);
		ptr->setRot(x,y,z,w);
		Py_RETURN_NONE;
	}
	PyObject* GameObject::setScl(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z;
		PyArg_ParseTuple(args, "lfff", &ptr,&x, &y, &z);
		ptr->setScl(x,y,z);
		Py_RETURN_NONE;
	}

	PyObject* GameObject::getPos(PyObject *self, PyObject* args){
		PyObject* rv;
		GameObject* ptr;
		PyArg_ParseTuple(args, "l", &ptr);
		if (ptr == NULL){
			throw("Python getPos null pointer");
		}


		rv = PyList_New(3);
		Ogre::Vector3 pos = ptr->getNode()->getPosition();
		PyList_SetItem(rv, 0, PyFloat_FromDouble(pos.x));
		PyList_SetItem(rv, 1, PyFloat_FromDouble(pos.y));
		PyList_SetItem(rv, 2, PyFloat_FromDouble(pos.z));

		return rv;
	}
	PyObject* GameObject::getRot(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z,w;
		PyArg_ParseTuple(args, "lffff", &ptr,&x, &y, &z,&w);
		ptr->setRot(x,y,z,w);
		Py_RETURN_NONE;
	}
	PyObject* GameObject::getScl(PyObject *self, PyObject* args){
		GameObject* ptr;
		float x,y,z;
		PyArg_ParseTuple(args, "lfff", &ptr,&x, &y, &z);
		ptr->setScl(x,y,z);
		Py_RETURN_NONE;
	}


	PyObject* GameObject::addAnim(PyObject *self, PyObject* args){
		char* name;
		char* type;
		float value;
		double time;

		GameObject* ptr;
		PyArg_ParseTuple(args, "ldssf", &ptr,&time, &name,&type, &value);
		if (ptr != (GameObject*)0xcccccccc){
			DYNAMIC_OBJ_ANIM_TRIG arg;
			arg.time = time;
			arg.anim = ptr->animFromString((string)name);
			arg.type = ptr->animTypeFromString((string)type);
			arg.dest = value;
			ptr->mAnimTrig.push_back(arg);
		}else{
			throw("Python addAnim param error : "+Core::getSingleton()->getPythonException());
		}

		Py_RETURN_NONE;

	}
	PyObject* GameObject::takeDamage(PyObject *self, PyObject* args){
		if (!PyLong_Check(PyTuple_GetItem(args,0))){
			throw("TakeDamage parameter 0 must be pointer to gameobject");
		}
		Shmoulette::PythonExtra::ValidateTuple("takeDamage", args, "lxd");

		GameObject* go = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));

		DamagePackage dp(PyTuple_GetItem(args,1));
		double time = PyFloat_AsDouble(PyTuple_GetItem(args,2));
		go->takeDamage(dp, time);
		Py_RETURN_NONE;
	}

	PyObject* GameObject::animTrigger(PyObject *self, PyObject* args){
		char* name;
		char* type;
		float value;
		GameObject* ptr;

		Util::Log("AAAAAAAAAAAAAAA", 0);
		
		if (PyTuple_Size(args) == 4){
			Shmoulette::PythonExtra::ValidateTuple("animTrigger", args, "lssf");
			PyArg_ParseTuple(args, "lssf", &ptr,&name,&type, &value);
		}else if (PyTuple_Size(args) == 3){
			Shmoulette::PythonExtra::ValidateTuple("animTrigger", args, "lss");
			PyArg_ParseTuple(args, "lss", &ptr,&name,&type);
			value = 0;
		}
		
		if (ptr != (GameObject*)0xcccccccc){
			DYNAMIC_OBJ_ANIM_TRIG arg;
			arg.anim = ptr->animFromString((string)name);
			arg.type = ptr->animTypeFromString((string)type);
			arg.dest = value;
			ptr->animTrigger(arg);
		}else{
			throw("Python animTrigger param error : "+Core::getSingleton()->getPythonException());
		}
		Util::Log("EEEEEEEE", 0);
		Py_RETURN_NONE;
	}

	PyObject* GameObject::attach(PyObject *self, PyObject* args){
		GameObject* parent, *child;
		PyArg_ParseTuple(args, "ll", &parent,&child);
		if (parent == NULL) throw("Python attach call : Parent is NULL");
		if (child == NULL) throw("Python attach call : Child is NULL");
		parent->attach(child);
		Py_RETURN_NONE;
	}
	PyObject* GameObject::attachHook(PyObject *self, PyObject* args){
		GameObject* parent;
		PyArg_ParseTuple(args, "l", &parent);
		if (parent == NULL) throw("Python attach call : Parent is NULL");
		if (parent->isHooked()) throw("Python attach call : Attaching the Hook to a Hooked object is a nice way to get in trouble, mister.");

		parent->attach(parent->mLevelSegment->getCurrentHook());
		parent->mLevelSegment->forceSecondUpdate();
		Py_RETURN_NONE;
	}

	/*PyObject* GameObject::setCollGroup(PyObject *self, PyObject* args){
		char* name;
		GameObject* ptr;
		PyArg_ParseTuple(args, "ls", &ptr,&name);
		ptr->setCollGroup(name);
		Py_RETURN_NONE;
	}*/

	PyObject* GameObject::setShadows(PyObject *self, PyObject* args){
		bool value;
		//GameObject* ptr;
		PythonExtra::ValidateTuple("setShadows", args, "lb");

		GameObject* ptr = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));
		//ptr = PyLong_AsLong(PyTuple_GetItem(args, 0));
		value = PyTuple_GetItem(args, 1) == Py_True;

		/*if (!PyArg_ParseTuple(args, "lb", &ptr,&value)){
			throw("setShadow : "+Core::getSingleton()->getPythonException());
		};*/
		ptr->mCastShadows = value;
		Py_RETURN_NONE;
	}

	PyObject* GameObject::createChild(PyObject* self, PyObject* args){
		GameObject* ptr;
		//string label;
		string bone;
		string type;
		
			
		
		ptr = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));
		if (PyTuple_Check(args) == false || PyTuple_Size(args) <2)
			throw("createChild args must be tuple with 3 members "+ptr->getId());

		PyObject* createChildParams = PyTuple_GetItem(args, 1);
		
		if (PyList_Check(createChildParams) == false || PyList_Size(createChildParams) < 2)
			throw("createChild args 1 must be list of at least 2 members "+ptr->getId());

		type = PyString_AsString(PyList_GetItem(createChildParams,0));
		if (PyList_GetItem(createChildParams,1) != Py_None){
			bone = PyString_AsString(PyList_GetItem(createChildParams,1));
		}else{
			bone = "";
		}
		GameObject* collForward = ptr;
		if (PyList_Size(createChildParams) > 2){
			collForward = (GameObject*)PyLong_AsLong(PyList_GetItem(createChildParams,2));
		}else{
			collForward = NULL;
		}


		//label = PyString_AsString(PyTuple_GetItem(args,3));
		/*PyArg_ParseTuple(args, "lsss", 
			&ptr,
			&type,
			&bone,
			&label
			);*/
		
		PyObject* methodargs = PyTuple_GetItem(args, 2);

		GAME_OBJECT_CHILD c;

		/*if (type=="dynamic" || type=="mesh"){
			//c.gameObject = new Shmoulette::DynamicObj(methodargs);
			throw("DO SOMETHING");
			c.gameObject->setCollisionForward(ptr);
		}else*/ if (type=="object"){
			c.gameObject = new Shmoulette::GameObject();
			c.gameObject->init(methodargs);
			c.gameObject->setCollisionForward(collForward);
		}else if (type=="drone"){
			c.gameObject = Core::getSingleton()->getDroneDB()->createDrone(methodargs);
			//c.gameObject->init(methodargs);
			c.gameObject->setCollisionForward(collForward);
		}else if (type=="billboard"){
			c.gameObject = new Shmoulette::Billboard();
			c.gameObject->init(methodargs);
			c.gameObject->setCollisionForward(collForward);
		}else if (type=="fx"){
			FX* fx = new Shmoulette::FX();
			fx->init(methodargs);

			if (bone == ""){
				fx->setParent(ptr->getSceneNode());
			}else{
				fx->setParent(ptr, bone);
			}
			c.gameObject = fx;
			c.gameObject->setCollisionForward(collForward);
		}else if (type=="group"){
			/*string fn = xml.getString("filename");
			string context = xml.getString("context");
			File f(fn, context);
			if (f.error() != FILE_OK){
				throw ("Children group ("+fn+") not found in "+context);
			}
			XML childXml(f.getStrContent());
			addChildren(&childXml);*/
			//c.gameObject->disableCollisionTest();
		}else if (type=="light"){
			c.gameObject = new Shmoulette::Light();
			c.gameObject->init(methodargs);
			c.gameObject->setCollisionForward(ptr);
			//c.gameObject->disableCollisionTest();
		}else if (type=="particle"){
			c.gameObject = new Shmoulette::ParticleEmitter();
			c.gameObject->init(methodargs);
			c.gameObject->setCollisionForward(ptr);
			//c.gameObject->disableCollisionTest();
		}else{
			throw("Python createChild type not found : "+type);
		}

		if (type != "group"){
			//Util::Profile("Creating Child");
			c.bone = bone;
			c.gameObject->setId(ptr->getId()+"_"+c.gameObject->getId());
			//c.gameObject->setDoNotHook(true);
			
			c.gameObject->setHooked(false);
			c.gameObject->setParentConnector(ptr, c.bone, false);
			//c.gameObject->setStreamBuild(ptr->mStreamBuild);
			//c.gameObject->moveTimes(ptr->getTimeOn());
			ptr->mChild.push_back(c);
		}

		return 	PyLong_FromLong((long)c.gameObject);
	}

	PyObject* GameObject::setLife(PyObject* self, PyObject* args){
		GameObject* obj;
		float life;
		PyArg_ParseTuple(args, "lf", &obj,&life);
		obj->setLife(life);
		Py_RETURN_NONE;
	}

	PyObject* GameObject::getLife(PyObject* self, PyObject* args){
		GameObject* ptr;
		PyArg_ParseTuple(args, "l", &ptr);

		double rv = ptr->getLife();
		return PyFloat_FromDouble(rv);
	}
	PyObject* GameObject::cloneMaterials(PyObject* self, PyObject* args){
		GameObject* ptr;
		PyArg_ParseTuple(args, "l", &ptr);
		Ogre::Entity* entity;
		entity = ptr->getEntity();
		for (int i=0;i<entity->getNumSubEntities();i++){
			Ogre::MaterialPtr mat = entity->getSubEntity(i)->getMaterial();
			Ogre::MaterialPtr cloneMat = mat->clone(mat->getName()+ts(GameElement::getNextIdCounter()));
			entity->getSubEntity(i)->setMaterialName(cloneMat->getName());
		}
		Py_RETURN_NONE;
	}

	PyObject* GameObject::setColour(PyObject* self, PyObject* args){
		GameObject* obj = (GameObject*)PyLong_AsLong(PyTuple_GetItem(args,0));
		PyObject* pyArgs = PyTuple_GetItem(args, 1);

		string materialName;
		Ogre::ColourValue d, s, a, e;
		bool dM, dD, dS, dA, dE;


		if (PyList_Check(pyArgs) == true && PyList_Size(pyArgs) == 5){
			PyObject* argElem = PyList_GetItem(pyArgs, 0);
			if (argElem != Py_None){
				materialName == PyString_AsString(PyList_GetItem(pyArgs, 0));
				dM = true;
			}else{
				dM = false;
			}

			argElem = PyList_GetItem(pyArgs, 1);
			if (argElem != Py_None){
				d = Ogre::ColourValue(
					PyFloat_AsDouble(PyList_GetItem(argElem, 0)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 1)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 2)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 3))
					);
				dD = true;
			}else{
				dD = false;
			}

			argElem = PyList_GetItem(pyArgs, 2);
			if (argElem != Py_None){
				s = Ogre::ColourValue(
					PyFloat_AsDouble(PyList_GetItem(argElem, 0)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 1)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 2)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 3))
					);
				dS = true;
			}else{
				dS = false;
			}
			argElem = PyList_GetItem(pyArgs, 3);
			if (argElem != Py_None){
				a = Ogre::ColourValue(
					PyFloat_AsDouble(PyList_GetItem(argElem, 0)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 1)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 2)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 3))
					);

				dA = true;
			}else{
				dA = false;
			}
			argElem = PyList_GetItem(pyArgs, 4);
			if (argElem != Py_None){
				e = Ogre::ColourValue(
					PyFloat_AsDouble(PyList_GetItem(argElem, 0)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 1)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 2)),
					PyFloat_AsDouble(PyList_GetItem(argElem, 3))
					);

				dE = true;
			}else{
				dE = false;
			}

			Ogre::Entity* entity;
			entity = obj->getEntity();
			for (int i=0;i<entity->getNumSubEntities();i++){
				Ogre::MaterialPtr mat = entity->getSubEntity(i)->getMaterial();

				if (!dM || mat->getName() == materialName){
					for (int j=0;j<mat->getNumTechniques();j++){
						Ogre::Technique* tech = mat->getTechnique(j);
						for (int k=0;k<tech->getNumPasses();k++){
							Ogre::Pass* pass = tech->getPass(k);
							if (dD) pass->setDiffuse(d);
							if (dS) pass->setSpecular(s);
							if (dA) pass->setAmbient(a);
							if (dE) pass->setSelfIllumination(e);
						}
					}
				}
			}


		}else{
			throw("GameObject : PyObject should be an array of at least size 2");
		}

		Py_RETURN_NONE;
	}

	PyObject* GameObject::activate(PyObject *self, PyObject* args){
		GameObject* ptr;
		PythonExtra::ValidateTuple("Activate", args, "l");
		PyArg_ParseTuple(args, "l", &ptr);
		ptr->activate();
		Py_RETURN_NONE;
	}

	PyObject* GameObject::fire(PyObject *self, PyObject* args){
		int value;
		GameObject* ptr;
		PythonExtra::ValidateTuple("Fire", args, "li");
		PyArg_ParseTuple(args, "li", &ptr,&value);
		ptr->fire(value);
		Py_RETURN_NONE;
	}
	
	PyObject* GameObject::ceaseFire(PyObject *self, PyObject* args){
		int value;
		GameObject* ptr;
		PythonExtra::ValidateTuple("Fire", args, "li");
		PyArg_ParseTuple(args, "li", &ptr,&value);
		ptr->ceaseFire(value);
		Py_RETURN_NONE;
	}
	
	/*	PyObject* GameObject::setAlpha(PyObject* self, PyObject* args){
		GameObject* obj;
		float alpha;
		PyArg_ParseTuple(args, "lf", &obj,&alpha);
		Ogre::Entity* entity;
		entity = obj->getEntity();
		for (int i=0;i<entity->getNumSubEntities();i++){
			Ogre::MaterialPtr mat = entity->getSubEntity(i)->getMaterial();
			for (int j=0;j<mat->getNumTechniques();j++){
				Ogre::Technique* tech = mat->getTechnique(j);
				for (int k=0;k<tech->getNumPasses();k++){
					Ogre::Pass* pass = tech->getPass(k);

					Ogre::ColourValue c;
					c = pass->getDiffuse();
					c.a = alpha;
					pass->setDiffuse(c);

					c = pass->getSpecular();
					c.a = alpha;
					pass->setSpecular(c);
					
					c = pass->getAmbient();
					c.a = alpha;
					pass->setAmbient(c);
					
					c = pass->getSelfIllumination();
					c.a = alpha;
					pass->setSelfIllumination(c);


				}
			}
		}
		
		Py_RETURN_NONE;
	}*/


	static PyMethodDef GameObjectMethods[] = {
		{"setVisible", GameObject::setVisible, METH_VARARGS,"Dis or Dat."},
		{"getChild", GameObject::getChild, METH_VARARGS,"Dis or Dat."},
		{"timeAlive", GameObject::timeAlive, METH_VARARGS,"Dis or Dat."},
		{"movePos", GameObject::movePos, METH_VARARGS,"Dis or Dat."},
		{"moveRot", GameObject::moveRot, METH_VARARGS,"Dis or Dat."},
		{"moveScl", GameObject::moveScl, METH_VARARGS,"Dis or Dat."},
		{"setPos", GameObject::setPos, METH_VARARGS,"Dis or Dat."},
		{"setRot", GameObject::setRot, METH_VARARGS,"Dis or Dat."},
		{"setScl", GameObject::setScl, METH_VARARGS,"Dis or Dat."},
		{"getPos", GameObject::getPos, METH_VARARGS,"Dis or Dat."},
		//{"getRot", GameObject::getRot, METH_VARARGS,"Dis or Dat."},
		//{"getScl", GameObject::getScl, METH_VARARGS,"Dis or Dat."},
		{"animTrigger", GameObject::animTrigger, METH_VARARGS,"Dis or Dat."},
		{"railToOrbit", GameObject::railToOrbit, METH_VARARGS,"Dis or Dat."},
		{"attach", GameObject::attach, METH_VARARGS,"Dis or Dat."},
		{"attachHook", GameObject::attachHook, METH_VARARGS,"Dis or Dat."},
		{"createChild", GameObject::createChild, METH_VARARGS,"Dis or Dat."},
		{"addMaterialReplace", GameObject::addMaterialReplace, METH_VARARGS,"Dis or Dat."},
		//{"setCollGroup", GameObject::setCollGroup, METH_VARARGS,"Dis or Dat."},
		{"addAnim", GameObject::addAnim, METH_VARARGS,"Dis or Dat."},
		{"setShadows", GameObject::setShadows, METH_VARARGS,"Dis or Dat."},
		{"addGoAnim", GameObject::addGoAnim, METH_VARARGS,"Dis or Dat."},
		{"takeDamage", GameObject::takeDamage, METH_VARARGS,"Dis or Dat."},
		{"setLife", GameObject::setLife, METH_VARARGS,"Dis or Dat."},
		{"getLife", GameObject::getLife, METH_VARARGS,"Dis or Dat."},
		{"addWeapon", GameObject::addWeapon, METH_VARARGS,"Dis or Dat."},
		{"fire", GameObject::fire, METH_VARARGS,"Dis or Dat."},
		{"ceaseFire", GameObject::ceaseFire, METH_VARARGS,"Dis or Dat."},
		{"activate", GameObject::activate, METH_VARARGS,"Dis or Dat."},
		//{"setAlpha", GameObject::setAlpha, METH_VARARGS,"Dis or Dat."},
		{"setRenderQueue", GameObject::setRenderQueue, METH_VARARGS,"[queue, includechildren]"},
		{"setTimes", GameObject::setTimes, METH_VARARGS,"[materialname, diffuse, specular, ambient, emit]"},
		{"getChildren", GameObject::getChildren, METH_VARARGS,"[materialname, diffuse, specular, ambient, emit]"},
		{"setColour", GameObject::setColour, METH_VARARGS,"[materialname, diffuse, specular, ambient, emit]"},
		{"cloneMaterials", GameObject::cloneMaterials, METH_VARARGS,"[materialname, diffuse, specular, ambient, emit]"},
		{NULL, NULL, 0, NULL}
	};


	void GameObject::registerPython(){
		Py_InitModule("sh_gameobject", GameObjectMethods);
	}

	void GameObject::animTrigger(DYNAMIC_OBJ_ANIM_TRIG& it){
		if (mEntity != NULL){
			DYNAMIC_OBJ_ANIM* anim = it.anim;
			if (anim->animState == NULL){
				anim->animState = mEntity->getAnimationState(anim->animName);
			}
			switch(it.type){
				case GOTO_AND_STOP:
					anim->animState->setEnabled(true);
					anim->animState->setTimePosition(it.dest);
					if (anim->status == PLAYING_REVERSE){
						anim->status = STOPPED_REVERSE;
					}else{
						anim->status = STOPPED;
					}
				break;
				case PLAY_SPEED:
					anim->animState->setEnabled(true);
					anim->speedMultiplier = it.dest;
					Util::Log("New Speed Modifyer : "+ts(anim->speedMultiplier),1);
				break;
				case GOTO_AND_PLAY:
					anim->animState->setEnabled(true);
					anim->animState->setTimePosition(it.dest);
					anim->status = PLAYING;
				break;
				case REVERSE_PLAY:
					anim->animState->setEnabled(true);
					anim->animState->setTimePosition(it.dest);
					anim->status = PLAYING_REVERSE;
				break;
				case STOP:
					anim->animState->setEnabled(true);
					if (anim->status == PLAYING_REVERSE){
						anim->status = STOPPED_REVERSE;
					}else{
						anim->status = STOPPED;
					}
				break;
				case PLAY:
					anim->animState->setEnabled(true);
					anim->status = PLAYING;
				break;
				case DISABLE_LOOP:
					anim->animState->setLoop(false);
				break;
				case ENABLE_LOOP:
					anim->animState->setLoop(true);
				break;
				case DISABLE:
					anim->animState->setEnabled(false);
				break;
				case ENABLE:
					anim->animState->setEnabled(true);
				break;
			}
		}else{
			throw("Trying to animate an unbuilt entity");
		}
	}
	Ogre::Node* GameObject::getNode(){
		if (mParentConnector.mBone == "")
			return mSceneNode;
		else{
			if (mEntity == NULL){
				//return mParentConnector.mGameObject->getNode();
				throw ("getNode query on an NULL entity : "+mId);
			}

			if (mEntity->getParentNode() == NULL){
				throw ("No SceneNode and entity has no node either.  : "+mId);
			}

			return mEntity->getParentNode();
		}
	}

	bool GameObject::update(double timeDelta, double parentTime){
		updateFromCoordinator(false);
		if (mPythonInstance.hasUpdate){
			PyObject* c=PyObject_CallMethodObjArgs(mPythonInstance.pythonInstance, mPythonInstance.updateString,NULL);
			if (c == NULL){
				throw("Python Exception on Update "+mId+" : "+Core::getSingleton()->getPythonException());
			}
		}

		/*if (mStreamBuild){		
			list<GAME_OBJECT_CHILD>::iterator it;
			for (it=mChild.begin();it!=mChild.end();it++){
				if (!(*it).gameObject->isBuild()){
					(*it).gameObject->build();
					if (isOn()) (*it).gameObject->show();
					break;
				}
				//(*it).gameObject->attachToBone(mEntity, (*it).bone);
			}
			if (it == mChild.end())
				mStreamBuild = false;
		}*/

		mForcedUpdate = false;
		//float timeDelta = mLevelSegment->getTimeDelta();
			
		//getNode()->_update(false, false); // OPTIMIZATION : THIS COSTS A LOT
		//Ogre::Node* node = getNode();
//		mOldPos = node->_getFullTransform().getTrans();
		
		/*list<CollisionElement>::iterator it;
		if (mCollisionElement.size() > 0){
			mSceneNode->_update(true, true);
			//mEntity->_updateAnimation();
			//mEntity->getSkeleton()->_updateTransforms();
		}
		for (it=mCollisionElement.begin();it!=mCollisionElement.end();it++){
			(*it).update();
		}*/
		if (mLife > 0){
			vector<Weapon*>::iterator it;
			for (it = mWeapon.begin();it!=mWeapon.end();it++){
				(*it)->update(timeDelta, parentTime);
			}
		}

		{
			list<GAME_OBJECT_CHILD>::iterator it;
			for (it=mChild.begin();it!=mChild.end();it++){
				if ((*it).gameObject->updateIfOn(timeDelta, timeAlive())){
					Util::Log("*Destroying Child",1);
					mLevelSegment->removeCollisionTest((*it).gameObject);
					(*it).gameObject->hide();
					(*it).gameObject->destroy();
					//mVectorDroneStandby.push_back(*it);
					//delete(*it);
					//(*it)=NULL;
				}
				/*if ((*it).gameObject->isBuild()){
					if ((*it).gameObject->isMarkedForTermination()){
						(*it).gameObject->destroy();
					}else{
						//if ((*it).gameObject->isOn()){
							//(*it).gameObject->update();
						//}
					}
				}*/
			}

		}
		{
			vector<GAME_OBJECT_MAT_REPLACEMENTS>::iterator it;
			for (it = mMatReplace.begin();it != mMatReplace.end();it++){
				if (LevelSegment::isTimeInFrame((*it).time, timeAlive(), timeDelta)){
					materialReplace((*it).replace, (*it).with);
				}
			}
		}
		{
			list<DYNAMIC_OBJ_ANIM_TRIG>::iterator it;
			for (it=mAnimTrig.begin();it!=mAnimTrig.end();it++){
				if (LevelSegment::isTimeInFrame((*it).time, timeAlive(), timeDelta)){
					animTrigger(*it);
				}
			}
		}{
			list<DYNAMIC_OBJ_ANIM>::iterator it;
			for (it = mAnim.begin();it!=mAnim.end();it++){
				if ((*it).status == PLAYING){
					float delta = timeDelta*(*it).speedMultiplier;
					(*it).animState->addTime(delta);
				}
				if ((*it).status == PLAYING_REVERSE){
					float delta = timeDelta*(*it).speedMultiplier;
					(*it).animState->addTime(0-delta);
				}
				if ((*it).animState != NULL){
					Util::Log((*it).animState->getAnimationName()+"->"+ts((*it).animState->getTimePosition())+" and is of status "+ts((*it).status),1);
				}

			}
		}{
			list<GoAnim::GoAnim*>::iterator it;
			for (it = mGoAnim.begin();it!=mGoAnim.end();it++){
				(*it)->updateIfOn(timeDelta, timeAlive());
				/*double crissdetabarnak = mLevelSegment->getTimeTrack()-getTimeOn();
				double calissedecrisse = (*it)->getTimeOn();
				if (!(*it)->isBuild() && (*it)->shouldBeBuild(crissdetabarnak)){
					(*it)->build();
				}
				if ((*it)->shouldBeOn(crissdetabarnak)){
					(*it)->update();
				}*/
			}

		}

		Parent::update(timeDelta, parentTime);

		if (getQueryFlags() != SceneQueryFlag::IMMATERIAL)
			mLevelSegment->addCollisionTest(this);
		return false;
	}

	bool GameObject::postFrameUpdate(double timeDelta, double parentTime){
		//float timeDelta = mLevelSegment->getTimeDelta();
		Ogre::Vector3 newPos = getNode()->_getFullTransform().getTrans();
		if (timeDelta != 0){
			mInertia.x = (newPos.x - mOldPos.x)/timeDelta;
			mInertia.y = (newPos.y - mOldPos.y)/timeDelta;
			mInertia.z = (newPos.z - mOldPos.z)/timeDelta;
		}
		mOldPos = newPos;

		if (getQueryFlags() != SceneQueryFlag::IMMATERIAL)
			if (mCollisionStructure)
				mCollisionStructure->update();

		{
			list<GAME_OBJECT_CHILD>::iterator it;
			for (it=mChild.begin();it!=mChild.end();it++){
				//if ((*it).gameObject != NULL){
				if ((*it).gameObject->isOn()){
					bool terminate = (*it).gameObject->isMarkedForTermination();
					if (!terminate){
						if ((*it).gameObject->postFrameUpdate(timeDelta, parentTime) == true){
							(*it).gameObject->markForTermination();
							terminate = true;
						}
					}
					if (terminate){
						mLevelSegment->removeCollisionTest((*it).gameObject);
						notifyChildDeath((*it).gameObject);
						GameObject* tmp = (*it).gameObject;
						(*it).gameObject = NULL;
						if (tmp->isOn()) tmp->hide();
						if (tmp->isBuild())	tmp->destroy();
						delete tmp;
					};
				}
				//}			
			}
			for (it=mChild.begin();it!=mChild.end();){
				if ((*it).gameObject == NULL){
					mChild.erase(it);
					it = mChild.begin();
					if (mChild.size()==0) break;
				}else{
					it++;
				};
			}
		}
		return Parent::postFrameUpdate();
	}

	void GameObject::notifyChildDeath(GameObject* go){
	}


	void GameObject::forceUpdate(){
		if (mSceneNode){
			Ogre::SceneNode* psn;
			psn = mSceneNode->getParentSceneNode();
			if (psn != NULL){
				psn->_update(false, false);
			}
			mSceneNode->_update(false, false);
		}
		mForcedUpdate = true;
	}

	void GameObject::setPos(float x, float y, float z){
		Ogre::Node* node = getNode();
		if (node != NULL) node->setPosition(x, y, z);
	}

	void GameObject::setRot(float x, float y, float z, float deg){
		Vector3 v;
		v.x=x;
		v.y=y;
		v.z=z;
		Degree d;
		Radian r;
		d = deg;
		r = d;
		Quaternion q;
		q.FromAngleAxis(r, v);
		getNode()->setOrientation(q);
	}

	void GameObject::setScl(float x, float y, float z){
		getNode()->setScale(x, y, z);
	}

	void GameObject::movePos(float x, float y, float z){
		getNode()->translate(x, y, z);
	}
	void GameObject::moveRot(float x, float y, float z){
		getNode()->pitch((Ogre::Radian)(Ogre::Degree)x);
		getNode()->yaw((Ogre::Radian)(Ogre::Degree)y);
		getNode()->roll((Ogre::Radian)(Ogre::Degree)z);
	}
	void GameObject::moveScl(float x, float y, float z){
		getNode()->setScale(
			mSceneNode->getScale().x+x,
			mSceneNode->getScale().y+y,
			mSceneNode->getScale().z+z);
	}

	void GameObject::onMove(){

	}

	void GameObject::pitch(float val, bool maintenance){
		if (mIsBuild){
			getNode()->pitch((Ogre::Radian)(Ogre::Degree)val);
		}
	}

	void GameObject::pitch(Ogre::Radian r){
		if (mIsBuild){
			getNode()->pitch(r);
		}
	}
	void GameObject::turn(float val, bool maintenance){
		if (mIsBuild){
			Ogre::Radian r;
			Ogre::Degree d;
			d = val;
			r = d;
			
			getNode()->roll(r);
			if (maintenance){
			}
		}
	}
	void GameObject::yaw(Ogre::Radian r){
		if (mIsBuild){
			getNode()->yaw(r);
		}
	}
	void GameObject::roll(float val, bool maintenance){
		if (mIsBuild){
			Ogre::Radian r;
			Ogre::Degree d;
			d = val;
			r = d;
			
			getNode()->roll(r);
			if (maintenance){
			}
		}
	}
	void GameObject::roll(Ogre::Radian r){
		if (mIsBuild){
			getNode()->roll(r);
		}
	}

	void GameObject::grow(float val, bool maintenance){
		moveScl(val, val, val);
	}

	void GameObject::move(float val, bool maintenance){
		if (mIsBuild){
			Ogre::Vector3 v;
			v.x = 0;
			v.y = 0;
			v.z = val;
			getNode()->translate(v, Ogre::Node::TS_LOCAL);
		}
	}

	void GameObject::climb(float val, bool maintenance){
		if (mIsBuild){
			Ogre::Vector3 v;
			v.x = 0;
			v.y = val;
			v.z = 0;
			getNode()->translate(v, Ogre::Node::TS_LOCAL);
		}
	}

	void GameObject::strafe(float val, bool maintenance){
		if (mIsBuild){
			Ogre::Vector3 v;
			v.x = val;
			v.y = 0;
			v.z = 0;
			getNode()->translate(v, Ogre::Node::TS_LOCAL);
		}
	}

	void GameObject::setType(OBJ_TYPE type){
		mType = type;
	}

	void GameObject::setFilename(const string& filename){
		MACRO_PROFILER_GET("B1", "GameObjectNew")->start();

		mFilename = filename;
		/*if (mResourcePath == "."){
			mResourcePath = mFilename;
		}*/
		MACRO_PROFILER_GET("B1", "GameObjectNew")->end();
		MACRO_PROFILER_GET("B2", "GameObjectNew")->start();
		if (File::exists(filename+".xml", "media/mesh")){
			XML x((std::string)(char*)File(mFilename+".xml", "media/mesh").content());
			LodInfo li;
			li.meshName = x.getString("basefilename");
			li.distance = 0;

			mLodInfo.push_back(li);
			XMLIterator xit(&x);
			xit.setElemName("mesh>lod");
			while (xit.gotoNext()){
				LodInfo li;
				li.meshName = xit.getElem().getString("filename");
				li.distance = xit.getElem().getFloat("dist");
				mLodInfo.push_back(li);
			}

			checkForChildren();
			checkForInitScript();
			loadCollisionData();
			checkForDamagePackage();

			//mLevelSegment->addResource(mFilename, "Material");
			if (File::exists(mLodInfo[0].meshName+"-armature.skeleton", "media\\models\\"+mLodInfo[0].meshName) || 
				File::exists(mLodInfo[0].meshName+"-armature.skeleton", "media\\mesh")
				){
			}


			/*it.gotoZero();
			it.setElemName("children>child");
			while(it.gotoNext()!=false){*/
			int g=0;
		}else{
			MACRO_PROFILER_GET("B2A", "GameObjectNew")->start();
			if (mChildrenFilename=="."){
				mChildrenFilename = mFilename+"_children.xml";
			}
			MACRO_PROFILER_GET("B2A", "GameObjectNew")->end();
			MACRO_PROFILER_GET("B2B", "GameObjectNew")->start();
			checkForChildren();
			checkForInitScript();
			MACRO_PROFILER_GET("B2B", "GameObjectNew")->end();
			MACRO_PROFILER_GET("B2C", "GameObjectNew")->start();
			loadCollisionData();
			MACRO_PROFILER_GET("B2C", "GameObjectNew")->end();
			MACRO_PROFILER_GET("B2D", "GameObjectNew")->start();
			checkForDamagePackage();
			MACRO_PROFILER_GET("B2E", "GameObjectNew")->end();
		}
		MACRO_PROFILER_GET("B2", "GameObjectNew")->end();
		MACRO_PROFILER_GET("B3", "GameObjectNew")->start();
		mRootEntity = mLevelSegment->addPreloadEntity(mFilename);
		MACRO_PROFILER_GET("B3", "GameObjectNew")->end();
		// TODO : For the love of all that is holy remove the line below
		//mLevelSegment->getSceneManager()->createEntity("PreLoadBullshitStuff"+mId+ts(getNextIdCounter()), );
	}

	void GameObject::setLight(int lighting){
		if (!lighting){
			//DGSDK dbSetObjectAmbience(objNb, 0xFFFFFF);
		}
		//DGSDK dbSetObjectLight   ( objNb, 0 );
	}

	void GameObject::setRotFromObj(GameObject* obj){
		getNode()->setOrientation(obj->getSceneNode()->getOrientation());
	}

	void GameObject::setPosFromObj(GameObject* obj){
	}

	void GameObject::hit(DamagePackage& pack, GameObject* hitter, GameObject* forwardFrom, float distance, double time){
		if (mCollisionForward){
			if (forwardFrom == NULL) forwardFrom = this;
			mCollisionForward->hit(pack, hitter, forwardFrom,distance, time);
		}else{
			takeDamage(pack, time);
		}


		/*mTimeOff = mLevelSegment->getTimeTrack();
		mTimeDestroy = mTimeOff;*/
	}


	void GameObject::takeDamage(DamagePackage& pack, double time){
		if (pack.getDamageCycle() == DAM_CYCLE_INSTANT){
			mLife-=pack.getTotalDamage();
		}else{
			mLife-=pack.getTotalDamage()*time;//Level::getSingleton()->getTimeDelta();
		}
	}


	void GameObject::setToObjectOrientation(GameObject* obj){
	}
	void GameObject::point(float x, float y, float z){
		if (mSceneNode){
			getNode()->resetOrientation();
			Ogre::Vector3 v;
			v.x = x;
			v.y = y;
			v.z = z;
			mSceneNode->lookAt(v, mHooked?Node::TS_PARENT:Node::TS_WORLD);
		}
	}

	void GameObject::point(GameObject* obj){
		if (mSceneNode){
			mSceneNode->lookAt(obj->getSceneNode()->getPosition(), Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);
		}
	}

	Ogre::Entity* GameObject::getEntity(){
		return mEntity;
	}

	GameObject* GameObject::getChild(string str){
		string possibleChildren;
		size_t firstSlashPos = str.find("/");
		if (firstSlashPos != string::npos){
			string rootName = str.substr(0,firstSlashPos);
			GameObject* child = getChild(rootName);
			if (child == NULL){
				throw("Child not found ("+str+")");
				return NULL;
			}else{
				return child->getChild(str.substr(firstSlashPos+1));
			}
			//GameObject* rootObj = getObjectByName(rootName);
			//return rootObj->getChild(name.substr(firstSlashPos+1));
		}else{
			list<GAME_OBJECT_CHILD>::iterator it;
			for (it=mChild.begin();it!=mChild.end();it++){
				string fullName = (*it).gameObject->getId();
				int nb = getId().length()+1;
				if (nb >=fullName.length()){
					throw ("Child naming fuckup error thing that should never happen");
				}
				string croppedName = fullName.substr(nb);

				possibleChildren+="["+croppedName+"]";
				string str1 = (*it).gameObject->getId();
				string str2 = getId()+"_"+str;
				if (str1 == str2){
					return (*it).gameObject;
				}
			}
		}
 		throw("Child of "+mId+" not found ("+str+")\n Possible Children : "+possibleChildren);
		return NULL;
	}

	/*bool GameObject::isCollisionElementIntersect(CollisionElement* ce){
		if (mCollisionElement.size() == 0){
			CollisionElement ceb(mEntity->getWorldBoundingBox(true), this);
			if (ceb.intersects(ce)){
				return true;
			}
		}else{
			list<CollisionElement>::iterator it;
			for (it=mCollisionElement.begin();it!=mCollisionElement.end();it++){
				if ((*it).intersects(ce)) return true;
			}
		}
		return false;
	}*/

	COLLISION_RESULT GameObject::isColliding(GameObject* obj){
		COLLISION_RESULT rv;

		/*list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			COLLISION_RESULT tcr;
			tcr = (*it).gameObject->isColliding(obj);
			if (tcr.mOccured == true){
				return tcr;
			}
		}*/

		if (mEntity != NULL){
			INTERSECTS_RESULT cr = mCollisionStructure->intersects(obj->getCollisionStructure());
			if (cr.mIntersects){
				rv.mDistance = cr.mDistance;
				rv.mOccured = true;
				rv.mTarget = this;
				rv.mInvulnerable = cr.mInvulnerable;
				//rv.mChild = NULL;
				return rv;
			};
			/*if (mCollisionElement.size() == 0){

				CollisionElement ce(mEntity->getWorldBoundingBox(true), this);
				if (obj->isCollisionElementIntersect(&ce)){
					rv.mOccured = true;
					rv.mTarget = this;
					rv.mChild = NULL;
					Util::Log(obj->getId()+" hits "+mId);
					return rv;
				}
			}else{
				if (mEntity->getWorldBoundingBox(true).intersects(obj->getEntity()->getWorldBoundingBox(true))){
					list<CollisionElement>::iterator it;
					for (it=mCollisionElement.begin();it!=mCollisionElement.end();it++){
						if (obj->isCollisionElementIntersect(&(*it))){
							rv.mOccured = true;
							rv.mTarget = this;
							rv.mChild = NULL;
							Util::Log(obj->getId()+" hits "+mId);
							return rv;
						}
					}
					
				}
			}*/
		}

		rv.mOccured = false;
		rv.mTarget = this;
		//rv.mChild = NULL;
		return rv;
	}

	float GameObject::getAbsoluteX(){return 0;}//dbObjectPositionX(objNb); }
	float GameObject::getAbsoluteY(){return 0;}//dbObjectPositionY(objNb); }
	float GameObject::getAbsoluteZ(){return 0;}//dbObjectPositionZ(objNb); }

	void GameObject::setPaintOver(Ogre::Material* mat){
		Util::Log("PaintOverStart");
		setPaintOver(mat->getName());
		Util::Log("PaintOverEnd");
	}
	void GameObject::setPaintOver(string mat){

		int i;
		Util::Log("Paint Over : "+mId ,1);
		if (mAllowPaintOver){
			Util::Log("Really Paint Over : "+mId ,1);

			if (!isPaintedOver()){
				Ogre::MaterialPtr tmpMat;
				if (mEntity != NULL){
					for (i=0;i<mEntity->getNumSubEntities();i++){
						tmpMat = mEntity->getSubEntity(i)->getMaterial();
						mBackupMaterial.push_back(tmpMat);
						mEntity->getSubEntity(i)->setMaterialName(mat);
					}
				}

				/*if (mChildEntity.size() > 0){
					list<Ogre::Entity*>::iterator it;
					for (it=mChildEntity.begin();it!=mChildEntity.end();it++){
						for (i=0;i<(*it)->getNumSubEntities();i++){
							tmpMat = (*it)->getSubEntity(i)->getMaterial();
							mBackupMaterial.push_back(tmpMat);
							(*it)->getSubEntity(i)->setMaterialName(mat);
						}
					}
				}*/
				mIsPaintedOver = true;
			}else{
				if (mEntity != NULL){
					for (i=0;i<mEntity->getNumSubEntities();i++){
						mEntity->getSubEntity(i)->setMaterialName(mat);
					}
				}
			}
		}
		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			Util::Log("Woah, this one has children :"+ts((int)mChild.size()));

			(*it).gameObject->setPaintOver(mat);
		}

		/*if (mDependent != NULL){
			Util::Log("Woah, this one has a dependent :");
			mDependent->setPaintOver(mat);
		}*/
		if (mParentConnector.mTransferPaintOver == true){
			mParentConnector.mGameObject->setPaintOver(mat);
		}
	}


	void GameObject::removePaintOver(){
		Util::Log("Removing Paint Over : "+mId ,1);

		if (mAllowPaintOver){
			Util::Log("Really Removing Paint Over : "+mId ,1);
			int i;
			list<Ogre::MaterialPtr>::iterator matIt;
			matIt = mBackupMaterial.begin();
			if (mEntity != NULL){
				for (i=0;i<mEntity->getNumSubEntities();i++){
					mEntity->getSubEntity(i)->setMaterialName((*matIt)->getName());
					matIt++;
				}
			}
			mIsPaintedOver = false;
		}
		list<GAME_OBJECT_CHILD>::iterator it;
		for (it=mChild.begin();it!=mChild.end();it++){
			(*it).gameObject->removePaintOver();
		}
		if (mParentConnector.mTransferPaintOver == true){
			mParentConnector.mGameObject->removePaintOver();
		}
	}


	/*void GameObject::collision(COLLISION_RESULT cr){
		if (cr.mOccured){
			cr.mTarget->hit(this, cr.mChild);
			hit(cr.mTarget);
			//impact(1);
			//targetHit->hit(this, cr.mChild);
		}else{
			throw ("Target received a collision element that's not occuring.  Should never happen");
		}
		
	}*/


}
