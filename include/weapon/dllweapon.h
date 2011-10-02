#ifndef ___DEF___DLLWEAPON_H___
#define ___DEF___DLLWEAPON_H___

namespace Shmoulette{

	class _ShmouletteExport DLLWeaponDBE:public WeaponDBE{
		typedef WeaponDBE Parent;

		string mDll;

		public:
			~DLLWeaponDBE();
			DLLWeaponDBE(string id, XML& xml);
			virtual class Weapon* spawn(WeaponSupply* ws);
	};
}

#endif