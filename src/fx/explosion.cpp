#include "stdafx.h"

#include "fx\explosion.h"
#include "level.h"

namespace Shmoulette{
using namespace Ogre;
/*
Explosion::Explosion(Vector3 pos, float decay, GameObject* parent, string particle, float delay)
	:Parent(parent!=NULL?parent->getId()+"_explosion":"explosion",true, parent){
		mParticleName = particle; mDecay=decay;
		mSceneNode->setPosition(pos);
		mDelayDie = 5;
	destroyCollisionStructure();
}

void Explosion::build(){
//	mDoNotHook = true;
	setHooked(false);

	Parent::build();
	mParticleSystem = Level::getSingleton()->getSceneManager()->createParticleSystem(getId()+"_particles",mParticleName);
	mSceneNode->attachObject(mParticleSystem);
	mEmissionRate = mParticleSystem->getEmitter(0)->getEmissionRate();
	mMaxSpeed = mParticleSystem->getEmitter(0)->getMaxParticleVelocity();
	mMinSpeed = mParticleSystem->getEmitter(0)->getMinParticleVelocity();
	mDecayEmit = mEmissionRate / mDecay;
	mDecayMaxSpeed = mMaxSpeed / mDecay;
	mDecayMinSpeed = mMinSpeed / mDecay;

}

void Explosion::destroy(){
	mSceneNode->detachObject(mParticleSystem);
	Level::getSingleton()->getSceneManager()->destroyParticleSystem(mParticleSystem);
	Parent::destroy();
}
bool Explosion::update(){
	Parent::update();
	mEmissionRate -= mDecayEmit*mLevelSegment->getTimeDelta();
	mMinSpeed -= mDecayMinSpeed*mLevelSegment->getTimeDelta();
	mMaxSpeed -= mDecayMaxSpeed*mLevelSegment->getTimeDelta();
	
	if (mEmissionRate > 0){
		mParticleSystem->getEmitter(0)->setEmissionRate(mEmissionRate);
		mParticleSystem->getEmitter(0)->setMaxParticleVelocity(mMaxSpeed);
		mParticleSystem->getEmitter(0)->setMinParticleVelocity(mMinSpeed);
	}else{
		mParticleSystem->getEmitter(0)->setEmissionRate(0);
		mParticleSystem->getEmitter(0)->setMaxParticleVelocity(0);
		mParticleSystem->getEmitter(0)->setMinParticleVelocity(0);
		mDelayDie -=mLevelSegment->getTimeDelta();
		if (mDelayDie <=0){
			return true;
		}
	}
	
	return false;

}
*/
}