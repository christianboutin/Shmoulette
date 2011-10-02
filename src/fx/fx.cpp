#include "stdafx.h"
#include "goanims/translation.h"
#include "goanims/rotation.h"

#include "fx/fx.h"

namespace Shmoulette{
	DebrisInfo::DebrisInfo(XML& xml){

		mGravity = false;
		mSpin = xml.getFloat("spin");
		if (xml.getError() != XML_OK){
			mSpin = 0;
		}
		mSpeed = xml.getFloat("speed");
		if (xml.getError() != XML_OK){
			mSpeed = 0;
		}

		{
			vector<string> v;
			Tokenize(xml.getString("params"), v, ";");
			for (vector<string>::iterator it = v.begin();it!=v.end();it++){
				if ((*it) == "gravity"){
					mGravity = true;
				}
				
			}
		}
		{
			vector<string> v;
			Tokenize(xml.getString("qty"), v, "-");
			mQuantity.first = atoi(v[0].c_str());
			mQuantity.second = atoi(v[1].c_str());
		}
		{
			vector<string> v;
			Tokenize(xml.getString("uniques"), v, ";");
			for (vector<string>::iterator it = v.begin();it!=v.end();it++) mUnique.push_back(*it);
		}
		{
			vector<string> v;
			Tokenize(xml.getString("generics"), v, ";");
			for (vector<string>::iterator it = v.begin();it!=v.end();it++) mGeneric.push_back(*it);
		}
		int g=0;
		/*list<string>mUniqueDebris;
		list<string>mGenericDebris;
		pair<int,int>mDebrisQuantity;*/
	}

	void DebrisInfo::preload(LevelSegment* ls){
		for (list<string>::iterator it2 = mGeneric.begin();it2!= mGeneric.end();it2++){
			ls->addPreloadEntity((*it2));
		}
		for (list<string>::iterator it2 = mUnique.begin();it2!= mUnique.end();it2++){
			ls->addPreloadEntity((*it2));
		}
	}

	void DebrisInfo::spawnDebris(Ogre::Vector3 v){
		Debris* go;
		
		for (list<string>::iterator it = mUnique.begin();it != mUnique.end();it++){
			go = new Debris();
			go->init((*it), v);
		}
		for (list<string>::iterator it = mGeneric.begin();it != mGeneric.end();it++){
			go = new Debris();
			go->init((*it), v);
		}
		float time = Level::getSingleton()->getCurrentSegment()->getTimeTrack();
		go->setTimeBuild(time);
		go->setTimeOn(time);
		go->setTimeOff(time+5);
		go->setTimeDestroy(time+5);

		if (mGravity){
			go->addGoAnim("gravity", GoAnimParams());
		}
		
		{
			GoAnim::TranslateParams tp;
			tp.setTimes(time, time+5);
			tp.mSpeed = Ogre::Vector3(Math::RangeRandom(0-mSpeed, mSpeed), Math::RangeRandom(0-mSpeed, mSpeed), Math::RangeRandom(0-mSpeed, mSpeed));
			go->addGoAnim("translate", tp);//0, "-"+ts(mSpeed)+":"+ts(mSpeed)+","+"-"+ts(mSpeed)+":"+ts(mSpeed)+","+"-"+ts(mSpeed)+":"+ts(mSpeed));
		}{
			GoAnim::RotateParams tp;
			tp.mSpeed = Math::RangeRandom(0-mSpin, mSpin);
			go->addGoAnim("yaw", tp);
			tp.mSpeed = Math::RangeRandom(0-mSpin, mSpin);
			go->addGoAnim("pitch", tp);
			tp.mSpeed = Math::RangeRandom(0-mSpin, mSpin);
			go->addGoAnim("roll", tp);

		}
		
		
		Level::getSingleton()->getCurrentSegment()->addGameObj(go);
		
	}

	FXDB::FXDB(){
		FileIterator fi("*.xml", "media/fx");
		string fn = fi.getFirstFilename();
		do{
			fn = fn.substr(0,fn.length()-4);
			this->getEntry(fn);
			fn = fi.getNextFilename();
		}while(fn != "");

	}

	void FXDBE::preload(LevelSegment* ls){
		for (list<DebrisInfo>::iterator it = mDebrisInfo.begin();it!= mDebrisInfo.end();it++){
			(*it).preload(ls);
		}
		for (list<string>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
			ls->addPreloadParticleSystem(*it);
		}
	}

	FXDBE::FXDBE(string id){
		mId = id;
		File f(id+".xml", "media/fx");
		if (f.error() != FILE_OK){
			throw("FX file "+id+" not found");
		}
		XML xml(f.getStrContent());
		XMLIterator it(&xml);
		it.gotoZero();
		it.setElemName("fx>particlesystem");
		while(it.gotoNext() == true){
			string s = it.getElem().getString("name");
			mParticleSystem.push_back(s);
		}
		it.setElemName("fx>debris");
		while(it.gotoNext() == true){
			
			mDebrisInfo.push_back(
				DebrisInfo(it.getElem())
			);
		}
		
		mSound = xml.getString("fx", "sound");
		if (xml.getError() != XML_OK){
			mSound = "";
		}


		mCoreMesh.mMesh = xml.getString("fx", "mesh");
		if (xml.getError() == XML_OK){
			string s;

			s = xml.getString("fx", "start_scale");
			if (xml.getError() == XML_OK){
				mCoreMesh.mStartScale = vector3FromString(s);
			}else{
				mCoreMesh.mStartScale = Ogre::Vector3(1,1,1);
			}
			
			s = xml.getString("fx", "anim_scale");
			if (xml.getError() == XML_OK){
				mCoreMesh.mAnimScale = vector3FromString(s);
			}else{
				mCoreMesh.mAnimScale = Ogre::Vector3(0,0,0);
			}

			s = xml.getString("fx", "start_rotation");
			if (xml.getError() == XML_OK){
				mCoreMesh.mStartRotation = vector3FromString(s);
				vector<string> vs;
				Tokenize(s, vs, ",");
				if (vs[0] == "R"){
					mCoreMesh.mStartRotation.x = FLT_MAX;
				}else{
					mCoreMesh.mStartRotation.x = Ogre::Degree(mCoreMesh.mStartRotation.x).valueRadians();
				}
				if (vs[1] == "R"){
					mCoreMesh.mStartRotation.y = FLT_MAX;
				}else{
					mCoreMesh.mStartRotation.y = Ogre::Degree(mCoreMesh.mStartRotation.y).valueRadians();
				}
				if (vs[2] == "R"){
					mCoreMesh.mStartRotation.z = FLT_MAX;
				}else{
					mCoreMesh.mStartRotation.z = Ogre::Degree(mCoreMesh.mStartRotation.z).valueRadians();
				}
			}

			s = xml.getString("fx", "anim_rotation");
			if (xml.getError() == XML_OK){
				mCoreMesh.mAnimRotation = vector3FromString(s);
				mCoreMesh.mAnimRotation.x = Ogre::Degree(mCoreMesh.mAnimRotation.x).valueRadians();
				mCoreMesh.mAnimRotation.y = Ogre::Degree(mCoreMesh.mAnimRotation.y).valueRadians();
				mCoreMesh.mAnimRotation.z = Ogre::Degree(mCoreMesh.mAnimRotation.z).valueRadians();
			}
			mCoreMesh.mTime = xml.getFloat("fx", "time_visible");
			if (xml.getError() != XML_OK){
				mCoreMesh.mTime = .5;
			}


		}else{
			mCoreMesh.mMesh = "";
		}
	}

	FXDB* FXDB::mSingleton = NULL;

	void FX::init(PyObject* pyObject){
		GameObject::init("##NOMESH##", true);
		mType = OBJ_TYPE_NOMESH;
		if (PyList_Check(pyObject) == true && PyList_Size(pyObject) > 3){
			PyObject* o = PyList_GetItem(pyObject, 3);
			if (PyString_Check(o) == false){
				throw("FX : o should be a string.");
			}
			mFXDbe = FXDB::getSingleton()->getEntry(PyString_AsString(o));
			mParentGameObject = NULL;
			mParentSceneNode = NULL;
			mBone = "";
			mType = OBJ_TYPE_NOMESH;
			mEntity=NULL;
			mSound = NULL;
			
		}else{
			throw("FX : PyObject should be an array of at least size 4");
		}
	}

	void FX::init(string name){
		GameObject::init("##NOMESH##", true);
		mFXDbe = FXDB::getSingleton()->getEntry(name);
		mParentGameObject = NULL;
		mParentSceneNode = NULL;
		mBone = "";
		mType = OBJ_TYPE_NOMESH;
		mEntity=NULL;
		mSound = NULL;
	}
	void FX::show(){
		Parent::show();
	}
	void FX::hide(){
		deactivate();
		for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
			for (int i=0;i<(*it)->getNumEmitters();i++){
				(*it)->setVisible(false);
			}
		}
		Parent::hide();
	}
	void FX::setScale(Ogre::Vector3 s){
		if (mEntity){
			mEntity->getParentNode()->setScale(s);
		}
		for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
			(*it)->getParentNode()->setScale(s);
		}

	}
	void FX::setParent(GameObject* go, string bone){
		mUseParentSceneNode = false;
		mParentGameObject = go;
		mBone = bone;
	}

	void FX::setParent(Ogre::SceneNode* scenenode){
		mUseParentSceneNode = true;
		mParentSceneNode = scenenode;
	}
	void FX::activate(){
		if (mSound){
			mSound->play();
		}
		if (mEntity){
			mEntity->setVisible(true);
			mEntity->getParentNode()->setScale(mFXDbe->mCoreMesh.mStartScale);
			
			mEntity->getParentNode()->setOrientation(Ogre::Quaternion::IDENTITY);
			if (mFXDbe->mCoreMesh.mStartRotation.x != FLT_MAX){
				mEntity->getParentNode()->pitch(Ogre::Radian(mFXDbe->mCoreMesh.mStartRotation.x));
			}else{
				mEntity->getParentNode()->pitch(Ogre::Radian(Ogre::Math::RangeRandom(0, 2*Math::PI)));
			}
			if (mFXDbe->mCoreMesh.mStartRotation.y != FLT_MAX){
				mEntity->getParentNode()->yaw(Ogre::Radian(mFXDbe->mCoreMesh.mStartRotation.y));
			}else{
				mEntity->getParentNode()->yaw(Ogre::Radian(Ogre::Math::RangeRandom(0, 2*Math::PI)));
			}
			if (mFXDbe->mCoreMesh.mStartRotation.z != FLT_MAX){
				mEntity->getParentNode()->roll(Ogre::Radian(mFXDbe->mCoreMesh.mStartRotation.z));
			}else{
				mEntity->getParentNode()->roll(Ogre::Radian(Ogre::Math::RangeRandom(0, 2*Math::PI)));
			}
			mTimeLeft = mFXDbe->mCoreMesh.mTime;
		}else{
			mTimeLeft = .5;
		}

		for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
			if ((*it)->getVisible() == false){
				(*it)->clear();
			}
			(*it)->setVisible(true);
			for (int i=0;i<(*it)->getNumEmitters();i++){
				(*it)->getEmitter(i)->setEnabled(true);
			}
		}
		for (list<DebrisInfo>::iterator it = mFXDbe->mDebrisInfo.begin();it!= mFXDbe->mDebrisInfo.end();it++){
			Ogre::Matrix4 xform;
			if (mUseParentSceneNode){
				xform = mParentSceneNode->_getFullTransform();
			}else{
				xform = mParentGameObject->getEntity()->getParentSceneNode()->_getFullTransform();
			}
			(*it).spawnDebris(xform.getTrans());
		}
	}

	void FX::deactivate(){
		if (mEntity){
			mEntity->setVisible(false);
			mTimeLeft = 0;
		}

		for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
			for (int i=0;i<(*it)->getNumEmitters();i++){
				(*it)->getEmitter(i)->setEnabled(false);
			}
		}
	}

	void FX::build(){
		mSkipHook = true;
		if (mFXDbe->mSound != ""){
			mSound = SoundManager::getSingleton()->createSound("fx\\"+mFXDbe->mSound);
		}
		mParentConnector.mBone = "GLUGLU"; // This must be non "" for the parent class, but content doesn't matter.  It's super fugly and should be fixed eventually.  Like many other things.
		mParentConnector.mGameObject = this;
		
		Parent::build();
		
		if (mFXDbe->mCoreMesh.mMesh != ""){
			mEntity = Level::getSingleton()->getSceneManager()->createEntity(mFXDbe->mCoreMesh.mMesh+ts(GameElement::getNextIdCounter()), mFXDbe->mCoreMesh.mMesh+".mesh");
			mEntity->setVisible(false);
			mEntity->setRenderQueueGroup(Level::getSingleton()->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+3);
			mEntity->setQueryFlags(SceneQueryFlag::IMMATERIAL);
		}

		for (list<string>::iterator it = mFXDbe->mParticleSystem.begin();it!= mFXDbe->mParticleSystem.end();it++){
			Ogre::ParticleSystem* ps;
			ps = Level::getSingleton()->getSceneManager()->createParticleSystem(
				"FX_"+mId+"_part_"+(*it)+"_"+ts( GameElement::getNextIdCounter() ),
				(*it)
			);
			ps->setRenderQueueGroup(Level::getSingleton()->getSceneManager()->getRenderQueue()->getDefaultQueueGroup()+3);
			ps->setVisible(false);
			ps->setQueryFlags(SceneQueryFlag::IMMATERIAL);
			mParticleSystem.push_back(ps);
		}
		if (mUseParentSceneNode){
			if (mParentSceneNode == NULL){
				throw("Trying to build an FX class without it having a proper parent (use setParent())");
			}
			if (mEntity){
				mParentSceneNode->attachObject(mEntity);
			}
			for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
				mParentSceneNode->attachObject(*it);
			}
		}else{
			if (mParentGameObject->getEntity() == NULL){
				throw("Trying to build an FX class without it having a proper parent (use setParent())");
			}
			for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
				if (mBone == ""){
					throw("Trying to attach FX to a noname Bone");
				}
				mParentGameObject->getEntity()->attachObjectToBone(mBone, (*it));
			}
			if (mEntity){
				if (mBone == ""){
					throw("Trying to attach FX to a noname Bone");
				}
				mParentGameObject->getEntity()->attachObjectToBone(mBone, mEntity);
			}
		}
	}
	bool FX::isDone(){
		if (mTimeLeft > 0){
			return false;
		}
		for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
			if ((*it)->getNumParticles() > 0){
				return false;
			}
		}
		return true;
	}

	void FX::destroy(){
		if (mSound){
			SoundManager::getSingleton()->destroySound(mSound);
		}

		if (mUseParentSceneNode){
			for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
				mParentSceneNode->detachObject(*it);
			}
			if (mEntity){
				mParentSceneNode->detachObject(mEntity);
			}
		}else{
			for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
				mParentGameObject->getEntity()->detachObjectFromBone((*it));
			}
			if (mEntity){
				mParentGameObject->getEntity()->detachObjectFromBone(mEntity);
			}
		}
		while (mParticleSystem.size() > 0){
			Level::getSingleton()->getSceneManager()->destroyParticleSystem(*mParticleSystem.begin());
			mParticleSystem.pop_front();
		}
		if (mFXDbe->mCoreMesh.mMesh != ""){
			Level::getSingleton()->getSceneManager()->destroyEntity(mEntity);
			mEntity = NULL;
		}

		Parent::destroy();
	}
	bool FX::update(double timeDelta, double parentTime){
		if (mTimeLeft > 0){
			float td = timeDelta;//Level::getSingleton()->getTimeDelta();
			mTimeLeft -= td;
			if (mEntity){
				Ogre::Node* pn = mEntity->getParentNode();
				Ogre::Vector3 scl = pn->getScale();
				Ogre::Vector3 scladd = td*mFXDbe->mCoreMesh.mAnimScale;
				pn->setScale(
					scl+scladd
					);
				pn->pitch(Ogre::Radian(td*mFXDbe->mCoreMesh.mAnimRotation.x));
				pn->yaw(Ogre::Radian(td*mFXDbe->mCoreMesh.mAnimRotation.y));
				pn->roll(Ogre::Radian(td*mFXDbe->mCoreMesh.mAnimRotation.z));
				
				if (mTimeLeft <= 0){
					mEntity->setVisible(false);
				}
			}
		}

		return Parent::update(timeDelta, parentTime);

	}

	Ogre::Node* FX::getNode(){  // Gameobject's getNode will give you crap on this.
		if (mUseParentSceneNode){
			return mParentSceneNode;
		}else{
			if (mEntity != NULL){
				return mEntity->getParentNode();
			}else{
				if (mParticleSystem.size()>0){
					return (*mParticleSystem.begin())->getParentNode();
				}else{
					throw("For now an FX requires at least an Entity or a ParticleSystem");
				}
			}
		}
	}

	void FX::limitParticleZone(Ogre::AxisAlignedBox b){
		Particle* particle;
		Ogre::Vector3 min = b.getMinimum();
		Ogre::Vector3 max = b.getMaximum();
		for (list<Ogre::ParticleSystem*>::iterator it = mParticleSystem.begin();it!= mParticleSystem.end();it++){
			Ogre::ParticleIterator pit = (*it)->_getIterator();
			while (!pit.end()){
				particle = pit.getNext();
				if (particle->position.z < min.z) particle->timeToLive=0;
				if (particle->position.y < min.y) particle->timeToLive=0;
				if (particle->position.x < min.x) particle->timeToLive=0;
				if (particle->position.z > max.z) particle->timeToLive=0;
				if (particle->position.y > max.y) particle->timeToLive=0;
				if (particle->position.x > max.x) particle->timeToLive=0;
			}
			

		}
	}


/*#ifndef ___DEF___FX_H___
#define ___DEF___FX_H___

// An FX structure doesn't do anything that can't be done through standard gameobject inheritance.  It is way less versatile but is built from speed
// * No XML allowed
// * Only 1 scenenode with a series 
// * No children
// * First iteration is nothing more than a chain of particle effects.  More to come as necessary (like sound);

namespace Shmoulette{

	class FX::public GameObject{
		class FXDBE mFXDbe;
		list<Ogre::ParticleSystem*> mParticleSystem;
		float mTimeline;

		public:
			FX(string name);
			virtual void show();
			virtual void hide();
			virtual void build();
			virtual void activate(); // Resets timeline to zero, returns all particle emitter to 0 timeline
			virtual void destroy();
			virtual bool update();
	};

};

#endif
*/


}