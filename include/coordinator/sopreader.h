// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___SOPREADER_H___
#define ___DEF___SOPREADER_H___

namespace Shmoulette{

	class SOPPoint{
		public:
			double mTime;
			vector<float>mValue;
	};

	class SOP{
			int mNBParams;
			vector<SOPPoint>mPoint;
			vector<SOPPoint>::iterator mCurrentPoint;
		public:
			SOP(string filename);
			SOPPoint& getPoint(int index);
			int getNbPoints(){return mPoint.size();}
			//SOPPoint& getPointFromTime(double time);
			//SOPPoint& getPoint(){return (*mCurrentPoint);}
	};

}
#endif