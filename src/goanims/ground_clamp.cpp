#include "stdafx.h"
#include "goanims\ground_clamp.h"
#include "OgreTerrainSceneManager.h"
#include "world\world.h"
namespace Shmoulette{
	namespace GoAnim{
		typedef enum{
				WD_CORNER_FL,
				WD_CORNER_FR,
				WD_CORNER_BL,
				WD_CORNER_BR,
				WD_CORNER_TOTAL
		}WD_CORNER;

		typedef enum{
			WD_MIDCROSS_FRONT,
			WD_MIDCROSS_BACK,
			WD_MIDCROSS_LEFT,
			WD_MIDCROSS_RIGHT,
			WD_MIDCROSS_TOTAL
		}WD_MIDCROSS;

		void GroundClamp::init(GameObject* go, GoAnimParams& params){
			Parent::init(go, params);
			throw("Not implemented yet");
#ifdef DEBUG_GROUNDCLAMP
			for (int i=0;i<4;i++){
				mPreGoAnimBallSceneNode[i]=NULL;
				mPreGoAnimBallEntity[i]=NULL;
				mPostGoAnimBallSceneNode[i]=NULL;
				mPostGoAnimBallEntity[i]=NULL;
			}
			mDoIt = false;
#endif
		}


		void GroundClamp::init(GameObject* go, PyObject* po){
			Parent::init(go, po);
			PyObject* localObj = PyList_GetItem(po, 1);

			if (localObj != Py_None){
				mAltitude = PyFloat_AsDouble(localObj);
			}
			
#ifdef DEBUG_GROUNDCLAMP
			for (int i=0;i<4;i++){
				mPreGoAnimBallSceneNode[i]=NULL;
				mPreGoAnimBallEntity[i]=NULL;
				mPostGoAnimBallSceneNode[i]=NULL;
				mPostGoAnimBallEntity[i]=NULL;
			}
			mDoIt = false;
#endif

			/*XMLIterator it(xml);
			it.setElemName("goanim>segment");
			it.gotoZero();
			while(it.gotoNext() != false){
				BLINKER_DELAY bo;

				bo.mTimeOff = it.getElem().getFloat("timeOff");
				bo.mTimeOn = it.getElem().getFloat("timeOn");
				
				mBlinkerDelay.push_back(bo);
			}
			mCurrentSegment=0;*/
		}


		void GroundClamp::init(GameObject* go, XML* xml){
			Parent::init(go, xml);
			mAltitude = xml->getFloat("altitude");
			if (xml->getError() != XML_OK){
				mAltitude  = 0;
			}
#ifdef DEBUG_GROUNDCLAMP
			for (int i=0;i<4;i++){
				mPreGoAnimBallSceneNode[i]=NULL;
				mPreGoAnimBallEntity[i]=NULL;
				mPostGoAnimBallSceneNode[i]=NULL;
				mPostGoAnimBallEntity[i]=NULL;
			}
			mDoIt = false;
#endif
			/*XMLIterator it(xml);
			it.setElemName("goanim>segment");
			it.gotoZero();
			while(it.gotoNext() != false){
				BLINKER_DELAY bo;

				bo.mTimeOff = it.getElem().getFloat("timeOff");
				bo.mTimeOn = it.getElem().getFloat("timeOn");
				
				mBlinkerDelay.push_back(bo);
			}
			mCurrentSegment=0;*/
		}

		void GroundClamp::update(float time){
#ifdef DEBUG_GROUNDCLAMP
			if (mPreGoAnimBallSceneNode[0] == NULL){
				for (int i=0;i<4;i++){
					mPreGoAnimBallSceneNode[i]=Level::getSingleton()->getCurrentSegment()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
					mPreGoAnimBallEntity[i]=Level::getSingleton()->getCurrentSegment()->getSceneManager()->createEntity(mGo->getId()+"PreCube"+ts(i),"Cube.mesh");
					mPreGoAnimBallSceneNode[i]->attachObject(mPreGoAnimBallEntity[i]);

					mPostGoAnimBallSceneNode[i]=Level::getSingleton()->getCurrentSegment()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
					mPostGoAnimBallEntity[i]=Level::getSingleton()->getCurrentSegment()->getSceneManager()->createEntity(mGo->getId()+"PostCube"+ts(i),"Cube.mesh");
					mPostGoAnimBallSceneNode[i]->attachObject(mPostGoAnimBallEntity[i]);
				}
			}
			if (mDoIt){;
#endif

			Parent::update(time);
			Ogre::SceneNode* sceneNode = mGo->getSceneNode();
			Ogre::SceneNode* parentNode = mGo->getSceneNode()->getParentSceneNode();
			
			parentNode->_update(true,false);
			
			Ogre::Vector3 v = sceneNode->_getDerivedPosition();
			//Ogre::Vector3 vdummy = sceneNode->getPosition();
			//Ogre::Quaternion tankHeading = sceneNode->_getDerivedOrientation();


			//Ogre::Radian tempAngle;
			//Ogre::Vector3 dummyVec;

			//tankHeading.ToAngleAxis(tempAngle, dummyVec);
			//Util::Log("ANGLE : "+ts(tempAngle.valueDegrees()));
			

			//v.y = static_cast<Ogre::TerrainSceneManager*>(mLevelSegment->getSceneManager())->getHeightAt(v.x, v.z);
			Ogre::Vector3 corner[4];
			Ogre::Vector3 midcross[4];

			for (int i=0;i<4;i++){
				Ogre::Entity* ent;
				ent = mGo->getEntity();
				switch(i){
					case 0:
						//corner[i] = sceneNode->_getDerivedPosition()+*/ent->getSkeleton()->getBone("fl_hook")->_getDerivedPosition();
						corner[i] = (sceneNode->_getFullTransform()*ent->getSkeleton()->getBone("fl_hook")->_getFullTransform()).getTrans();
					break;
					case 1:
						//corner[i] = sceneNode->_getDerivedPosition()+*/ent->getSkeleton()->getBone("fr_hook")->_getDerivedPosition();
						corner[i] = (sceneNode->_getFullTransform()*ent->getSkeleton()->getBone("fr_hook")->_getFullTransform()).getTrans();
					break;
					case 2:
						//corner[i] = sceneNode->_getDerivedPosition()+*/ent->getSkeleton()->getBone("bl_hook")->_getDerivedPosition();
						corner[i] = (sceneNode->_getFullTransform()*ent->getSkeleton()->getBone("bl_hook")->_getFullTransform()).getTrans();
					break;
					case 3:
						//corner[i] = sceneNode->_getDerivedPosition()+*/ent->getSkeleton()->getBone("br_hook")->_getDerivedPosition();
						corner[i] = (sceneNode->_getFullTransform()*ent->getSkeleton()->getBone("br_hook")->_getFullTransform()).getTrans();
					break;
				}
				//corner[i] = mCorner[i]->_getDerivedPosition();
				//corner[i].y = static_cast<Ogre::TerrainSceneManager*>(mLevelSegment->getSceneManager())->getHeightAt(corner[i].x, corner[i].z);
#ifdef DEBUG_GROUNDCLAMP
				mPreGoAnimBallSceneNode[i]->setPosition(corner[i]);
#endif
				corner[i].y = Level::getSingleton()->getCurrentSegment()->getWorld()->getHeightAt(corner[i].x, corner[i].z);
#ifdef DEBUG_GROUNDCLAMP
				mPostGoAnimBallSceneNode[i]->setPosition(corner[i]);
#endif
				//mDebugCorner[i]->setPosition(Vector3(corner[i].x, corner[i].y, corner[i].z));
			}
			midcross[WD_MIDCROSS_FRONT] = (corner[WD_CORNER_FL]+corner[WD_CORNER_FR])/2; //Front
			midcross[WD_MIDCROSS_BACK] = (corner[WD_CORNER_BL]+corner[WD_CORNER_BR])/2; //Back
			midcross[WD_MIDCROSS_LEFT] = (corner[WD_CORNER_FL]+corner[WD_CORNER_BL])/2; //Left
			midcross[WD_MIDCROSS_RIGHT] = (corner[WD_CORNER_FR]+corner[WD_CORNER_BR])/2; //Right

			//v.y = (corner[0].y + corner[1].y + corner[2].y + corner[3].y) /4;
			v.y = (midcross[0].y + midcross[1].y + midcross[2].y + midcross[3].y) /4;
			//v.y+=60;
			//v.y ++;
			v.y += mAltitude;


			//worldPosition.setTrans(v);

			Ogre::Vector3 v1, v2,normal;
			v1 = midcross[WD_MIDCROSS_FRONT]-midcross[WD_MIDCROSS_BACK];
			v1.normalise();
			v2 = midcross[WD_MIDCROSS_LEFT]-midcross[WD_MIDCROSS_RIGHT];
			v2.normalise();

			normal = v1.crossProduct(v2);

			//Util::Log("Normal : "+ts(normal.x)+","+ts(normal.y)+","+ts(normal.z));
			Ogre::Quaternion q;
			q.FromAxes(v2, normal, v1);

			Ogre::Matrix4 worldPosition(Ogre::Matrix4::IDENTITY);


			/*Ogre::Entity* dummyEntity = mGo->getEntity()->clone(ts(GameElement::getNextIdCounter()));
			Ogre::SceneNode* dummySceneNode = Level::getSingleton()->getSceneManager()->getRootSceneNode()->createChildSceneNode(ts(GameElement::getNextIdCounter()));
			dummySceneNode->attachObject(dummyEntity);
			dummySceneNode->setPosition(v);*/

			worldPosition.setTrans(v);
			worldPosition.setScale(Ogre::Vector3::UNIT_SCALE);

			Ogre::Matrix4 result = parentNode->_getFullTransform().inverse()*worldPosition;
			Ogre::Vector3 rt = result.getTrans();
			Ogre::Quaternion rq = parentNode->getOrientation();//result.extractQuaternion();

			sceneNode->setPosition(rt);
			sceneNode->setOrientation(q); // Should be RQ, somehow doesn't work :(


			//Util::Log("TankHeading : "+ts(tankHeading)+" > "+ts(q), 4);
			int g=0;
#ifdef DEBUG_GROUNDCLAMP
			mDoIt = false;
		}else{
			mDoIt = true;
		}
#endif

		}
	}
}