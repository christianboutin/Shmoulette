// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___LIGHT_H___
#define ___DEF___LIGHT_H___

#include "gameobject.h"
namespace Shmoulette{

	class _ShmouletteExport Light:public GameObject{
		typedef GameObject Parent;

		Ogre::Light* mLight;

		bool mCastsShadows;
		Ogre::Light::LightTypes mLightType;

		Coordinator::LightCoordinator* mLightCoordinator;

		public:
		virtual void init(XML* xml);
		virtual void init(PyObject* pyObject);

		//XML* getXml();
		virtual Ogre::Node* getNode(){
			if (mParentConnector.mBone == "")
				return mSceneNode;
			else{
				if (mLight == NULL){
					throw ("getNode query on an NULL entity : "+mId);
				}

				if (mLight->getParentNode() == NULL){
					throw ("No SceneNode and entity has no node either.  : "+mId);
				}

				return mLight->getParentNode();
			}
		}
		virtual bool update(double timeDelta, double parentTime);
		virtual void updateFromCoordinator(bool force = false);
		virtual void build();
		virtual void destroy();
		virtual Ogre::MovableObject* getMovableObject(){return mLight;}
	};

}
#endif

