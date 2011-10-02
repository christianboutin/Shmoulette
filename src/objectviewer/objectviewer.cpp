#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>
#include "objectviewer/objectviewer.h"

#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/elements/CEGUICombobox.h>
#include <CEGUI/elements/CEGUIListbox.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIScrollbar.h>
#include "..\..\Samples\Common\CEGUIRenderer\include\OgreCEGUIRenderer.h"
#include "..\..\Samples\Common\CEGUIRenderer\include\OgreCEGUIResourceProvider.h"
#include "ogresupp/misc.h"

/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    CameraTrack.cpp
Description: An example of using AnimationTracks to smoothly make a node
             follow a predefined path, with spline interpolation. Also 
             uses the auto tracking ability of the camera.
-----------------------------------------------------------------------------
*/

//#include "ExampleApplication.h"

//AnimationState* mAnimState;

// Event handler 
/*class CameraTrackListener: public ExampleFrameListener
{
protected:
public:
    CameraTrackListener(RenderWindow* win, Camera* cam)
        : ExampleFrameListener(win, cam)
    {
    }

};*/

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}



class CameraTrackApplication : public ShmouletteBasicApp
{
	list<Ogre::AnimationState*> mAnimState;
public:
    bool frameRenderingQueued(const FrameEvent& evt)
    {
	if( ShmouletteBasicApp::frameRenderingQueued(evt) == false )
		return false;

		if (mKeyboard->isKeyDown(OIS::KC_S)){
			FILE* fff = fopen((char*)(mFilename+"_coord.sov").c_str(), "wb+");
			fwrite((void*)(&mLightNode->getPosition()), sizeof(Ogre::Vector3), 1, fff);
			fwrite((void*)(&mLightNode->getOrientation()), sizeof(Ogre::Quaternion), 1, fff);
			fwrite((void*)(&mObjectNode->getPosition()), sizeof(Ogre::Vector3), 1, fff);
			fwrite((void*)(&mObjectNode->getOrientation()), sizeof(Ogre::Quaternion), 1, fff);
			fwrite((void*)(&mCamNode->getPosition()), sizeof(Ogre::Vector3), 1, fff);
			fwrite((void*)(&mCamNode->getOrientation()), sizeof(Ogre::Quaternion), 1, fff);
			fclose(fff);
		}

		if (mKeyboard->isKeyDown(OIS::KC_L)){
			FILE* fff = fopen((char*)(mFilename+"_coord.sov").c_str(), "rb+");
			Ogre::Vector3 vec;
			Ogre::Quaternion quat;

			fread(&vec, sizeof(Ogre::Vector3), 1, fff);
			fread(&quat, sizeof(Ogre::Quaternion), 1, fff);
			mLightNode->setPosition(vec);
			mLightNode->setOrientation(quat);
			fread(&vec, sizeof(Ogre::Vector3), 1, fff);
			fread(&quat, sizeof(Ogre::Quaternion), 1, fff);
			mObjectNode->setPosition(vec);
			mObjectNode->setOrientation(quat);
			fread(&vec, sizeof(Ogre::Vector3), 1, fff);
			fread(&quat, sizeof(Ogre::Quaternion), 1, fff);
			mCamNode->setPosition(vec);
			mCamNode->setOrientation(quat);
			fclose(fff);
		}



        //mAnimState->addTime(evt.timeSinceLastFrame);

		SceneNode* nodeToMove = mCamNode;
		if (mKeyboard->isKeyDown(OIS::KC_LCONTROL)){
			nodeToMove = mLightNode;
		}else if (mKeyboard->isKeyDown(OIS::KC_LMENU)){
			nodeToMove = mObjectNode;
		}

		if (nodeToMove == mObjectNode){
			nodeToMove->roll(Ogre::Radian(mMouse->getMouseState().X.rel)/100);
		}else{
			nodeToMove->yaw(Ogre::Radian(mMouse->getMouseState().X.rel)/100);
		}
		nodeToMove->pitch(Ogre::Radian(0-mMouse->getMouseState().Y.rel)/100);
		
		//mLightNode->yaw(Ogre::Radian(0-evt.timeSinceLastFrame));


        return true;
    }

	bool frameStarted(const FrameEvent& evt){
		for (list<Ogre::AnimationState*>::iterator it = mAnimState.begin();it!= mAnimState.end();it++){
			(*it)->addTime(evt.timeSinceLastFrame);
		}
		return true;
	}
    CameraTrackApplication() {
    
    }

    ~CameraTrackApplication() {  }
	
	static int dummyCounter;
protected:
	SceneNode* mCamNode;
	SceneNode* mFountainNode;
	SceneNode* mLightNode;
	//Light * mLight;
	SceneNode* mObjectNode;
	string mFilename;


    // Just override the mandatory create scene method
	void createLight(Ogre::ColourValue d, Ogre::ColourValue s, Ogre::Vector3 p, float distance, Ogre::Light::LightTypes type = Ogre::Light::LT_POINT, bool useBalls=false){
		Ogre::SceneNode* ln1 = mLightNode->createChildSceneNode();
		stringstream ss;
		ss << dummyCounter++;

		if (useBalls){
			Ogre::Entity* ent = mSceneMgr->createEntity("ln1Cube"+ss.str(),"DummyCube.mesh");
			//ent->setMaterial(MaterialManager::getSingleton().getByName("DummyCubeEmit"));
			Ogre::MaterialPtr m = MaterialManager::getSingleton().getByName("DummyCube");

			ent->setMaterial(
				m->clone("DummyCubeMat"+ss.str())
			);
			ent->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->setSelfIllumination(d);
			ent->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue::Black);
		
			ln1->attachObject(ent);
		}
		ln1->setPosition(p);
		
        Light* l1 = mSceneMgr->createLight("MainLight"+ss.str());
		l1->setCastShadows(true);
		l1->setType(type);
        l1->setPosition(0,0,0);
        l1->setDirection(0-p.x,0-p.y,0-p.z);
		l1->setAttenuation(FLT_MAX,0,0,0);
		//mLight->setDiffuseColour(.5, .5, .75);
		l1->setDiffuseColour(d);
		l1->setSpecularColour(s);
		ln1->attachObject(l1);
	}
    void createScene(void)
    {
		//mSceneMgr->setShowDebugShadows(true);
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		mSceneMgr->setShadowTextureSettings(1024, 2);
        mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));

        // Set ambient light
        //mSceneMgr->setAmbientLight(ColourValue(0,0,0));
        mSceneMgr->setAmbientLight(ColourValue(.75, .75, .75));

        // Create a skydome
        
		string skybox = "";
		list<string> anim;
		float scale = 1;

		ConfigFile cf;
		cf.load("ShmouletteObjectViewer.cfg");

		Ogre::ConfigFile::SettingsIterator sit = cf.getSettingsIterator(cf.getSetting("Section"));
		//sit = cf.getSettingsIterator("ACTUAL");
		
		//Ogre::ConfigFile::SettingsMultiMap
		vector<string> lightInfo;

		while(sit.hasMoreElements()){
			string key = sit.peekNextKey();
			string value = sit.getNext();
			if (key == "Mesh"){
				mFilename = value;
			}
			if (key == "Skybox"){
				skybox = value;
			}
			if (key == "Scale"){
				scale = atof((const char*)value.c_str());
			}
			if (key == "Light"){
				lightInfo.push_back(value);
			}
		}

		//mFilename = cf.getSetting("Mesh");
		
		//skybox = cf.getSetting("Skybox");

		

		if (skybox != ""){
			mSceneMgr->setSkyBox(true, skybox ,20);
		}

        Entity *ent;

        mObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		mObjectNode->setPosition(0,0,0);




		/*MeshPtr pMesh = MeshManager::getSingleton().load(mFilename,
               ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,    
		       HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
			   HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
				true, true); //so we can still read it
        // Build tangent vectors, all our meshes use only 1 texture coordset 
		// Note we can build into VES_TANGENT now (SM2+)
        unsigned short src, dest;
        if (!pMesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
        {
            pMesh->buildTangentVectors(VES_TANGENT, src, dest);
			// Second mode cleans mirrored / rotated UVs but requires quality models
			//pMesh->buildTangentVectors(VES_TANGENT, src, dest, true, true);
        }
        // Create entity*/
        ent =  mSceneMgr->createEntity("Entity", 
            mFilename);
		ent->setCastShadows(true);
		//ent = mSceneMgr->createEntity("Entity", "B397.mesh");


        mObjectNode->attachObject(ent);
		mObjectNode->setScale(scale,scale,scale);

		Ogre::MeshPtr meshPtr = ent->getMesh();
		float distance=0;
		size_t vertex_count,index_count;
		Vector3* vertices;
		unsigned long* indices;
		
		Shmoulette::OgreSupp::getMeshInformation(meshPtr,
			vertex_count,
			vertices,
			index_count,
			indices);

		Ogre::Mesh::SubMeshIterator smi = meshPtr->getSubMeshIterator();
		Ogre::AxisAlignedBox ab;

		while(smi.hasMoreElements()){
			Ogre::SubMesh* sm = smi.getNext();
			for (int i = 0;i<sm->vertexData->vertexCount;i++){
					Ogre::VertexBoneAssignment vbal;
					//vbal = it.getNext();

					Ogre::Vector3 vpos;
					vpos = vertices[i];
					if (fabs(vpos.x)>distance){
						distance = fabs(vpos.x);
					}
					if (fabs(vpos.y)>distance){
						distance = fabs(vpos.y);
					}
					if (fabs(vpos.z)>distance){
						distance = fabs(vpos.z);
					}
			}
				

		}
		int g=0;


		/*Ogre::Entity* dumbent = mSceneMgr->createEntity("DummyPlane", "DummyPlane.mesh");
		Ogre::SceneNode* scenenode = mObjectNode->createChildSceneNode("DummyPlane");
		scenenode->attachObject(dumbent);
		scenenode->setPosition(0, 0-distance, 0);
		scenenode->setScale(5,5,1);
		scenenode->setVisible(true);
		scenenode->pitch(Ogre::Radian(0-Math::PI/2));
		dumbent->setVisible(true);*/


        // Make sure the camera track this node
        //mCamera->setAutoTracking(true, headNode);

        // Create the camera node & attach camera
        mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mCamNode->attachObject(mCamera);
		mCamera->setPosition(0,0,distance*4);
		mCamera->setNearClipDistance(.1);
		mCamNode->setPosition(0,0,0);
		mCamNode->setOrientation(0,0,1,0);
		//mSceneMgr->setAmbientLight(Ogre::ColourValue(0,0,0));

		Ogre::CompositorInstance* oI = CompositorManager::getSingleton().addCompositor(mWindow->getViewport(0), "Bloom");
		oI->setEnabled(true);

		mLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		mLightNode->setPosition(0,0,0);

		for (vector<string>::iterator lit = lightInfo.begin();lit!= lightInfo.end();lit++){

			vector<string> params;
			Tokenize((*lit),params,"|");

			vector<string> diff,spec,pos;
			Tokenize(params[0], diff, ",");
			Tokenize(params[1], spec, ",");
			Tokenize(params[2], pos, ",");

			Ogre::Light::LightTypes lt;
			bool useBalls = false;
			if (params.size() > 4){
				if (params[4] == "B"){
					useBalls = true;
				}

			}

			if (params.size() > 3){
				if (params[3] == "D"){
					lt = Ogre::Light::LT_DIRECTIONAL;
				}else if (params[3] == "S"){
					lt = Ogre::Light::LT_SPOTLIGHT;
				}else{
					lt = Ogre::Light::LT_POINT;
				}
			}else{
				lt = Ogre::Light::LT_POINT;
			}

			
			createLight(
				Ogre::ColourValue(atof(diff[0].c_str()),atof(diff[1].c_str()),atof(diff[2].c_str())), 
				Ogre::ColourValue(atof(spec[0].c_str()),atof(spec[1].c_str()),atof(spec[2].c_str())), 
				Ogre::Vector3(
					distance*atof(pos[0].c_str()),
					distance*atof(pos[1].c_str()),
					distance*atof(pos[2].c_str())
				),
				distance*8,
				lt,
				useBalls

			);
		}
		/*this->createLight(Ogre::ColourValue(0,0,1), Ogre::ColourValue::White, Ogre::Vector3(0-distance*4,0,0), distance*4);
		this->createLight(Ogre::ColourValue(1,0,0), Ogre::ColourValue::White, Ogre::Vector3(distance*4,0,0), distance*4);*/

		/*Ogre::SceneNode* ln1 = mLightNode->createChildSceneNode();
		ln1->attachObject(mSceneMgr->createEntity("ln1Cube","DummyCube.mesh"));
		ln1->setPosition(distance*4,0,0);
        Light* l1 = mSceneMgr->createLight("MainLight");
        l1->setPosition(0,0,0);
		l1->setAttenuation(distance*4,0,0,0);
		//mLight->setDiffuseColour(.5, .5, .75);
		l1->setDiffuseColour(0,0,1);
		l1->setSpecularColour(1, 1, 1);
		ln1->attachObject(l1);

		Ogre::SceneNode* ln2 = mLightNode->createChildSceneNode();
		ln2->setPosition(0-distance*4,0,0);
		Ogre::Light* l2 = mSceneMgr->createLight("MainLight2");
		l2->setAttenuation(distance*4,0,0,0);
        l2->setPosition(0,0,0);
		//mLight->setDiffuseColour(.5,.75,.5);
		l2->setDiffuseColour(1,0,0);
		l2->setSpecularColour(1,1,1);
		ln2->attachObject(l2);
		ln2->attachObject(mSceneMgr->createEntity("ln2Cube","DummyCube.mesh"));*/

		//Ogre::SceneNode* extraLight = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//mLightNode->attachObject(mLight);

		//ent->getAnimationState("Deploy")->setEnabled(true);
		//ent->getAnimationState("Deploy")->setTimePosition(.2);

		if (anim.size() > 0){
			for (list<string>::iterator it = anim.begin();it!= anim.end();it++){
				Ogre::AnimationState* as;
				as = ent->getAnimationState(*it);
				as->setEnabled(true);
				mAnimState.push_back(as);
			}
		}

		if (mFilename == "GAMkI32AP.mesh"){
			string anim="aim";
			ent->getAnimationState(anim)->setEnabled(true);
			ent->getAnimationState(anim)->setTimePosition(.5);
			anim="fire";
			ent->getAnimationState(anim)->setEnabled(true);
			ent->getAnimationState(anim)->setTimePosition(.5);
		}

		if (mFilename == "B56.mesh"){
			string anim="LeftSweep";
			ent->getAnimationState(anim)->setEnabled(true);
			ent->getAnimationState(anim)->setTimePosition(1);
		}

		if (mFilename == "AGDD1000Camera.mesh"){
			string anim="ApertureClose";
			ent->getAnimationState(anim)->setEnabled(true);
			ent->getAnimationState(anim)->setTimePosition(.5);
		}


		if (mFilename == "DefaultBeam.mesh"){
			ent->getParentNode()->setScale(.04,.04,2);
			
			//ent->getAnimationState(anim)->setEnabled(true);
			//ent->getAnimationState(anim)->setTimePosition(1);
		}

		/*SkeletonPtr skel = SkeletonManager::getSingleton().load("SM334.skeleton", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Animation* anim = skel->getAnimation("Deploy");*/
		
		
		//Animation::NodeTrackIterator trackIter = anim->getNodeTrackIterator();



        // set up spline animation of node
        /*Animation* anim = mSceneMgr->createAnimation("CameraTrack", 10);
        // Spline it for nice curves
        anim->setInterpolationMode(Animation::IM_SPLINE);
        // Create a track to animate the camera's node
        NodeAnimationTrack* track = anim->createNodeTrack(0, mCamNode);
        // Setup keyframes
        TransformKeyFrame* key = track->createNodeKeyFrame(0); // startposition
		float vdist = distance/2;
		distance*=2;
        key->setTranslate(Vector3(0-distance,vdist,0));
        key = track->createNodeKeyFrame(2.5);
        key->setTranslate(Vector3(0,vdist,0-distance));
        key = track->createNodeKeyFrame(5);
        key->setTranslate(Vector3(distance,vdist,0));
        key = track->createNodeKeyFrame(7.5);
        key->setTranslate(Vector3(0,vdist,distance));
        key = track->createNodeKeyFrame(10);
        key->setTranslate(Vector3(0-distance,vdist,0));
        // Create a new animation state to track this
        mAnimState = mSceneMgr->createAnimationState("CameraTrack");
        mAnimState->setEnabled(true);*/

        // Put in a bit of fog for the hell of it
        //mSceneMgr->setFog(FOG_EXP, ColourValue::White, 0.0002);

    }

    // Create new frame listener
    /*void createFrameListener(void)
    {
        mFrameListener= new CameraTrackListener(mWindow, mCamera);
        mRoot->addFrameListener(mFrameListener);

    }*/


};

int CameraTrackApplication::dummyCounter = 0;


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    CameraTrackApplication app;

    try {
        app.go();
    } catch( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
    }


    return 0;
}

#ifdef __cplusplus
}
#endif