// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___PARTICLEEMITTER_H___
#define ___DEF___PARTICLEEMITTER_H___

#include "..\elements\gameobject.h"

namespace Shmoulette{

	class ParticleEmitter:public GameObject{
		typedef GameObject Parent;

			string mParticleName;
			vector<float> mEmissionRate;
		protected:
			Ogre::ParticleSystem* mParticleSystem;
			
		public:
			//ParticleEmitter();
			virtual void init(XML* xml);
			virtual void init(PyObject* pyObject);
			//virtual void init(vector<string>& params);
			virtual void show();
			virtual void hide();
			virtual void build();
			virtual void destroy();
			virtual bool update(double timeDelta, double parentTime);
			virtual Ogre::Node* getNode(){
				if (mParentConnector.mBone == "")
					return Parent::getNode();
				else{
					return mParticleSystem->getParentNode();
				}
			}

			virtual void attachToBone(Ogre::Entity* ent, string bone);
			virtual COLLISION_RESULT isColliding(GameObject* obj){COLLISION_RESULT rv;rv.mOccured=false;return rv;};
	};

}

#endif
