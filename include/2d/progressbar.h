// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___PROGRESSBAR_H___
#define ___DEF___PROGRESSBAR_H___
#if 0
#include "Ogre.h"
#include "OgreResourceGroupManager.h"
#include "OgreOverlay.h"
#include "OgreOverlayElement.h"
#include "OgreOverlayManager.h"
#include "util/file.h"
#include <OgreTextAreaOverlayElement.h>
#include <OgreFontManager.h> 
#include "text.h"
#include "util/localstr.h"

namespace Shmoulette{
	using namespace Ogre;
	
	class SilentProgressBar:public Ogre::ResourceGroupListener, public Shmoulette::FileLoadCallback{
	public:
		void resourceGroupScriptingStarted(const Ogre::String &,size_t){}
		void Ogre::ResourceGroupListener::scriptParseStarted(const Ogre::String &, bool&){}
		void Ogre::ResourceGroupListener::scriptParseEnded(const Ogre::String &, bool){}
		void Ogre::ResourceGroupListener::resourceGroupScriptingEnded(const Ogre::String &){}
		void Ogre::ResourceGroupListener::resourceGroupLoadStarted(const Ogre::String &,size_t){}
		void Ogre::ResourceGroupListener::resourceLoadStarted(const Ogre::ResourcePtr &){}
		void Ogre::ResourceGroupListener::resourceLoadEnded(void){}
		void Ogre::ResourceGroupListener::worldGeometryStageStarted(const Ogre::String &){}
		void Ogre::ResourceGroupListener::worldGeometryStageEnded(void){}
		void Ogre::ResourceGroupListener::resourceGroupLoadEnded(const Ogre::String &){}
		void Shmoulette::FileLoadCallback::fileLoadStart(std::string){}
		void Shmoulette::FileLoadCallback::fileLoadEnd(std::string){}
		virtual void draw(Ogre::RenderWindow* window){}
		virtual void start(
				unsigned short numGroupsInit = 1, 
				unsigned short numGroupsLoad = 1, 
				Ogre::Real initProportion = 0.70f){}
		virtual void finish(void){}
	};

	class ProgressBar:public Ogre::ResourceGroupListener, public Shmoulette::FileLoadCallback{
		protected:
			RenderWindow* mWindow;
			Overlay* mLoadOverlay;
			Real mInitProportion;
			unsigned short mNumGroupsInit;
			unsigned short mNumGroupsLoad;
			Real mProgressBarMaxSize;
			Real mProgressBarScriptSize;
			Real mProgressBarInc;
			OverlayElement* mLoadingBarElement;
			TextAreaOverlayElement* mLoadingDescriptionElement;
			TextAreaOverlayElement* mFactoid[2];
			Ogre::OverlayContainer* mLoadingDescriptionContainer;

			//OverlayElement* mLoadingCommentElement;

			Ogre::SceneManager* mLoadingSceneManager;
			Ogre::Camera* mLoadingCam;
			Ogre::Viewport* mLoadingVP;

		public:
			ProgressBar() {}
			virtual ~ProgressBar(){}

			virtual void draw(Ogre::RenderWindow* window);
			/** Show the loading bar and start listening.
			@param window The window to update 
			@param numGroupsInit The number of groups you're going to be initialising
			@param numGroupsLoad The number of groups you're going to be loading
			@param initProportion The proportion of the progress which will be taken
				up by initialisation (ie script parsing etc). Defaults to 0.7 since
				script parsing can often take the majority of the time.
			*/
			virtual void start( 
				unsigned short numGroupsInit = 1, 
				unsigned short numGroupsLoad = 1, 
				Ogre::Real initProportion = 0.70f);

			/** Hide the loading bar and stop listening. 
			*/
			virtual void finish(void);


			// ResourceGroupListener callbacks
			void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount);
			void setCaption(string newCaption);
			void scriptParseStarted(const Ogre::String& scriptName, bool&);
			void scriptParseEnded(const Ogre::String& scriptName, bool);
			void resourceGroupScriptingEnded(const Ogre::String& groupName);
			void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount);
			void resourceLoadStarted(const Ogre::ResourcePtr& resource);
			void resourceLoadEnded(void);
			void worldGeometryStageStarted(const Ogre::String& description);
			void worldGeometryStageEnded(void);
			void fileLoadStart(string name);
			void fileLoadEnd(string name);
			void resourceGroupLoadEnded(const Ogre::String& groupName);
		};
}

#endif
#endif