#include "stdafx.h"

#include "main.h"
#include "world\groundplane.h"
#include "level.h"
#if 0

namespace Shmoulette{
	using namespace Ogre;

	GroundPlane::GroundPlane(XML* xml):Parent(xml){
		mType = OBJ_TYPE_GROUNDPLANE;
		//setCollisionGroup(COLLGROUP_ALL);
		setMaterial(xml->getString("material"));
		if (xml->getError() != XML_OK){
			throw("Ground Plane without Material");
		}
		Ogre::Vector2 v = vector2FromString(xml->getString("zonesize"));
		if (xml->getError() != XML_OK){
			throw("Ground Plane without Zone Size");
		}
		setZoneSize(v.x, v.y);//xml->getFloat("zonesizex"), xml->getFloat("zonesizey"));

		v = vector2FromString(xml->getString("uv_tile"));
		if (xml->getError() != XML_OK){
			v.x=1;
			v.y=1;
		}
		mUVTile = v;
	}

	void GroundPlane::build(){
		Util::Log("Building Ground Plane"+mId);
		Parent::build();
		Plane iP;
		iP.normal = Vector3::UNIT_Y;
		iP.d = 1;
		
		MeshManager::getSingleton().createPlane(
		getId()+"[InfiniteGroundPlane]", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
           iP, mZoneSize.x, mZoneSize.y, 10,10, true, 1, mUVTile.x, mUVTile.y, Vector3::UNIT_Z);
		
		mEntity = Level::getSingleton()->getSceneManager()->createEntity(getId()+"[InfiniteGroundPlane]", getId()+"[InfiniteGroundPlane]");
		mEntity->setMaterialName(mMaterialName);
		mSceneNode->attachObject(mEntity);
		//mSceneNode->setPosition(0,-15,0);
		mEntity->setCastShadows(false);

		Util::Log("...built");
	}

	void GroundPlane::destroy(){
	}

	void GroundPlane::show(){
		Parent::show();

	}
	void GroundPlane::hide(){
		Parent::hide();
	}
	

	bool GroundPlane::update(){
		Parent::update();
		return false;
	}



}

#endif