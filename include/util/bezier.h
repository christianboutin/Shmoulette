// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___BEZIER_H___
#define ___DEF___BEZIER_H___

	#include "xml.h"
	#include <string>
	#define QUADRATIC_BEZIER
	namespace Util{
		class TrackPoint{
			public:
				double time;
				float value;
		};

		class BezierPoint:public TrackPoint{
			public:
				float cp;
		};

		class _ShmouletteExport OneDimensionTrack{
			list<TrackPoint>mPoint;
			public:
				void setValue(double time, float value);
				void appendValue(double time, float value);
				float getValue(TrackPoint* p1, TrackPoint* p2, float a);
				float getValue(double time);


		};

#if 0
		typedef struct TAG_BEZIER_POINT{
			double time;
			float value;
			bool control_defined;
#ifdef QUADRATIC_BEZIER
			float cp;
#else
			struct{
				float prev;
				float next;
			}relativeCP, absoluteCP;
#endif
		}BEZIER_POINT;
#endif

		class _ShmouletteExport Bezier{
			private:
				list<BezierPoint>mBezierPoint;
				bool mDirty;
#ifndef QUADRATIC_BEZIER
				void recalcAbsoluteCPs();
#else
				void autocalcCPs();
#endif
			public:
#ifdef QUADRATIC_BEZIER
				void setValue(double time, float value, float cp=FLT_MAX);
				void setCP(double time, float value);
#else
				void setValue(double time, float value, float prevControl=.3, float nextControl=.3);
#endif
				float getValue(double time);
				static float getValue(BezierPoint* p1, BezierPoint* p2, float a);
				//static float getValue(int i1, int i2, float a);
				void removeKeyFrame(int i);
				int	getKeyFrameIndex(double time);
				bool isKeyFrame(double time);
				int getNbKeyFrames(){return mBezierPoint.size();}
				Bezier();
		};

	/*	typedef struct LINEAR_POINT{
			double time;
			float value;
		}LINEAR_POINT;

		class _ShmouletteExport LinearTrack{
			list<LINEAR_POINT>mPoint;
			public:
				static float getValue(LINEAR_POINT* p1, LINEAR_POINT* p2, float a);
				void setValue(double time, float value);
				float getValue(double time);
				double getPrevKeyFrameTime(double time);
				double getNextKeyFrameTime(double time);
				double getKeyFrameTime(int i);
				void setKeyFrameTime(int i, double time);
				float getKeyFrameValue(int i);

				void moveKeyFrameTime(int i, double timeDelta);

				int getNbKeyFrames(){return mPoint.size();}
				LinearTrack();
		};*/

		/*class TransformBezier{
				list<BEZIER_POINT>mX;
				list<BEZIER_POINT>mY;
				list<BEZIER_POINT>mZ;
			public:

		}*/
	}

#endif
