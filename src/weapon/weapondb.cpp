#include "stdafx.h"

#include "main.h"
#include "weapon\weapondb.h"
#include "weapon\ammoweapon.h"
#include "weapon\dllweapon.h"


namespace Shmoulette{

	WeaponDB* WeaponDB::sWeaponDB = NULL;

	WeaponDB& WeaponDB::getSingleton(){
		if (!sWeaponDB){
			sWeaponDB = new WeaponDB();
		}
		return *sWeaponDB;
	}

	WeaponDB::WeaponDB(){}

	WeaponDBE* WeaponDB::get(string id){
		for (list<WeaponDBE*>::iterator it = mEntry.begin();it!=mEntry.end();it++){
			if ((*it)->getId() == id){
				return (*it);
			};
		}
		File f(id+".xml", "media\\weapons\\weapons");
		XML* xml;
		if (f.error() != FILE_OK){
			throw("Error accessing file : "+id+" in context weapons\\weapons");
			xml = NULL;
		}else{
			xml = new XML(f.getStrContent());
		}
		string type = xml->getString("weapon","type");
		if (xml->getError() != XML_OK){
			throw("Error : Weapon requires type");
		}
		
		WeaponDBE* rv;
		if (type == "dll"){
			rv = new DLLWeaponDBE(id, *xml);
		}else if (type == "internal"){
			string subtype = xml->getString("weapon","subtype");
			if (xml->getError() != XML_OK){
				throw("Error : Weapon requires subtype");
			}
			if (subtype == "ammo"){
				rv = new AmmoWeaponDBE(id, *xml);
			}else if (subtype == "beam"){
				rv = new BeamWeaponDBE(id, *xml);
			}else{
				throw("Subtype : "+subtype+" not recognized");
			}
		}else{
			throw("Type : "+type+" not recognized");
		}
		//rv->setMount(launcher, mMount);
		delete xml;
		mEntry.push_back(rv);
		return rv;

	}
}