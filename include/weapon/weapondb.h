// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___WEAPONDB_H___
#define ___DEF___WEAPONDB_H___


namespace Shmoulette{
	class WeaponDB{
		static WeaponDB* sWeaponDB;
		
		list<WeaponDBE*>mEntry;
		WeaponDB();

	public:
		WeaponDBE* get(string id);
		static WeaponDB& getSingleton();

	};
}
#endif
