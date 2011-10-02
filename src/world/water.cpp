#include "stdafx.h"

#include "main.h"
#include "world\water.h"
#include "elements\camera.h"
#include "level.h"

#if 0
namespace Shmoulette{
using namespace Ogre;
void ReflectionTextureListener::preRenderTargetUpdate(const RenderTargetEvent& evt)
{
    // Hide plane and objects below the water
    mWater->setVisible(false);


	/*list<Ogre::Entity*> entList = Level::getSingleton()->getEntityList();
	list<Ogre::Entity*>::iterator it;
	for (it = entList.begin();it != entList.end();it++){
		if ((*it)->isVisible()){
			Ogre::AxisAlignedBox box = (*it)->getWorldBoundingBox();
			if (box.getMinimum().y < mWater->getSceneNode()->_getDerivedPosition().y){
				(*it)->setVisible(false);
				belowWaterEnts.push_back((*it));
			}
		}
	}*/

    /*std::vector<Entity*>::iterator i, iend;
    iend = belowWaterEnts.end();
    for (i = belowWaterEnts.begin(); i != iend; ++i)
    {
        (*i)->setVisible(false);
    }*/

	Ogre::SceneManager::MovableObjectIterator it = Level::getSingleton()->getSceneManager()->getMovableObjectIterator("Entity");
	while(it.hasMoreElements()){
		Ogre::MovableObject *m = it.getNext();	
		if (m->isVisible() && m->getParentSceneNode()->_getDerivedPosition().y < mWater->getSceneNode()->_getDerivedPosition().y){
			m->setVisible(false);
			belowWaterEnts.push_back(m);
		}
	}


	Ogre::Camera* cam = Level::getSingleton()->getCurrentSegment()->getActiveCamera()->getCamera();
	cam->enableReflection(mWater->getReflectionPlane());

}
void ReflectionTextureListener::postRenderTargetUpdate(const RenderTargetEvent& evt)
{
    // Show plane and objects below the water
	

    mWater->setVisible(true);
	std::vector<Ogre::MovableObject*>::iterator i, iend;
	iend = belowWaterEnts.end();
    for (i = belowWaterEnts.begin(); i != iend; ++i)
    {
        (*i)->setVisible(true);
    }
	belowWaterEnts.clear();

	/*Ogre::SceneManager::MovableObjectIterator it = Level::getSingleton()->getSceneManager()->getMovableObjectIterator("Entity");
	while(it.hasMoreElements()){
		Ogre::MovableObject *m = it.getNext();	
		if (m->getParentSceneNode()->_getDerivedPosition().y < mWater->getSceneNode()->_getDerivedPosition().y){
			m->setVisible(true);
		}
	}*/
	

	Ogre::Camera* cam = Level::getSingleton()->getCurrentSegment()->getActiveCamera()->getCamera();
	cam->disableReflection();
}

void RefractionTextureListener::preRenderTargetUpdate(const RenderTargetEvent& evt)
{
    // Hide plane and objects above the water
    mWater->setVisible(false);

	//Level::getSingleton()->getPlayerShip()->getSceneNode()->setVisible(false);

	/*list<Ogre::Entity*> entList = Level::getSingleton()->getEntityList();
	list<Ogre::Entity*>::iterator it;
	for (it = entList.begin();it != entList.end();it++){
		if ((*it)->isVisible()){
			Ogre::AxisAlignedBox box = (*it)->getWorldBoundingBox();
			if (box.getMaximum().y > mWater->getSceneNode()->_getDerivedPosition().y){
				(*it)->setVisible(false);
				aboveWaterEnts.push_back((*it));
			}
		}
	}*/


    /*std::vector<Entity*>::iterator i, iend;
    iend = aboveWaterEnts.end();
    for (i = aboveWaterEnts.begin(); i != iend; ++i)
    {
        (*i)->setVisible(false);
    }*/

	Ogre::SceneManager::MovableObjectIterator it = Level::getSingleton()->getSceneManager()->getMovableObjectIterator("Entity");
	while(it.hasMoreElements()){
		Ogre::MovableObject *m = it.getNext();	
		if (m->isVisible() && m->getParentSceneNode()->_getDerivedPosition().y > mWater->getSceneNode()->_getDerivedPosition().y){
			m->setVisible(false);
			aboveWaterEnts.push_back(m);
		}
	}

}
void RefractionTextureListener::postRenderTargetUpdate(const RenderTargetEvent& evt)
{
         // Show plane and objects above the water
    mWater->setVisible(true);
	//Level::getSingleton()->getPlayerShip()->getSceneNode()->setVisible(true);

    std::vector<MovableObject*>::iterator i, iend;
	iend = aboveWaterEnts.end();
    for (i = aboveWaterEnts.begin(); i != iend; ++i)
    {
        (*i)->setVisible(true);
    }
	aboveWaterEnts.clear();

		//std::vector<Entity*>::iterator i, iend;
        //iend = aboveWaterEnts.end();
	
		
	/*Ogre::SceneManager::MovableObjectIterator it = Level::getSingleton()->getSceneManager()->getMovableObjectIterator("Entity");
	while(it.hasMoreElements()){
		Ogre::MovableObject *m = it.getNext();	
		if (m->getParentSceneNode()->_getDerivedPosition().y > mWater->getSceneNode()->_getDerivedPosition().y){
			m->setVisible(true);
		}
	}*/
}



void WaterZone::destroy(){
}
void WaterZone::build(){
	mReflectionListener = new ReflectionTextureListener();
	mRefractionListener = new RefractionTextureListener();
	mReflectionListener->setWater(this);
	mRefractionListener->setWater(this);
	RenderTarget *rttTex;    
    TexturePtr mRefractionTexture = TextureManager::getSingleton().createManual( "Refraction", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
		512, 512, 0, PF_R8G8B8, TU_RENDERTARGET );
    //RenderTexture* rttTex = mRoot->getRenderSystem()->createRenderTexture( "Refraction", 512, 512 );
    rttTex = mRefractionTexture->getBuffer()->getRenderTarget();
	
    {
        Viewport *v = rttTex->addViewport( Level::getSingleton()->getCamera() );
        MaterialPtr mat = MaterialManager::getSingleton().getByName(mMaterialName);
        mat->getTechnique(0)->getPass(0)->getTextureUnitState(2)->setTextureName("Refraction");
        v->setOverlaysEnabled(false);
        rttTex->addListener(mRefractionListener);
    }
    
	TexturePtr mReflectionTexture = TextureManager::getSingleton().createManual( "Reflection", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
		512, 512, 0, PF_R8G8B8, TU_RENDERTARGET );
    //rttTex = mRoot->getRenderSystem()->createRenderTexture( "Reflection", 512, 512 );
    rttTex = mReflectionTexture->getBuffer()->getRenderTarget();
    {
        Viewport *v = rttTex->addViewport( Level::getSingleton()->getCamera() );
        MaterialPtr mat = MaterialManager::getSingleton().getByName(mMaterialName);
        mat->getTechnique(0)->getPass(0)->getTextureUnitState(1)->setTextureName("Reflection");
        v->setOverlaysEnabled(false);
        rttTex->addListener(mReflectionListener);
    }
	
	
	
	
	/*mRefractionTexture = Core::getSingleton()->getRoot()->getRenderSystem()->createRenderTexture( "Refraction", 256, 256 );
	{
        Viewport *v = mRefractionTexture->addViewport( Level::getSingleton()->getCurrentSegment()->getActiveCamera()->getCamera() );
        MaterialPtr mat = MaterialManager::getSingleton().getByName("Examples/FresnelReflectionRefraction");
        mat->getTechnique(0)->getPass(0)->getTextureUnitState(2)->setTextureName("Refraction");
        v->setOverlaysEnabled(false);
        mRefractionTexture->addListener(mRefractionListener);
	}
    mReflectionTexture = Core::getSingleton()->getRoot()->getRenderSystem()->createRenderTexture( "Reflection", 256, 256 );
	{
		Viewport *v = mReflectionTexture->addViewport( Level::getSingleton()->getCurrentSegment()->getActiveCamera()->getCamera() );
		MaterialPtr mat = MaterialManager::getSingleton().getByName("Examples/FresnelReflectionRefraction");
		mat->getTechnique(0)->getPass(0)->getTextureUnitState(1)->setTextureName("Reflection");
		v->setOverlaysEnabled(true);
		mReflectionTexture->addListener(mReflectionListener);
	}*/

	
	mReflectionPlane.normal = Vector3::UNIT_Y;
    mReflectionPlane.d = 0;
        MeshManager::getSingleton().createPlane("ReflectPlane",
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            mReflectionPlane,
            1000,1000,10,10,true,1,3,5,Vector3::UNIT_Z);

	
	/*MeshManager::getSingleton().createPlane("ReflectPlane",
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        mReflectionPlane,
        300,300,10,10,true,1,5,5,Vector3::UNIT_Z);*/
	mEntity = Level::getSingleton()->getSceneManager()->createEntity( "plane", "ReflectPlane" );
    mEntity->setMaterialName(mMaterialName);
	//Level::getSingleton()->getSceneManager()->getRootSceneNode()->attachObject(mEntity);
	mSceneNode->attachObject(mEntity);

	mSceneNode->setPosition(Vector3(0, 0, 0));
	mSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
	//mSceneNode->roll((Ogre::Radian)(Ogre::Degree)45);
    //mSceneMgr->getRootSceneNode()->createChildSceneNode()

	Parent::build();


}

WaterZone::WaterZone(XML* xml):Parent(xml){
	mType = OBJ_TYPE_WATERZONE;
	setId("Water", true);
	mMaterialName = xml->getString("material");

}

WaterZone::~WaterZone(){
}

bool WaterZone::update(){
	mSceneNode->setPosition(0,Ogre::Math::RangeRandom(-10,5),0);
	return Parent::update();
}


}
#endif