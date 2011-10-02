#include "stdafx.h"
#include "OgreResourceGroupManager.h"


#include "main.h"
//#include "weapon\\weapon.h"
#include "elements\playership.h"


#include "drone\dronewave.h"
#include "2d\text.h"
#include "2d\talkwindow.h"
#include "2D/progressbar.h"
#include "world\world.h"
#include "trigger\event.h"
#include "trigger\trigger.h"
#include "elements\camera.h"
#include "elements\light.h"
#include "level.h"
#include "util\util.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gui/cinemabars.h"
#include <winbase.h>
#include <wtypes.h>
#include "util\file.h"
#include "util\profiler.h"
#include "fx\fx.h"
#include "input.h"
#ifdef ALPHA_SCREENSHOT
#include "quickgui.h"
#endif
namespace Shmoulette{
#define MACRO_EACHPLAYERSHIP(method){\
{\
	list<Shmoulette::PlayerShip*>::iterator it;\
	for (it=mPlayerShip.begin();it!=mPlayerShip.end();it++){\
		(*it)->method;\
	}\
}\
}

	
	//.281?
_ShmouletteExport Level* Level::mSingleton = NULL;

WeaponChain::WeaponChain(XML& xml){
	fromXml(xml);
}

void WeaponChain::fromXml(XML& xml){
	mTimeBetweenShots = xml.getFloat("time_between_shots");
	XMLIterator it(&xml);
	it.setElemName("weaponchain>weapon");
	it.gotoZero();
	while(it.gotoNext()==true){
		WeaponInfo wi(&it.getElem());
		/*wd.DLL = it.getElem().getString("dll");
		wd.XML = it.getElem().getString("xml");
		wd.mount = it.getElem().getString("mount");
		mWeapon.push_back(wd);*/
		mWeapon.push_back(wi);

	}
}

void PlayerDetails::fromXml(XML& xml){
	mMesh = xml.getString("ship", "mesh");
	mDll = xml.getString("ship", "dll");
	mShield = xml.getFloat("ship", "shield");
	mShieldRegen = xml.getFloat("ship", "shield_regen");
	mArmour = xml.getFloat("ship", "armour");
	mEnergy = xml.getFloat("ship", "energy");
	mEnergyRegen = xml.getFloat("ship", "energy_regen");
	mAmmo = xml.getInt("ship", "ammo");
	mSpeed = xml.getFloat("ship", "speed");
	mDeathFX = xml.getString("death_fx");
	if (xml.getError() != XML_OK){
		mDeathFX = "";
	}

	{
		XMLIterator it(&xml);
		it.setElemName("ship>weaponchain");
		it.gotoZero();
		while(it.gotoNext()==true){
			WeaponChain wc(it.getElem());
			mWeaponChain.push_back(wc);
		}
	}{
		XMLIterator it(&xml);
		it.setElemName("ship>reactor");
		it.gotoZero();
		while(it.gotoNext()==true){
			mReactorNodes.push_back(it.getElem().getString("childName"));
		}
	}


/*
	XMLIterator it(&xml);
	it.setElemName("");
	it.gotoZero();
	it.gotoNext();
	XML elem;

	if (it.gotoNext() == false){
		throw("Ship Definition File Missing 3 Weapon Chain");
	}
	elem = it.getElem();

	if (it.gotoNext() == false){
		throw("Ship Definition File Missing 2 Weapon Chain");
	}
	elem = it.getElem();
	mSecondaryWeapon.fromXml(elem);

	if (it.gotoNext() == false){
		throw("Ship Definition File Missing 1 Weapon Chain");
	}
	elem = it.getElem();
	mSuperWeapon.fromXml(elem);*/
	int g=0;
}

PlayerDetails::PlayerDetails(XML& xml){
	fromXml(xml);
}


static int profilerStep = 0;

//Singleton<Root>::ms_Singleton = 0;

void Level::_resetStdVar(){
	mLogKeyUp = true;
	mLoadedSegments = 0;
	mMaxFrameTime = .06;
	mMinFrameTime = 0;
	mNeedPostFrameUpdate = false;
	mTimer = 0;
	mNbResGroups=1;
	mSpeedModifier.wanted = 1;
	mSpeedModifier.current = 1;
	mSpeedModifier.acceleration = 0;
	mPaused = false;
	mDebugCamera = false;
	mEndSegmentFlag = false;
	mIsCinema = false;  // Actually, things start in cinema mode, see ::load for the final touch;
	mTimeAbsolute = 0;
	mTimeCatchup = 0;
	mCurrentSegment = NULL;
	mReset = false;
	mFastForward = false;
}

Level::Level(){
	_resetStdVar();
	mViewport= NULL;

	FXDB* fxdbcreator = FXDB::getSingleton();  //variable is unused, but we call the getSingleton here because it inits the db
	mProfiler.timeWithOgre = Shmoulette::Profiler::ProfilerFactory::getSingleton()->getProfiler("TimeWithOgre","Runtime");
	mProfiler.postFrametimeWithOgre = Shmoulette::Profiler::ProfilerFactory::getSingleton()->getProfiler("PostFrameTimeWithOgre","Runtime");
	mProfiler.completeFrameCycle = Profiler::ProfilerFactory::getSingleton()->getProfiler("CompleteFrameCycle", "Runtime");
	mProfiler.started = false;

	mInputBridge = new MouseAndKeyboardInputBridge();
	//mInputBridge = new KeyboardOnlyInputBridge();
	


	mSensorOverlay = new SensorOverlay();



	//mCamera = NULL;


	mHudDll = "HudUSCA";
	
	mCinemaBars = new Gui::CinemaBars(500);
	//mPlayerShip = NULL;
	//cinema(true);
	mCinemaBars->deactivate();
	

	/*NewTexture = (Texture *)TextureManager->create("T0");
	NewTexture->create(8, 8);
	NewMaterial->addTextureLayer("T0"); */
	registerPython();

}

/*void Level::openLevelFile(string filename){
	Util::Log("Loading : "+(string)filename);
	SetCurrentDirectory("media\\levels\\");
	mFile = new File(filename);
	if (mFile->error() != FILE_OK){
		throw((string)("Level file not found : "+(string)filename));
	}
	mXml = new XML();
	mXml->setContent((char*)mFile->content());
	SetCurrentDirectory("..\\..\\");
	Util::Log(" ... Load Complete\n");
}
void Level::closeLevelFile(){
	Util::Log("Closing XML File...");
	delete mXml;
	delete mFile;
	Util::Log("... XML File Closed");
}*/

void Level::removeAllPlayers(){
	mPlayerDetails.clear();
}
void Level::addPlayer(PlayerDetails pd){
	mPlayerDetails.push_back(pd);
}




void Level::addSegment(string filename, string context, string id, float length, bool append){
	//mProgressBar->levelParseStarted(filename);
	XML* xml = new XML(filename, context);
	addSegment(
		xml, 
		id,
		length, append);
	
	/*File* f;
	f = new File(filename, context);
	Util::Log("Loading : "+filename, 4);
	if (f->error() != FILE_OK){
		throw((string)("Level file not found : "+(string)filename));
	}else{
		XML xml;
		xml.setContent(f->getStrContent());
		addSegment(
			xml, 
			id,
			length, append);
			//mCurrentSegment = new LevelSegment();
			//mCurrentSegment->build(&xml, elem.getString("name").substr(0,elem.getString("name").length()-4), elem.getFloat("length"));
			//mSegment.push_back(mCurrentSegment);
	}
	delete f;*/
	//mProgressBar->levelParseEnded(filename);
	
}




void Level::addSegment(XML* xml, string id, float length, bool append){
	Profiler::ProfilerEntry* ent = Profiler::ProfilerFactory::getSingleton()->getProfiler("Segment Loading "+id,"LoadSegments");
	ent->start();

	if (append && mCurrentSegment != NULL){
		mCurrentSegment->append(xml, id, length);
	}else{
		mCurrentSegment = new LevelSegment();
		mCurrentSegment->build(xml, id, length);
		mSegment.push_back(mCurrentSegment);
	}
	ent->end();
	
///	mNbResGroups++;
}

PyObject* Level::setSkyBox(PyObject *self, PyObject* args){
	char* newMat;
	float dist;
	PyArg_ParseTuple(args, "sf", &newMat, &dist);
	if (strlen(newMat) > 0){
		getSingleton()->getSceneManager()->setSkyBox(true, newMat, dist);
	}else{
		getSingleton()->getSceneManager()->setSkyBox(false, newMat, dist);
	}
	Py_RETURN_NONE;

}

PyObject* Level::getTimeTrack(PyObject *self, PyObject* args){
	return PyFloat_FromDouble(getSingleton()->mCurrentSegment->getTimeTrack());
}

PyObject* Level::getTimeDelta(PyObject *self, PyObject* args){
	return PyFloat_FromDouble(getSingleton()->mCurrentSegment->getTimeDelta());
}

PyObject* Level::getObjectPtr(PyObject *self, PyObject* args){
	GameObject* ptr;
	char* objName;
	PyArg_ParseTuple(args, "s", &objName);

	ptr = getSingleton()->getCurrentSegment()->getObjectByName(objName);
	if (ptr == NULL){
		throw("Python : Object not found : "+(string)(objName));
	}
	
	PyObject* rv = PyLong_FromLong((long)ptr);

	return rv;
}

PyObject* Level::getPlayerShipObjectPtr(PyObject *self, PyObject* args){
	PyObject* rv = PyLong_FromLong((long)Level::getSingleton()->getPlayerShip());

	return rv;
}


PyObject* Level::setSecondLifeHud(PyObject *self, PyObject* args){
	GameObject* target;
	PyArg_ParseTuple(args, "l", &target);
	if (target != NULL){
		getSingleton()->mHud->setSecondLifeTarget(target);
	}else{
		getSingleton()->mHud->clearSecondLifeTarget();
	}
	
	Py_RETURN_NONE;
	//return rv;
}

PyObject* Level::addXmlBranch(PyObject *self, PyObject* args){
	char* objName;
	PyArg_ParseTuple(args, "s", &objName);
	string s = objName;
	s = "<level>"+s+"</level>";
	getSingleton()->getCurrentSegment()->pushObjects(&XML(s));
	Py_RETURN_NONE;

	//return PyFloat_FromDouble(getSingleton()->mCurrentSegment->getTimeDelta());
}

static PyMethodDef LevelMethods[] = {
	{"getTimeTrack", Level::getTimeTrack, METH_VARARGS,"Dis or Dat."},
	{"getTimeDelta", Level::getTimeDelta, METH_VARARGS,"Dis or Dat."},
	{"getObjectPtr", Level::getObjectPtr, METH_VARARGS,"Dis or Dat."},
	{"setSkyBox", Level::setSkyBox, METH_VARARGS,"Dis or Dat."},
	{"getPlayerShipObjectPtr", Level::getPlayerShipObjectPtr, METH_VARARGS,"Dis or Dat."},
	{"setSecondLifeHud", Level::setSecondLifeHud, METH_VARARGS,"Dis or Dat."},
	{"addXmlBranch", Level::addXmlBranch, METH_VARARGS,"Dis or Dat."},
    {NULL, NULL, 0, NULL}
};


void Level::registerPython(){
	Py_InitModule("sh_level", LevelMethods);
}


void Level::uninit(){
	for (mSegmentIterator=mSegment.begin();mSegmentIterator!= mSegment.end();mSegmentIterator++){
		mCurrentSegment = (*mSegmentIterator);
		(*mSegmentIterator)->destroy();
		delete (*mSegmentIterator);
	}
	mSegment.clear();
	ResourceGroupManager::getSingleton().destroyResourceGroup("Level");
}

void Level::init(string filename){
	_resetStdVar();
	mViewport= NULL;

	//mProgressBar->draw(Core::getSingleton()->getWindow());
	//File::setLoadCallback(mProgressBar);
	ResourceGroupManager::getSingleton().createResourceGroup("Level");

	//File* lf;
	XML* lx;
	/*lf = new File(filename, "media\\levels");
	if (lf->error() != FILE_OK){
		throw((string)("Level file not found : "+filename));
	}
	lx = new XML(lf->getStrContent());*/
	lx = new XML(filename, "media\\levels");

	string loadingSoundString;
	loadingSoundString = lx->getString("level","loadingsound");
	if (lx->getError() == XML_OK){
		mLoadingSound = SoundManager::getSingleton()->createSound(loadingSoundString+".ogg", true);
		mLoadingSound->play();
	}else{
		mLoadingSound = NULL;
	}
	mTimer = 0;
	
	/*mContainer->setSolid(false);
	mContainer->setType(OBJ_TYPE_CONTAINER);
	mContainer->build();*/

	
	//openLevelFile(filename);
	//Util::Log("Loading World...");

	
	XMLIterator XMLIt(lx);
	XMLIt.setElemName("level>segment");
	XMLIt.gotoZero();
	
	//LevelSegment* seg;


	while(XMLIt.gotoNext()==true){
		XML elem = XMLIt.getElem();
		addSegment(
			elem.getString("name"), 
			"media\\levels", 
			elem.getString("name").substr(0,elem.getString("name").length()-4),
			elem.getFloat("length"), elem.getString("append")=="1");

		
		/*File* f;
		XML elem = XMLIt.getElem();
		f = new File(elem.getString("name"),"levels");
		Util::Log("Loading : "+elem.getString("name"), 4);
		if (f->error() != FILE_OK){
			throw((string)("Level file not found : "+(string)filename));
		}else{
			XML xml;
			xml.setContent(f->getStrContent());
			addSegment(
				xml, 
				elem.getString("name").substr(0,elem.getString("name").length()-4),
				elem.getFloat("length"));
			
			//mCurrentSegment = new LevelSegment();
			//mCurrentSegment->build(&xml, elem.getString("name").substr(0,elem.getString("name").length()-4), elem.getFloat("length"));
			//mSegment.push_back(mCurrentSegment);
		}
		delete f;*/
	}
	if (mSegment.size() == 0) throw ("Your level has 0 segments.");
	delete lx;
	//delete lf;

//	closeLevelFile();
}

void Level::addResource(string name, string type, string groupName){
	if (name==""){
		throw("Adding null filename");
	}
	if (type != "Material"){
		name = name+"."+type;
	}
	list<string>::iterator it;
	bool found = false;
	if (mResource.size() > 0){
		for (it = mResource.begin();it != mResource.end();it++){
			if ((*it) == name){
				found = true;
			}
		}
	}
	//Ogre::ResourceGroupManager::getSingleton().resourceExists(groupName, name);
	if (!found){
		mResource.push_back(name);
		Ogre::ResourceGroupManager::getSingleton().declareResource(name, type, groupName);
	}
}

void Level::addResourceContext(string context, string parentContext, string groupName){
	if (context==""){
		throw("Adding null context");
	}
	
	list<string>::iterator it;
	bool found = false;
	if (mResourceLocation.size() > 0){
		for (it = mResourceLocation.begin();it != mResourceLocation.end();it++){
			if ((*it) == parentContext+context){
				found = true;
			}else{
				if (stricmp((*it).c_str(), (parentContext+context).c_str()) == 0){
					throw("Case-sensitivity issue "+(*it));
				}
			}
		}
	}

	if (!found){
		mResourceLocation.push_back(parentContext+context);
		if (parentContext != ""){
			parentContext+="\\";
		}
		Ogre::ResourceGroupManager::getSingleton().
			addResourceLocation("media\\"+parentContext+context, "FileSystem", groupName);
	}
}

void Level::load(){
	/*typedef Shmoulette::PlayerShip* (CALLBACK* DLLCallback)();

	HMODULE playerShipModule = LoadLibrary((LPCSTR)"MyDLL");
	addResourceContext("models\\angel");
	DLLCallback dllFunc = (DLLCallback)::GetProcAddress(playerShipModule, "getInstance");
	mPlayerShip = dllFunc();*/
	
	GameElement::setLoadTimeSegmentPlaceholder(*(mSegment.begin()));
	list<PlayerDetails>::iterator it;
	for (it=mPlayerDetails.begin();it != mPlayerDetails.end();it++){
		typedef PlayerShip* (__cdecl* DLLCallback)(PlayerDetails& pd);

#ifdef _DEBUG
		HMODULE playerShipModule = LoadLibrary((((*it).mDll)+"d").c_str());
#else
		HMODULE playerShipModule = LoadLibrary((((*it).mDll)).c_str());
#endif
		DLLCallback dllFunc = (DLLCallback)::GetProcAddress(playerShipModule, "getInstance");
		PlayerDetails pd;
		pd = (*it);
		PlayerShip* ps = dllFunc(pd);//new PlayerShip(&pd);//
		mPlayerShip.push_back(ps);
	}
	mInputBridge->activate();
	if (mHudDll != ""){
		typedef Hud* (__cdecl* DLLCallback)(string prefix);
#ifdef _DEBUG
		HMODULE hudModule = LoadLibrary(((mHudDll)+"d").c_str());
#else
		HMODULE hudModule = LoadLibrary(((mHudDll)).c_str());
#endif
		DLLCallback dllFunc = (DLLCallback)::GetProcAddress(hudModule, "getInstance");
		mHud = dllFunc("boli");//new PlayerShip(&pd);//
	}else{
		mHud = new HudBlank();
	}

	{
		mSegmentIterator = mSegment.begin();
		mCurrentSegment = *(mSegmentIterator);


		//mProgressBar->start(mNbResGroups, mNbResGroups);

		GameElement::setLoadTimeSegmentPlaceholder(mCurrentSegment);

		// Initialise the rest of the resource groups, parse scripts etc
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		list<LevelSegment*>::iterator it;

		ResourceGroupManager::getSingleton().loadResourceGroup("Level",
			true, true);

		if (mHud != NULL){
			mHud->build();
		}

		for (it = mSegmentIterator;it!= mSegment.end();it++){
			(*it)->load();
			mCurrentSegment = (*it);
			(*it)->buildStartObjects();
			mLoadedSegments++;
		}
		mCurrentSegment = *(mSegmentIterator);

		/*Ogre::ResourceManager::ResourceMapIterator iter = Ogre::TextureManager::getSingleton().getResourceIterator();
		while (iter.hasMoreElements())
		{
			Ogre::ResourceManager::ResourceHandleMap::mapped_type resource = iter.getNext();
			if (Ogre::ResourceGroupManager::getSingleton().resourceExists("Level", resource->getName()))
			{
				resource->load();
			}
		} */

		// Back to full rendering
		/*mSceneMgr->clearSpecialCaseRenderQueues();
		mSceneMgr->setSpecialCaseRenderQueueMode(SceneManager::SCRQM_EXCLUDE);*/

		//mProgressBar->finish();
		//File::setLoadCallback(NULL);

		if (mHud != NULL){
			mHud->show();
		}

		Util::Log("Level Loading Complete", 4);
	}
	cinema(true);

	//Level::cinema(false);

	/*while(!Core::getSingleton()->getKey[KC_ESCAPE].isPressed()){


	};*/



}

void Level::unload(){

	if (mHud != NULL){
		mHud->show();
		mHud->destroy();
		delete mHud;
	}
	if (mCurrentSegment->hasHooks()){
		MACRO_EACHPLAYERSHIP(destroy());
	}
	for (list<class PlayerShip*>::iterator it = mPlayerShip.begin();it != mPlayerShip.end();it++){
		delete (*it);
	}

	for (mSegmentIterator=mSegment.begin();mSegmentIterator!= mSegment.end();mSegmentIterator++){
		mCurrentSegment = (*mSegmentIterator);
		(*mSegmentIterator)->unload();
	}
	mCurrentSegment = NULL;
	GameElement::setLoadTimeSegmentPlaceholder(NULL);
	mInputBridge->deactivate();

	mPlayerShip.clear();
}

list<Ogre::Entity*> Level::getEntityList(){
	list<Ogre::Entity*> rv;
	rv = mCurrentSegment->getEntityList();
	rv.push_back((*mPlayerShip.begin())->getEntity());

	return rv;
}

void Level::setCamera(Ogre::Camera* cam){
	if (mViewport){
		/*Core::getSingleton()->getWindow()->removeViewport(15);
		mViewport = Core::getSingleton()->getWindow()->addViewport(cam,15,0,0,1,1);*/
		Ogre::Viewport* vp = mViewport; // intellisense sucks
		Ogre::CompositorChain* cc = CompositorManager::getSingleton().getCompositorChain(vp);

		std::list<std::pair<std::string, bool>> pushCompositors;

		{
			Ogre::CompositorChain::InstanceIterator it = cc->getCompositors();
			while(it.hasMoreElements()){
				Ogre::CompositorInstance *compositor = it.getNext();
				std::pair<std::string, bool> entry;
				entry.first = compositor->getCompositor()->getName();
				entry.second = compositor->getEnabled();
				pushCompositors.push_back(entry);
			}
			cc->removeAllCompositors();
#ifdef ALPHA_SCREENSHOT
			CompositorManager::getSingleton().getCompositorChain(mRenderTextureTarget->getViewport(0))->removeAllCompositors();
#endif
		}
		/*if (mFastForward){
			removeCompositor(mFastForwardCompositor);
		}*/
#ifdef ALPHA_SCREENSHOT
		mRenderTextureTarget->getViewport(0)->setCamera(cam);
#endif

		mViewport->setCamera(cam);
		cam->setAspectRatio(Ogre::Real(Level::getSingleton()->getViewport()->getActualWidth()) / Ogre::Real(Level::getSingleton()->getViewport()->getActualHeight()));

		{
			while (pushCompositors.size() > 0){
#ifdef ALPHA_SCREENSHOT
				CompositorManager::getSingleton().addCompositor(mRenderTextureTarget->getViewport(0),(*pushCompositors.begin()).first,-1);
#endif

				Ogre::CompositorInstance* oI = CompositorManager::getSingleton().addCompositor(mViewport,(*pushCompositors.begin()).first,-1);
				oI->setEnabled((*pushCompositors.begin()).second);
				pushCompositors.pop_front();
			}


			/*Ogre::CompositorChain::InstanceIterator it = cc->getCompositors();
			while(it.hasMoreElements()){
				Ogre::CompositorInstance* cmp = it.getNext();
				cmp->setEnabled(true);
			}*/
		}
		/*if (mFastForward){
			mFastForwardCompositor = addCompositor("B&W");
		}*/

	}
}

void Level::uninitCameraAndViewport(){
	Core::getSingleton()->getWindow()->removeViewport(14);
#ifdef ALPHA_SCREENSHOT
	throw ("Clean your alpha screenshot, dude");
#endif

	mViewport = NULL;
}

void Level::initCameraAndViewport(){ // Real start of level and shit
		
		Ogre::Camera* cam = mCurrentSegment->getActiveCamera()->getCamera();
		mViewport = Core::getSingleton()->getWindow()->addViewport(cam,14,0,0,1,1);
		mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0,1));
		


#ifdef ALPHA_SCREENSHOT
		Ogre::SceneManager* mgr = Core::getSingleton()->getRoot()->createSceneManager(Ogre::ST_GENERIC,"BKGSceneManager");
		Ogre::Camera* bkgcam = mgr->createCamera("BkgCam");
		bkgcam->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
		Ogre::Viewport* backdrop = Core::getSingleton()->getWindow()->addViewport(bkgcam,13,0,0,1,1);
		backdrop->setBackgroundColour(Ogre::ColourValue(.5, .5, .5,.5));
		

		mRenderTexture = Ogre::TextureManager::getSingleton().createManual("RttTex",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 1920, 1080, 0, PF_R8G8B8A8,
			TU_RENDERTARGET);

		mRenderTextureTarget = mRenderTexture->getBuffer()->getRenderTarget();
		mRenderTextureTarget->addViewport(cam);
		mRenderTextureTarget->getViewport(0)->setClearEveryFrame(true);
		mRenderTextureTarget->getViewport(0)->setBackgroundColour(ColourValue(1,1,1,.01));
		mRenderTextureTarget->getViewport(0)->setOverlaysEnabled(false);

		/*QuickGUI::Root* guiRoot = new QuickGUI::Root();
		QuickGUI::SkinTypeManager::getSingleton().loadTypes();
		QuickGUI::GUIManagerDesc d;
		d.viewport = backdrop;
		QuickGUI::GUIManager* guiM = QuickGUI::Root::getSingletonPtr()->createGUIManager(d);

		QuickGUI::SheetDesc* sd = QuickGUI::DescManager::getSingleton().getDefaultSheetDesc();

		sd->widget_dimensions.size = QuickGUI::Size(1360,768);

		sd->containerwidget_supportScrollBars = false;
		
		
		QuickGUI::Sheet* sheet = QuickGUI::SheetManager::getSingleton().createSheet(sd);
		guiM->setActiveSheet(sheet);
		
		QuickGUI::Image* bkgimg = sheet->createImage(QuickGUI::DescManager::getSingleton().getDefaultImageDesc());
		bkgimg->setImage("loadscreen_02.jpg");
		QuickGUI::Rect r;
		r.size.height = 768;
		r.size.width = 1360;
		r.position.x = 0;
		r.position.y = 0;
		bkgimg->setDimensions(r);*/
		

#endif

//		
		mProfiler.started = true;
		mProfiler.completeFrameCycle->start();
		mProfiler.postFrametimeWithOgre->start();
		mProfiler.timeWithOgre->start();

		float w = Core::getSingleton()->getWindow()->getWidth();
		float h = Core::getSingleton()->getWindow()->getHeight();
		Coordinate::getSingleton()->setAspectRatio(w/h);
  
}


void Level::removeCompositor(string type){
	CompositorManager::getSingleton().removeCompositor(mViewport,type);
	/*vector<Ogre::CompositorInstance*>::iterator it;
	for (it = mCompositor.begin();it != mCompositor.end();it++){
		if ((*it) == oI){
			mCompositor.erase(it);
			break;
		}
	}*/

}
/*void Level::removeCompositor(Ogre::CompositorInstance* oI){
	vector<Ogre::CompositorInstance*>::iterator it;
	for (it = mCompositor.begin();it != mCompositor.end();it++){
		if ((*it) == oI){
			CompositorManager::getSingleton().removeCompositor(mViewport, oI->getCompositor()->getName());
			mCompositor.erase(it);
			break;
		}
	}

}*/


Ogre::CompositorInstance* Level::addCompositor(string type, string params){
#ifdef ALPHA_SCREENSHOT
	CompositorManager::getSingleton().addCompositor(mRenderTextureTarget->getViewport(0), type,-1);
	
#endif

	Ogre::CompositorInstance* oI = CompositorManager::getSingleton().addCompositor(mViewport, type,-1);
	if (oI == NULL){
		throw ("Compositor type : "+type+" not found");
	}
	oI->setEnabled(true);
	/*for (vector<CompositorInstance*>::iterator it = mCompositor.begin();it!=mCompositor.end();it++){
		if ((*it)->getCompositor() == oI->getCompositor()){
			throw("Trying to add a compositor that is already there");
		}
	}
	mCompositor.push_back(oI);*/

	return oI;
}



void Level::destroy(){

		throw("Cleanup for non-editor not done yet");
		/*typedef Shmoulette::PlayerShip* (CALLBACK* DLLCallback)();

		HMODULE playerShipModule = LoadLibrary((LPCSTR)"MyDLL");
		addResourceContext("models\\angel");
		DLLCallback dllFunc = (DLLCallback)::GetProcAddress(playerShipModule, "getInstance");
		mPlayerShip = dllFunc();*/
		
	GameElement::setLoadTimeSegmentPlaceholder(*(mSegment.begin()));
	{
		list<PlayerDetails>::iterator it;
		for (it=mPlayerDetails.begin();it != mPlayerDetails.end();it++){
			typedef PlayerShip* (__cdecl* DLLCallback)(PlayerDetails pd);

#ifdef _DEBUG
			HMODULE playerShipModule = LoadLibrary((((*it).mDll)+"d").c_str());
#else
			HMODULE playerShipModule = LoadLibrary((((*it).mDll)).c_str());
#endif
			DLLCallback dllFunc = (DLLCallback)::GetProcAddress(playerShipModule, "getInstance");
			PlayerDetails pd;
			pd = (*it);
			PlayerShip* ps = dllFunc(pd);//new PlayerShip(&pd);//
			mPlayerShip.push_back(ps);
		}
	}{
	typedef Hud* (__cdecl* DLLCallback)();
#ifdef _DEBUG
	HMODULE hudModule = LoadLibrary(((mHudDll)+"d").c_str());
#else
	HMODULE hudModule = LoadLibrary(((mHudDll)).c_str());
#endif
	DLLCallback dllFunc = (DLLCallback)::GetProcAddress(hudModule, "getInstance");
	mHud = dllFunc();//new PlayerShip(&pd);//
	}
	//mSegmentIterator = mSegment.begin();
	//mCurrentSegment = *(mSegmentIterator);
	{
		list<LevelSegment*>::iterator it;
		for (it = mSegment.begin();it!= mSegment.end();it++){
			(*it)->destroy();
			delete (*it);
		}
	}
	mSegment.clear();

	throw ("Don't forget to destroy the Hud");
		mHud->build();
		//mCurrentSegment->init();
		mHud->show();

}


/*void Level::worldConfigChange(string filename){
	mWorld->configChange(filename);
}*/




void Level::cinema(bool cinema){
	if (mIsCinema != cinema){
		mIsCinema = cinema;
		if (cinema){
			if (mCurrentSegment->hasHooks()){
				MACRO_EACHPLAYERSHIP(lock());
			}
			//mPlayerShip->lock();
			mSensorOverlay->deactivate();
			mCinemaBars->activate();
			mHud->hide();
		}else{
			if (mFastForward == true){
				mFastForward = false;
				removeCompositor("B&W");
			}
			if (getCurrentSegment()->getCurrentHook() == NULL){
				throw("Attempting to enter game mode without an active rail");
			}
			if (mCurrentSegment->hasHooks()){
				MACRO_EACHPLAYERSHIP(unlock());
			}
			mSensorOverlay->activate();
			//mPlayerShip->unlock();
			mCinemaBars->deactivate();
			mHud->show();
		}
	}
}

void Level::_endSegment(double timeOverflow){
	if (mCurrentSegment->hasHooks()){
		MACRO_EACHPLAYERSHIP(destroy());
	}
	//Ogre::SceneNode* psn = mPlayerShip->getSceneNode();
	//psn->getParentSceneNode()->removeChild(mPlayerShip->getSceneNode());
	
	//mCurrentSegment->getSceneManager()->destroyCamera(mCamera); // WHY IS THIS COMMENTED?  SEE BELOW


	//CompositorManager::getSingleton().removeCompositorChain(mViewport);
	/*while(mCompositor.size() > 0){
		Ogre::CompositorInstance* oI =mCompositor.back();
		mCompositor.pop_back();
		CompositorManager::getSingleton().removeCompositor(mViewport, oI->getCompositor()->getName());
	}*/

	//mCurrentSegment->destroy(); // Important note : This will, by definition, destroy mCamera, since mCamera is a child of the segment's scene manager

	mSegmentIterator++;
	if (mSegmentIterator == mSegment.end()){
		throw("EndOfGame");
	}
	mCurrentSegment = *(mSegmentIterator);
	GameElement::setLoadTimeSegmentPlaceholder(mCurrentSegment);
	if (mCurrentSegment->hasHooks()){
		MACRO_EACHPLAYERSHIP(setLevelSegment(mCurrentSegment));
		MACRO_EACHPLAYERSHIP(recreateSceneNode());
		MACRO_EACHPLAYERSHIP(build());
	}

	//mCamera = mCurrentSegment->getSceneManager()->createCamera("commonCam");
	//mCamera->setQueryFlags(SceneQueryFlag::IMMATERIAL);
	mViewport->setCamera(mCurrentSegment->getActiveCamera()->getCamera());


	//mCurrentSegment->getContainer()->addChild(psn);
	if (timeOverflow > mMaxFrameTime){
		mTimeCatchup = timeOverflow-mMaxFrameTime;
		timeOverflow = mMaxFrameTime;
	}else{
		mTimeCatchup = 0;
	}
	mCurrentSegment->update(timeOverflow);
	mEndSegmentFlag = false;
}

Ogre::SceneManager* Level::getSceneManager(){return mCurrentSegment->getSceneManager();}
void Level::postFrameUpdate(){
	if (mProfiler.started) mProfiler.timeWithOgre->end();

	if (mNeedPostFrameUpdate){
		list<Shmoulette::PlayerShip*>::iterator it;
		for (it=mPlayerShip.begin();it!=mPlayerShip.end();it++){
			if ((*it)->isOn()){
				(*it)->postFrameUpdate(mTimeDelta, mCurrentSegment->getTimeTrack());
			}
		}
		mCurrentSegment->postFrameUpdate();
		mNeedPostFrameUpdate = false;
	}
	if (mProfiler.started) mProfiler.postFrametimeWithOgre->start();
}

void Level::_reset(){
	Ogre::CompositorChain* cc = CompositorManager::getSingleton().getCompositorChain(mViewport);
	cc->removeAllCompositors();

	if (mCurrentSegment->hasHooks()){
		MACRO_EACHPLAYERSHIP(destroy());
	}

	list<LevelSegment*>::iterator it;
	for (it = mSegment.begin();it!= mSegment.end();it++){
		mCurrentSegment = *it;
		mCurrentSegment->reset();
	}

	SoundManager::getSingleton()->musicFadeOut(.5);

	_resetStdVar();

	for (it = mSegment.begin();it!= mSegment.end();it++){
		mCurrentSegment = *it;
		mCurrentSegment->buildStartObjects();
	}

	mSegmentIterator = mSegment.begin();
	mCurrentSegment = *mSegment.begin();

	if (mCurrentSegment->hasHooks()){
		MACRO_EACHPLAYERSHIP(setLevelSegment(mCurrentSegment));
		MACRO_EACHPLAYERSHIP(recreateSceneNode());
		MACRO_EACHPLAYERSHIP(resetStats());
		MACRO_EACHPLAYERSHIP(build());
	}
	/*mTimeCatchup = 0;
	mTimeAbsolute = 0;*/
	cinema(true);
	/*mReset = false;
	mDebugCamera = false;
	mPaused = false;
	mEndSegmentFlag = false;*/
}

void Level::reset(){
	mReset = true;
}

void Level::DronePreloadAdd(string s){
	for (list<string>::iterator it = mDronePreloadList.begin();it != mDronePreloadList.end();it++){
		if ((*it) == s)
			return;
	}
	mDronePreloadList.push_back(s);
}

void Level::update(double timeDelta){
	if (mProfiler.started) mProfiler.postFrametimeWithOgre->end();

	if (mFastForward){
		timeDelta = mMaxFrameTime*20; // 20 is the correct value
	}else{
		timeDelta += mTimeCatchup;
		if (timeDelta > mMaxFrameTime){
			mTimeCatchup = timeDelta-mMaxFrameTime;
			timeDelta = mMaxFrameTime;
		}else{
			mTimeCatchup = 0;
		}
	}

	if (mProfiler.started) mProfiler.completeFrameCycle->end();
	if (mProfiler.started) mProfiler.completeFrameCycle->start();

	if (mEndSegmentFlag == true){
		_endSegment(0);
		timeDelta = 0;
	}
	mTimeAbsolute+=timeDelta;

	MACRO_PROFILER_GET("KeyCheck","Temporary")->start();

	if (Core::getSingleton()->getKey(OIS::KC_R)->isPressed() || mReset == true){
		SoundManager::getSingleton()->muteAll();
		mTimeCatchup = 0;
		_reset();
		mFastForward = true;
  		addCompositor("B&W");
	}

	if (Core::getSingleton()->getKey(OIS::KC_SPACE)->isPressed()){
		if (isCinema() == true){
			mFastForward = true;
			SoundManager::getSingleton()->muteAll();
  			addCompositor("B&W");
			mTimeCatchup = 0;
		}
	}

	if (Core::getSingleton()->getKey(OIS::KC_RBRACKET)->isDown()){
		timeDelta*=8;
		Ogre::ControllerManager::getSingleton().setTimeFactor(8);

	}
	if (Core::getSingleton()->getKey(OIS::KC_LBRACKET)->isDown()){
		timeDelta/=8;
		Ogre::ControllerManager::getSingleton().setTimeFactor(1/8);
	}
	if (Core::getSingleton()->getKey(OIS::KC_PERIOD)->isDown()){
		if (mLogKeyUp == true){
			mLogKeyUp = false;
			Util::LogLowerLevel();
		}
	}else{
		mLogKeyUp = true;
	}
	if (Core::getSingleton()->getKey(OIS::KC_COMMA)->isDown()){
		if (mLogKeyUp == true){
			mLogKeyUp = false;
			Util::LogRaiseLevel();
		}
	}else{
		mLogKeyUp = true;
	}

	if (Core::getSingleton()->getKey(OIS::KC_P)->isPressed()){
		if (!mPaused)
			pause();
		else
			unPause();
	}
	if (Core::getSingleton()->getKey(OIS::KC_1)->isPressed()){
		getSceneManager()->setShowDebugShadows(!getSceneManager()->getShowDebugShadows());
	}

	if (Core::getSingleton()->getKey(OIS::KC_TAB)->isPressed()){
		debugCameraToggle();
	}

	MACRO_PROFILER_GET("KeyCheck","Temporary")->end();


	if (mDebugCamera){
		float multiplier = 50;
		if (Core::getSingleton()->getKey(OIS::KC_LSHIFT)->isDown() ||
			Core::getSingleton()->getKey(OIS::KC_RSHIFT)->isDown()){
				multiplier*=100;
		}
		
		if (Core::getSingleton()->getKey(OIS::KC_W)->isDown()){
			mDebugCamNode->translate(0,0,-multiplier*timeDelta, Ogre::Node::TS_LOCAL);
		}
		if (Core::getSingleton()->getKey(OIS::KC_S)->isDown()){
			mDebugCamNode->translate(0,0,multiplier*timeDelta, Ogre::Node::TS_LOCAL);
		}
		if (Core::getSingleton()->getKey(OIS::KC_A)->isDown()){
			mDebugCamNode->translate(-multiplier*timeDelta,0,0, Ogre::Node::TS_LOCAL);
		}
		if (Core::getSingleton()->getKey(OIS::KC_D)->isDown()){
			mDebugCamNode->translate(multiplier*timeDelta,0,0, Ogre::Node::TS_LOCAL);
		}
		float mx = 0-Core::getSingleton()->getMouse()->getDX();
		float my = Core::getSingleton()->getMouse()->getDY();
		
		if (Core::getSingleton()->getKey(OIS::KC_LCONTROL)->isDown() ||
			Core::getSingleton()->getKey(OIS::KC_RCONTROL)->isDown()
			){
			mDebugCamNode->roll((Ogre::Radian)mx);
		}else{
			mDebugCamNode->yaw((Ogre::Radian)mx);
		}
		mDebugCamNode->pitch((Ogre::Radian)my);
	}

	if (mPaused){
		//mCurrentSegment->viewerCamControl(timeDelta);


		timeDelta = 0;
		Ogre::ControllerManager::getSingleton().setTimeFactor(0);
	}else{
		Ogre::ControllerManager::getSingleton().setTimeFactor(mSpeedModifier.current);
	}

	if (timeDelta > 0){
		mTimeDelta = timeDelta*= mSpeedModifier.current;


		if (mTimer > 0){
			mTimer -= timeDelta;
			if (mTimer <= 0) throw("Timer expired.  Game over");
		}
		MACRO_PROFILER_GET("A","Temporary")->start();

		mCinemaBars->update(mTimeDelta);
		float timeOverflow = mCurrentSegment->update(mTimeDelta);
		MACRO_PROFILER_GET("A","Temporary")->end();
		if (timeOverflow>0){
			_endSegment(timeOverflow);
		}
		MACRO_PROFILER_GET("A2","Temporary")->start();
		if (mCurrentSegment->hasHooks()){
			MACRO_EACHPLAYERSHIP(updateIfOn(timeDelta, mCurrentSegment->getTimeTrack()));
		}
		MACRO_PROFILER_GET("A2","Temporary")->end();
		MACRO_PROFILER_GET("A3","Temporary")->start();
		mHud->update(timeDelta, mCurrentSegment->getTimeTrack());
		MACRO_PROFILER_GET("A3","Temporary")->end();
		MACRO_PROFILER_GET("A4","Temporary")->start();
		mSensorOverlay->update(timeDelta, mCurrentSegment->getTimeTrack());
		MACRO_PROFILER_GET("A4","Temporary")->end();

		MACRO_PROFILER_GET("B","Temporary")->start();

		float delta = fabs(mSpeedModifier.wanted - mSpeedModifier.current);
		if (delta > fabs(mSpeedModifier.acceleration * timeDelta)){
			mSpeedModifier.current += mSpeedModifier.acceleration*timeDelta;
		}else{
			mSpeedModifier.current = mSpeedModifier.wanted;
			mSpeedModifier.acceleration = 0;
		}
		mNeedPostFrameUpdate = true;
		MACRO_PROFILER_GET("B","Temporary")->end();
		if (mProfiler.started) mProfiler.timeWithOgre->start();
	}
	mInputBridge->update();
	//Util::Profile("During Update");
}
Ogre::Root* Level::getRoot(){
	return Core::getSingleton()->getRoot();
}
void Level::score(int score, GameObject* scorer){
	
	list<Shmoulette::PlayerShip*>::iterator it;
	for (it=mPlayerShip.begin();it!=mPlayerShip.end();it++){\
		if ((*it)==scorer){
			dynamic_cast<PlayerShip*>(scorer)->score(score);
		}
	}
	if (scorer == NULL){
		MACRO_EACHPLAYERSHIP(score(score));
	}
}

Ogre::Camera* Level::getCamera(){
	return mCurrentSegment->getActiveCamera()->getCamera();
}

void Level::debugCameraToggle(){
	if (mDebugCamera == false){
		mDebugCam = mCurrentSegment->getSceneManager()->createCamera("debugCam");
		mDebugCam->setNearClipDistance(.5);
		mDebugViewport = Core::getSingleton()->getWindow()->addViewport(mDebugCam,15,0,0,1,1);
		mDebugCam->setAspectRatio(Ogre::Real(mDebugViewport->getActualWidth()) / mDebugViewport->getActualHeight());
		
		mDebugCam->setAutoAspectRatio(false);
		
		mDebugCamNode = getSceneManager()->getRootSceneNode()->createChildSceneNode("debugCamNode");
		mDebugCamNode->attachObject(mDebugCam);
		
		Ogre::SceneNode* sn = mCurrentSegment->getActiveCamera()->getCamera()->getParentSceneNode();
		if (sn == NULL){
			throw("Camera Scene Node is Null.  Camera's parent inactive?");
		}
		mDebugCamNode->setPosition(sn->_getDerivedPosition());
		mDebugCamNode->setOrientation(sn->_getDerivedOrientation());
		mDebugCamera = true;
	}else{
		mDebugCamNode->detachAllObjects();
		getSceneManager()->destroySceneNode(mDebugCamNode->getName());
		Core::getSingleton()->getWindow()->removeViewport(15);
		getSceneManager()->destroyCamera(mDebugCam);
		mDebugCamera = false;
	}

}


void Level::pause(){
	if (mPaused != true){
		mPaused = true;
		mCurrentSegment->Pause();
		SoundManager::getSingleton()->pause();
	}
}
void Level::unPause(){
	if (mPaused != false){
		mPaused = false;
		mCurrentSegment->UnPause();
		SoundManager::getSingleton()->resume();
	}
}


PlayerShip* Level::getPlayerShip(){
	return (*mPlayerShip.begin());
}



}