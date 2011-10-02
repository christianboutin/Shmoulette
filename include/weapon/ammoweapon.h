// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___AMMOWEAPON_H___
#define ___DEF___AMMOWEAPON_H___
namespace Shmoulette{

	typedef struct TAG_WEAPON_AMMO{
		AmmoFactory* mAmmoFactory;
		Ogre::Radian mXDeflection;
		Ogre::Radian mYDeflection;
		double mDelayFire;
	}WEAPON_AMMO;
	
	class _ShmouletteExport AmmoWeaponDBE:public WeaponDBE{
		typedef WeaponDBE Parent;

		list<WEAPON_AMMO> mAmmo; // List of ammos associated with the weapon

		public:
			~AmmoWeaponDBE();
			AmmoWeaponDBE(string id, XML& xml);
			list<WEAPON_AMMO>& getAmmo(){return mAmmo;}
			virtual class Weapon* spawn(WeaponSupply* ws);
	};
	
	class _ShmouletteExport AmmoWeapon:public Weapon{
		typedef Weapon Parent;
		AmmoWeaponDBE* mAmmoWeaponDBE;
		protected:
			list<WEAPON_AMMO> mVolley; // List of ammos in the currently firing volley.  copies mAmmo then destroys itself.
			float mVolleyTime;
			virtual void _actualFiring();  // the method "fire" only puts the firing flag to on.  _actualFiring does the job of firing, 1 bullet or activate beam or whatever.
		public:
			virtual bool update(double timeDelta, double parentTime);
			AmmoWeapon(AmmoWeaponDBE* dbe, WeaponSupply* ws);
			//AmmoWeapon(XML* xml, WeaponSupply* ws);
			virtual void setMount(GameObject* obj, string bone);
			virtual void fire();
			virtual void ceaseFire();
	};

}
#endif
