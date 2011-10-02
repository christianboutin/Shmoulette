#include "stdafx.h"

#include "trigger\popup.h"
#include "2d\cell.h"
#include "2d\image.h"
#include "2d\text.h"
#include "util\localstr.h"
#include <OgreTextAreaOverlayElement.h>
#include "core.h"

using namespace Ogre;

namespace Shmoulette{
	namespace EventList{

		Popup::Popup(XML* xml){
			string filename = xml->getString("filename");
			string label = xml->getString("label");
			double time = 0;//xml->getDouble("time");
			mLife = xml->getDouble("life");
			float x = xml->getFloat("x");
			float y = xml->getFloat("y");
			float w = xml->getFloat("w");
			float h = xml->getFloat("h");
			{
			TiXmlElement cell( "cell" );
			cell.SetAttribute( "x", "0" );
			cell.SetAttribute( "y", "0" );
			cell.SetAttribute( "timeon", ts(time) );
			cell.SetAttribute( "life", ts(mLife) );
			
			TiXmlElement tz1( "image" );
			tz1.SetAttribute( "name", "allblack_jpg" );
			tz1.SetAttribute( "z", "18" );
			tz1.SetAttribute( "x", ts(x) );
			tz1.SetAttribute( "y", ts(y) );

			float width = w*Core::getSingleton()->getWindow()->getWidth()/2;
			float height = h*Core::getSingleton()->getWindow()->getHeight()/2;
			
			TiXmlElement point( "point" );
			point.SetAttribute( "time", "0" );
			point.SetAttribute( "posx", "0" );
			point.SetAttribute( "posy", "0" );
			point.SetAttribute( "sclx", ts(width));
			point.SetAttribute( "scly", "1" );
			point.SetAttribute( "angle", "0" );
			point.SetAttribute( "alpha", ".66" );

			TiXmlElement point2( "point" );
			point2.SetAttribute( "time", ".25" );
			point2.SetAttribute( "posx", "0" );
			point2.SetAttribute( "posy", "0" );
			point2.SetAttribute( "sclx", ts(width) );
			point2.SetAttribute( "scly", ts(height) );
			point2.SetAttribute( "angle", "0" );
			point2.SetAttribute( "alpha", ".66" );


			cell.InsertEndChild(tz1);
			cell.InsertEndChild(point);
			cell.InsertEndChild(point2);

			XML cellXml(&cell);
			string s= cellXml.getContent();

			mBlocker = new Cell();
			mBlocker->init(&cellXml);
			}

			{
			TiXmlElement cell( "cell" );
			cell.SetAttribute( "x", "0" );
			cell.SetAttribute( "y", "0" );
			cell.SetAttribute( "timeon", ts(time+.25) );
			cell.SetAttribute( "life", ts(mLife-.25) );

			TiXmlElement tz1( "textzone" );
			tz1.SetAttribute( "z", "19" );
			tz1.SetAttribute( "type", "static" );
			tz1.SetAttribute( "filename", filename );
			tz1.SetAttribute( "label", label );
			tz1.SetAttribute( "style", "tutorial" );
			tz1.SetAttribute( "x", ts(x) );
			tz1.SetAttribute( "y", ts(y) );
			tz1.SetAttribute( "w", ts(w) );
			tz1.SetAttribute( "h", ts(h) );
			tz1.SetAttribute( "align", "center" );
			tz1.SetAttribute( "valign", "center" );

			TiXmlElement point( "point" );
			point.SetAttribute( "time", "0" );
			point.SetAttribute( "posx", "0" );
			point.SetAttribute( "posy", "0" );
			point.SetAttribute( "sclx", "1" );
			point.SetAttribute( "scly", "1" );
			point.SetAttribute( "angle", "0" );
			point.SetAttribute( "alpha", "1" );
			
			cell.InsertEndChild(tz1);
			cell.InsertEndChild(point);

			XML cellXml(&cell);
			string s= cellXml.getContent();

			mCell = new Cell();
			mCell->init(&cellXml);
			}

			
		}
		void Popup::trigger(){
			mCell->moveTimes(Level::getSingleton()->getCurrentSegment()->getTimeTrack());
			mBlocker->moveTimes(Level::getSingleton()->getCurrentSegment()->getTimeTrack());
			Level::getSingleton()->getCurrentSegment()->addGameElement(mCell);
			Level::getSingleton()->getCurrentSegment()->addGameElement(mBlocker);
		}
		void Popup::moveTimes(double delta){
			//mCell->moveTimes(delta);
			//mBlocker->moveTimes(delta);
		}

		Arrow::Arrow(XML* xml){
			double time = 0;//xml->getDouble("time");
			mLife = xml->getDouble("life");
			float x = xml->getFloat("x");
			float y = xml->getFloat("y");
			float angle = xml->getFloat("angle");
			float w = 1;
			float h = 1;
			{
			TiXmlElement cell( "cell" );
			cell.SetAttribute( "timeon", ts(time) );
			cell.SetAttribute( "life", ts(mLife) );
			
			TiXmlElement tz1( "image" );
			if (angle == 0){
				tz1.SetAttribute( "name", "arrow_u_png" );
			}else if (angle > 0 && angle < 90){
				tz1.SetAttribute( "name", "arrow_ur_png" );
			}else if (angle == 90){
				tz1.SetAttribute( "name", "arrow_r_png" );
			}else if (angle > 90 && angle < 180){
				tz1.SetAttribute( "name", "arrow_dr_png" );
			}else if (angle == 180){
				tz1.SetAttribute( "name", "arrow_d_png" );
			}else if (angle > 180 && angle < 270){
				tz1.SetAttribute( "name", "arrow_dl_png" );
			}else if (angle == 270){
				tz1.SetAttribute( "name", "arrow_l_png" );
			}else if (angle > 270 && angle < 360){
				tz1.SetAttribute( "name", "arrow_ul_png" );
			}

			tz1.SetAttribute( "z", "615" );
			tz1.SetAttribute( "x", "-.0325");
			tz1.SetAttribute( "y", "-.0416" );
			tz1.SetAttribute( "angle", ts(angle) );

			Ogre::Vector2 p1,p2;
			p1.x = x-(Math::Cos((Ogre::Radian)(Ogre::Degree)(90.0f-angle))*.06);
			p1.y = y+(Math::Sin((Ogre::Radian)(Ogre::Degree)(90.0f-angle))*.08);
			
			p2.x = x-(Math::Cos((Ogre::Radian)(Ogre::Degree)(90.0f-angle))*.03);
			p2.y = y+(Math::Sin((Ogre::Radian)(Ogre::Degree)(90.0f-angle))*.04);

			cell.InsertEndChild(tz1);

			for (double i=0;i<mLife;i+=.5){
				TiXmlElement* point = new TiXmlElement( "point" );
				point->SetAttribute( "time", ts(i) );
				point->SetAttribute( "posx",  ts(p1.x) );
				point->SetAttribute( "posy",  ts(p1.y) );
				point->SetAttribute( "sclx", "1");
				point->SetAttribute( "scly", "1" );
				point->SetAttribute( "angle", ts(angle) );
				point->SetAttribute( "alpha", "1" );
				cell.InsertEndChild(*point);

				point = new TiXmlElement( "point" );
				point->SetAttribute( "time", ts(i+.25) );
				point->SetAttribute( "posx", ts(p2.x) );
				point->SetAttribute( "posy", ts(p2.y) );
				point->SetAttribute( "sclx", "1" );
				point->SetAttribute( "scly", "1" );
				point->SetAttribute( "angle", ts(angle) );
				point->SetAttribute( "alpha", "1" );
				cell.InsertEndChild(*point);
			}


			XML cellXml(&cell);
			string s= cellXml.getContent();

			mCell = new Cell();
			mCell->init(&cellXml);
			}

			
		}
		void Arrow::trigger(){
			mCell->moveTimes(Level::getSingleton()->getCurrentSegment()->getTimeTrack());
			Level::getSingleton()->getCurrentSegment()->addGameElement(mCell);
		}
		void Arrow::moveTimes(double delta){
			//mCell->moveTimes(delta);
		}


	}
}