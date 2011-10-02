#include "stdafx.h"

#include "main.h"
#include "world\world.h"
#include "OgreTerrainSceneManager.h"

namespace Shmoulette{
void World::init(LevelSegment* ls){
	Parent::init();
	mLevelSegment = ls;
	mLoadGeometry = false;
	mTerrain.mType = ST_GENERIC;
	mTerrain.mConfig = "";
	mSky.mType = SKY_NONE;
	mFogOn = false;
	mAmbientLight.current.r=.2;
	mAmbientLight.current.g=.2;
	mAmbientLight.current.b=.2;
	mFogTimeLeft = 0;
	mShadow = SHADOWTYPE_NONE;
}

Ogre::ShadowTechnique World::shadowTypeFromString(string s){
	if (s == "NONE"){
		return SHADOWTYPE_NONE;
	}else if (s == "TEXTURE_MODULATIVE_INTEGRATED"){
		return SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED;
	}else if (s == "TEXTURE_ADDITIVE_INTEGRATED"){
		return SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED;
	}else if (s == "TEXTURE_ADDITIVE"){
        return SHADOWTYPE_TEXTURE_ADDITIVE;
	}else if (s == "TEXTURE_MODULATIVE"){
        return SHADOWTYPE_TEXTURE_MODULATIVE;
	}else if (s == "STENCIL_ADDITIVE"){
        return SHADOWTYPE_STENCIL_ADDITIVE;
	}else if (s == "STENCIL_MODULATIVE"){
        return SHADOWTYPE_STENCIL_MODULATIVE;
	}else{
		throw ("Unrecognized shadowtype : "+s);
	}

}

void World::init(XML* xml, LevelSegment* ls){
	Parent::init(xml);
	mLevelSegment = ls;
	mTerrain.mConfig = xml->getString("world", "config");
	if (xml->getError() == XML_OK){
		Level::getSingleton()->addResourceContext(mTerrain.mConfig,"scene");

		mLoadGeometry = true;
		string s = xml->getString("world", "type");
		if (xml->getError() != XML_OK){
			mTerrain.mType = ST_EXTERIOR_CLOSE;
		}else{
			if (s == "ST_EXTERIOR_CLOSE")
				mTerrain.mType = ST_EXTERIOR_CLOSE;
			if (s == "ST_EXTERIOR_FAR")
				mTerrain.mType = ST_EXTERIOR_FAR;
			if (s == "ST_EXTERIOR_VERYFAR")
				mTerrain.mType = ST_EXTERIOR_REAL_FAR;
			if (s == "ST_INTERIOR")
				mTerrain.mType = ST_INTERIOR;
		}
	}else{
		mLoadGeometry = false;
		mTerrain.mType = ST_GENERIC;
		mTerrain.mConfig ="";
	}
	string s = xml->getString("ambientlight");
	if (xml->getError() == XML_OK){
		mAmbientLight.current = colourValueFromString(s);
	}else{
		mAmbientLight.current.r=xml->getFloat("world>ambientlight", "r");
		mAmbientLight.current.g=xml->getFloat("world>ambientlight", "g");
		mAmbientLight.current.b=xml->getFloat("world>ambientlight", "b");
	}
	mAmbientLight.wanted.r=mAmbientLight.current.r;
	mAmbientLight.wanted.g=mAmbientLight.current.g;
	mAmbientLight.wanted.b=mAmbientLight.current.b;
	mAmbientLight.speed.r = 0;
	mAmbientLight.speed.g = 0;
	mAmbientLight.speed.b = 0;
	mFogTimeLeft=0;
	
	mSky.mMaterial = xml->getString("skymat");
	if (xml->getError() != XML_OK){
		mSky.mMaterial = xml->getString("world>sky", "material");
	}
	string skyType = xml->getString("skytype");
	if (xml->getError() != XML_OK){
		skyType = xml->getString("world>sky", "type");
	}
	if (xml->getError() == XML_OK){
		if (skyType == "dome"){
			/*mLevelSegment->getSceneManager()->setSkyDome(
				true, 
				mSky.mMaterial, 
				xml->getFloat("sky", "curvature"), 
				xml->getFloat("sky", "tiling")
				);*/
			mSky.mType = SKY_DOME;

		}else if (skyType == "cube"){
			mSky.mType = SKY_CUBE;
			mSky.mDistance = xml->getFloat("skydist");
			if (xml->getError() != XML_OK){
				mSky.mDistance = xml->getFloat("world>sky", "distance");
				if (xml->getError() != XML_OK){
					mSky.mDistance = 20000;
				}
			}

		}else if (skyType == "plane"){
			mSky.mType = SKY_PLANE;
		}else{
			throw("Sky Type unrecognized : "+skyType);
		}
	}else{
		mSky.mType = SKY_NONE;
	}

		

	mFog.current.colour = colourValueFromString(xml->getString("world>fog", "colour"));
	if (xml->getError() != XML_OK){
		mFog.current.colour = Ogre::ColourValue::Black;
	}
	string tmp;
	tmp = xml->getString("world>fog", "mode");
	mFogOn = true;
	if (tmp == "linear"){
		mFog.current.mode = Ogre::FOG_LINEAR;
	}else if (tmp == "exp"){
		mFog.current.mode = Ogre::FOG_EXP;
	}else if (tmp == "exp2"){
		mFog.current.mode = Ogre::FOG_EXP2;
	}else{
		mFogOn = false;
	}
	
	mFog.current.expValue = xml->getFloat("world>fog", "exp");
	if (xml->getError() != XML_OK) mFog.current.expValue = 0;
	mFog.current.linearStart = xml->getFloat("world>fog", "linearstart");
	if (xml->getError() != XML_OK) mFog.current.linearStart = 0;
	mFog.current.linearEnd = xml->getFloat("world>fog", "linearend");
	if (xml->getError() != XML_OK) mFog.current.linearEnd = 0;
	mFog.wanted = mFog.current;

	tmp = xml->getString("shadow");
	if (xml->getError() != XML_OK){
		tmp = xml->getString("world>shadow", "type");
	}
	if (xml->getError() != XML_OK){
		tmp = "NONE";
	}

	mShadow = shadowTypeFromString(tmp);
	
	//Level::getSingleton()->getSceneManager()->setDisplaySceneNodes(true);
	//gCamera = Shmoulette::Level::getSingleton()->getSceneManager()->createCamera("PlayerCam");
    //Shmoulette::Level::getSingleton()->getActiveCamera()->getCamera()->setAspectRatio(1.333f);

	/*int current = 1;
	string tmp;
	tmp = xml->getString("world>water", "posx"); // Only used to trigger an error, doesn't use the info
	while (xml->getError() == XML_OK){
		WaterZone* wm = new WaterZone();

		tmp = "water<"+ts(current);

		wm->dataFromXML(xml->extractElem(tmp));
		
		//mVectorEnviroZone.push_back(wm);
		Level::getSingleton()->vectorGameObj.push_back(wm);

		current++;
		stringstream strCurrent;
		strCurrent << current ;
		 
		tmp = xml->getString(
			(string)"world>water<"+strCurrent.str(), 
			(string)"posx"); // Only used to trigger an error, doesn't use the info
		
	}*/
}

World::~World(){
	//Core::getSingleton()->getRoot()->destroySceneManager(Level::getSingleton()->getSceneManager());
}


void World::build(){
	Parent::build();
	if (mLoadGeometry){
		//Level::getSingleton()->ChangeSceneManager(mTerrain.mType, mTerrain.mConfig);
		//Ogre::Camera* cam;
		//cam = Level::getSingleton()->getSceneManager()->createCamera("DummyCam");
		//Ogre::Viewport* vp;
		//vp = Core::getSingleton()->getWindow()->addViewport(cam,64,0,0,1,1);
		mLevelSegment->getSceneManager()->setWorldGeometry( "media\\scene\\"+mTerrain.mConfig+"\\"+mTerrain.mConfig+".cfg" );

		
		/*Core::getSingleton()->getWindow()->removeViewport(64);
		Level::getSingleton()->getSceneManager()->destroyCamera(cam);*/

		/*Ogre::SceneManager* sm = mLevelSegment->getSceneManager();
		Ogre::Camera* cam;
		cam = sm->createCamera("StupidAssCam");
		Ogre::Viewport* vp;
		vp = Core::getSingleton()->getWindow()->addViewport(cam,64,0,0,.5,.5);
		cam->setDirection(0, 0, 1);
		cam->setPosition(0,0,0);
		//sm->setWorldGeometry( mTerrain.mConfig );
		Core::getSingleton()->getWindow()->removeViewport(64);
		sm->destroyCamera(cam);*/


		//Util::Log("Building Geometry");
	}
	switch(mSky.mType){
		case SKY_CUBE:
			mLevelSegment->getSceneManager()->setSkyBox(
				true,
				mSky.mMaterial, 
				mSky.mDistance
				);
		break;
		case SKY_NONE:
		break;
		default:
			throw("Too lazy to do other than sky cube");
		break;
	}
	
	
	mLevelSegment->getSceneManager()->setShadowTechnique(mShadow);
	mLevelSegment->getSceneManager()->setShadowColour(mAmbientLight.current);
	
	


	if (
		mShadow == SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED ||
		mShadow == SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED ||
        mShadow == SHADOWTYPE_TEXTURE_ADDITIVE ||
		mShadow == SHADOWTYPE_TEXTURE_MODULATIVE){
			mLevelSegment->getSceneManager()->setShadowDirectionalLightExtrusionDistance(1000000);
			mLevelSegment->getSceneManager()->setShadowTextureSize(1024);
	}


	mLevelSegment->getSceneManager()->setAmbientLight(mAmbientLight.current);
	if (mFogOn){
		mLevelSegment->getSceneManager()->setFog(mFog.current.mode, mFog.current.colour, mFog.current.expValue, mFog.current.linearStart,
			mFog.current.linearEnd);
	}

	/*for (unsigned int i=0;i<mVectorEnviroZone.size();i++){
		mVectorEnviroZone[i]->build();
	}*/

}

void World::destroy(){
	Parent::destroy();
	
}

float World::getHeightAt(float x, float z){
	if (mLoadGeometry == true){
		return static_cast<Ogre::TerrainSceneManager*>(mLevelSegment->getSceneManager())->getHeightAt(x, z);	
		//Level::getSingleton() used to be at mLevelSegment (for editor, check if necessary)
	}
	return 0-FLT_MAX;
}

void World::configChange(string filename){
	Ogre::SceneNode* tempSceneNode;
	Ogre::SceneManager* tempSceneManager;
	tempSceneManager = Core::getSingleton()->getRoot()->createSceneManager(mTerrain.mType, "World2");
	
	mLevelSegment->getContainer()->getParentSceneNode()->removeChild(
		mLevelSegment->getContainer()
		);


	mLevelSegment->getSceneManager()->destroyAllStaticGeometry();//>destroyStaticGeometry("World/Terrain");
	//Level::getSingleton() used to be at mLevelSegment (for editor, check if necessary)

	//Level::getSingleton()->getSceneManager()->setWorldGeometry( mTerrain.mConfig );
}

void World::fogChange(FOG_INFO newFog, Ogre::Real timer){
	mFog.wanted = newFog;
	mFog.speed.colour.r = ( mFog.wanted.colour.r - mFog.current.colour.r ) / timer;
	mFog.speed.colour.g = ( mFog.wanted.colour.g - mFog.current.colour.g ) / timer;
	mFog.speed.colour.b = ( mFog.wanted.colour.b - mFog.current.colour.b ) / timer;
	mFog.speed.linearEnd = ( mFog.wanted.linearEnd - mFog.current.linearEnd ) / timer;
	mFog.speed.linearStart = ( mFog.wanted.linearStart - mFog.current.linearStart ) / timer;
	mFogTimeLeft = timer;
}

bool World::update(double timeDelta, double parentTime){

	//double timeDelta = mLevelSegment->getTimeDelta();
	if (mFogOn){
		if (mFogTimeLeft > timeDelta){
			mFog.current.colour.r+=mFog.speed.colour.r*timeDelta;
			mFog.current.colour.g+=mFog.speed.colour.g*timeDelta;
			mFog.current.colour.b+=mFog.speed.colour.b*timeDelta;
			mFog.current.linearEnd+=mFog.speed.linearEnd*timeDelta;
			mFog.current.linearStart+=mFog.speed.linearStart*timeDelta;
			mFogTimeLeft -= timeDelta;
		}else{
			mFog.current.colour.r = mFog.wanted.colour.r;
			mFog.current.colour.g = mFog.wanted.colour.g;
			mFog.current.colour.b = mFog.wanted.colour.b;
			mFog.current.linearEnd = mFog.wanted.linearEnd;
			mFog.current.linearStart = mFog.wanted.linearStart;
			mFogTimeLeft = 0;
		}
		mLevelSegment->getSceneManager()->setFog(mFog.current.mode, mFog.current.colour, mFog.current.expValue, mFog.current.linearStart,
			mFog.current.linearEnd);
	}

	return Parent::update(timeDelta, parentTime);
	//build();
	/*for (unsigned int i=0;i<mVectorEnviroZone.size();i++){
		mVectorEnviroZone[i]->update();
	}*/
//	return false;
}



}