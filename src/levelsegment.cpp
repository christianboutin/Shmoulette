#include "stdafx.h"
#include "main.h"
#include "levelsegment.h"
#include "elements\rail.h"
#include "world\world.h"
#include "trigger\event.h"
#include "trigger\trigger.h"
#include "fx/particleemitter.h"
#include "elements\billboard.h"
#include "elements\camera.h"
#include "elements\light.h"
#include "drone\dronewave.h"
#include "2D\cell.h"
#include "trigger\popup.h"
#include "util\file.h"

namespace Shmoulette{
	/*#define XMLLoad(TAG_XML,TAG_TYPE,TAG_ELEM,TAG_VECTOR) { \
		XMLIterator it(TAG_XML); \
		it.setElemName(TAG_ELEM); \
		it.gotoZero(); \
		while(it.gotoNext() != false){ \
			Util::Log("Loading : "+it.getElem().getString("id"));\
			XML macroXml = it.getElem();\
			TAG_TYPE* obj = new TAG_TYPE(&macroXml); \
			TAG_VECTOR.push_back(obj); \
		}}*/


	LevelSegment::LevelSegment(){
		mRailType = RAIL_NONE;
	}

	void LevelSegment::Pause(){
	}
	void LevelSegment::UnPause(){
	}

	void LevelSegment::viewerCamControl(double timeDelta){

	}
	void LevelSegment::_preloadMaterial(string materialName){
		Ogre::MaterialPtr mp = Ogre::MaterialManager::getSingleton().getByName(materialName);
		mp->load();
		Ogre::Material::TechniqueIterator tit = mp->getTechniqueIterator();
		

		while (tit.hasMoreElements()){
			Ogre::Technique* t = tit.getNext();
			Ogre::Technique::PassIterator pit = t->getPassIterator();
			while (pit.hasMoreElements()){
				Ogre::Pass* p = pit.getNext();
				Ogre::Pass::TextureUnitStateIterator tui = p->getTextureUnitStateIterator();
				while (tui.hasMoreElements()){
					Ogre::TextureUnitState* tus = tui.getNext();
					tus->getName();
					tus->_load();
				}
			}
		}
	}
	Ogre::Entity* LevelSegment::addPreloadEntity(string s){
		for (list<Ogre::Entity*>::iterator it = mPreloadEntityBuffer.begin();it != mPreloadEntityBuffer.end();it++){
			if ((*it)->getName() == "LEVELSEGMENT_PRELOAD_ENTITY_"+s){
				return (*it);
			}
		}
		Ogre::Entity* e = mSceneMgr->createEntity("LEVELSEGMENT_PRELOAD_ENTITY_"+s, s+".mesh");
		for (int i=0;i<e->getNumSubEntities();i++){
			_preloadMaterial(e->getSubEntity(i)->getMaterialName());
		}
		
		mPreloadEntityBuffer.push_back(e);
		return e;
	}
	void LevelSegment::addPreloadParticleSystem(string s){
		for (list<Ogre::ParticleSystem*>::iterator it = mPreloadParticleBuffer.begin();it != mPreloadParticleBuffer.end();it++){
			if ((*it)->getName() == "LEVELSEGMENT_PRELOAD_PARTICLE_"+s){
				return;
			}
		}

		Ogre::ParticleSystem* ps = getSceneManager()->createParticleSystem("LEVELSEGMENT_PRELOAD_PARTICLE_"+s, s);
		_preloadMaterial(ps->getMaterialName());
		mPreloadParticleBuffer.push_back(ps);
	}



	void LevelSegment::setZoneInfo(Ogre::Vector3 size, Ogre::Vector3 ammoSize, Ogre::Vector2 camSize,Ogre::Vector2 camTilt,double eta){
		mZoneInfo.eta = eta;
		if (eta == 0){
			mZoneInfo.mSize.current = size;
			mZoneInfo.mAmmoSize.current = ammoSize;
			mZoneInfo.mCamRange.current = camSize;
			mZoneInfo.mCamTilt.current = camTilt;
			
			
		}else{
			mZoneInfo.mSize.wanted = size;
			mZoneInfo.mAmmoSize.wanted = size;

			mZoneInfo.mCamRange.wanted = camSize;
			mZoneInfo.mCamTilt.wanted = camTilt;

			mZoneInfo.mSize.delta = (mZoneInfo.mSize.wanted - mZoneInfo.mSize.current)/eta;
			mZoneInfo.mAmmoSize.delta = (mZoneInfo.mAmmoSize.wanted - mZoneInfo.mAmmoSize.current)/eta;
			mZoneInfo.mCamRange.delta = (mZoneInfo.mCamRange.wanted - mZoneInfo.mCamRange.current)/eta;
			mZoneInfo.mCamTilt.delta = (mZoneInfo.mCamTilt.wanted - mZoneInfo.mCamTilt.current)/eta;
		}
	}

	void LevelSegment::pushObjects(XML* xml, double deltaTime){
		XMLIterator it(xml);
		it.setElemName("level>##anything##", true);
		GameObject* tmpPtr;

		while(it.gotoNext() == true){
			string type = it.getElem().getFirstChildName();
			string content = it.getElem().getContent();

			Profiler::ProfilerEntry* ent = Profiler::ProfilerFactory::getSingleton()->getProfiler(getName()+"_"+type+"_"+it.getElem().getString("id"),"LoadObjects");
			ent->start();
	
			if (type == "rail"){
				RailHook *rh = new RailHook();
				rh->init(&it.getElem());
				rh->moveTimes(deltaTime);
				rh->capTimes(mLength);
				mVectorRailHook.push_back(rh);
				addGameObj(rh, true);
			}else if (type == "light"){
				tmpPtr = new Light();
				tmpPtr->init(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			}else if (type == "camera"){
				tmpPtr = new Camera();
				tmpPtr->init(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			}else if (type == "object"){
				tmpPtr = new GameObject();
				tmpPtr->init(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			/*}else if (type == "static"){
				tmpPtr = new StaticObj(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			}else if (type == "dynamic"){
				tmpPtr = new DynamicObj(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);*/
			}else if (type == "drone"){
				throw("Drones on the root no longer allowed, must be a child of dronewave");
			/*}else if (type == "empty"){
				tmpPtr = new DynamicEmpty(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);*/
			}else if (type == "billboard"){
				tmpPtr = new Billboard();
				tmpPtr->init(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			/*}else if (type == "waterzone"){
				tmpPtr = new WaterZone(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);*/
			}else if (type == "module"){
				typedef GameElement* (__cdecl* DLLCallback)(XML* xml);
				#ifdef _DEBUG
					HMODULE module = LoadLibrary((it.getElem().getString("name")+"d.dll").c_str());
				#else
					HMODULE module = LoadLibrary((it.getElem().getString("name")+".dll").c_str());
				#endif
				if (module == NULL){
					throw("Module "+it.getElem().getString("name")+".dll not found.");
				}
				DLLCallback dllFunc = (DLLCallback)::GetProcAddress(module, "getInstance");
				if (dllFunc == NULL){
					throw("Module "+it.getElem().getString("name")+".dll doesn't have a getInstance function.");
				}
				GameElement* ge = dllFunc(&it.getElem());//new PlayerShip(&pd);//
				
				addGameElement(ge);
			}else if (type == "world"){
			/*}else if (type == "groundplane"){
				tmpPtr = new GroundPlane(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);*/
			}else if ((type == "particleemitter") || (type=="particle")){
				tmpPtr = new ParticleEmitter();
				tmpPtr->init(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			}else if (type == "cloudzone"){
				tmpPtr = new CloudZone();
				tmpPtr->init(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			}else if (type == "append"){
				string filename = it.getElem().getString("filename");
				string context = "media/levels/"+it.getElem().getString("context");
				/*File* f;
				f = new File(filename, context);
				Util::Log("Loading : "+filename, 4);
				if (f->error() != FILE_OK){
					throw((string)("Level file not found : "+(string)filename));
				}else{
					XML xml;
					xml.setContent(f->getStrContent());
					pushObjects(&xml, deltaTime);
				}*/
				//try{
					XML xml(filename, context);
					if (xml.getError() == XML_OK){
						pushObjects(&xml, deltaTime);
					}
				//}catch(...){
				//}
				
				//delete f;
			}else if (type == "cell"){
				GameElement* gep = new Cell();
				gep->init(&it.getElem());
				gep->moveTimes(deltaTime);
				gep->capTimes(mLength);
				addGameElement(gep, true);
			}else if (type == "dronewave"){
				DroneWave* gep = new DroneWave();
				gep->init(&it.getElem());
				gep->moveTimes(deltaTime);
				gep->capTimes(mLength);
				mVectorDroneWave.push_back(gep);
			}else if (type == "trigger"){
				Trigger* gep = new Trigger(&it.getElem());
				gep->moveTimes(deltaTime);
				mVectorTrigger.push_back(gep);
			}else if (type == "anim"){
				XML* xml = &it.getElem();
				std::string name = xml->getString("object");
				GameObject* obj = getObjectByName(name);
				if (obj == NULL){
					throw("Object ("+name+") not found for anim.");
				}
				obj->addAnim(&it.getElem());
			}else if (type == "fx"){
				tmpPtr = new FXObject();
				tmpPtr->init(&it.getElem());
				tmpPtr->moveTimes(deltaTime);
				tmpPtr->capTimes(mLength);
				addGameObj(tmpPtr, true);
			}else if (type == "goanim"){
				XML* xml = &it.getElem();
				std::string name = xml->getString("object");
				GameObject* obj = getObjectByName(name);
				if (obj == NULL){
					throw("Object ("+name+") not found for goanim.");
				}
				obj->addGoAnim(&it.getElem());
			}
			std::string s;
			s = "Item : ";
			XML elem = it.getElem();

			s = elem.getString("id");
			if (elem.getError() != XML_OK) s = elem.getString("model");
			if (elem.getError() != XML_OK) s = elem.getString("type");
			if (elem.getError() != XML_OK) s = type;
			
			ent->end();
			_moveAddOnsToObj();
		};
	}
	void LevelSegment::append(XML* xml, string name, double length){
		mName += "_"+name;
		double deltaTime = mLength;
		mLength+=length;
		ResourceGroupManager::getSingleton().createResourceGroup(mName);
		pushObjects(xml, deltaTime);
	}

	list<Drone*> LevelSegment::getActiveDronesList(){
		list<Drone*>rv;
		vector<class DroneWave*>::iterator it;
		
		for (it = mVectorDroneWave.begin();it!=mVectorDroneWave.end();it++){
			list<Drone*>add;
			add = (*it)->getActiveDroneList();
			list<Drone*>::iterator addIt;
			for (addIt = add.begin();addIt != add.end();addIt++){
				rv.push_back(*addIt);
			}

		}
		return rv;
		
	}
	void LevelSegment::_destroyEverything(){
		{
			vector<pair<GameObject*,bool>>::iterator it = mVectorGameObj.begin();
			while( it != mVectorGameObj.end() ){
				if ((*it).first->isOn()){
					(*it).first->hide();
				}
				if ((*it).first->isBuild()){
					(*it).first->destroy();
				}
				if ((*it).second == false){
					delete (*it).first;
					it = mVectorGameObj.erase(it);
				}else{
					it++;
				}
			}
		}{
			vector<pair<GameElement*,bool>>::iterator it = mVectorGameElement.begin();
			while( it != mVectorGameElement.end() ){
				if ((*it).first->isOn()){
					(*it).first->hide();
				}
				if ((*it).first->isBuild()){
					(*it).first->destroy();
				}
				if ((*it).second == false){
					delete (*it).first;
					it = mVectorGameElement.erase(it);
				}else{
					it++;
				}
			}
		}{
			vector<DroneWave*>::iterator it = mVectorDroneWave.begin();
			while( it != mVectorDroneWave.end() ){
				(*it)->reset();
				it++;
			}
		}
	}
	void LevelSegment::reset(){
		mTimeAbsolute = 0;
		mTimeTrack = 0;
		mTimeDelta = 0;
		mActiveCamera = NULL;
		mCurrentHook = NULL;

		for (int i=0;i<mVectorTrigger.size();i++){
			mVectorTrigger[i]->reset();
		}

		_destroyEverything();

	}

	
	void LevelSegment::build(XML* xml, string name, double length){
		string profilerName = name;
		while(profilerName.find("\\") != string::npos){
			profilerName[profilerName.find("\\")] = '_';
		}
		while(profilerName.find("/") != string::npos){
			profilerName[profilerName.find("/")] = '_';
		}
		mProfiler.collision = Profiler::ProfilerFactory::getSingleton()->getProfiler("Collisions", "PostFrameSegmentUpdate"+profilerName);
		mProfiler.objects = Profiler::ProfilerFactory::getSingleton()->getProfiler("Objects", "PostFrameSegmentUpdate"+profilerName);
		mProfiler.update = Profiler::ProfilerFactory::getSingleton()->getProfiler("Updates", "SegmentUpdate"+profilerName);
		mProfiler.postFrame = Profiler::ProfilerFactory::getSingleton()->getProfiler("PostFrame", "SegmentUpdate"+profilerName);

		mName = name;
		mLength = length;
		GameElement::setLoadTimeSegmentPlaceholder(this);

		mLevel = Level::getSingleton(); // I do this to prevent repetitive "ifs" in the getSingleton method
		reset();
		mTimeAbsolute = 0;
		mTimeTrack = 0;
		mTimeDelta = 0;
		mActiveCamera = NULL;
		mCurrentHook = NULL;

		string zss = xml->getString("level", "zonesize");
		if (xml->getError() != XML_OK){
			mZoneInfo.mSize.current.x = 96;
			mZoneInfo.mSize.current.y = 64;
			mZoneInfo.mSize.current.z = 96;
			mZoneInfo.eta = 0;
		}else{
			mZoneInfo.mSize.current = vector3FromString(zss);
			mZoneInfo.eta = 0;
		}

		XML worldSeg = xml->extractElem("level>world");
		if (xml->getError() == XML_OK){
			mWorld = new World();//&(xml->extractElem("level>world")));
			mWorld->init(&worldSeg, this);
		}else{
			mWorld = new World();
			mWorld->init(this);
		}

		//if (mWorld->getType() != "Generic"){
			mSceneMgr = Core::getSingleton()->getRoot()->createSceneManager(mWorld->getType(), name+"_World"+ts(GameElement::getNextIdCounter()));
		//}else{
//			mSceneMgr = Core::getSingleton()->getRoot()->createSceneManager(ST_GENERIC, name+"_World"+ts(GameElement::getNextIdCounter()));
		//}



		//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
		//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
		//mSceneMgr->setShadowFarDistance(5000);
		//mSceneMgr->setShadowDirectionalLightExtrusionDistance(10000);
		//mSceneMgr->setShadowTextureSize(1024);
		//mSceneMgr->showBoundingBoxes(true);

		ResourceGroupManager::getSingleton().createResourceGroup(mName);
		string geoName = mWorld->getSceneName();
		if (geoName != ""){
			//mSceneMgr->setWorldGeometry(geoName);
			ResourceGroupManager::getSingleton().linkWorldGeometryToResourceGroup(
				mName, 
				geoName+".cfg", mSceneMgr);
			//ResourceGroupManager::getSingleton().loadResourceGroup(mName);
		}

		mContainer = mSceneMgr->getRootSceneNode()->createChildSceneNode("Container");

		pushObjects(xml);
		hookContainerToFirstAvailableRail();
	}
	void LevelSegment::load(){
		//Camera* cam = getActiveCamera();
		//gProfiler.segmentLoad->start();
		mWorld->build();

		if (mWorld->getSceneName() != ""){
			getActiveCamera()->build();
		}
		ResourceGroupManager::getSingleton().loadResourceGroup(mName,
			true, true);
		if (mWorld->getSceneName() != ""){
			getActiveCamera()->destroy();
		}
		/*if (mWorld->getSceneName() != ""){
			//Ogre::Camera* tc;
			//Ogre::Viewport* vp;
			//tc = mSceneMgr->createCamera("tempCam");

			getActiveCamera()->build();
			
			
			//vp = Core::getSingleton()->getWindow()->addViewport(tc,45,0,0,.001,.001);
			mSceneMgr->setWorldGeometry("level3_canyonapproach.cfg");
			//Core::getSingleton()->getWindow()->removeViewport(45);
			//mSceneMgr->destroyCamera(tc);
		}*/

		/*Ogre::Camera* tc;
		Ogre::Viewport* vp;
		tc = mSceneMgr->createCamera("tempCam");
		vp = Core::getSingleton()->getWindow()->addViewport(tc,45,0,0,.001,.001);
		ResourceGroupManager::getSingleton().loadResourceGroup(mName,
			false, true);
		Core::getSingleton()->getWindow()->removeViewport(45);
		mSceneMgr->destroyCamera(tc);*/
	}

	void LevelSegment::unload(){
		mWorld->destroy();
	}
	void LevelSegment::destroy(){
		int i;

		/*getActiveCamera()->destroy();
		for (i=0;i<vectorGameElement.size();i++){
			delete(vectorGameElement[i]);
		}
		return;*/

		for (i=0;i<mVectorTrigger.size();i++){
			delete(mVectorTrigger[i]);
		}
		//vector<GameObject*>::iterator it = vectorGameObj.begin();
		while(mVectorGameObj.size() > 0){
			//vectorGameObj[0]->update();
			if (mVectorGameObj[0].first->isOn()) mVectorGameObj[0].first->hide();
			if (mVectorGameObj[0].first->isBuild()) mVectorGameObj[0].first->destroy();

			delete(mVectorGameObj[0].first);
			mVectorGameObj.erase(mVectorGameObj.begin());
		}
		for (i=0;i<mVectorGameElement.size();i++){
			delete(mVectorGameElement[i].first);
		}
		for (i=0;i<mVectorDroneWave.size();i++){
			delete(mVectorDroneWave[i]);
		}
		delete(mWorld);
		ResourceGroupManager::getSingleton().destroyResourceGroup(mName);

		Core::getSingleton()->getRoot()->destroySceneManager(mSceneMgr);
	}
	LevelSegment::~LevelSegment(){}
	/*GameObject* LevelSegment::getContainer(){
		return getContainer();
	}*/
	SceneNode* LevelSegment::getContainer(){
		return mContainer;
	}
	void LevelSegment::breakCurrentLoop(){
		for (int i=0;i<mVectorTrigger.size();i++){
			if (typeid(*mVectorTrigger[i]->getEvent()) == typeid(EventList::Goto)){
				if (mVectorTrigger[i]->getTime() > getTimeTrack()){
					mVectorTrigger[i]->disable();
					break;
				}
			}
		}
	}

	void LevelSegment::hookContainerToFirstAvailableRail(){
		for (int i=0;i<mVectorRailHook.size();i++){
			if (mVectorRailHook[i]->shouldBeOn()){
				mCurrentHook = mVectorRailHook[i];
				Ogre::SceneNode* parentSceneNode;
				Ogre::SceneNode* containerSceneNode;
				Ogre::SceneNode* currentHookSceneNode;

				currentHookSceneNode = mCurrentHook->getSceneNode();
				containerSceneNode = getContainer();
				parentSceneNode = containerSceneNode->getParentSceneNode();


				if (parentSceneNode != NULL){
					parentSceneNode->removeChild(containerSceneNode);
				}


				currentHookSceneNode->addChild(containerSceneNode);
				if (getContainer()->getParentSceneNode() != mCurrentHook->getSceneNode()){
					throw("ERROR");
				}
				getContainer()->_update(true, true);
				return;
			}
		}
	}
	/*GameObject* Level::getContainer(){
		return getContainer();
		//return mCurrentHook;
	}*/

	void LevelSegment::postFrameUpdate(){
		mProfiler.postFrame->start();
		mProfiler.objects->start();
		//Util::Profile("Post Frame Update Total");

		//Util::Log("postFrameUpdate", 7);
		int i;
		for (i=0;i<mVectorGameElement.size();i++){
			if (mVectorGameElement[i].first->isOn()){
				mVectorGameElement[i].first->postFrameUpdate();
			}
		}
		vector<pair<GameObject*,bool>>::iterator it;
		for (it=mVectorGameObj.begin();it!=mVectorGameObj.end();it++){
			if ((*it).first->isOn()){
				(*it).first->postFrameUpdate(mTimeDelta, mTimeTrack);
			};
		}
		//Util::Log("postFrameUpdate - world", 7);
		mWorld->postFrameUpdate();

		//Util::Log("postFrameUpdate - drones", 7);
		for (i=0;i<mVectorDroneWave.size();i++){
			mVectorDroneWave[i]->postFrameUpdate(mTimeDelta, mTimeTrack);
		}
		//Util::Log("postFrameUpdate - preparing collisions", 7);

		vector<GameObject*>::iterator source;
		vector<GameObject*>::iterator destination;
		
		mProfiler.objects->end();
		mProfiler.collision->start();

		vector<GameObject*>::iterator source_end = mVectorCollision.end();
		if (mVectorCollision.size() > 0){
			source_end--;
		}
		for (source = mVectorCollision.begin(); source != source_end;source++){
			for (destination = source+1; destination != mVectorCollision.end();destination++){

				SceneQueryFlag::SceneQueryFlag srcFlag = (*source)->getQueryFlags();
				SceneQueryFlag::SceneQueryFlag dstFlag = (*destination)->getQueryFlags();
				
				SceneQueryFlag::SceneQueryFlag result = (SceneQueryFlag::SceneQueryFlag)((int)srcFlag & (int)dstFlag);

				SceneQueryFlag::SceneQueryFlag extraSkip = (SceneQueryFlag::SceneQueryFlag)(srcFlag | dstFlag);

				if (extraSkip == SceneQueryFlag::AMMO){ // Ammos shouldn't be tested against other ammos.  End of story.
					result = SceneQueryFlag::ALL_MATERIAL; // Another way of skipping the test
				}
				if (extraSkip == (SceneQueryFlag::AMMO_FRIENDLY|SceneQueryFlag::DRONE_FRIENDLY)){ // No collision test required between same-sides
					result = SceneQueryFlag::ALL_MATERIAL; // Another way of skipping the test
				}
				if (extraSkip == (SceneQueryFlag::AMMO_FRIENDLY|SceneQueryFlag::PLAYER_SHIP)){ // No collision test required between same-sides
					result = SceneQueryFlag::ALL_MATERIAL; // Another way of skipping the test
				}
				if (extraSkip == (SceneQueryFlag::AMMO_ENEMY|SceneQueryFlag::DRONE_ENEMY)){ // No collision test required between same-sides
					result = SceneQueryFlag::ALL_MATERIAL; // Another way of skipping the test
				}


				
				if (result==0){
					//Util::Profile("Checking Collisions "+(*source)->getId()+" with "+(*destination)->getId());
					COLLISION_RESULT cr;
					cr = (*source)->isColliding((*destination));
					if (cr.mOccured){
						COLLISION_RESULT cr2 = (*source)->isColliding((*destination)); // Why?
						//Util::Log("Collision : "+(*source)->getId()+" with "+(*destination)->getId(),0);
						if ((cr.mInvulnerable & INVULNERABLE_SOURCE) != INVULNERABLE_SOURCE){
							(*source)->hit((*destination)->getDamagePackage(), (*destination), NULL, cr.mDistance, mTimeDelta);
						}
						if ((cr.mInvulnerable & INVULNERABLE_DESTINATION) != INVULNERABLE_DESTINATION){
							(*destination)->hit((*source)->getDamagePackage(), (*source), NULL, cr.mDistance, mTimeDelta);
						}
					}
					//Util::Profile("Checking Collisions "+(*source)->getId()+" with "+(*destination)->getId());
				}

				//}else{
					//throw("Never should happen (source and destination are the same in collision detection)");
				//}
			}
			{
				if ((*source)->getParentConnector().mGameObject == NULL){  // If a child smacks the floor, who cares.
					Vector3 absPos = (*source)->getNode()->_getDerivedPosition();
					Real h = mWorld->getHeightAt(absPos.x, absPos.z);
					if (absPos.y < h){
						DamagePackage dp;
						dp.setDamageCycle(DAM_CYCLE_INSTANT);
						dp.setDamageTotal(DAM_TOTAL_ADD);
						dp.addDamage(DAM_TYPE_CONCUSSION, 10000);
						(*source)->getNode()->translate(Ogre::Vector3(0,(h-absPos.y)+1,0), Ogre::Node::TS_WORLD);
						(*source)->hit(dp, NULL, NULL, FLT_MAX, mTimeDelta);
					}
				}
			}
		}
		//Util::Log("postFrameUpdate - collisions_end", 7);

		while(mVectorCollision.size() > 0){
			mVectorCollision.pop_back();
		}
		mProfiler.collision->end();
		_moveAddOnsToObj();

		mProfiler.postFrame->end();
	}
	void LevelSegment::_moveAddOnsToObj(){
		vector<pair<GameObject*, bool>>::iterator it;
		if (mVectorGameObjAddon.size() > 0){
			for (it=mVectorGameObjAddon.begin();it!=mVectorGameObjAddon.end();it++){
				mVectorGameObj.push_back((*it));
			}
			while(mVectorGameObjAddon.size() > 0){
				mVectorGameObjAddon.erase(mVectorGameObjAddon.begin());
			}
		}
	}

	void LevelSegment::buildStartObjects(){
		vector<pair<GameObject*,bool>>::iterator it;
		for (it=mVectorGameObj.begin();it!=mVectorGameObj.end();it++){
			if (typeid(*(*it).first) != typeid(Shmoulette::Camera)){
				if ((*it).first->shouldBeBuild()){
					(*it).first->build();
				}
			}else{
				if ((*it).first->shouldBeBuild()){
					setActiveCamera(dynamic_cast<Shmoulette::Camera*>((*it).first));
				}
			}
		}
	}

	/*void LevelSegment::destroyViewport(){
		vector<GameObject*>::iterator it;
		for (it=vectorGameObj.begin();it!=vectorGameObj.end();it++){
			if (typeid(*(*it)) == typeid(Shmoulette::Camera)){
				if ((*it)->isBuild()){
					(*it)->destroy();
				}
			}
		}
	}*/


	double LevelSegment::update(double timeDelta){
		mProfiler.update->start();
		MACRO_PROFILER_GET("A", "LevelSegmentUpdate")->start();
		if (mTimeTrack >= 3.9){
			int g=0;
		}

		if (mZoneInfo.eta > 0){
			mZoneInfo.mSize.current += mZoneInfo.mSize.delta*timeDelta;
			mZoneInfo.mAmmoSize.current += mZoneInfo.mAmmoSize.delta*timeDelta;
			mZoneInfo.mCamRange.current += mZoneInfo.mCamRange.delta*timeDelta;
			mZoneInfo.mCamTilt.current += mZoneInfo.mCamTilt.delta*timeDelta;
			mZoneInfo.eta -= timeDelta;
			
			if (mZoneInfo.eta <= 0){
				mZoneInfo.mSize.current = mZoneInfo.mSize.current;
				mZoneInfo.mAmmoSize.current = mZoneInfo.mAmmoSize.current;
				mZoneInfo.mCamRange.current = mZoneInfo.mCamRange.current;
				mZoneInfo.mCamTilt.current = mZoneInfo.mCamTilt.current;
			}
		}
		MACRO_PROFILER_GET("A", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("B", "LevelSegmentUpdate")->start();

		if (mForceSecondUpdate == true){
			while(mVectorCollision.size() > 0){
				mVectorCollision.pop_back();
			}
		}
		mForceSecondUpdate = false;
		unsigned int i;

		MACRO_PROFILER_GET("B", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("C", "LevelSegmentUpdate")->start();

		if (mCurrentHook != NULL){
			if (mCurrentHook->shouldBeOn() == false){
				hookContainerToFirstAvailableRail();
			}
		}else{
			hookContainerToFirstAvailableRail();
		}

		/*if (Core::getSingleton()->getKey(OIS::KC_SPACE)->isPressed()){
			if (mLevel->isCinema() == true){
				double furthestTime=FLT_MAX;
				for (i=0;i<vectorTrigger.size();i++){
					if (typeid(*vectorTrigger[i]->getEvent()) == typeid(EventList::CinemaOff)){
						if (
							vectorTrigger[i]->getTime() > mTimeTrack && 
							vectorTrigger[i]->getTime() < furthestTime){
							furthestTime = vectorTrigger[i]->getTime();
						}
					}
				}
				if (furthestTime != FLT_MAX){
					timeDelta = furthestTime-mTimeTrack;
				}else{
					timeDelta = FLT_MAX;
				}
			}
		}*/
		MACRO_PROFILER_GET("B", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("C", "LevelSegmentUpdate")->start();

		mTimeDelta = timeDelta; // Used by all members of the level to know how to move;
		mTimeAbsolute += mTimeDelta;
		mTimeTrack += mTimeDelta;
		if (mTimeAbsolute < 0){
			mTimeDelta -= mTimeAbsolute;
			mTimeAbsolute = 0;
			mTimeTrack = 0;
		}
		if (mTimeTrack > mLength){
			_destroyEverything();
			return mTimeTrack-mLength;
		}
		MACRO_PROFILER_GET("C", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("D", "LevelSegmentUpdate")->start();

		for (i=0;i<mVectorGameElement.size();i++){
#ifdef PROFILER_ON
			mVectorGameElement[i].first->getProfiler()->start();
#endif
			mVectorGameElement[i].first->updateIfOn(mTimeDelta, mTimeTrack);
#ifdef PROFILER_ON
			mVectorGameElement[i].first->getProfiler()->end();
#endif
		}
		MACRO_PROFILER_GET("D", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("E", "LevelSegmentUpdate")->start();
		vector<pair<GameObject*,bool>>::iterator it;
		for (it=mVectorGameObj.begin();it!=mVectorGameObj.end();it++){
			bool terminate = (*it).first->isMarkedForTermination();
#ifdef PROFILER_ON
			(*it).first->getProfiler()->start();
#endif
			if (!terminate) terminate == (*it).first->updateIfOn(mTimeDelta, mTimeTrack);
#ifdef PROFILER_ON
			(*it).first->getProfiler()->end();
#endif
			
			if (terminate == true){
				removeCollisionTest((*it).first);
				GameObject* tmp = (*it).first;
				
				vector<std::pair<GameObject*, bool>>::iterator prev_it = it-1;
				
				mVectorGameObj.erase(it);
				it = prev_it;
				if (tmp->isOn()) tmp->hide();
				if (tmp->isBuild()) tmp->destroy();

				delete tmp;
			};
		}
		MACRO_PROFILER_GET("E", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("F", "LevelSegmentUpdate")->start();

		mWorld->updateIfOn(mTimeDelta, mTimeTrack);
		/*{
			vector<GameElement*>::iterator it;
			for (it=vectorGameElement.begin();it!=vectorGameElement.end();it++){
				bool terminate = (*it)->isMarkedForTermination();
				if (!terminate) terminate == (*it)->updateIfOn();
				
				if (terminate == true){
					GameElement* tmp = *it;
					vector<GameElement*>::iterator prev_it = it-1;
					vectorGameElement.erase(it);
					it = prev_it;
					if (tmp->isOn()) tmp->hide();
					if (tmp->isBuild()) tmp->destroy();

					delete tmp;
				};
			}
		}*/
		MACRO_PROFILER_GET("F", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("G", "LevelSegmentUpdate")->start();

		for (i=0;i<mVectorTrigger.size();i++){
			mVectorTrigger[i]->checkAndRun(getTimeTrack());
		}
		MACRO_PROFILER_GET("G", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("H", "LevelSegmentUpdate")->start();
		for (i=0;i<mVectorDroneWave.size();i++){
			if (mVectorDroneWave[i]->updateIfOn(mTimeDelta, mTimeTrack)){
				// TODO : Remove item from list, cleanup
			};
		}
		MACRO_PROFILER_GET("H", "LevelSegmentUpdate")->end();
		MACRO_PROFILER_GET("I", "LevelSegmentUpdate")->start();

		if (mForceSecondUpdate == true){
			update(.02);
		}
		MACRO_PROFILER_GET("I", "LevelSegmentUpdate")->end();
		mProfiler.update->end();

		return false;
	}
	void LevelSegment::addGameObj(GameObject* obj, bool perma){
		pair<GameObject*, bool> p;
		p.first = obj;
		p.second = perma;
		mVectorGameObjAddon.push_back(p);
	}

	void LevelSegment::addCollisionTest(GameObject* object){
#ifdef DEBUG
		if (object->getFilename() == "DefaultBeam"){
			int g=0;
		}
		for (vector<GameObject*>::iterator it = mVectorCollision.begin();it!= mVectorCollision.end();it++){
			if ((*it) == object){
				throw("Trying to add the same object to the collision chain twice.  Why?");
			}
		}
#endif
		mVectorCollision.push_back(object);
	}

	void LevelSegment::removeCollisionTest(GameObject* object){
		vector<GameObject*>::iterator it;
		for (it = mVectorCollision.begin(); it != mVectorCollision.end();it++){
			if ((*it) == object){
				mVectorCollision.erase(it);
				break;
			}
		}
	}

	GameObject* LevelSegment::getCurrentHook(){
		if (mCurrentHook == NULL) hookContainerToFirstAvailableRail();// One last chance for it to be a hook
		/*if (mCurrentHook == NULL){ // Still NULL, check if there's any hook at all, if yes then it's an exception
			if (hasHooks()){
				throw("If a level segment has a hook, it must be present in the beginning");
			}
		}*/
		return (GameObject*)mCurrentHook;
	};
	void LevelSegment::fogChange(FOG_INFO newFog, Ogre::Real timer){
		mWorld->fogChange(newFog, timer);
	}


	/*COLLISION_RESULT LevelSegment::collisionTest(GameObject* obj, IFF iff){
		COLLISION_RESULT rv;
		if (iff == IFF_TARGET_ENEMY){
			for (unsigned int i=0;i<vectorGameObj.size();i++){
				if (vectorGameObj[i] != obj){
					if (vectorGameObj[i]->isOn()){
						if (vectorGameObj[i]->isSolid() == true){
							rv = vectorGameObj[i]->isColliding(obj);
							if (rv.mOccured){
								return rv;
							}
						}
					}
				}
			}
			for (unsigned int i=0;i<vectorDroneWave.size();i++){
				if (vectorDroneWave[i]->isOn()){
					COLLISION_RESULT rv = vectorDroneWave[i]->collisionTest(obj);
					if (rv.mOccured != false){
							return rv;
					}
				}
			}
		}else{
			rv = mLevel->getPlayerShip()->isColliding(obj);
			return rv;
		}
		rv.mOccured = false;
		return rv;
	}*/

	list<Ogre::Entity*> LevelSegment::getEntityList(){
		list<Ogre::Entity*> rv;
		{
			vector<pair<GameObject*,bool>>::iterator it;
			for (it = mVectorGameObj.begin();it!=mVectorGameObj.end();it++){
				//list<Ogre::Entity*> objEnt = (*it)->getEntityList();
				if ((*it).first->getEntity() != NULL){
					rv.push_back((*it).first->getEntity());
				}
			}
		}
		{
			list<Drone*> dl = getActiveDronesList();
			list<Drone*>::iterator it;
			for (it=dl.begin();it!=dl.end();it++){
				if ((*it)->getEntity() != NULL){
					rv.push_back((*it)->getEntity());
				}
			}
		}
		return rv;
	}
	void LevelSegment::setActiveCamera(Shmoulette::Camera* cam){
		mActiveCamera = cam;
		if (cam != NULL){
			Level::getSingleton()->setCamera(cam->getCamera());
		}

	}
	Shmoulette::Camera* LevelSegment::getActiveCamera(){
		if (mActiveCamera == NULL){
			throw("No Active Camera");
		}
		return mActiveCamera;
	}

	GameObject* LevelSegment::getObjectBySceneNode(Ogre::SceneNode* node){
		vector <pair<GameObject*,bool>>::iterator it;
		for (it = mVectorGameObj.begin();it != mVectorGameObj.end();it++){
			if ((*it).first->isBuild()){ // If it's not built, then f'get it.  the node could be attached to a dead entity and all those sorts of nasty things
				if ((*it).first->getNode() == node){
					return (*it).first;
				}
			}
		}
	}

	GameObject* LevelSegment::getObjectByName(string name){
		size_t firstSlashPos = name.find("/");
		if (firstSlashPos != string::npos){
			string rootName = name.substr(0,firstSlashPos);
			GameObject* rootObj = getObjectByName(rootName);
			if (rootObj == NULL) return NULL;
			return rootObj->getChild(name.substr(firstSlashPos+1));
		}else{
			vector <pair<GameObject*,bool>>::iterator it;
			for (it = mVectorGameObj.begin();it != mVectorGameObj.end();it++){
				if ((*it).first->getId() == name){
					return (*it).first;
				}
			}
		}
		if (name == "playership"){
			return Level::getSingleton()->getPlayerShip();
		}
		return NULL;
	}



}