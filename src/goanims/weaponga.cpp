#include "stdafx.h"
#include "goanims\weaponga.h"

namespace Shmoulette{
	namespace GoAnim{
		void WeaponFire::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not supported yet");
		}

		void WeaponFire::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);
			if (PyList_Check(localObj)){
				int listSize = PyList_Size(localObj);
				for (int i =0;i<listSize;i++){
					mWeapon.push_back(PyInt_AsLong(PyList_GetItem(localObj,i)));
				}
			}else{
				if (!PyInt_Check(localObj)){
					throw("Weapon Fire requires a single int or a list of ints");
				}
				mWeapon.push_back(PyInt_AsLong(localObj));
			}
		}
		
		void WeaponFire::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string ws = xml->getString("mount");
			vector<string> wv;
			Tokenize(ws, wv, ",");
			for (vector<string>::iterator it = wv.begin();it!=wv.end();it++){
				mWeapon.push_back(atoi((*it).c_str()));
			}
		}

		void WeaponFire::show(){
			for (list<int>::iterator it = mWeapon.begin();it!=mWeapon.end();it++){
				mGo->fire(*it);
			}
		}
		
		void WeaponFire::hide(){
			for (list<int>::iterator it = mWeapon.begin();it!=mWeapon.end();it++){
				mGo->ceaseFire(*it);
			}
		}
	}
}

