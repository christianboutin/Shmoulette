// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___AMMOFACTORYFACTORY_H___
#define ___DEF___AMMOFACTORYFACTORY_H___

#include "ammo.h"

namespace Shmoulette{

	class AmmoFactoryFactory{
		vector<AmmoFactory*>mEntry;
		static class AmmoFactoryFactory* mSingleton;
	public:
		static AmmoFactoryFactory* getSingleton(){
			if (mSingleton == NULL){
				mSingleton = new AmmoFactoryFactory();
			}
			return mSingleton;
		}
			

		AmmoFactory* getFactory(string type);
	};
}

#endif
