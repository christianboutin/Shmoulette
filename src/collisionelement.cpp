#include "stdafx.h"
#include "ogresupp\misc.h"
#include "OgreWireBoundingBox.h"

//#define DEBUG_BOX

namespace Shmoulette{
	using namespace Ogre;
//static int bullshitincrementer = 0;
	CollisionElement::CollisionElement(GameObject* gameObject,XML* xml, COLLISION_ELEMENT_TYPES dtype){
		mGameObject = gameObject;
		mSceneNode = NULL;
		string inv = xml->getString("invulnerable");
		if (inv == "1"){
			mInvulnerable = true;
		}else{
			mInvulnerable = false;
		}

		mBoneName = xml->getString("bone");
		if (xml->getError() != XML_OK){
			mBoneName ="";
		}
		mId = xml->getString("label");
		mBone = NULL;
		mType = dtype;
		mIsBuild = false;
	}

	CollisionElement::CollisionElement(GameObject* gameObject, string boneName, COLLISION_ELEMENT_TYPES dtype){
		mGameObject = gameObject;
		mSceneNode = NULL;
		mBoneName = boneName;
		mBone = NULL;
		mType = dtype;
		mIsBuild = false;
		mInvulnerable = false;
	}

	CollisionElement::CollisionElement(GameObject* gameObject, COLLISION_ELEMENT_TYPES dtype){
		mGameObject = gameObject;
		mSceneNode = NULL;
		mBoneName = "";
		mBone = NULL;
		mType = dtype;
		mIsBuild = false;
		mInvulnerable = false;
	}

	CollisionElement::CollisionElement(Ogre::SceneNode* sn, COLLISION_ELEMENT_TYPES dtype){
		mGameObject = NULL;
		mSceneNode = sn;
		mBoneName = "";
		mBone = NULL;
		mType = dtype;
		mIsBuild = false;
		mInvulnerable = false;
	}

	CollisionElement::~CollisionElement(){
	}

	void CollisionElement::setInvulnerable(bool inv){
		mInvulnerable = inv;
#ifdef DEBUG_BOX
		if (mInvulnerable){
			mWbb->setMaterial("Pure/Red");
			//mWDbb->setMaterial("Pure/Red");
		}
#endif
	}

	INTERSECTS_RESULT CollisionElement::intersects(CollisionElement* ce){
		INTERSECTS_RESULT rv;
		rv.mIntersects = false;
		rv.mDistance = FLT_MAX;
		rv.mInvulnerable = INVULNERABLE_BOTH;

		if (ce == NULL){
			rv.mInvulnerable = INVULNERABLE_NONE;
			return rv;
		}
		if (
			this->mType == COLLISION_ELEMENT_BOX &&
			ce->mType == COLLISION_ELEMENT_BOX){
				Ogre::AxisAlignedBox* b1 = dynamic_cast<BoxCollisionElement*>(this)->getBox();
				Ogre::AxisAlignedBox* b2 = dynamic_cast<BoxCollisionElement*>(ce)->getBox();
				rv.mIntersects = b1->intersects(*b2);
			//if (rv == true){
				//int gluglu = 23;
			//}
		}else if (
			this->mType == COLLISION_ELEMENT_SPHERE &&
			ce->mType == COLLISION_ELEMENT_BOX){
				Ogre::Sphere* b1 = dynamic_cast<SphereCollisionElement*>(this)->getSphere();
				Ogre::AxisAlignedBox* b2 = dynamic_cast<BoxCollisionElement*>(ce)->getBox();
				rv.mIntersects = b1->intersects(*b2);
				//if (rv)
					//int gluglu = 12;
		}else if (
			this->mType == COLLISION_ELEMENT_BOX &&
			ce->mType == COLLISION_ELEMENT_SPHERE){
				Ogre::AxisAlignedBox* b1 = dynamic_cast<BoxCollisionElement*>(this)->getBox();
				Ogre::Sphere* b2 = dynamic_cast<SphereCollisionElement*>(ce)->getSphere();
				rv.mIntersects = b1->intersects(*b2);
				//if (rv)
				//	int gluglu = 12;
		}else if (
			this->mType == COLLISION_ELEMENT_SPHERE &&
			ce->mType == COLLISION_ELEMENT_SPHERE){
				Ogre::Sphere* b1 = dynamic_cast<SphereCollisionElement*>(this)->getSphere();
				Ogre::Sphere* b2 = dynamic_cast<SphereCollisionElement*>(ce)->getSphere();
				rv.mIntersects = b1->intersects(*b2);
				//if (rv)
				//	int gluglu = 12;
		}else if (
			this->mType == COLLISION_ELEMENT_RAY &&
			ce->mType == COLLISION_ELEMENT_BOX){
				Ogre::Ray* b1 = dynamic_cast<RayCollisionElement*>(this)->getRay();
				Ogre::AxisAlignedBox* b2 = dynamic_cast<BoxCollisionElement*>(ce)->getBox();
				std::pair<bool, Real> rayrv = b1->intersects(*b2);
				rv.mIntersects = rayrv.first;
				rv.mDistance = rayrv.second;
				//if (rv)
					//int gluglu = 12;
		}else if (
			this->mType == COLLISION_ELEMENT_RAY &&
			ce->mType == COLLISION_ELEMENT_SPHERE){
				Ogre::Ray* b1 = dynamic_cast<RayCollisionElement*>(this)->getRay();
				Ogre::Sphere* b2 = dynamic_cast<SphereCollisionElement*>(ce)->getSphere();
				std::pair<bool, Real> rayrv = b1->intersects(*b2);
				rv.mIntersects = rayrv.first;
				rv.mDistance = rayrv.second;
				//if (rv)
					//int gluglu = 12;
		}else if (
			this->mType == COLLISION_ELEMENT_BOX &&
			ce->mType == COLLISION_ELEMENT_RAY ){
				Ogre::Ray* b1 = dynamic_cast<RayCollisionElement*>(ce)->getRay();
				Ogre::AxisAlignedBox* b2 = dynamic_cast<BoxCollisionElement*>(this)->getBox();
				std::pair<bool, Real> rayrv = b1->intersects(*b2);
				rv.mIntersects = rayrv.first;
				rv.mDistance = rayrv.second;
				//if (rv)
					//int gluglu = 12;
		}else if (
			this->mType == COLLISION_ELEMENT_SPHERE &&
			ce->mType == COLLISION_ELEMENT_RAY){
				Ogre::Ray* b1 = dynamic_cast<RayCollisionElement*>(ce)->getRay();
				Ogre::Sphere* b2 = dynamic_cast<SphereCollisionElement*>(this)->getSphere();
				std::pair<bool, Real> rayrv = b1->intersects(*b2);

				rv.mIntersects = rayrv.first;
				rv.mDistance = rayrv.second;
				
				//if (rv)
					//int gluglu = 12;
		}else{
			throw("Can't handle these kinds of collisions");
		}
		if (rv.mIntersects){
			if (ce->mInvulnerable && this->mInvulnerable){
				throw("Both invulnerable?");
			}
			if (!ce->mInvulnerable){
				rv.mInvulnerable &= ~INVULNERABLE_DESTINATION;
			}else{
				int g=0;
			}
			if (!this->mInvulnerable){
				rv.mInvulnerable &= ~INVULNERABLE_SOURCE;
			}else{
				int g=0;
			}

		}
		return rv;
	}

	RayCollisionElement::RayCollisionElement(GameObject* gameObject, XML* xml):Parent(gameObject, xml, COLLISION_ELEMENT_RAY){
		mRay = new Ogre::Ray();
		mTRay = new Ogre::Ray();

		string s;
		s = xml->getString("origin");
		mRay->setOrigin(vector3FromString(s));
		s = xml->getString("direction");
		mRay->setDirection(vector3FromString(s));


#ifdef DEBUG_BOX
		mWbb = new Ogre::WireBoundingBox();
		mWDbb = new Ogre::WireBoundingBox();
#endif
	}
	RayCollisionElement::RayCollisionElement(GameObject* gameObject, string boneName):Parent(gameObject, boneName, COLLISION_ELEMENT_RAY){
		mRay = new Ogre::Ray();
		mRay->setOrigin(Vector3(0,0,0));
		mRay->setDirection(Vector3(0,0,1));
		mTRay = new Ogre::Ray();
#ifdef DEBUG_BOX
		mWbb = new Ogre::WireBoundingBox();
		mWDbb = new Ogre::WireBoundingBox();
		if (mInvulnerable){
			mWbb->setMaterial("Pure/Red");
			mWDbb->setMaterial("Pure/Red");
		}
#endif
	}
	RayCollisionElement::~RayCollisionElement(){
#ifdef DEBUG_BOX
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->detachObject(mWbb);
		delete mWbb;
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->detachObject(mWDbb);
		delete mWDbb;
#endif
	}
	void RayCollisionElement::update(){
		if (mRay != NULL){
			//*mTRay = *mRay;
			Ogre::Node* n = mGameObject->getNode();
			Ogre::Vector3 s = n->getScale();
			n->setScale(1,1,1);
			//n->_update(false, false);
			Ogre::Matrix4 snt = n->_getFullTransform();
				//mGameObject->getEntity()->_getParentNodeFullTransform();
			n->scale(s);

			Ogre::Matrix4 rayMatrix = Ogre::Matrix4::IDENTITY;
			rayMatrix.setTrans(mRay->getOrigin());
			
			Ogre::Matrix4 dirMatrix = Ogre::Matrix4::IDENTITY;
			dirMatrix.setTrans(mRay->getOrigin()+mRay->getDirection());

			if (mBone != NULL){
				throw("BONE!");
				Ogre::Matrix4 bnt;
				mBone->_update(true, true); //OPTIMIZATION : THIS COSTS
				mBone->_getOffsetTransform(bnt);
				bnt.setScale(Vector3::UNIT_SCALE);

				mTRay->setOrigin((bnt*rayMatrix).getTrans());
				mTRay->setDirection((bnt*dirMatrix).getTrans());
			}else{
				Ogre::Vector3 ori = (snt*rayMatrix).getTrans();
				mTRay->setOrigin(ori);
				Ogre::Vector3 dir = (snt*dirMatrix).getTrans()-ori;
				//dir.normalise();
				
				mTRay->setDirection(dir);
			}
			#ifdef DEBUG_BOX
				Ogre::AxisAlignedBox aabb;
				aabb.setExtents(mTRay->getOrigin()-.5,
								mTRay->getOrigin()+.5);
				mWbb->setupBoundingBox(aabb);

				aabb.setExtents(mTRay->getOrigin()+mTRay->getDirection()-.25,
								mTRay->getOrigin()+mTRay->getDirection()+.25);
				mWDbb->setupBoundingBox(aabb);

				Util::Log("BB : "+ts(mTRay->getOrigin())+"->"+ts(mTRay->getDirection()),2);
			#endif			
		}

		Parent::update();
	}
	void RayCollisionElement::build(){
		Parent::build();
#ifdef DEBUG_BOX
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->attachObject(mWbb);
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->attachObject(mWDbb);
#endif
	}
	SphereCollisionElement::SphereCollisionElement(SceneNode* sceneNode):Parent(sceneNode, COLLISION_ELEMENT_SPHERE){
		mSphere = new Ogre::Sphere();
		mTSphere = new Ogre::Sphere();
#ifdef DEBUG_BOX
		mWbb = new Ogre::WireBoundingBox();
		if (mInvulnerable){
			mWbb->setMaterial("Pure/Red");
		}
#endif
		mSphere->setCenter(Ogre::Vector3(0,0,0));
		mSphere->setRadius(1);
	}

	SphereCollisionElement::SphereCollisionElement(GameObject* gameObject, XML* xml):Parent(gameObject, xml,COLLISION_ELEMENT_SPHERE){
		mSphere = new Ogre::Sphere();
		mTSphere = new Ogre::Sphere();
		
#ifdef DEBUG_BOX
		mWbb = new Ogre::WireBoundingBox();
		if (mInvulnerable){
			mWbb->setMaterial("Pure/Red");
		}
#endif
		Ogre::Vector3 v = vector3FromString(xml->getString("center"));

		mSphere->setCenter(v);
		float r = xml->getFloat("radius");
		mSphere->setRadius(r);
	}

	void SphereCollisionElement::setRadius(float rad){
		mSphere->setRadius(rad);
	}

	void SphereCollisionElement::update(){
		Parent::update();
		if (mSphere != NULL){
			*mTSphere = *mSphere;
			Ogre::Matrix4 snt;
			if (mGameObject){
				snt = mGameObject->getEntity()->_getParentNodeFullTransform();
			}else{
				if (mSceneNode){
					snt = mSceneNode->_getFullTransform();
				}else{
					throw("Collision Sphere has neither a Gameobject parent nor a scenenode parent");
				}
			}
			Ogre::Matrix4 sphereMatrix = Ogre::Matrix4::IDENTITY;
			sphereMatrix.setTrans(mSphere->getCenter());
			if (mBone != NULL){
				Ogre::Matrix4 bnt;
				mBone->_update(true, true); //OPTIMIZATION : THIS COSTS
				mBone->_getOffsetTransform(bnt);
				mTSphere->setCenter((bnt*sphereMatrix).getTrans());
			}else{
				mTSphere->setCenter((snt*sphereMatrix).getTrans());
			}
			#ifdef DEBUG_BOX
				Ogre::AxisAlignedBox aabb;
				aabb.setExtents(mTSphere->getCenter()-mTSphere->getRadius(),
								mTSphere->getCenter()+mTSphere->getRadius());
				mWbb->setupBoundingBox(aabb);
			#endif			
		}
	}

	void SphereCollisionElement::build(){
		Parent::build();
#ifdef DEBUG_BOX
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->attachObject(mWbb);
#endif
	}
	SphereCollisionElement::~SphereCollisionElement(){
#ifdef DEBUG_BOX
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->detachObject(mWbb);
		delete mWbb;
#endif
	}

	AutoBoxCollisionElement::AutoBoxCollisionElement(GameObject* gameObject, XML* xml):Parent(gameObject, xml){
		string gluglu = xml->getContent();
		mScale = vector3FromString(xml->getString("elem","scale"));
		if (xml->getError() != XML_OK){
			mScale.x = xml->getFloat("elem","scale_x");
			mScale.y = xml->getFloat("elem","scale_y");
			mScale.z = xml->getFloat("elem","scale_z");
		}

		if (mScale.x == FLT_MAX) mScale.x = 1;
		if (mScale.y == FLT_MAX) mScale.y = 1;
		if (mScale.z == FLT_MAX) mScale.z = 1;
	

	}

	void AutoBoxCollisionElement::stretchBox(Ogre::MeshPtr meshPtr, int boneHandle){
		size_t vertex_count,index_count;
		Vector3* vertices;
		unsigned long* indices;

		Shmoulette::OgreSupp::getMeshInformation(meshPtr,
			vertex_count,
			vertices,
			index_count,
			indices);

		Ogre::Mesh::SubMeshIterator smi = meshPtr->getSubMeshIterator();
		
		/*{
			Ogre::Mesh::BoneAssignmentIterator it = meshPtr->getBoneAssignmentIterator();
			while(it.hasMoreElements()){
				Ogre::VertexBoneAssignment vbal;
				vbal = it.getNext();

				if (vbal.boneIndex == boneHandle || boneHandle == -1){
					Ogre::Vector3 vpos;

					vpos = vertices[vbal.vertexIndex];

					//vpos = //??? (mEntity->getMesh()->get___SOMETHING?(vbal.vertexIndex));

					Ogre::AxisAlignedBox ab;
					ab.setNull();
					ab.setExtents(vpos, vpos);
					mBox->merge(ab);
				}
			}

		}*/
		
		
		while(smi.hasMoreElements()){
			Ogre::Mesh::BoneAssignmentIterator it = smi.getNext()->getBoneAssignmentIterator();
			while(it.hasMoreElements()){
				Ogre::VertexBoneAssignment vbal;
				vbal = it.getNext();

				if (vbal.boneIndex == boneHandle || boneHandle == -1){
					Ogre::Vector3 vpos;

					vpos = vertices[vbal.vertexIndex];

					//vpos = //??? (mEntity->getMesh()->get___SOMETHING?(vbal.vertexIndex));

					Ogre::AxisAlignedBox ab;
					ab.setNull();
					ab.setExtents(vpos, vpos);
					mBox->merge(ab);
				}
			}
		}
	}

	void CollisionElement::build(){
		if (mBoneName != ""){
			mBone = mGameObject->getEntity()->getSkeleton()->getBone(mBoneName);
		}else{
			mBone = NULL;
		}
		mIsBuild = true;
		

		/*if (mAuto == true){
			if (mBox == NULL){
				mBox = new Ogre::AxisAlignedBox();
				if (mGameObject->getEntity() != NULL){
					*mBox = mGameObject->getEntity()->getBoundingBox();
				}else{
					delete mBox;
					mBox = NULL;
				}
			}else{
				Ogre::MeshPtr mesh = mGameObject->getEntity()->getMesh();
				stretchBox(mesh, mBone->getHandle());
			}
		}*/

	}
	void AutoBoxCollisionElement::build(){
		Parent::build();
		Ogre::Entity* entity = mGameObject->getEntity();
		if (mBox->isNull()){
			if (entity != NULL){
				Ogre::MeshPtr mesh = entity->getMesh();
				if (mBone != NULL){
					stretchBox(mesh, mBone->getHandle());
				}else{
					*mBox = entity->getBoundingBox();
				}
				//mBox->scale(Vector3(.9, .9, .9));
			}else{
				mBox->setExtents(Vector3(FLT_MAX, FLT_MAX, FLT_MAX),
					Vector3(FLT_MAX,FLT_MAX,FLT_MAX));
			}
			mBox->scale(mScale);
		}
	}

	void CollisionElement::update(){
	}

	CollisionStructure::CollisionStructure(GameObject* gameObject, string boneName):Parent(gameObject, boneName){

	}

	CollisionStructure::CollisionStructure(GameObject* gameObject):Parent(gameObject, ""){
	}

	CollisionStructure::CollisionStructure(GameObject* gameObject, XML* xml):Parent(gameObject, xml){
		XMLIterator it(xml);
		it.gotoZero();
		it.setElemName("elem>elem");
		while(it.gotoNext()!=false){
			XML lxml = it.getElem();
			//string type = lxml.getString("type");
			addElement(gameObject, &lxml);
		}
		int g=0;
	}

	void CollisionStructure::addElement(GameObject* gameObject, XML* xml){
		string dtype = xml->getString("type");
		if (xml->getError() != XML_OK) throw("Collision Element Type Undefined");
		string boneName = xml->getString("bone");
		if (xml->getError() != XML_OK) boneName="";


		if (dtype == "struct"){
			addElement(new CollisionStructure(gameObject, xml));
		}else if (dtype == "sphere"){
			addElement(new SphereCollisionElement(gameObject, xml));
		}else if (dtype == "split_box"){
			addElement(new SplitBoxCollisionElement(gameObject, xml));
		}else if (dtype == "ray"){
			addElement(new RayCollisionElement(gameObject, xml));
		}else if (dtype == "auto_box"){
			addElement(new AutoBoxCollisionElement(gameObject, xml));
		}else if (dtype == "box"){
			addElement(new BoxCollisionElement(gameObject, xml));
		}else{
			addElement(gameObject, boneName, dtype);
		}/*else{
			throw("Unsupported collision element of type : "+dtype);
		}*/
	}

	void BoxCollisionElement::setBox(Ogre::AxisAlignedBox aabb){
		mBox->setExtents(aabb.getMinimum(), aabb.getMaximum());
	}


	void CollisionStructure::addElement(GameObject* gameObject, string dtype){

	}

	void CollisionStructure::addElement(CollisionElement* ce){
		mChildElem.push_back(ce);
	}

	void CollisionStructure::addElement(GameObject* gameObject, string boneName, string dtype){
		CollisionElement* ce;
		if (dtype == "auto_box"){
			ce = new AutoBoxCollisionElement(gameObject, boneName);
			ce->setInvulnerable(this->mInvulnerable);
		}else if (dtype == "box"){
			ce = new BoxCollisionElement(gameObject, boneName);
			ce->setInvulnerable(this->mInvulnerable);
		}else if (dtype == "split_box"){
			ce = new SplitBoxCollisionElement(gameObject, boneName, Vector3(2,2,2));
			ce->setInvulnerable(this->mInvulnerable);
		}
		addElement(ce);
	}

	void CollisionStructure::update(){
		Parent::update();
		vector<CollisionElement*>::iterator it;
		for (it=mChildElem.begin();it!=mChildElem.end();it++){
			(*it)->setDirty(true);
#ifdef DEBUG_BOX
			(*it)->update();
#endif
		}
		setDirty(false);
	}

	void CollisionStructure::build(){
		Parent::build();
		vector<CollisionElement*>::iterator it;
		for (it=mChildElem.begin();it!=mChildElem.end();it++){
			(*it)->build();
			if ((*it)->getType() == COLLISION_ELEMENT_BOX){
				Ogre::AxisAlignedBox childBox = *(dynamic_cast<BoxCollisionElement*>((*it))->getRootBox());
				mBox->merge(childBox);
			}
		}

	}

	INTERSECTS_RESULT CollisionStructure::intersects(CollisionElement* cElem){
		if (isDirty()) update();
		if (cElem->isDirty()) cElem->update();

		INTERSECTS_RESULT rv;
		INTERSECTS_RESULT collResult, collResultB;
		rv.mIntersects = false;
		rv.mDistance = FLT_MAX;
		rv.mInvulnerable = INVULNERABLE_NONE;

		collResult = Parent::intersects(cElem);

		if (collResult.mIntersects){
			if (!hasChildren()){
				if (!cElem->hasChildren()){
					return collResult;
				}else{
					INTERSECTS_RESULT ir = cElem->intersects(this);
					if (ir.mInvulnerable == INVULNERABLE_SOURCE)
						ir.mInvulnerable = INVULNERABLE_DESTINATION;
					else if (ir.mInvulnerable == INVULNERABLE_DESTINATION)
						ir.mInvulnerable = INVULNERABLE_SOURCE;

					return ir;
				}
			}else{
				vector<CollisionElement*>::iterator elem;
				for (elem=mChildElem.begin();elem!=mChildElem.end();elem++){
					if ((*elem)->isDirty()) (*elem)->update();
					collResult = (*elem)->intersects(cElem);
					if (collResult.mIntersects){
						vector<CollisionElement*>::iterator elem;
						for (elem=mChildElem.begin();elem!=mChildElem.end();elem++){
							collResultB = cElem->intersects(*elem);
							if (collResultB.mInvulnerable == INVULNERABLE_SOURCE)
								collResultB.mInvulnerable = INVULNERABLE_DESTINATION;
							else if (collResultB.mInvulnerable == INVULNERABLE_DESTINATION)
								collResultB.mInvulnerable = INVULNERABLE_SOURCE;

							if (collResultB.mIntersects){
								// Bunch of tests for debugging, remove at the end

								/*if (collResultB.mInvulnerable == 0)
									if (collResult.mInvulnerable != 0)
										throw("Invulnerability Inconsistency A");
								
								if (collResult.mInvulnerable == 0)
									if (collResultB.mInvulnerable != 0)
										throw("Invulnerability Inconsistency B");

								if (collResult.mInvulnerable == INVULNERABLE_SOURCE)
									if (collResultB.mInvulnerable != INVULNERABLE_DESTINATION)
										throw("Invulnerability Inconsistency C");

								if (collResult.mInvulnerable == INVULNERABLE_DESTINATION)
									if (collResultB.mInvulnerable != INVULNERABLE_SOURCE)
										throw("Invulnerability Inconsistency D");

								if (collResultB.mInvulnerable == INVULNERABLE_SOURCE)
									if (collResult.mInvulnerable != INVULNERABLE_DESTINATION)
										throw("Invulnerability Inconsistency E");

								if (collResultB.mInvulnerable == INVULNERABLE_DESTINATION)
									if (collResult.mInvulnerable != INVULNERABLE_SOURCE)
										throw("Invulnerability Inconsistency F");*/

								if (collResult.mDistance < collResultB.mDistance){
									return collResult;
								}else{
									return collResultB;
								}
							}
						}
					}
				}
			}
		}
		rv.mIntersects = false;
		return rv;
	}

	SplitBoxCollisionElement::SplitBoxCollisionElement(GameObject* gameObject, XML* xml):
		Parent(gameObject, xml){
		mSplit = vector3FromString(xml->getString("split"));
		if (xml->getError() != XML_OK){
			mSplit.x = xml->getInt("split_x");
			if (xml->getError() != XML_OK) throw("Split Boxes Requires 'split' value");
			mSplit.y = xml->getInt("split_y");
			if (xml->getError() != XML_OK) throw("Split Boxes Requires 'split' value");
			mSplit.z = xml->getInt("split_z");
			if (xml->getError() != XML_OK) throw("Split Boxes Requires 'split' value");
		}
		if (xml->getInt("elem", "force_all") == 1){
			mForceAll = 1;
		}else{
			mForceAll = 0;
		}
	}
	SplitBoxCollisionElement::SplitBoxCollisionElement(GameObject* gameObject, string boneName, Ogre::Vector3 split):
		Parent(gameObject, boneName){
		mSplit = split;
	}
	void SplitBoxCollisionElement::build(){
		Parent::build();
		Vector3 mSplitCounter;
		for (mSplitCounter.x = 0; mSplitCounter.x < mSplit.x; mSplitCounter.x++){
			for (mSplitCounter.y = 0; mSplitCounter.y < mSplit.y; mSplitCounter.y++){
				for (mSplitCounter.z = 0; mSplitCounter.z < mSplit.z; mSplitCounter.z++){
					Ogre::AxisAlignedBox aabb;
					aabb = *(mBox);
					Ogre::Vector3 max = aabb.getMaximum();
					Ogre::Vector3 min = aabb.getMinimum();
					Ogre::Vector3 siz = max-min;
					aabb.setExtents(
						min+((siz/mSplit)*mSplitCounter),
						(min+(siz/mSplit)*(mSplitCounter+1))
						);

					// Check if there's at least 1 point inside the box.  If not then skip

					bool thereIsAPointInside = false;

					if (!mForceAll){
						size_t vertex_count,index_count;
						Vector3* vertices;
						unsigned long* indices;
						Ogre::MeshPtr meshPtr = mGameObject->getEntity()->getMesh();
						Shmoulette::OgreSupp::getMeshInformation(meshPtr,
							vertex_count,
							vertices,
							index_count,
							indices);

						Ogre::Mesh::SubMeshIterator smi = meshPtr->getSubMeshIterator();
						Ogre::AxisAlignedBox ab;

						while(smi.hasMoreElements()){
							Ogre::Mesh::BoneAssignmentIterator it = smi.getNext()->getBoneAssignmentIterator();
							while(it.hasMoreElements()){
								Ogre::VertexBoneAssignment vbal;
								vbal = it.getNext();

								Ogre::Vector3 vpos;
								vpos = vertices[vbal.vertexIndex];
								//vpos = //??? (mEntity->getMesh()->get___SOMETHING?(vbal.vertexIndex));
								Ogre::AxisAlignedBox ab;
								ab.setNull();
								ab.setExtents(vpos, vpos);
								if (ab.intersects(aabb)){
									thereIsAPointInside = true;
									break;
								}
							}
							if (thereIsAPointInside) break;
						}
					}
					// Check if there's at least 1 point inside the box.  If not then skip
					if (thereIsAPointInside || mForceAll){
						BoxCollisionElement* ce;
						ce = new BoxCollisionElement(mGameObject, this->mBoneName);
						ce->setInvulnerable(this->mInvulnerable);
						ce->setBox(aabb);
						aabb.scale(mScale);
						ce->build();
						mChildElem.push_back(ce);
					}
				}
			}
		}

	}

	/*bool CollisionStructure::isColliding(CollisionStructure* cStruct){
		if (intersects(cStruct)){
			if (mChildElem.size() == 0) return true;
			vector<CollisionElement*>::iterator elem;
			for (elem=mChildElem.begin();elem!=mChildElem.end();elem++){
				if (cStruct->isColliding(*elem)) return true;
			}
		}

		return false;
	}*/
	
	/*SplitBoxCollisionElement::SplitBoxCollisionElement(Shmoulette::GameObject* gameObject, XML* xml):Parent(gameObject, xml){

	}

	SplitBoxCollisionElement::SplitBoxCollisionElement(Shmoulette::GameObject* gameObject, string boneName, Vector3 split):Parent(gameObject, boneName){

	}*/

	AutoBoxCollisionElement::AutoBoxCollisionElement(GameObject* gameObject, string boneName)
		:Parent(gameObject, boneName){
		mScale.x = 1;
		mScale.y = 1;
		mScale.z = 1;
	}

	BoxCollisionElement::BoxCollisionElement(GameObject* gameObject, string boneName)
		:Parent(gameObject, boneName, COLLISION_ELEMENT_BOX){
		mBox = new Ogre::AxisAlignedBox();
		mTBox = new Ogre::AxisAlignedBox();
#ifdef DEBUG_BOX
		mWbb = new Ogre::WireBoundingBox();
		if (mInvulnerable){
			mWbb->setMaterial("Pure/Red");
		}
#endif
	}

	BoxCollisionElement::BoxCollisionElement(GameObject* gameObject, XML* xml)
		:Parent(gameObject, xml, COLLISION_ELEMENT_BOX){
		string goupsie = xml->getContent();
		mBox = new Ogre::AxisAlignedBox();
		mTBox = new Ogre::AxisAlignedBox();
#ifdef DEBUG_BOX
		mWbb = new Ogre::WireBoundingBox();
		if (mInvulnerable){
			mWbb->setMaterial("Pure/Red");
		}
#endif
		Ogre::AxisAlignedBox aabb;
		Ogre::Vector3 pos = vector3FromString(xml->getString("center"));
		if (xml->getError() != XML_OK){
			float x1 = xml->getFloat("x1");
			if (xml->getError() == XML_OK){
				float y1 = xml->getFloat("y1");
				if (xml->getError() != XML_OK) throw("Box Collision Element missing y1 field :"+xml->getContent());
				float z1 = xml->getFloat("z1");
				if (xml->getError() != XML_OK) throw("Box Collision Element missing z1 field :"+xml->getContent());
				float x2 = xml->getFloat("x2");
				if (xml->getError() != XML_OK) throw("Box Collision Element missing x2 field :"+xml->getContent());
				float y2 = xml->getFloat("y2");
				if (xml->getError() != XML_OK) throw("Box Collision Element missing y2 field :"+xml->getContent());
				float z2 = xml->getFloat("z2");
				if (xml->getError() != XML_OK) throw("Box Collision Element missing z2 field :"+xml->getContent());
				aabb.setExtents(x1,y1,z1,x2,y2,z2);
				setBox(aabb);
			}else{
				int g=0;
			}
		}else{
			Ogre::Vector3 size = vector3FromString(xml->getString("size"));
			if (size.x < 0) size.x = 0-size.x;
			if (size.y < 0) size.y = 0-size.y;
			if (size.z < 0) size.z = 0-size.z;
			Ogre::Vector3 min = pos-(size/2);
			Ogre::Vector3 max = pos+(size/2);
			aabb.setExtents(min, max);
			setBox(aabb);
		}
	}

	void BoxCollisionElement::build(){
		Parent::build();
#ifdef DEBUG_BOX
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->attachObject(mWbb);
#endif
	}
	BoxCollisionElement::~BoxCollisionElement(){
#ifdef DEBUG_BOX
		Level::getSingleton()->getSceneManager()->getRootSceneNode()->detachObject(mWbb);
		delete mWbb;
#endif
		delete mBox;
		delete mTBox;
	}

	void BoxCollisionElement::update(){
		Parent::update();
			if (mBox != NULL){
			*mTBox = *mBox;
			//Level::getSingleton()->getSceneManager()->_updateSceneGraph(NULL);
			//mGameObject->getSceneNode()->_update(true, true);
//			mGameObject->forceUpdate();
			//mGameObject->getSceneNode()->_update(true, true);
			//Ogre::Vector3 gpos = mGameObject->getEntity()->_getDerivedPosition();

			Ogre::Matrix4 snt = mGameObject->getEntity()->_getParentNodeFullTransform();
			if (mBone != NULL){
				Ogre::Matrix4 bnt;
				mBone->_update(true, true); //OPTIMIZATION : THIS COSTS
				mBone->_getOffsetTransform(bnt);
				mTBox->transformAffine(snt*bnt);
			}else{
				mTBox->transformAffine(snt);
			}
#ifdef DEBUG_BOX
			mWbb->setupBoundingBox(*mTBox);
#endif			
#ifdef DEBUG_CUBES
			mDebugSceneNode[0]->setPosition(mTBox->getMinimum());
			Ogre::Vector3 v;
			v = mTBox->getMinimum();
			v.z = mTBox->getMaximum().z;
			mDebugSceneNode[1]->setPosition(v);
			v = mTBox->getMinimum();
			v.x = mTBox->getMaximum().x;
			mDebugSceneNode[2]->setPosition(v);
			v = mTBox->getMinimum();
			v.y = mTBox->getMaximum().y;
			mDebugSceneNode[3]->setPosition(v);

			v = mTBox->getMaximum();
			v.x = mTBox->getMinimum().x;
			mDebugSceneNode[4]->setPosition(v);

			v = mTBox->getMaximum();
			v.y = mTBox->getMinimum().y;
			mDebugSceneNode[5]->setPosition(v);

			v = mTBox->getMaximum();
			v.z = mTBox->getMinimum().z;
			mDebugSceneNode[6]->setPosition(v);

			mDebugSceneNode[7]->setPosition(mTBox->getMaximum());
#endif
		}

	}
}



