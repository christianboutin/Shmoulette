// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___SHMOULETTE_MAIN_H___
#define ___DEF___SHMOULETTE_MAIN_H___


//#pragma message("MAIN_H")

#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <malloc.h>
#include <vector>
#include <list>
#include <string>
//#include <d3dx9math.h>
#include "Ogre.h"
#include "OgreConfigFile.h"
//#include "OgreKeyEvent.h"
//#include "OgreEventListeners.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
//#include "ogresupp\Entity.h"
#include <sstream>
#include <conio.h>
#include <windows.h>
#include <winuser.h>
using namespace std;

#include "export.h"
#include "util\stringutil.h"

#include "util\util.h"
//#include "util\file.h"
#include "util\xml.h"

#include "types.h"

//

namespace Shmoulette{

	//void Log(string out, int level);
	/*extern Key* key[MAX_KEYS];

	typedef struct TAG_CONFIG{
		struct{
			int w, h;
		}screen;
	}Config;

	extern Config config;
	extern Ogre::Root *Core::getSingleton()->getRoot();
	extern Ogre::RenderWindow* Core::getSingleton()->getWindow();

	extern class DroneDB* Core::getSingleton()->getDroneDB();
	extern class WeaponDB* Core::getSingleton()->getWeaponDB();
	extern Mouse Core::getSingleton()->getMouse();
	extern Key Core::getSingleton()->getKey[0xED+1];
	extern class Font* gFontComic;
	extern class FontStyle* gFontStyleSmall;
	extern class FontStyle* gFontStyleComic;*/


	void PrintWindow(string st);
}

	#include "core.h"
#endif
