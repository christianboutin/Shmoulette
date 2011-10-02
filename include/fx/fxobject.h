// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___FXOBJECT_H___
#define ___DEF___FXOBJECT_H___

//#include "..\main.h"
#include "..\elements\gameobject.h"
#include "fx.h"
namespace Shmoulette{

class _ShmouletteExport FXObject:public GameObject{
	typedef GameObject Parent;
	FX* mFX;
public:
	FXObject();
	virtual void init(XML* xml);
	virtual void init(string type, Ogre::SceneNode* parentNode = NULL, Ogre::Matrix4 worldPosition = Ogre::Matrix4::IDENTITY);
	virtual void init(PyObject* pyObject);
	
	virtual Ogre::Node* getNode();
	virtual bool update(double timeDelta, double parentTime);
	virtual void build();
	virtual void destroy();
	virtual void show();
	virtual void hide();
};

}

#endif
