// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___RAIL_H___
#define ___DEF___RAIL_H___

#include "gameobject.h"
namespace Shmoulette{

class RailHook:public GameObject{
	typedef GameObject Parent;

	public:

		//RailHook();
		virtual void init(XML* xml);
	virtual bool update(double timeDelta, double parentTime);
	virtual void build();
};

}

#endif
