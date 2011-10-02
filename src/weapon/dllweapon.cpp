#include "stdafx.h"
#include "weapon\dllweapon.h"
#include "weapon\superweapon.h"


namespace Shmoulette{
	DLLWeaponDBE::DLLWeaponDBE(string id, XML& xml):Parent(id,xml){
		mDll = xml.getString("weapon", "subtype");
		if (xml.getError() != XML_OK){
			throw("Dll weapon type requires subtype");
		}
	}
	DLLWeaponDBE::~DLLWeaponDBE(){}
	Weapon* DLLWeaponDBE::spawn(WeaponSupply* ws){
		typedef Shmoulette::Weapon* (__cdecl* DLLCallback)(WeaponDBE* wdbe, WeaponSupply* ws);
		#ifdef _DEBUG
			string name = mDll+"d";
			HMODULE weaponModule = LoadLibrary(name.c_str());
		#else
			HMODULE weaponModule = LoadLibrary(mDll.c_str());
		#endif
		DLLCallback dllFunc = (DLLCallback)::GetProcAddress(weaponModule, "getInstance");
		if (dllFunc == NULL){
			throw("Weapons DLL must have getInstance function");
		}
		//string s=xml->getContent();
		Weapon* rv = dllFunc(this, ws);
		return rv;
	}
}