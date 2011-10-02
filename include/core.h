// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___CORE_H___
#define ___DEF___CORE_H___

/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    Game.h
Description: Base class for all the OGRE examples
-----------------------------------------------------------------------------
*/

#include "Ogre.h"
#include "OgreConfigFile.h"
//#include "OgreKeyEvent.h"
//#include "OgreEventListeners.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "input.h"
#include "python.h"
//using namespace Ogre;
//#include "level.h"

namespace Shmoulette{

	//extern Ogre::Root* Shmoulette::Core::getSingleton()->getRoot();

	/*class DefaultListenerListener: public Ogre::FrameListener, public Ogre::KeyListener{
		public:
	};*/

	class CoreListener: public Ogre::FrameListener{
		protected:
			int mSceneDetailIndex ;
			Ogre::Overlay* mDebugOverlay;

			bool mGameCanStart;
			void updateStats(void);
			bool mRecorder; // if true, will save every frame as a numbered frame.  
		public:
			// Constructor takes a RenderWindow because it uses that to determine input context
			CoreListener(bool useBufferedInputKeys = false, bool useBufferedInputMouse = false);
			virtual ~CoreListener();
			void activateRecorder(){mRecorder = true;}
			void startGame();
			void showDebugOverlay(bool show);
			// Override frameStarted event to process that (don't care about frameEnded)
			bool frameStarted(const Ogre::FrameEvent& evt);
			bool frameEnded(const Ogre::FrameEvent& evt);
			
		protected:
			Ogre::Camera* mCamera;

			Ogre::Vector3 mTranslateVector;
			bool mStatsOn;
			bool mUseBufferedInputKeys, mUseBufferedInputMouse, mInputTypeSwitchingOn;
			unsigned int mNumScreenShots;
			float mMoveScale;
			Ogre::Degree mRotScale;
			// just to stop toggles flipping too fast
			Ogre::Real mTimeUntilNextToggle ;
			Ogre::Radian mRotX, mRotY;
			Ogre::TextureFilterOptions mFiltering;
			int mAniso;

	};

	/** Base class which manages the standard startup of an Ogre application.
		Designed to be subclassed for specific examples if required.
	*/
		//using namespace Ogre;
	class _ShmouletteExport Core{
	public:
		static Core* getSingleton(){
			if (mSingleton == NULL){
				mSingleton = new Core();
			}
			return mSingleton;
		}
		static PyObject* Log(PyObject *self, PyObject *args);
		void Log(string s, int level);
		string getPythonException();
		virtual ~Core();
		
		virtual bool setup(void);
		virtual void go(void);
		virtual void load(string s);
		virtual void unload();
		Ogre::Root* getRoot(){return mRoot;}
		Ogre::RenderWindow* getWindow(){return mWindow;}
		class SoundManager* getSoundManager(){return mSndMgr;}
		class DroneDB* getDroneDB(){return mDroneDB;}
		//class WeaponDB* getWeaponDB(){return mWeaponDB;}
		class Key* getKey(int index){return mInputManager->getKey(index);}
		class Mouse* getMouse(){return mInputManager->getMouse();}
		class InputManager* getInputManager(){return mInputManager;}
		//class FontStyle* getDefaultFontStyle(){return mFontStyleComic;}
		bool isInControl(){return mShmouletteInControl;}
		void giveControl(){mShmouletteInControl = true;}
		void takeControl(){mShmouletteInControl = false;}
		void startGame(){mFrameListener->startGame();}
		void updateInputManager();
		//Ogre::EventProcessor* getEventProcessor(){return mFrameListener->getEventProcessor();}
		void activateRecorder(){mFrameListener->activateRecorder();}

	protected:
		bool mShmouletteInControl;


		Ogre::Root* mRoot;
		Ogre::RenderWindow* mWindow;
		class InputManager* mInputManager;
		class SoundManager* mSndMgr;
		//class WeaponDB* mWeaponDB;
		class DroneDB* mDroneDB;
		//class Font* mFontComic;
		//class FontStyle* mFontStyleSmall;
		//class FontStyle* mFontStyleComic;

		static Core* mSingleton;
		Core();
		CoreListener* mFrameListener;

		// These internal methods package up the stages in the startup process
		/** Sets up the application - returns false if the user chooses to abandon configuration. */
		virtual void cleanUp(void);
		
		/** Configures the application - returns false if the user chooses to abandon configuration. */
		virtual bool configure(bool quick = false);

		virtual void createFrameListener(void);
		/// Method which will define the source of resources (other than current folder)
		virtual void setupResources(void);
		/// Optional override method where you can create resource listeners (e.g. for loading screens)
		//virtual void createResourceListener(void);
		/// Optional override method where you can perform resource group loading
		/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		virtual void loadResources(void);
	};



}

#endif
