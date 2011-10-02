// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___MATERIALCHAIN_H___
#define ___DEF___MATERIALCHAIN_H___

#include "goanims\goanim.h"

namespace Shmoulette{
	namespace GoAnim{
		class _ShmouletteExport MaterialChain:public GoAnim{
			typedef GoAnim Parent;

			vector<Ogre::SubEntity*> mEntityToReplace;
			string mSource;
			vector<string> mMaterialChain;
			vector<string>::iterator mIterator;
			double mDelay;
			double mTimeLeft;

			public:
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				virtual void init(GameObject* go, GoAnimParams& params);

				virtual void build();
				
				void update(float time);

		};
	}

}
#endif