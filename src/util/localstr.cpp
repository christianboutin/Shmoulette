#include <string>
#include <sstream>
using namespace std;
#include "util\localstr.h"
#include "util\file.h"
#include "ogre.h"

namespace Util{
	LocalStr* LocalStr::mLocalStr = NULL;

	LocalStr::LocalStr(){
	}

	string LocalStr::getString(string id, string entry){
		if (id.find(".") == string::npos){
			id = id + ".txt";
		}
		if (entry == ""){
			Shmoulette::File file(id, "media\\loc\\en");
			if (file.error() != Shmoulette::FILE_OK){
				throw ((string)("Localized String File Not Found : "+id+".txt"));
			}
			return file.getStrContent();
		}
		Ogre::ConfigFile* cf;
		cf = new Ogre::ConfigFile();
		cf->load("media\\loc\\en\\"+id);
		string rv = cf->getSetting(entry, Ogre::StringUtil::BLANK, entry+"\nCONFIGFILE_ERROR ");

		delete (cf);

		while (rv.find("\\n") != std::string::npos){
			rv.replace(rv.find("\\n"), 2, "\n");
		}
		return rv;
	}

}

