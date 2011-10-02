#include "stdafx.h"
#include "coordinator/sopreader.h"

namespace Shmoulette{

	SOP::SOP(string filename){
		MACRO_PROFILER_GET(filename, "SOP")->start();
		File* f = new File(filename, ".");
		if (f->error() != FILE_OK){
			throw("Binary Path not found : "+filename);
		}
		unsigned char* c = (unsigned char*)f->content();
		int length = f->getLength();
		unsigned char* end = c + length;
		
		char tag[4];
		tag[0]=(*c);
		tag[1]=*(c+1);
		tag[2]=*(c+2);
		tag[3]='\0';
		string stag = tag;
		if (stag != "SOP"){
			throw("New SOP format requires SOP tag.  Reexport level");
		}
		c+=3;
		mNBParams = *(int*)c;
		c+=sizeof(int);

		while(c<end){
			SOPPoint point;
			point.mTime = *(double*)c;
			c+=sizeof(double);
			if (c>end){
				throw ("SOP Corrupt");
			}
			for (int i=0;i<mNBParams;i++){
				float val = *(float*)c;
				c+=sizeof(float);
				if (c>end){
					throw ("SOP Corrupt");
				}
				point.mValue.push_back(val);
			}
			mPoint.push_back(point);
		}
		MACRO_PROFILER_GET(filename, "SOP")->end();
	}

	SOPPoint& SOP::getPoint(int index){
		return mPoint[index];
	}
};