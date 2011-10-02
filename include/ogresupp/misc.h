#include "Ogre.h"


namespace Shmoulette{
	namespace OgreSupp{
		void getMeshInformation( Ogre::MeshPtr mesh, size_t &vertex_count,
										 Ogre::Vector3* &vertices,
										 size_t &index_count, unsigned long* &indices,
										 const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
										 const Ogre::Quaternion &orient = Ogre::Quaternion::IDENTITY,
										 const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);
	}
}



    /*const Vector3 &position = Vector3::ZERO,
    const Quaternion &orient = Quaternion::IDENTITY,const Vector3 &scale = Vector3::UNIT_SCALE)*/