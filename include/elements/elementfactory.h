// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___ELEMENTFACTORY_H___
#define ___DEF___ELEMENTFACTORY_H___

//#pragma message("GAMEELEMENT_H")
namespace Shmoulette{
	class _ShmouletteExport ElementFactory{
		static class ElementFactory* mSingleton;
		ElementFactory();
		
		public:
			static ElementFactory* getSingleton(){
				if (mSingleton == NULL){
					mSingleton = new ElementFactory();
				}
				return mSingleton;
			}

			GameElement* create(XML& xml);
	};
}

#endif
