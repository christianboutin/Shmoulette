#include "stdafx.h"
#include "goanims\translation.h"

namespace Shmoulette{
	namespace GoAnim{
		
		TranslateParams::TranslateParams(){
			mSpeed = Ogre::Vector3(0,0,0);
//			mResetPosition = dynamic_cast<TranslateParams&>(params).mResetPosition;
			mZone.setInfinite();
			//mOnBreak = dynamic_cast<TranslateParams&>(params).mOnBreak;
		}

		void Translate::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			mSpeed = dynamic_cast<TranslateParams&>(params).mSpeed;
//			mResetPosition = dynamic_cast<TranslateParams&>(params).mResetPosition;
			mZone = dynamic_cast<TranslateParams&>(params).mZone;
			//mOnBreak = dynamic_cast<TranslateParams&>(params).mOnBreak;
//			mTriggerList = dynamic_cast<TranslateParams&>(params).mTriggerList;
		}

		void Translate::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);
			float deg;
			if (PyList_Size(localObj) == 3){
				mSpeed = Ogre::Vector3(
					PyFloat_AsDouble(PyList_GetItem(localObj, 0)),
					PyFloat_AsDouble(PyList_GetItem(localObj, 1)),
					PyFloat_AsDouble(PyList_GetItem(localObj, 2))
					);
				mZone.setInfinite();
			}else{
				throw("Translate Requires 3 parameters in the list through python, change code as needed.");

			}
		}
		
		void Translate::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string s = xml->getString("speed");
			if (xml->getError() != XML_OK){
				throw("Translate Goanim requires speed");
			}
			vector<string> vs;
			Tokenize(s, vs, ",");
			if (vs[0].find(":") != string::npos){
				vector<string> as;
				Tokenize(vs[0], as, ":");
				mSpeed.x = Ogre::Math::RangeRandom(atof(as[0].c_str()), atof(as[1].c_str()));
			}else{
				mSpeed.x = atof(vs[0].c_str());
			}
			if (vs[1].find(":") != string::npos){
				vector<string> as;
				Tokenize(vs[1], as, ":");
				mSpeed.y = Ogre::Math::RangeRandom(atof(as[0].c_str()), atof(as[1].c_str()));
			}else{
				mSpeed.y = atof(vs[1].c_str());
			}
			if (vs[2].find(":") != string::npos){
				vector<string> as;
				Tokenize(vs[2], as, ":");
				mSpeed.z = Ogre::Math::RangeRandom(atof(as[0].c_str()), atof(as[1].c_str()));
			}else{
				mSpeed.z = atof(vs[2].c_str());
			}
			
			if (xml->getError() != XML_OK){ 
				throw("Translation GoAnim without Speed");
			}
			mZone.setMinimum(vector3FromString(xml->getString("zone_min")));
			if (xml->getError() != XML_OK){ 
				xml->getString("zone_max");
				if (xml->getError() == XML_OK){
					throw("Translation GoAnim Zone has maximum but not minimum");
				}else{
					mZone.setMinimum(-10,-10,-10);
					mZone.setMaximum(-10,-10,-10);
					mZone.setInfinite();
				}
			}else{
				mZone.setMaximum(vector3FromString(xml->getString("zone_max")));
				if (xml->getError() != XML_OK){
					throw("Translation GoAnim Zone has minimum but not maximum");
				}
			}
		}

		void Translate::update(float time){
			Parent::update(time);
			mGo->getNode()->translate(mSpeed*time);//Level::getSingleton()->getTimeDelta()
			if (!mZone.isInfinite()){
				Ogre::Vector3 pos = mGo->getNode()->getPosition();
				Ogre::Vector3 min = mZone.getMinimum();
				Ogre::Vector3 max = mZone.getMaximum();
				Ogre::Vector3 size = mZone.getSize();
				if (pos.x < min.x) mGo->getNode()->translate(size.x,0,0);
				if (pos.x > max.x) mGo->getNode()->translate(-size.x,0,0);
				if (pos.y < min.y) mGo->getNode()->translate(0,size.y,0);
				if (pos.y > max.y) mGo->getNode()->translate(0,-size.y,0);
				if (pos.z < min.z) mGo->getNode()->translate(0,0,size.z);
				if (pos.z > max.z) mGo->getNode()->translate(0,0,-size.z);
			}
			
		}
	}
}

/*
namespace Shmoulette{
	namespace GoAnim{

		class _ShmouletteExport Yaw:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Radian r;

			public:
				Yaw(GameObject* go, XML* xml);
				void update(float time);

		};

		class _ShmouletteExport Pitch:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Radian r;

			public:
				Pitch(GameObject* go, XML* xml);
				void update(float time);

		};

		class _ShmouletteExport Roll:public GoAnim{
			typedef GoAnim Parent;

			Ogre::Radian r;

			public:
				Roll(GameObject* go, XML* xml);
				void update(float time);

		};
	}
}
#endif*/