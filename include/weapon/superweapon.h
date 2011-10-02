#ifndef ___DEF___SUPERWEAPON_H___
#define ___DEF___SUPERWEAPON_H___

namespace Shmoulette{

namespace SuperWeapons{
	typedef enum TAG_SUPERWEAPON_STATE{
		STATE_STANDBY,
		STATE_READY,
		STATE_FIRING,
		STATE_STANDDOWN,
		STATE_TOTAL
	}SUPERWEAPON_STATE;

	class _ShmouletteExport SuperWeapon:public Weapon{
		typedef Weapon Parent;
	protected:
		SUPERWEAPON_STATE mState;
		float mEnergy;
		float mEnergyRequirements;

	public:
		SuperWeapon(WeaponDBE* dbe, WeaponSupply* ws);
		
		float getEnergy(){return mEnergy;}
		float getEnergyRequirements(){return mEnergyRequirements;}

		void addEnergy(float add);
		virtual bool setState(SUPERWEAPON_STATE state);
		void reset();
		//virtual bool update()=0;
		virtual void build(){
			Parent::build();
			resetState();
		}
		virtual void resetState(){
			SUPERWEAPON_STATE oldState = mState;
			mState = STATE_TOTAL; // Since setState will do nothing if the state is already at the right one, force-switch to an impossible state so the next function works.
			setState(oldState);
		}
		//virtual void destroy()=0;
		virtual void testEnergy();
		bool isReady(){return mState == STATE_READY;}

		virtual void fire();
		float getReadyPercent();
		void addReadyValue(float f);


		SUPERWEAPON_STATE getState(){return mState;}
	};

	class DestroyAll:public SuperWeapon{
		list<Drone*> mHitList;
		list<Drone*>::iterator mIt;
		Drone* mCurrentTarget;

		public:
			void build(){};
			void destroy(){};
			void fire();
			bool update(double timeDelta, double parentTime);
	};
}


}

#endif