#include "stdafx.h"
#include "main.h"
#include "elements\elementfactory.h"
namespace Shmoulette{
	ElementFactory* ElementFactory::mSingleton = NULL;
	ElementFactory::ElementFactory(){
	}
	GameElement* ElementFactory::create(XML& xml){
		return NULL;

	}
}