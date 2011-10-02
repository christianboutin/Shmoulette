#include "stdafx.h"

#include "2d\coord.h"
#include "util\file.h"

namespace Shmoulette{
	Coordinate* Coordinate::mCoord = NULL;

	Coordinate::Coordinate(){
		mAspectRatio = 1.776;
	}

	Coordinate* Coordinate::getSingleton(){
		if (mCoord == NULL){
			mCoord = new Coordinate();
		}
		return mCoord;
	}

	void Coordinate::setAspectRatio(float ar){
		mAspectRatio = ar;
		//<= 1.334 (4:3) >=1.776 (16:9 or 16:10)

	}

	XYWH Coordinate::getPixelCoord(string XmlFile, string context, string graphicsFile){
		XYWH rv;

		File f(XmlFile, context, FILE_READ);
		XML xml(f.getStrContent());
		XMLIterator it(&xml);
		it.setElemName("root>coord");
		it.gotoZero();
		while (it.gotoNext() == true){
			string id = it.getElem().getString("id");
			if (id == graphicsFile){
				rv.x = it.getElem().getInt("x");
				rv.y = it.getElem().getInt("y");
				rv.w = it.getElem().getInt("w");
				rv.h = it.getElem().getInt("h");
				return rv;
			}
		}
		throw("Coordinates Not Found : "+XmlFile+" ("+context+") -> "+graphicsFile);
		
	}
	RXYWH Coordinate::getRelativeCoord(string XmlFile, string context, string GraphicsFile, REL_RULE rr){
		RXYWH rv;
		XYWH pix = getPixelCoord(XmlFile, context, GraphicsFile);
		rv.y = (float)pix.y / 768.0f;
		rv.h = (float)pix.h / 768.0f;
		if (mAspectRatio >= 1.6){
			rv.x = (float)pix.x / 1360.0f;
			rv.w = (float)pix.w / 1360.0f;
			if (rv.x > .565){
				rv.x += .25;
			}else if (rv.x > .188){
				rv.x += .12;
			}
		}else{
			rv.x = (float)pix.x / 1024.0f;
			rv.w = (float)pix.w / 1024.0f;
		}
		return rv;
	}

}