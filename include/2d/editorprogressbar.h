// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___EDITORPROGRESSBAR_H___
#define ___DEF___EDITORPROGRESSBAR_H___

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

			//OverlayElement* mLoadingCommentElement;

			//Ogre::SceneManager* mLoadingSceneManager;
			//Ogre::Camera* mLoadingCam;
			//Ogre::Viewport* mLoadingVP;
			bool mFirstDraw;

		public:
			ProgressBar() {mFirstDraw = true;}
			virtual ~ProgressBar(){}

			virtual void draw(Ogre::RenderWindow* window){
				mWindow = window;

				if (mFirstDraw){
					//mLoadingSceneManager = Core::getSingleton()->getRoot()->createSceneManager(ST_GENERIC, "loadingbar");
					//mLoadingCam = mLoadingSceneManager->createCamera("loadingcam");
					//mLoadingVP = Core::getSingleton()->getWindow()->addViewport(mLoadingCam,89,0,0,0.781250,0.781250);

					//mLoadingSceneManager->clearSpecialCaseRenderQueues();
					//mLoadingSceneManager->addSpecialCaseRenderQueue(RENDER_QUEUE_OVERLAY);
					//mLoadingSceneManager->setSpecialCaseRenderQueueMode(SceneManager::SCRQM_INCLUDE);


					OverlayManager& omgr = OverlayManager::getSingleton();
				
					// We need to pre-initialise the 'Bootstrap' group so we can use
					// the basic contents in the loading screen
					//Ogre::ResourceGroupManager::ResourceLocation rl;
					//ResourceGroupManager::getSingleton().addResourceLocation("media/bootstrap", "FileSystem", "Bootstrap");
					ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
				
					mLoadOverlay = (Overlay*)omgr.getByName("Shmoulette/LoadOverlay");
				
					/*mLoadingDescriptionContainer = static_cast<OverlayContainer*>(
						OverlayManager::getSingleton().createOverlayElement("Panel", "loadingdetailscontainer"));*/

					if (!mLoadOverlay)
					{
						OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
							"Cannot find loading overlay", "ExampleLoadingBar::start");
					}
					mLoadingBarElement = omgr.getOverlayElement("Shmoulette/LoadPanel/Bar/Progress");
					mLoadingBarElement->show();
					mProgressBarMaxSize = mLoadingBarElement->getWidth();
				}
				mLoadOverlay->show();

				// Save links to the bar and to the loading text, for updates as we go
				//mLoadingCommentElement = omgr.getOverlayElement("Core/LoadPanel/Comment");
				//mLoadingDescriptionElement = omgr.getOverlayElement("Core/LoadPanel/Description");

				//OverlayElement* barContainer = omgr.getOverlayElement("Shmoulette/LoadPanel/Bar");
				mLoadingBarElement->setWidth(0);

				mFirstDraw = false;
			}
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
				Ogre::Real initProportion = 0.70f)
			{
					
				mNumGroupsInit = numGroupsInit;
				mNumGroupsLoad = numGroupsLoad;
				mInitProportion = initProportion;


				// self is listener
				ResourceGroupManager::getSingleton().addResourceGroupListener(this);
			}

			/** Hide the loading bar and stop listening. 
			*/
			virtual void finish(void)
			{
				// hide loading screen
				mLoadOverlay->hide();
				//Core::getSingleton()->getWindow()->removeViewport(89);
				//mLoadingSceneManager->destroyCamera(mLoadingCam);
				//Core::getSingleton()->getRoot()->destroySceneManager(mLoadingSceneManager);	

				// Unregister listener
				ResourceGroupManager::getSingleton().removeResourceGroupListener(this);

			}


			// ResourceGroupListener callbacks
			void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount)
			{
				assert(mNumGroupsInit > 0 && "You stated you were not going to init "
					"any groups, but you did! Divide by zero would follow...");
				// Lets assume script loading is 70%
				mProgressBarInc = (mProgressBarMaxSize/4) * mInitProportion / (Real)scriptCount;
				mProgressBarInc /= mNumGroupsInit;
				//mLoadingDescriptionElement->setCaption("Parsing scripts...");
				mWindow->update();
			}
			void setCaption(string newCaption){
				//mLoadingCommentElement->setCaption(newCaption);
				mWindow->update();
			}
			void scriptParseStarted(const Ogre::String& scriptName)
			{
				//mLoadingCommentElement->setCaption(scriptName);
				mWindow->update();
			}
			void scriptParseEnded(const Ogre::String& scriptName)
			{
				mLoadingBarElement->setWidth(
					mLoadingBarElement->getWidth() + mProgressBarInc);
				/*if (mLoadingBarElement->getWidth()>mProgressBarMaxSize/2){
					mLoadingBarElement->setWidth(mProgressBarMaxSize);
				}*/
				mWindow->update();
			}

			/*void levelParseStarted(const Ogre::String& scriptName)
			{
				//mLoadingCommentElement->setCaption(scriptName);
				mWindow->update();
			}
			void levelParseEnded(const Ogre::String& scriptName)
			{
				mLoadingBarElement->setWidth(
					mLoadingBarElement->getWidth() + .1);
				mWindow->update();
				if (mLoadingBarElement->getWidth()>mProgressBarMaxSize/2){
					mLoadingBarElement->setWidth(mProgressBarMaxSize);
				}
			}*/



			void resourceGroupScriptingEnded(const Ogre::String& groupName)
			{
			}
			void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount)
			{
				assert(mNumGroupsLoad > 0 && "You stated you were not going to load "
					"any groups, but you did! Divide by zero would follow...");
				mProgressBarInc = (mProgressBarMaxSize/4) * (mInitProportion) / 
					(Real)resourceCount;
				mProgressBarInc /= mNumGroupsLoad;
				//mLoadingDescriptionElement->setCaption("Loading resources...");
				mWindow->update();
			}
			void resourceLoadStarted(const Ogre::ResourcePtr& resource)
			{
				if (mLoadingBarElement->getWidth() < mProgressBarMaxSize){
					mProgressBarInc = 2;
				}

				//mLoadingCommentElement->setCaption(resource->getName());
				mWindow->update();
			}
			void resourceLoadEnded(void)
			{
				mLoadingBarElement->setWidth(
				mLoadingBarElement->getWidth() + mProgressBarInc);
				if (mLoadingBarElement->getWidth()>mProgressBarMaxSize){
					mLoadingBarElement->setWidth(mProgressBarMaxSize);
				}
			}
			void worldGeometryStageStarted(const Ogre::String& description)
			{
				//mLoadingCommentElement->setCaption(description);
				mWindow->update();
			}
			void worldGeometryStageEnded(void)
			{
				mLoadingBarElement->setWidth(
					mLoadingBarElement->getWidth() + mProgressBarInc);
				mWindow->update();
			}
			void fileLoadStart(string name){
				//mLoadingCommentElement->setCaption("Loading : "+name);
				mWindow->update();
			}
			void fileLoadEnd(string name){
				mLoadingBarElement->setWidth(
					mLoadingBarElement->getWidth() + .05);
				if (mLoadingBarElement->getWidth()>mProgressBarMaxSize/2){
					mLoadingBarElement->setWidth(mProgressBarMaxSize/2);
				}
				mWindow->update();
			}
			void resourceGroupLoadEnded(const Ogre::String& groupName)
			{

				mLoadingBarElement->setWidth(0);
			}

		};
}

#endif
