#include "stdafx.h"

#include "weapon\ammofactoryfactory.h"
#include "weapon\bullet.h"
#include "weapon\beam.h"
#include "util\file.h"

namespace Shmoulette{

AmmoFactoryFactory* AmmoFactoryFactory::mSingleton = NULL;

AmmoFactory* AmmoFactoryFactory::getFactory(string s){
	for (unsigned int i=0;i<mEntry.size();i++){
		if (stricmp (mEntry[i]->getId().c_str(), s.c_str()) == 0){
			return mEntry[i];
		}
	}
		
	/*File file(s+".xml", "media\\weapons\\ammo");
	if (file.error() != FILE_OK){
		throw("Ammo type not found : "+s);
	}
	XML* xml;
	xml = new XML();
	xml->setContent((char*)file.content());*/
	XML* xml = new XML(s+".xml", "media\\weapons\\ammo");

	XML* xml2 = new XML();
	*xml2 = xml->extractElem("ammo");

	string type = xml2->getString("type");
	AmmoFactory* rv;
	if (type == "bullet"){
		rv = new BulletFactory(s, xml2);
//	}else if (type == "rocket"){
//		rv = new RocketFactory(s, xml2);
//	}else if (type == "beam"){
		//rv = new BeamFactory(s, xml2);
	}else if (type == "spray"){
		throw("Unhandled bullet type : "+type);
	}else{
		throw("Unexpected bullet type : "+type);
	}
	//rv->init();
	mEntry.push_back(rv);
	return rv;

	
	
	return NULL;
}

}