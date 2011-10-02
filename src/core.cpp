#include "stdafx.h"
#include "main.h"
#include "core.h"
#include "drone\\drone.h"
#include "weapon\\weapon.h"
#include "sound.h"
#include "world\\world.h"
#include "level.h"
#include "elements\camera.h"
#include "2d\text.h"
#include "python.h"
#include "util\profiler.h"
#include "QuickGUI.h"

namespace Shmoulette{

Core* Core::mSingleton = NULL;;

using namespace Ogre;
void CoreListener::startGame(){
	Level::getSingleton()->initCameraAndViewport();
	mGameCanStart = true;
}

void CoreListener::updateStats(void)
{
	static String currFps = "Current FPS: ";
    static String avgFps = "Average FPS: ";
    static String bestFps = "Best FPS: ";
    static String worstFps = "Worst FPS: ";
    static String tris = "Triangle Count: ";
    static String missTime = "Mission Time: ";

    // update stats when necessary
    try {
        OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("USCA/AverageFps");
        OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("USCA/CurrFps");
        OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("USCA/BestFps");
        OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("USCA/WorstFps");
        OverlayElement* guiTime = OverlayManager::getSingleton().getOverlayElement("USCA/MissionTime");

		const RenderTarget::FrameStats& stats = Shmoulette::Core::getSingleton()->getWindow()->getStatistics();

		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS)+"("+ts(Shmoulette::Level::getSingleton()->getCurrentSegment()->getTimeTrack())+")");
        guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
        guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
            +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
        guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
            +" "+StringConverter::toString(stats.worstFrameTime)+" ms");
		guiTime->setCaption(missTime + ts(Level::getSingleton()->getCurrentSegment()->getTimeTrack())
            +" "+StringConverter::toString(stats.worstFrameTime)+" secs");
        OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("USCA/NumTris");
        guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

        OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("USCA/DebugText");
        //guiDbg->setCaption(Shmoulette::Core::getSingleton()->getWindow()->getDe->getDebugText());
    }
    catch(...)
    {
        // ignore
    }
}


// Constructor takes a RenderWindow because it uses that to determine input context
CoreListener::CoreListener(bool useBufferedInputKeys, bool useBufferedInputMouse)
{
    mDebugOverlay = OverlayManager::getSingleton().getByName("USCA/DebugOverlay");
    mUseBufferedInputKeys = useBufferedInputKeys;
	mUseBufferedInputMouse = useBufferedInputMouse;
	mInputTypeSwitchingOn = mUseBufferedInputKeys || mUseBufferedInputMouse;
	mGameCanStart=false;    //mRotateSpeed = 36;
	mRecorder = false;

    mStatsOn = true;
	mNumScreenShots = 0;
	mTimeUntilNextToggle = 0;
    mSceneDetailIndex = 0;
    mMoveScale = 0.0f;
    mRotScale = 0.0f;
    mTranslateVector = Vector3::ZERO;
    mAniso = 1;
    mFiltering = TFO_BILINEAR;

    showDebugOverlay(true);
}
CoreListener::~CoreListener()
{
}

void CoreListener::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
    {
        if (show)
        {
            mDebugOverlay->show();
        }
        else
        {
            mDebugOverlay->hide();
        }
    }
}
int recordedframenb=0;
//bool recorder = false;
// Override frameStarted event to process that (don't care about frameEnded)
bool CoreListener::frameStarted(const FrameEvent& evt)
{
	mTimeUntilNextToggle -= evt.timeSinceLastFrame;
	Core::getSingleton()->updateInputManager();
	if(Shmoulette::Core::getSingleton()->getWindow()->isClosed())
		return false;

	if (Core::getSingleton()->getKey(OIS::KC_F12)->isDown())// && mTimeUntilNextToggle <= 0)
    {
		char tmp[256];

		char datebuf[256];
		_strdate(datebuf);
		char timebuf[256];
		_strtime_s(timebuf, 9);
		string datestr;
		datestr = datebuf[6];
		datestr+= datebuf[7];
		datestr+= datebuf[0];
		datestr+= datebuf[1];
		datestr+= datebuf[3];
		datestr+= datebuf[4];
		datestr+= timebuf[0];
		datestr+= timebuf[1];
		datestr+= timebuf[3];
		datestr+= timebuf[4];
		datestr+= timebuf[6];
		datestr+= timebuf[7];

		sprintf(tmp, "screenshots/screenshot_%s.png", datestr.c_str());
        Shmoulette::Core::getSingleton()->getWindow()->writeContentsToFile(tmp);

#ifdef ALPHA_SCREENSHOT
		sprintf(tmp, "screenshots/screenshot_%s_hd.png", datestr.c_str());
		Level::getSingleton()->mRenderTextureTarget->update();
		Level::getSingleton()->mRenderTextureTarget->writeContentsToFile(tmp);
#endif
        //mTimeUntilNextToggle = 0.5;
		//Shmoulette::Core::getSingleton()->getWindow()->setDebugText(String("Wrote ") + tmp);
    }
	if (Core::getSingleton()->getKey(OIS::KC_F11)->isDown()){// && mTimeUntilNextToggle <= 0)
		Profiler::ProfilerFactory::getSingleton()->generateReport();
	}

    if (Core::getSingleton()->getKey(OIS::KC_F)->isDown() && mTimeUntilNextToggle <= 0)
    {
        mStatsOn = !mStatsOn;
        showDebugOverlay(mStatsOn);
        mTimeUntilNextToggle = 1;
    }


	/*if (mGameCanStart == false){
		//if (Core::getSingleton()->getMouse()->isButtonPressed(0)){
			mGameCanStart = true;
			
		//}
	}*/
	SoundManager::getSingleton()->update(evt.timeSinceLastFrame);

	if (mGameCanStart == true){

		/*if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
		{
			// one of the input modes is immediate, so setup what is needed for immediate mouse/key movement
			if (mTimeUntilNextToggle >= 0) 
				mTimeUntilNextToggle -= evt.timeSinceLastFrame;*/

			float tslf = evt.timeSinceLastFrame;

			if (mRecorder){
				Shmoulette::Level::getSingleton()->update(.033333333);
				Ogre::ControllerManager::getSingleton().setTimeFactor(.033333333);

				char tmp[20];
				sprintf(tmp, "screenshots/movie_%04d.png", ++recordedframenb);
				Core::getSingleton()->getWindow()->writeContentsToFile(tmp);

			}else{
				if (Core::getSingleton()->isInControl()){
					Shmoulette::Level::getSingleton()->update(evt.timeSinceLastFrame);
				}
			}
		//}
	}

    /*if (mUseBufferedInputKeys)
    {
        // no need to do any processing here, it is handled by event processor and 
		// you get the results as KeyEvents
    }
    else
    {
        if (processUnbufferedKeyInput(evt) == false)
		{
			return false;
		}
    }*/
    /*if (mUseBufferedInputMouse)
    {
        // no need to do any processing here, it is handled by event processor and 
		// you get the results as MouseEvents
    }
    else
    {
        if (processUnbufferedMouseInput(evt) == false)
		{
			return false;
		}
    }*/
	if (
		(Shmoulette::Core::getSingleton()->getKey(184)->isDown()||
		Shmoulette::Core::getSingleton()->getKey(56)->isDown())&&
		Shmoulette::Core::getSingleton()->getKey(62)->isDown()
		){
		return false;
	}
	Shmoulette::Core::getSingleton()->getMouse()->setDX(0);
	Shmoulette::Core::getSingleton()->getMouse()->setDY(0);
	return true;
}

bool CoreListener::frameEnded(const FrameEvent& evt)
{
	if (Core::getSingleton()->isInControl()){
		Shmoulette::Level::getSingleton()->postFrameUpdate();
		updateStats();
	}

	
    return true;
}


void Core::Log(string s, int level){
	Util::Log(s,level);
}

PyObject* Core::Log(PyObject *self, PyObject *args){
	string s = "Python : "+toString(args);
	Util::Log(s);
	Py_RETURN_NONE;
}

static PyMethodDef CoreMethods[] = {
	{"log", Core::Log, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {NULL, NULL, 0, NULL}
};

string Core::getPythonException(){
	PyObject* type, *value, *traceback;
	PyErr_Fetch(&type, &value, &traceback);
	return (string)("Python Exception\n"+toString(type)+"\n"+toString(value)+"\n"+toString(traceback));
}

Core::Core()
{
	srand( (unsigned)time( NULL ) );
	mShmouletteInControl = false;
    mFrameListener = 0;
	mRoot = 0;
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append(\"python\")");
    PyObject* goupille = Py_InitModule("sh_core", CoreMethods);
	GameObject::registerPython();
	int gluglu3 = PyRun_SimpleString("import sh_core");
//	int gluglu = PyRun_SimpleString("sh_core.log(\"la goupille est un gluglu\")");

}
/// Standard destructor
Core::~Core()
{
	Profiler::ProfilerFactory::getSingleton()->generateReport();
	if (mFrameListener)
        delete mFrameListener;
    if (mRoot)
        delete mRoot;
	try{
		Py_Finalize();
	}catch(...){
		string h = getPythonException();
	}

}

/// Start the example
void Core::go(void)
{


	try {
	    mRoot->startRendering();
	} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
	} catch( string s){
		char buffer[256];
		GetCurrentDirectoryA(256,buffer);
		string s2 = s+"\nCurrent Directory : "+buffer;
		MessageBox( NULL, s2.c_str(), "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	} catch( char* s){
		MessageBox( NULL, (LPCSTR)s, "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	} catch( ...){
		MessageBox ( NULL, "Exception Format Not Recognized", "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		throw;
	}



    // clean up
	cleanUp();
//    destroyScene();
}

bool Core::setup(void)
{

#ifdef _DEBUG
    mRoot = new Root("plugins_debug.cfg");
#else
    mRoot = new Root("plugins_release.cfg");
#endif
	Util::Log("Setting up resources\n");
    setupResources();

	Util::Log("Configuring\n");

	try{
		SoundManager::getSingleton();
	}catch(...){
		int g=0;
	}

    bool carryOn = configure(false);
    if (!carryOn) return false;
	Util::Log("About to initialize texture manager\n");
    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Create any resource listeners (for loading screens)
	//createResourceListener();
	// Load resources

	try {
		QuickGUI::registerScriptReader();
		loadResources();
	} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
	} catch( string s){
		char buffer[256];
		GetCurrentDirectoryA(256,buffer);
		string s2 = s+"\nCurrent Directory : "+buffer;
		MessageBox( NULL, s2.c_str(), "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	} catch( char* s){
		MessageBox( NULL, (LPCSTR)s, "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	} catch( ...){
		MessageBox ( NULL, "Exception Format Not Recognized", "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		throw;
	}


	// Create the scene
	
	/*mFontComic = new Shmoulette::Font("bluehigh");
	mFontStyleSmall = new Shmoulette::FontStyle(mFontComic);
	mFontStyleComic = new Shmoulette::FontStyle(mFontComic);
//	Shmoulette::gFontStyleComic->setColour(Ogre::ColourValue(0, 0, 0), Ogre::ColourValue(1, 0, 0));
	mFontStyleComic->setColour(
		Ogre::ColourValue(0, 0, 0), Ogre::ColourValue(0, 0, 0), 
		Ogre::ColourValue(.9, .9, .9), Ogre::ColourValue(.9, .9, .9) 
		);
	mFontStyleSmall->setColour(
		Ogre::ColourValue(1, 1, 1), Ogre::ColourValue(1, 1, 1), 
		Ogre::ColourValue(.5, .5, .5), Ogre::ColourValue(0, 0, 0) 
		);*/

	mDroneDB = new Shmoulette::DroneDB();
	//mWeaponDB = new Shmoulette::WeaponDB();
	//Shmoulette::gLevel = new Shmoulette::Level();


    createFrameListener();

	/*Ogre::SceneManager* sm = Shmoulette::Core::getSingleton()->getRoot()->createSceneManager(ST_GENERIC);
	Ogre::Camera* cam = sm->createCamera("DefCam");
	cam->setPosition(100,100,100);
	cam->lookAt(0,0,0);
	Ogre::Viewport* vp = Shmoulette::Core::getSingleton()->getWindow()->addViewport(cam,0,0,1,1);
	vp->setDimensions(0,0,1,1);
	Ogre::SceneNode* sn = sm->getRootSceneNode()->createChildSceneNode("SN");
	Ogre::Entity* en = sm->createEntity("entity","RZR-002.mesh");
	sn->attachObject(en);*/

    return true;
}

void Core::load(string s){
	Shmoulette::Level::getSingleton()->init(s);
	//Shmoulette::Core::getSingleton()->getDroneDB()->init();
//	Shmoulette::Core::getSingleton()->getWeaponDB()->init();
	Shmoulette::Level::getSingleton()->load();
}

void Core::unload(){
	Shmoulette::Level::getSingleton()->uninitCameraAndViewport();
	Shmoulette::Level::getSingleton()->unload();
	Shmoulette::Level::getSingleton()->uninit();
	mShmouletteInControl = false;
}

void Core::cleanUp(void){
	/*delete mFontComic;
	delete mFontStyleComic;
	delete mFontStyleSmall;*/
	delete mDroneDB;
	//delete mWeaponDB;
	//delete Shmoulette::gLevel;
}

/** Configures the application - returns false if the user chooses to abandon configuration. */
bool Core::configure(bool quick)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
	if (quick){
		mRoot->restoreConfig();
		mWindow = mRoot->initialise(true);
		mInputManager = new InputManager(mWindow);
		return true;
	}
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true);
		mInputManager = new InputManager(mWindow);
        return true;
    }
    else
    {
        return false;
    }
}

void Core::createFrameListener(void)
{
    mFrameListener= new CoreListener(true, true);
    mFrameListener->showDebugOverlay(false);
	
    mRoot->addFrameListener(mFrameListener);
/*	Shmoulette::Core::getSingleton()->getEventProcessor()->addMouseMotionListener(mFrameListener);
    Shmoulette::Core::getSingleton()->getEventProcessor()->addMouseListener(mFrameListener);
	Shmoulette::Core::getSingleton()->getEventProcessor()->addKeyListener(mFrameListener);*/



}

/// Method which will define the source of resources (other than current folder)
void Core::setupResources(void)
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

/// Optional override method where you can create resource listeners (e.g. for loading screens)
/*void Core::createResourceListener(void)
{

}*/




void Core::updateInputManager(){
	mInputManager->update();
};

/// Optional override method where you can perform resource group loading
/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
void Core::loadResources(void)
{
	// Initialise, parse scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

}

/*void Core::listSceneManagerAssets(){
	Ogre::SceneManager* sm = Level::getSingleton()->getSceneManager();
}*/