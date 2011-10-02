#include "stdafx.h"

#include "fx\fxobject.h"

namespace Shmoulette{
	
	FXObject::FXObject(){
	}

	void FXObject::init(string type, Ogre::SceneNode* parentNode, Ogre::Matrix4 worldPosition){
		Parent::init(type, parentNode, worldPosition);
		mType = OBJ_TYPE_NOMESH;
		mFX = new FX();
		mFX->init(type);
		mFX->setTransformCoordinator(mTransformCoordinator);
		mTransformCoordinator = NULL;
//#pragma message( "WARNING : FX should know when to disable/destroy itself" )
	}
	void FXObject::init(XML* xml){
		Parent::init(xml);
		mType = OBJ_TYPE_NOMESH;
		mFX = new FX();
		mFX->init(xml->getString("fx"));
	}
	void FXObject::init(PyObject* pyObject){
		Parent::init(pyObject);
		mType = OBJ_TYPE_NOMESH;
		if (PyList_Check(pyObject) == true && PyList_Size(pyObject) > 3){
			PyObject* o = PyList_GetItem(pyObject, 3);
			if (PyString_Check(o) == false){
				throw("FX : o should be a string.");
			}
			mFX = new FX();
			mFX->init(PyString_AsString(o));
			
		}else{
			throw("FX : PyObject should be an array of at least size 4");
		}
	}
	Ogre::Node* FXObject::getNode(){
		return mFX->getNode();
	}

	void FXObject::build(){
		mFX->setParent(mSceneNode);
		Parent::build();
		mFX->build();
	}
	void FXObject::destroy(){
		mFX->deactivate();
		mFX->hide();
		mFX->destroy();
		Parent::destroy();
	}
	void FXObject::show(){
		Parent::show();
		mFX->show();
		mFX->activate();
	}
	void FXObject::hide(){
		Parent::hide();
		mFX->deactivate();
	}
	bool FXObject::update(double timeDelta, double parentTime){
		Parent::update(timeDelta, parentTime);
		mFX->update(timeDelta, parentTime);
		if (mFX->isDone()){
			// TODO : SHould remove automanage and destroy, instead
			setTimeOff(parentTime);//Level::getSingleton()->getTimeTrack());
			setTimeDestroy(parentTime);//Level::getSingleton()->getTimeTrack());
			//destroy();
		}
		return false;
	}

}