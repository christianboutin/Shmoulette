#include "stdafx.h"
#include "goanims\goanim.h"
#include "goanims\blinker.h"
#include "goanims\randomscaler.h"
#include "goanims\ground_clamp.h"
#include "goanims\rotation.h"
#include "goanims\translation.h"
#include "goanims\gravity.h"
#include "goanims\jump.h"
#include "goanims\materialchain.h"
#include "goanims\boneaim.h"
#include "goanims\weaponga.h"

namespace Shmoulette{
	_ShmouletteExport GoAnimFactory* GoAnimFactory::mSingleton = NULL;

	GoAnimFactory::GoAnimFactory(){}

#define MACRO_GOANIM_CREATE(TAG_GO, TAG_TYPE, TAG_PARAM)\
		if (TAG_TYPE == "blinker") rv = new GoAnim::Blinker();\
		else if (TAG_TYPE == "random_scaler") rv = new GoAnim::RandomScaler();\
		else if (TAG_TYPE == "yaw")	rv = new GoAnim::Yaw();\
		else if (TAG_TYPE == "pitch") rv = new GoAnim::Pitch();\
		else if (TAG_TYPE == "roll") rv = new GoAnim::Roll();\
		else if (TAG_TYPE == "translate") rv = new GoAnim::Translate();\
		else if (TAG_TYPE == "aim")	rv = new GoAnim::Aim();\
		else if (TAG_TYPE == "ground_clamp") rv = new GoAnim::GroundClamp();\
		else if (TAG_TYPE == "gravity")	rv = new GoAnim::Gravity();\
		else if (TAG_TYPE == "jump_out") rv = new GoAnim::JumpOut();\
		else if (TAG_TYPE == "jump_in")	rv = new GoAnim::JumpIn();\
		else if (TAG_TYPE == "bone_aim") rv = new GoAnim::BoneAim();\
		else if (TAG_TYPE == "material_chain") rv = new GoAnim::MaterialChain();\
		else if (TAG_TYPE == "weapon_fire")	rv = new GoAnim::WeaponFire();\
		else throw("GoAnim of type "+type+" unsupported for obj :"+go->getId());\
		rv->init(TAG_GO, TAG_PARAM);



//#define MACRO_CREATE_DRONE(TAG_CLASS)virtual TAG_CLASS* createDrone(){TAG_CLASS*rv = new TAG_CLASS(this);if (mPythonCallback!="")rv->setPythonCallback(mPythonCallback);return rv;}

	GoAnim::GoAnim* GoAnimFactory::createGoAnim(GameObject* go, string type, GoAnimParams& params){
		GoAnim::GoAnim* rv;
		MACRO_GOANIM_CREATE(go, type, params); // rv is assigned in here
		rv->setTime(params.mTime);
		return rv;
	}

	GoAnim::GoAnim* GoAnimFactory::createGoAnim(GameObject* go, string type, double time, PyObject* params){
		GoAnim::GoAnim* rv;
		MACRO_GOANIM_CREATE(go, type, params); // rv is assigned in here
		rv->setTime(time);
		return rv;
	}


	GoAnim::GoAnim* GoAnimFactory::createGoAnim(GameObject* go, Shmoulette::XML* xml){
		GoAnim::GoAnim* rv;
		string type = xml->getString("type");
		MACRO_GOANIM_CREATE(go, type, xml); // rv is assigned in here

		/*string type = xml->getString("type");
		if (type == "blinker"){
			rv = new GoAnim::Blinker(go, xml);
		}else if (type == "random_scaler"){
			rv = new GoAnim::RandomScaler(go, xml);
		}else if (type == "yaw"){
			rv = new GoAnim::Yaw(go, xml);
		}else if (type == "pitch"){
			rv = new GoAnim::Pitch(go, xml);
		}else if (type == "roll"){
			rv = new GoAnim::Roll(go, xml);
		}else if (type == "translate"){
			rv = new GoAnim::Translate(go, xml);
		}else if (type == "aim"){
			rv = new GoAnim::Aim(go, xml);
		}else if (type == "ground_clamp"){
			rv = new GoAnim::GroundClamp(go, xml);
		}else{
			throw("GoAnim of type "+type+" unsupported through XML");
		}*/
		rv->setTime(xml->getFloat("startTime"));
		return rv;
	}
	namespace GoAnim{
		void GoAnim::init(GameObject* go, GoAnimParams& params){
			Parent::init(params);
			mLevelSegment = go->getLevelSegment();
			mGo = go;
			mTime = params.mTime;
		}

		void GoAnim::init(GameObject* go, XML* xml){
			Parent::init(xml);
			mLevelSegment = go->getLevelSegment();
			mGo = go;
			mTime=0;
		}

		void GoAnim::init(GameObject* go, PyObject* pyObject){
			Parent::init(pyObject);
			mLevelSegment = go->getLevelSegment();
			mGo = go;
			mTime=0;
		}

		bool GoAnim::update(double timeDelta, double parentTime){
			update(timeDelta);//Level::getSingleton()->getTimeDelta());
			return false;
		}
		void GoAnim::update(float time){
			mTime+=time;
		}
	}
}