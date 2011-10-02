// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___BILLBOARD_H___
#define ___DEF___BILLBOARD_H___

#include "elements\gameobject.h"
namespace Shmoulette{

	typedef struct TAG_BILLBOARD_ELEMENT{
		Ogre::Vector2 mSize;
		Ogre::Vector3 mPos;
		Ogre::ColourValue mColour;
	}BILLBOARD_ELEMENT;

class _ShmouletteExport Billboard:public GameObject{
	typedef GameObject Parent;
	protected:
		Ogre::BillboardSet* mBillboardSet;
		list<BILLBOARD_ELEMENT> mBillboardElement;

	public:
		void init(XML* xml);
		void init(PyObject* pyObject);
		//Billboard(vector<string>& params);
		//Billboard();
		virtual ~Billboard();
		virtual Ogre::Node* getNode(){
			if (mParentConnector.mBone == "")
				return Parent::getNode();
			else{
				return mBillboardSet->getParentNode();
			}
		}

		virtual Ogre::MovableObject* getMovableObject(){
			return mBillboardSet;
		}

		virtual void build();
		virtual void destroy();
		virtual void show();
		virtual bool update(double timeDelta, double parentTime);
		//virtual void setCollisionGroup(COLLGROUP cg, bool setAllChildren=true);

		void attachToBone(Ogre::Entity* ent, string bone);
		virtual void setVisible(bool vis);

};
}


#endif
