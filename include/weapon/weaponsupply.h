// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___WEAPONSUPPLY_H___
#define ___DEF___WEAPONSUPPLY_H___


namespace Shmoulette{
	class WeaponSupply{
			static WeaponSupply* sDefaultWeaponSupply;
		
		public:
			static WeaponSupply* getDefault(){
				if (sDefaultWeaponSupply == NULL){
					sDefaultWeaponSupply = new WeaponSupply();
				}
				return sDefaultWeaponSupply;
			}
			static void destroyDefault(){
				if (sDefaultWeaponSupply != NULL){
					delete sDefaultWeaponSupply;
				}
			}
			virtual bool requestFire(float value=FLT_MAX); // Must request a value, even though it's only pertinent for Energy Supplies... Meh.
			virtual bool canSupply(float value=FLT_MAX); // Must request a value, even though it's only pertinent for Energy Supplies... Meh.
			virtual bool update(double timeDelta, double parentTime);
			virtual float getGageValue(){return 1;}
	};

	class AmmoWeaponSupply:public WeaponSupply{
		int mNbAmmo;
		int mMaxAmmo;
		public:
			AmmoWeaponSupply(int nbAmmo);
			void addAmmo(int nbAmmo);
			virtual float getGageValue();
			virtual bool canSupply(float value=FLT_MAX);
			virtual bool requestFire(float value=FLT_MAX);
	};

	class EnergyWeaponSupply:public WeaponSupply{
		float mCurrent;
		float mRegen;
		float mMax;

		public:
			EnergyWeaponSupply(float max, float regen);
			virtual float getSupplyValue(){return mCurrent;}
			virtual bool canSupply(float value=FLT_MAX);
			virtual bool requestFire(float value=FLT_MAX);
			virtual bool update(double timeDelta, double parentTime);
			virtual float getGageValue();
	};

	class SuperWeaponSupply:public WeaponSupply{

	};
}
#endif
