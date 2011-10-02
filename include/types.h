// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___TYPES_H___
#define ___DEF___TYPES_H___

#ifndef GAMETYPES_H
#define GAMETYPES_H

namespace Shmoulette{
typedef struct TAG_COLLISION_RESULT{
	class GameObject* mTarget;
	class GameObject* mChild;
	int mInvulnerable;
	float mDistance;
	bool mOccured;
}COLLISION_RESULT;

typedef enum TAG_RAIL_TYPE{
	RAIL_VERTICAL,
	RAIL_HORIZONTAL,
	RAIL_DEPTH,
	RAIL_NONE
}RAIL_TYPE;

/*typedef struct TYPE_RAIL_POS{
	float x;
	float y;
	float z;
}fPos;*/

typedef struct TYPE_RAIL_POS_2D{
	float x;
	float y;
}rPos2D;

/*typedef enum TAG_IFF{
	IFF_TARGET_ENEMY,
	IFF_TARGET_FRIENDLY,
	IFF_TARGET_EVERYTHING
}IFF;*/

}

#endif

#endif
