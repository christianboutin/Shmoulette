#include "stdafx.h"
#include "goanims\rotation.h"

namespace Shmoulette{
	namespace GoAnim{
		void Yaw::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			Ogre::Degree d(dynamic_cast<RotateParams&>(params).mSpeed);
			r = d;
		}


		void Yaw::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);
			if (!PyList_Check(localObj)) throw("GoAnim value must be a list, even if it's just 1 value");

			float deg;
			if (PyList_Size(localObj) == 1){
				deg = PyFloat_AsDouble(PyList_GetItem(localObj, 0));
			}else{
				deg = Ogre::Math::RangeRandom(PyFloat_AsDouble(PyList_GetItem(localObj, 0)), PyFloat_AsDouble(PyList_GetItem(localObj, 1)));
			}
			Ogre::Degree d = (Ogre::Degree)deg;
			r = d;
		}

		void Yaw::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string s = xml->getString("value");
			if (xml->getError() != XML_OK){
				throw("Yaw goanim without 'value'");
			}
			float deg;
			if (s.find(",") != string::npos){
				Ogre::Vector2 v;
				v = vector2FromString(s);
				deg = Ogre::Math::RangeRandom(v.x, v.y);
			}else{
				deg = atof(s.c_str());
			}
			Ogre::Degree d = (Ogre::Degree)deg;
			r = d;
		}

		void Yaw::update(float time){
			Parent::update(time);
			mGo->getNode()->yaw(r*time);//Level::getSingleton()->getTimeDelta());
		}
		
		void Pitch::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			Ogre::Degree d(dynamic_cast<RotateParams&>(params).mSpeed);
			r = d;
		}

		void Pitch::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);
			float deg;
			if (!PyList_Check(localObj)) throw("GoAnim value must be a list, even if it's just 1 value");
			if (PyList_Size(localObj) == 1){
				deg = PyFloat_AsDouble(PyList_GetItem(localObj, 0));
			}else{
				deg = Ogre::Math::RangeRandom(PyFloat_AsDouble(PyList_GetItem(localObj, 0)), PyFloat_AsDouble(PyList_GetItem(localObj, 1)));
			}
			Ogre::Degree d = (Ogre::Degree)deg;
			r = d;
		}


		void Pitch::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string s = xml->getString("value");
			if (xml->getError() != XML_OK){
				throw("Pitch goanim without 'value'");
			}
			float deg;
			if (s.find(",") != string::npos){
				Ogre::Vector2 v;
				v = vector2FromString(s);
				deg = Ogre::Math::RangeRandom(v.x, v.y);
			}else{
				deg = atof(s.c_str());
			}
			Ogre::Degree d = (Ogre::Degree)deg;
			r = d;
		}

		void Pitch::update(float time){
			Parent::update(time);
			mGo->getNode()->pitch(r*time);//Level::getSingleton()->getTimeDelta());
		}

		void Roll::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			Ogre::Degree d(dynamic_cast<RotateParams&>(params).mSpeed);
			r = d;
		}

		void Roll::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);
			float deg;
			if (!PyList_Check(localObj)) throw("GoAnim value must be a list, even if it's just 1 value");
			if (PyList_Size(localObj) == 1){
				deg = PyFloat_AsDouble(PyList_GetItem(localObj, 0));
			}else{
				deg = Ogre::Math::RangeRandom(PyFloat_AsDouble(PyList_GetItem(localObj, 0)), PyFloat_AsDouble(PyList_GetItem(localObj, 1)));
			}
			Ogre::Degree d = (Ogre::Degree)deg;
			r = d;
		}

		
		void Roll::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			string s = xml->getString("value");
			if (xml->getError() != XML_OK){
				throw("Roll goanim without 'value'");
			}
			float deg;
			if (s.find(",") != string::npos){
				Ogre::Vector2 v;
				v = vector2FromString(s);
				deg = Ogre::Math::RangeRandom(v.x, v.y);
			}else{
				deg = atof(s.c_str());
			}
			Ogre::Degree d = (Ogre::Degree)deg;
			r = d;
		}

		void Roll::update(float time){
			Parent::update(time);
			mGo->getNode()->roll(r*time);//Level::getSingleton()->getTimeDelta());
		}

		void Aim::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
		}

		void Aim::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			throw("Unsupported through python (Aim)");
		}

		void Aim::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			/*string s = xml->getString("value");
			if (xml->getError() != XML_OK){
				throw("Roll goanim without 'value'");
			}
			float deg;
			if (s.find(",") != string::npos){
				Ogre::Vector2 v;
				v = vector2FromString(s);
				deg = Ogre::Math::RangeRandom(v.x, v.y);
			}else{
				deg = atof(s.c_str());
			}
			Ogre::Degree d = (Ogre::Degree)deg;
			r = d;*/
		}

		void Aim::update(float time){
			Parent::update(time);
			dynamic_cast<Ogre::SceneNode*>(mGo->getNode())->
				lookAt(Level::getSingleton()->getPlayerShip()->getNode()->_getDerivedPosition(),Ogre::Node::TransformSpace::TS_WORLD,Ogre::Vector3::UNIT_Z);
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