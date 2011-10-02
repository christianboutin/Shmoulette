#include "stdafx.h"
#if 0
#include "2d\progressbar.h"
#include "util/profiler.h"
#include "core.h"

namespace Shmoulette{

	void ProgressBar::draw(Ogre::RenderWindow* window){
				mWindow = window;

				mLoadingSceneManager = Core::getSingleton()->getRoot()->createSceneManager(ST_GENERIC, "loadingbar");
				mLoadingCam = mLoadingSceneManager->createCamera("loadingcam");
				mLoadingVP = Core::getSingleton()->getWindow()->addViewport(mLoadingCam,89,0,0,1,1);

				mLoadingSceneManager->clearSpecialCaseRenderQueues();
				mLoadingSceneManager->addSpecialCaseRenderQueue(RENDER_QUEUE_OVERLAY);
				mLoadingSceneManager->setSpecialCaseRenderQueueMode(SceneManager::SCRQM_INCLUDE);


				OverlayManager& omgr = OverlayManager::getSingleton();
				
				// We need to pre-initialise the 'Bootstrap' group so we can use
				// the basic contents in the loading screen
				//Ogre::ResourceGroupManager::ResourceLocation rl;
		        //ResourceGroupManager::getSingleton().addResourceLocation("media/bootstrap", "FileSystem", "Bootstrap");
				ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");



				
				mLoadOverlay = (Overlay*)omgr.getByName("Shmoulette/LoadOverlay");

				mLoadingDescriptionContainer = static_cast<OverlayContainer*>(
					OverlayManager::getSingleton().createOverlayElement("Panel", "loadingdetailscontainer"));


				mLoadingDescriptionElement = static_cast<TextAreaOverlayElement*>(omgr.createOverlayElement("TextArea", "loadingdetails", false));
				mLoadingDescriptionElement->setMetricsMode(Ogre::GMM_RELATIVE);
				mLoadingDescriptionElement->setPosition(.05, .92);
				mLoadingDescriptionElement->setDimensions(.9, .03);
				mLoadingDescriptionElement->setColour(Ogre::ColourValue(1,1,1,1));
				Ogre::Font* f;
				
				mLoadingDescriptionElement->setCharHeight(.03);//FontStyleManager::getSingleton()->requestFontStyle("loadingdetails")->getFont()->getPixelHeight());
				mLoadingDescriptionElement->setFontName(FontStyleManager::getSingleton()->requestFontStyle("loadingdetails")->getFont()->getName());
				mLoadingDescriptionElement->show();
				mLoadingDescriptionElement->setCaption("Loading..."); 
				float ef = Ogre::Math::RangeRandom(0,25);
				ef = Ogre::Math::RangeRandom(0,25);
				ef = Ogre::Math::RangeRandom(0,25);
				ef = Ogre::Math::RangeRandom(0,25);
				ef = Ogre::Math::RangeRandom(0,25);
				int e = (int)(ef);
				char charentry = 'A'+e;
				string stringentry;
				stringentry.push_back(charentry);



				string s = Util::LocalStr::getSingleton()->getString("loading_quotes.txt",stringentry);
				for (int i=0;i<2;i++){
					mFactoid[i] = static_cast<TextAreaOverlayElement*>(omgr.createOverlayElement("TextArea", "factoid"+ts(i), false));
					mFactoid[i]->setMetricsMode(Ogre::GMM_RELATIVE);
					mFactoid[i]->setDimensions(.9, .03);
					mFactoid[i]->setColour(Ogre::ColourValue(1,1,1,1));
					
					mFactoid[i]->setCharHeight(.03);//FontStyleManager::getSingleton()->requestFontStyle("loadingdetails")->getFont()->getPixelHeight());
					mFactoid[i]->setFontName(FontStyleManager::getSingleton()->requestFontStyle("loadingdetails")->getFont()->getName());
					mFactoid[i]->show();
					if (i==0){
						mFactoid[i]->setCaption((string)"\" "+s.substr(0,s.find("|"))); 
						mFactoid[i]->setPosition(.5, .44);
					}else{
						mFactoid[i]->setCaption(s.substr(s.find("|")+1)+" \""); 
						mFactoid[i]->setPosition(.5, .47);
					}
					mFactoid[i]->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Center);
					mLoadingDescriptionContainer->addChild(mFactoid[i]);
				}




				mLoadingDescriptionContainer->addChild(mLoadingDescriptionElement);
				mLoadingDescriptionContainer->setMaterialName("Images/loading_a_jpg");
				mLoadingDescriptionContainer->setMetricsMode(Ogre::GMM_RELATIVE);
				mLoadingDescriptionContainer->setPosition(0,0);
				mLoadingDescriptionContainer->setDimensions(1,1);
				mLoadingDescriptionContainer->show();
				mLoadOverlay->add2D(mLoadingDescriptionContainer);//omgr.getByName("Shmoulette/LoadPanel")->>add2D(mLoadingDescriptionElement);


				if (!mLoadOverlay)
				{
					OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
						"Cannot find loading overlay", "ExampleLoadingBar::start");
				}
				mLoadOverlay->show();

				// Save links to the bar and to the loading text, for updates as we go
				mLoadingBarElement = omgr.getOverlayElement("Shmoulette/LoadPanel/Bar/Progress");
				//mLoadingCommentElement = omgr.getOverlayElement("Core/LoadPanel/Comment");
				//mLoadingDescriptionElement = omgr.getOverlayElement("Core/LoadPanel/Description");

				//OverlayElement* barContainer = omgr.getOverlayElement("Shmoulette/LoadPanel/Bar");
				mProgressBarMaxSize = mLoadingBarElement->getWidth();
				mLoadingBarElement->setWidth(0);
			}


			void ProgressBar::start( 
				unsigned short numGroupsInit, 
				unsigned short numGroupsLoad, 
				Ogre::Real initProportion)
			{
					
				mNumGroupsInit = numGroupsInit;
				mNumGroupsLoad = numGroupsLoad;
				mInitProportion = initProportion;


				// self is listener
				ResourceGroupManager::getSingleton().addResourceGroupListener(this);
			}
			void ProgressBar::finish(void)
			{
				// hide loading screen
				mLoadOverlay->hide();
				Core::getSingleton()->getWindow()->removeViewport(89);
				mLoadingSceneManager->destroyCamera(mLoadingCam);
				Core::getSingleton()->getRoot()->destroySceneManager(mLoadingSceneManager);	

				// Unregister listener
				ResourceGroupManager::getSingleton().removeResourceGroupListener(this);

			}

			// ResourceGroupListener callbacks
			void ProgressBar::resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount)
			{
				assert(mNumGroupsInit > 0 && "You stated you were not going to init "
					"any groups, but you did! Divide by zero would follow...");
				// Lets assume script loading is 70%
				mProgressBarInc = (mProgressBarMaxSize/4) * mInitProportion / (Real)scriptCount;
				mProgressBarInc /= mNumGroupsInit;
				//mLoadingDescriptionElement->setCaption("Parsing scripts...");
				mWindow->update();
			}
			void ProgressBar::setCaption(string newCaption){
				//mLoadingCommentElement->setCaption(newCaption);
				mLoadingDescriptionElement->setCaption((string)(newCaption));
				mWindow->update();
			}
			void ProgressBar::scriptParseStarted(const Ogre::String& scriptName, bool& skip)
			{
				//mLoadingCommentElement->setCaption(scriptName);
				mLoadingDescriptionElement->setCaption((string)("script : "+scriptName));
				mWindow->update();
			}
			void ProgressBar::scriptParseEnded(const Ogre::String& scriptName, bool skip)
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



			void ProgressBar::resourceGroupScriptingEnded(const Ogre::String& groupName)
			{
			}
			void ProgressBar::resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount)
			{
				assert(mNumGroupsLoad > 0 && "You stated you were not going to load "
					"any groups, but you did! Divide by zero would follow...");
				mProgressBarInc = (mProgressBarMaxSize/4) * (mInitProportion) / 
					(Real)resourceCount;
				mProgressBarInc /= mNumGroupsLoad;
				//mLoadingDescriptionElement->setCaption("Loading resources...");
				mWindow->update();
				Profiler::ProfilerFactory::getSingleton()->getProfiler(groupName, "TotalLoadTime")->start();

			}
			void ProgressBar::resourceLoadStarted(const Ogre::ResourcePtr& resource)
			{
				if (mLoadingBarElement->getWidth() < mProgressBarMaxSize){
					mProgressBarInc = 2;
				}
				mLoadingDescriptionElement->setCaption((string)("res : "+resource->getName()));

				//mLoadingCommentElement->setCaption(resource->getName());
				mWindow->update();
			}
			void ProgressBar::resourceLoadEnded(void)
			{
				mLoadingBarElement->setWidth(
				mLoadingBarElement->getWidth() + mProgressBarInc);
				if (mLoadingBarElement->getWidth()>mProgressBarMaxSize){
					mLoadingBarElement->setWidth(mProgressBarMaxSize);
				}
			}
			void ProgressBar::worldGeometryStageStarted(const Ogre::String& description)
			{
				//mLoadingCommentElement->setCaption(description);
				mWindow->update();
			}
			void ProgressBar::worldGeometryStageEnded(void)
			{
				mLoadingBarElement->setWidth(
					mLoadingBarElement->getWidth() + mProgressBarInc);
				mWindow->update();
			}
			void ProgressBar::fileLoadStart(string name){
				Profiler::ProfilerFactory::getSingleton()->getProfiler(name, "LoadFiles")->start();
				mLoadingDescriptionElement->setCaption((string)("file : "+name));
				mWindow->update();
			}
			void ProgressBar::fileLoadEnd(string name){
				Profiler::ProfilerFactory::getSingleton()->getProfiler(name, "LoadFiles")->end();
				mLoadingBarElement->setWidth(
					mLoadingBarElement->getWidth() + .05);
				if (mLoadingBarElement->getWidth()>mProgressBarMaxSize/2){
					mLoadingBarElement->setWidth(mProgressBarMaxSize/2);
				}
				mWindow->update();
			}
			void ProgressBar::resourceGroupLoadEnded(const Ogre::String& groupName)
			{
				Profiler::ProfilerFactory::getSingleton()->getProfiler(groupName, "TotalLoadTime")->end();
				mLoadingDescriptionElement->setCaption("Loading Complete.  Click to Start"); 
				mLoadingDescriptionElement->setPosition(.5, .52);
				mLoadingDescriptionElement->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Center);

				mLoadingBarElement->setWidth(0);
			}

}
#endif