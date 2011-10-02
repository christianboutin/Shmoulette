#include "stdafx.h"
#include "coordinator/coordinator.h"
#include "coordinator/sopreader.h"

namespace Shmoulette{
	namespace Coordinator{
		CoordinatorFactory* CoordinatorFactory::mSingleton = NULL;
		CoordinatorFactory::CoordinatorFactory(){
		}
		Coordinator* CoordinatorFactory::get(string value){
			Coordinator* rv;
			vector<string> v;
			Tokenize(value, v, ":");
			if (v[0] == "sop_transform"){
				rv = new SOPTransformCoordinator(v[1]);
			}else if (v[0] == "static_transform"){
				rv = new StaticTransformCoordinator(v[1]);
			}else if (v[0] == "sop_camera"){
				rv = new SOPCameraCoordinator(v[1]);
			}else if (v[0] == "static_camera"){
				rv = new StaticCameraCoordinator(v[1]);
			}else if (v[0] == "sop_light"){
				rv = new SOPLightCoordinator(v[1]);
			}else if (v[0] == "static_light"){
				rv = new StaticLightCoordinator(v[1]);
			}else if (v[0] == "macro"){
				if (v[1] == "zero_transform"){
					rv = new StaticTransformCoordinator("0,0,0,0,1,0,0,1,1,1");
				}else{
					throw("Coordinator Macro Not Found : "+v[1]);
				}
			}else{
				throw("Coordinator Type Not Supported : "+v[0]);
			}

			return rv;
		}

		StaticTransformCoordinator::StaticTransformCoordinator(string value){
			vector<string> v;
			Tokenize(value, v, ",");
			if (v.size() >= 3){
				mTransform.mPos.x = atof(v[0].c_str());
				mTransform.mPos.y = atof(v[1].c_str());
				mTransform.mPos.z = atof(v[2].c_str());
				if (v.size() >= 7){
					mTransform.mRot = quaternionFromString(v[3]+","+v[4]+","+v[5]+","+v[6]);
					if (v.size() >= 10){
						mTransform.mScl.x = atof(v[7].c_str());
						mTransform.mScl.y = atof(v[8].c_str());
						mTransform.mScl.z = atof(v[9].c_str());
					}else{
						mTransform.mScl = Ogre::Vector3::UNIT_SCALE;
					}
				}else{
					mTransform.mRot = Ogre::Quaternion::IDENTITY;
					mTransform.mScl = Ogre::Vector3::UNIT_SCALE;
				}
			}else{
				throw("transform_c field must have at least 3 values");
			}
		}

		SOPTransformCoordinator::SOPTransformCoordinator(string value){
			SOP sop(value);
			for (int i=0;i<sop.getNbPoints();i++){
				SOPPoint p(sop.getPoint(i));
				mPos.x.setValue(p.mTime, p.mValue[0]);
				mPos.y.setValue(p.mTime, p.mValue[1]);
				mPos.z.setValue(p.mTime, p.mValue[2]);
				Ogre::Quaternion q;
				q.x = p.mValue[3];
				q.y = p.mValue[4];
				q.z = p.mValue[5];
				q.w = p.mValue[6];
				mRot.setValue(p.mTime, q);
				mScl.x.setValue(p.mTime, 1);
				mScl.y.setValue(p.mTime, 1);
				mScl.z.setValue(p.mTime, 1);
			}
			int g=0;
		}

		Transform SOPTransformCoordinator::getValue(double time){
			Transform rv;
			rv.mPos.x = mPos.x.getValue(time);
			rv.mPos.y = mPos.y.getValue(time);
			rv.mPos.z = mPos.z.getValue(time);
			rv.mRot = mRot.getValue(time);
			return rv;
		}

		StaticLightCoordinator::StaticLightCoordinator(string value){
			vector<string> v;
			Tokenize(value, v, ",");
			mLightInfo.mDiffuse.r = atof(v[0].c_str());
			mLightInfo.mDiffuse.g = atof(v[1].c_str());
			mLightInfo.mDiffuse.b = atof(v[2].c_str());
		}

		SOPLightCoordinator::SOPLightCoordinator(string value){
			SOP sop(value);
			for (int i=0;i<sop.getNbPoints();i++){
				SOPPoint p(sop.getPoint(i));
				mDiffuse.r.setValue(p.mTime, p.mValue[0]);
				mDiffuse.g.setValue(p.mTime, p.mValue[1]);
				mDiffuse.b.setValue(p.mTime, p.mValue[2]);
			}
		}

		StaticCameraCoordinator::StaticCameraCoordinator(string value){
			Ogre::Degree d(atof(value.c_str()));
			mFOV = d.valueRadians();
		}

		LightInfo SOPLightCoordinator::getValue(double time){
			LightInfo rv;
			rv.mDiffuse.r = mDiffuse.r.getValue(time);
			rv.mDiffuse.g = mDiffuse.g.getValue(time);
			rv.mDiffuse.b = mDiffuse.b.getValue(time);
			return rv;
		}

		SOPCameraCoordinator::SOPCameraCoordinator(string value){
			SOP sop(value);
			for (int i=0;i<sop.getNbPoints();i++){
				SOPPoint p(sop.getPoint(i));
				Ogre::Degree d(p.mValue[0]);
				mFOV.setValue(p.mTime, d.valueRadians());
			}
		}

		float SOPCameraCoordinator::getValue(double time){
			return mFOV.getValue(time);
		}


	}
}