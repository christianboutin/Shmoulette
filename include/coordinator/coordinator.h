// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___COORDINATOR_H___
#define ___DEF___COORDINATOR_H___

namespace Shmoulette{
	namespace Coordinator{

		class Coordinator{
			public:
				virtual bool hasChanged(){
					return true;
				}
		};

		class Transform{
			public:
				Ogre::Vector3 mPos;
				Ogre::Quaternion mRot;
				Ogre::Vector3 mScl;
		};

		/*class TransformCoordinatorArray{
				list<TransformCoordinator*>mTransformCoordinator;
			public:
				TransformCoordinatorArray();
				void getLength();
				Tranform getValue(double time);
				bool hasChanged();

				void add(string s);
		};*/
		class TransformCoordinator:public Coordinator{
			public:
				virtual Transform getValue(double time)=0;
				virtual float getLength()=0;
		};

		class StaticTransformCoordinator:public TransformCoordinator{
			private:
				Transform mTransform;
			public:
				StaticTransformCoordinator(string value);
				virtual Transform getValue(double time){return mTransform;}
				virtual bool hasChanged(){
					return false;
				}
				virtual float getLength(){return 0;}

		};

		class SOPTransformCoordinator:public TransformCoordinator{
			protected:
				struct{
					Util::Bezier x;
					Util::Bezier y;
					Util::Bezier z;
				}mPos,mScl;
				QuaternionBezier mRot;
			public:
				SOPTransformCoordinator(string value);
				virtual Transform getValue(double time);
				virtual float getLength(){return 0;}
		};

		class LightInfo{
			public:
				struct{
					float r, g, b;
				}mDiffuse;
		};

		class LightCoordinator:public Coordinator{
			public:
				virtual LightInfo getValue(double time)=0;

		};
		class StaticLightCoordinator:public LightCoordinator{
			LightInfo mLightInfo;
			public:
				StaticLightCoordinator(string value);
				virtual bool hasChanged(){
					return false;
				}
				virtual LightInfo getValue(double time){
					return mLightInfo;
				}
		};
		class SOPLightCoordinator:public LightCoordinator{
			struct{
				Util::Bezier r;
				Util::Bezier g;
				Util::Bezier b;
			}mDiffuse;

			protected:
			public:
				SOPLightCoordinator(string value);
				virtual LightInfo getValue(double time);
		};

		class CameraCoordinator:public Coordinator{
			public:
				virtual float getValue(double time)=0;
		};
		class StaticCameraCoordinator:public CameraCoordinator{
				float mFOV;
			public:
				StaticCameraCoordinator(string value);
				virtual bool hasChanged(){
					return false;
				}
				virtual float getValue(double time){return mFOV;}
		};
		class SOPCameraCoordinator:public CameraCoordinator{
			protected:
				Util::Bezier mFOV;
			public:
				SOPCameraCoordinator(string value);
				virtual float getValue(double time);

		};
		class CoordinatorFactory{
			static class CoordinatorFactory* mSingleton;
			CoordinatorFactory();
			
			public:
				static CoordinatorFactory* getSingleton(){
					if (mSingleton == NULL){
						mSingleton = new CoordinatorFactory();
					}
					return mSingleton;
				}

				Coordinator* get(string value);
		};
	}
}


#endif