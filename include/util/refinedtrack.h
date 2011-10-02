// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___REFINEDTRACK_H___
#define ___DEF___REFINEDTRACK_H___

// This uses Ogre's tracks but solves the weird interpolation problems by creating a refined track in which keyframes are guaranteed to occur every X seconds
#include <string>
#include "xml.h"
#include "Ogre.h"
#include "bezier.h"

#define REFINED_TRACK_NO_VALUE FLT_MAX

namespace Shmoulette{
	typedef struct{
			Ogre::Quaternion value;
			double time;
	}QUAT_BEZ_POINT;

	class _ShmouletteExport QuaternionBezier{
		private:
			list<QUAT_BEZ_POINT>mBezierPoint;
			
			/*bool mDirty;
			void recalcAbsoluteCPs();*/
		public:
			void setValue(double time, Ogre::Quaternion q, float prevControl=.3, float nextControl=.3);
			Ogre::Quaternion getValue(double time);
			static Ogre::Quaternion getValue(QUAT_BEZ_POINT* p1, QUAT_BEZ_POINT* p2, float a);
			static Ogre::Quaternion getValue(int i1, int i2, float a);
			void removeKeyFrame(int i);
			Ogre::Quaternion getKeyFrameValue(int index);
			virtual void moveKeyFrameTime(int i, double time);
		

			QuaternionBezier();
	};

	class _ShmouletteExport BaseRefinedTrack{
		protected:
			std::string mId;
			//static Ogre::SceneManager* mSceneManager;

			double mLength;
			double mRefinedDelta; // Maximum time distance between two points before interpolation begins

		public:
			virtual void commonConstructor(std::string id, double length);
			static double truncateTime(double time);

			void setRefinedDelta(double v){mRefinedDelta = v;}
			double getRefinedDelta(){return mRefinedDelta;}
			BaseRefinedTrack(std::string id, double length);
			BaseRefinedTrack();
	};

	class _ShmouletteExport RefinedNumericTrack:public BaseRefinedTrack{
		typedef BaseRefinedTrack Parent;
		Util::Bezier mBezier;
		Ogre::NumericKeyFrame* mNKF;

		public:
			//Ogre::NumericKeyFrame* createKeyFrame(double time);
			void createKeyFrame(double time, Ogre::Real value, Ogre::Real cp = FLT_MAX);
			void setKeyFrameValue(double time, Ogre::Real value, Ogre::Real cp = FLT_MAX);
			int getNumKeyFrames();
			Ogre::Real getValue(double time);
			double getKeyFrameTime(int index);
			RefinedNumericTrack(std::string id="gluglu", double length=0);
			RefinedNumericTrack(XML* xml, string tag);
		
			virtual void commonConstructor(std::string id, double length);
	};

#if 0

	class _ShmouletteExport ColourTrack:public BaseRefinedTrack{
		typedef BaseRefinedTrack Parent;
		Util::Bezier mRed, mGreen, mBlue, mAlpha;

		public:
			void setKeyFrameValue(double time, Ogre::ColourValue c, Ogre::ColourValue cp=Ogre::ColourValue(FLT_MAX, FLT_MAX,FLT_MAX,FLT_MAX));
			Ogre::ColourValue getValue(double time);
			int getNumKeyFrames();
			Ogre::ColourValue getKeyFrameValue(int index);
			double getKeyFrameTime(int index);
			ColourTrack(XML* xml, string tag);

			//virtual void commonConstructor(std::string id, double length);

	};

	class _ShmouletteExport TransformTrack:public BaseRefinedTrack{
		typedef BaseRefinedTrack Parent;

		struct{
			Util::Bezier x;
			Util::Bezier y;
			Util::Bezier z;
		}mPos,mScl;
		QuaternionBezier mRot;
		//Ogre::TransformKeyFrame* mKF;
		//Ogre::TransformKeyFrame* mKF2;
	protected:
		virtual void commonConstructor();

		public:
		static int mCounter;
		static void ResetCounter(){mCounter = 0;}
		TransformTrack();
		TransformTrack(std::string id, double length);
		TransformTrack(Shmoulette::XML* xml);
		~TransformTrack();

		void setKeyFrameValues(double time, Ogre::Vector3 p, Ogre::Quaternion q, Ogre::Vector3 s, Ogre::Vector3 p_cp, Ogre::Quaternion r_cp, Ogre::Vector3 s_cp);
		void setKeyFrameValues(double time, Ogre::Vector3 p, Ogre::Quaternion q, Ogre::Vector3 s);
		void setKeyFrameCP(double time, Ogre::Vector3 p, Ogre::Quaternion q, Ogre::Vector3 s);

		void setPoint(double time, Ogre::Vector3 v, Ogre::Quaternion q);
		Ogre::TransformKeyFrame getInterpolatedKeyFrame(double time);


		void removeKeyFrame(int index);

		double getLength(){return mLength;}
	};


	class _ShmouletteExport KeyFramedTransformTrack:public TransformTrack{
		typedef TransformTrack Parent;
		virtual void xmlConstructor(Shmoulette::XML* xml);
	public:
		KeyFramedTransformTrack(Shmoulette::XML* xml);
		//KeyFramedTransformTrack(std::string xmlFilename, std::string context, std::string elemName);
	};

	class _ShmouletteExport SOPTransformKeyFrame{
	public:
		double mTime;
		Ogre::Vector3 mP;
		Ogre::Quaternion mQ;
	};

	class _ShmouletteExport SOPTransformResult{
	public:
		Ogre::Vector3 mP;
		Ogre::Quaternion mQ;
	};

	class _ShmouletteExport SOPTransformTrack:public TransformTrack{
			typedef TransformTrack Parent;
			string mFilename;
			string mContext;
			Util::OneDimensionTrack mX,mY,mZ;
			QuaternionBezier mRot;

			vector<SOPTransformKeyFrame>::iterator mP1;
			vector<SOPTransformKeyFrame>::iterator mP2;
			vector<SOPTransformKeyFrame>mKeyFrame;

		private:
			void adjustPs(double time);

		protected:
			double mTime;
		public:
			void readAllKeyFrames();
			SOPTransformTrack(string filename, string context);
			SOPTransformTrack(Shmoulette::XML* xml);
			virtual unsigned char* readOneKeyFrame(unsigned char* s);
			virtual unsigned char* skipOneKeyFrame(unsigned char* s);
			//Ogre::Vector3 getLoc(double time);
			//Ogre::Quaternion getRot(double time);
			SOPTransformResult getTransformResult(double time);
	};

	class _ShmouletteExport SOPCameraTransformTrack:public SOPTransformTrack{
		typedef SOPTransformTrack Parent;
		Util::OneDimensionTrack mFOV;
		public:
			SOPCameraTransformTrack(Shmoulette::XML* xml);
			virtual unsigned char* readOneKeyFrame(unsigned char* s);
			virtual unsigned char* skipOneKeyFrame(unsigned char* s);
			Ogre::Radian getFOV(double time);
	};

	class _ShmouletteExport SOPLightTransformTrack:public SOPTransformTrack{
		typedef SOPTransformTrack Parent;
		Util::OneDimensionTrack mR, mG, mB;
		
		public:
			SOPLightTransformTrack(Shmoulette::XML* xml);
			virtual unsigned char* readOneKeyFrame(unsigned char* s);
			virtual unsigned char* skipOneKeyFrame(unsigned char* s);
			Ogre::ColourValue getDiffuse(double time);

	};


	/*class _ShmouletteExport LinearTransformTrack:public TransformTrack{
		typedef BaseRefinedTrack Parent;

		virtual void commonConstructor();
		void xmlConstructor(Shmoulette::XML* xml);
		

		struct{
			Util::LinearTrack x;
			Util::LinearTrack y;
			Util::LinearTrack z;
		}mPos,mScl;
		QuaternionBezier mRot;
		Ogre::TransformKeyFrame* mKF;
		Ogre::TransformKeyFrame* mKF2;

		public:
		static int mCounter;
		static void ResetCounter(){mCounter = 0;}

		LinearTransformTrack(std::string id, double length);
		LinearTransformTrack(Shmoulette::XML* xml);
		LinearTransformTrack(std::string xmlFilename, std::string context, std::string elemName);
		~LinearTransformTrack();

		void setKeyFrameValues(double time, Ogre::Vector3 p, Ogre::Quaternion q, Ogre::Vector3 s, Ogre::Vector3 p_cp, Ogre::Quaternion r_cp, Ogre::Vector3 s_cp);
		void setKeyFrameValues(double time, Ogre::Vector3 p, Ogre::Quaternion q, Ogre::Vector3 s);
		void setKeyFrameCP(double time, Ogre::Vector3 p, Ogre::Quaternion q, Ogre::Vector3 s);

		void setPoint(double time, Ogre::Vector3 v, Ogre::Quaternion q);
		
		Ogre::TransformKeyFrame* getKeyFrame(int index);
		Ogre::TransformKeyFrame* getKeyFrameCP(int index);

		Ogre::TransformKeyFrame getInterpolatedKeyFrame(double time);

		double getNextKeyFrameTime(double time);
		double getPrevKeyFrameTime(double time);
		bool isKeyFrame(double time);
		int getKeyFrameIndex(double time);

		void removeKeyFrame(int index);

		double getLength(){return mLength;}
		int getNumKeyFrames();
		virtual void moveKeyFrameTime(int i, double time);

	};*/

#endif 
}
#endif
