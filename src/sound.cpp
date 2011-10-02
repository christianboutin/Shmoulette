#include "stdafx.h"
#include "sound.h"

namespace Shmoulette{
	using namespace std;

	SoundManager* SoundManager::mSingleton = NULL;
	
	SoundManager::SoundManager(){

		/*std::vector<audiere::AudioDeviceDesc> deviceList;
		audiere::GetSupportedAudioDevices(deviceList);*/
		
#ifdef AUDIERE
		mDevice = audiere::OpenDevice("directsound");
		mSpeechFadeLength = 0;
		mMusicFadeOut = false;
		mMusic = NULL;
#endif
#ifdef IRRKLANG
		mEngine = irrklang::createIrrKlangDevice();
		mMusic = NULL;
		mMusicFadeOut = false;
#endif
#ifdef CAUDIO
		mAudioManager = cAudio::getAudioManager();
		mAudioManager->init(0,NULL);
		mListener = mAudioManager->getListener();
		mMusic = NULL;
		mMusicFadeOut = false;
#endif

		//audiere::OpenSound(mDevice,"media\\audio\\crossfire.wav",true)->play();
	}
	void SoundManager::muteAll(){
		list<pair<Sound*, int>>::iterator it;
		for (it = mSndPtr.begin();it != mSndPtr.end();it++){
			(*it).first->mute();
		}
#ifdef AUDIERE
		if (mMusic){
			mMusic->setVolume(0);
		}
#endif
#ifdef CAUDIO
		if (mMusic){
			mMusic->setVolume(0);
		}
#endif
#ifdef IRRKLANG
		if (mMusic){
			mMusic->setVolume(0);
		}
#endif
	}
	void SoundManager::unMuteAll(){
		list<pair<Sound*, int>>::iterator it;
		for (it = mSndPtr.begin();it != mSndPtr.end();it++){
			(*it).first->unMute();
		}
#ifdef AUDIERE
		if (mMusic){
			mMusic->setVolume(1);
		}
#endif
#ifdef CAUDIO
		if (mMusic){
			mMusic->setVolume(1);
		}
#endif
#ifdef IRRKLANG
		if (mMusic){
			mMusic->setVolume(1);
		}
#endif
	}

	SoundManager::~SoundManager(){
	}

	Sound* SoundManager::createSound(string filename, bool stream){

#ifndef CAUDIO
		list<pair<Sound*, int>>::iterator it;
		for (it = mSndPtr.begin();it != mSndPtr.end();it++){
			
			if ((*it).first->getLabel() == filename){
				(*it).second +=1;
				return (*it).first;
			}
		}
#endif
		pair<Sound*, int> elem;
		//Sound* snd;
		elem.first = new Sound(filename);
		elem.second = 1;

		
		mSndPtr.push_back(elem);
		return elem.first;
	}
	void SoundManager::destroySound(Sound* ptr){
		list<pair<Sound*, int>>::iterator it;
		for (it = mSndPtr.begin();it != mSndPtr.end();it++){
			if ((*it).first == ptr){
				(*it).second -=1;
				if ((*it).second == 0){
					delete (*it).first;
					mSndPtr.erase(it);
				}
				break;
			}
		}
	}

	void SoundManager::pause(){
#ifdef IRRKLANG
		if (mMusic != NULL){
			mMusic->setIsPaused(true);
		}
		list<pair<Sound*, int>>::iterator it;
		for (it = mSndPtr.begin();it != mSndPtr.end();it++){
			(*it).first->pause();
		}
#endif
	}

	void SoundManager::resume(){
#ifdef IRRKLANG
		if (mMusic != NULL){
			mMusic->setIsPaused(false);
		}
		list<pair<Sound*, int>>::iterator it;
		for (it = mSndPtr.begin();it != mSndPtr.end();it++){
			(*it).first->resume();
		}
#endif
	}

	void SoundManager::stopMusic(){
		if (mMusic != NULL){
			mMusic->stop();
		}
	}

	void SoundManager::startMusic(string filename, bool repeat){
		if (filename.find(".") == string::npos) filename +=".ogg";
		//mMusic = audiere::OpenSound(mDevice,((std::string)("media\\audio\\"+filename)).c_str(),true);

#ifdef AUDIERE
		stopMusic();
		audiere::SampleSource* ptr;
		ptr = audiere::OpenSampleSource(((std::string)("media\\audio\\"+filename)).c_str());
		if (ptr == NULL) throw("Music file not found : "+filename);
		mMusic = mDevice->openStream(ptr);
		
		mMusic->setRepeat(repeat);
		mMusic->reset();
		mMusic->play();
#endif
#ifdef CAUDIO
		stopMusic();
		mMusic = mAudioManager->>createFromFile(filename, "media\\audio\\"+filename, true);
		//mMusic->loop(true);
		mMusic->play2d(true);
#endif
		
#ifdef IRRKLANG
		stopMusic();
		
		mMusic = mEngine->play2D(((string)("media\\audio\\"+filename)).c_str(), true, false, true, irrklang::E_STREAM_MODE::ESM_STREAMING);
		
		//mMusic->loop(true);
		//mEngine->play2D(mMusic);

#endif
		mMusicFadeOut = false;
	}

	void SoundManager::musicFadeOut(double length){
		mMusicFadeOut = true;
		mMusicFadeOutDepth = length;
	}


	void SoundManager::update(float timePassed){
#ifdef AUDIERE
		if (mMusic != NULL){
			if (mMusicFadeOut){
				if (mMusic->getVolume() > timePassed/mMusicFadeOutDepth){
					mMusic->setVolume(mMusic->getVolume()-timePassed/mMusicFadeOutDepth);
				}else{
					mMusic->stop();
					mMusicFadeOut = false;
				}
			}else{
				if (mSpeechFadeLength > 0){
					if (mMusic->getVolume() > .5){
						mMusic->setVolume(mMusic->getVolume()-timePassed);
					}
					mSpeechFadeLength -= timePassed;
					if (mSpeechFadeLength < 0) mSpeechFadeLength = 0;
				}else{
					if (mMusic->getVolume() < 1){
						mMusic->setVolume(mMusic->getVolume()+timePassed);
					}
				}
			}
		}
#endif
#ifdef CAUDIO
		if (mMusic != NULL){
			if (mMusicFadeOut){
				if (mMusic->getVolume() > timePassed/mMusicFadeOutDepth){
					mMusic->setVolume(mMusic->getVolume()-timePassed/mMusicFadeOutDepth);
				}else{
					mMusic->stop();
					mMusicFadeOut = false;
				}
			}else{
				if (mSpeechPtr.size() > 0){
					if (mMusic->getVolume() > .5){
						mMusic->setVolume(mMusic->getVolume()-timePassed);
					}

					if (!(*mSpeechPtr.begin())->playing()){
						mSpeechPtr.pop_front();
					}
					/*bool clearList = true;
					for (list<cAudio::IAudio*>::iterator it =  mSpeechPtr.begin();it!=mSpeechPtr.end();it++){
						
						if ((*it)->playing()){
							clearList = false;
						}
					}
					if (clearList) mSpeechPtr.clear();*/
				}else{
					if (mMusic->getVolume() < 1){
						mMusic->setVolume(mMusic->getVolume()+timePassed);
					}
				}
			}
		}
#endif
#ifdef IRRKLANG
		if (mMusic != NULL){
			if (mMusicFadeOut){
				float cVol = mMusic->getVolume();
				float dVol = timePassed/mMusicFadeOutDepth;
				if (cVol > dVol){
					mMusic->setVolume(cVol-dVol);
				}else{
					mMusic->stop();
					mMusicFadeOut = false;
				}
			}else{
				if (mSpeechPtr.size() > 0){
					if (mMusic->getVolume() > .5){
						mMusic->setVolume(mMusic->getVolume()-timePassed);
					}

					if ((*mSpeechPtr.begin())->isFinished()){
						mSpeechPtr.pop_front();
					}
					/*bool clearList = true;
					for (list<cAudio::IAudio*>::iterator it =  mSpeechPtr.begin();it!=mSpeechPtr.end();it++){
						
						if ((*it)->playing()){
							clearList = false;
						}
					}
					if (clearList) mSpeechPtr.clear();*/
				}else{
					if (mMusic->getVolume() < 1){
						mMusic->setVolume(mMusic->getVolume()+timePassed);
					}
				}
			}
		}
#endif
	}


	Sound::Sound(std::string filename, bool isStreamed){
		if (filename.find(".") == string::npos) filename +=".ogg";
		if (!Shmoulette::File::exists(filename, "media\\audio")){
			throw("Sound not found : "+filename);
		}
		mLabel = filename;
#ifdef AUDIERE
		mSmpPtr = audiere::OpenSampleSource(((std::string)("media\\audio\\"+filename)).c_str());
		int tmp;
		int sampleRate;
		audiere::SampleFormat tmp2;
		mSmpPtr->getFormat(tmp, sampleRate, tmp2);
		mLength = ((double)mSmpPtr->getLength())/((double)sampleRate);
		mSndPtr = OpenSoundEffect(SoundManager::getSingleton()->getDevice(), mSmpPtr, audiere::MULTIPLE);
//		mLength = ((double)mSmpPtr->getLength())/((double)sampleRate);
		//mSndPtr->getLength
		if (mSndPtr==NULL) throw("Sound not found _OR_ sound corrupted (beats me dude) : "+filename);
#endif
#ifdef CAUDIO
		cAudio::IAudioManager* iam = SoundManager::getSingleton()->getManager();
		cAudio::IAudio* sndPtr = iam->createFromFile(filename+ts(GameElement::getNextIdCounter()), "media\\audio\\"+filename, isStreamed);
		
				
		mSndPtrInstance.push_back(sndPtr);
#endif
#ifdef IRRKLANG
		irrklang::E_STREAM_MODE est = irrklang::E_STREAM_MODE::ESM_NO_STREAMING;

		if (isStreamed)
			est = irrklang::E_STREAM_MODE::ESM_STREAMING;

		mSource = SoundManager::getSingleton()->getEngine()->addSoundSourceFromFile(((string)("media\\audio\\"+filename)).c_str(), est, true);
		if (mSource == NULL){
			 mSource = SoundManager::getSingleton()->getEngine()->getSoundSource(((string)("media\\audio\\"+filename)).c_str());
			if (mSource == NULL){
				 throw("Sound not found _OR_ sound corrupted (beats me dude) : "+filename);
			}
		}
		mLength = mSource->getPlayLength();
		mLength /= 1000;
#endif
		mPreMuteVolume = FLT_MAX;
	}

	void Sound::play(bool isSpeech){
#ifdef AUDIERE
		mSndPtr->setVolume(1.0f);
		mSndPtr->play();
		if (isSpeech){
			SoundManager::getSingleton()->speechFade(mLength);
		}
#endif
#ifdef CAUDIO
		//(*mSndPtrInstance.begin())->play3d(cAudio::cVector3(0,0,0),1.0f,false);
		if (!isSpeech){
			bool found = false;
			for (list<cAudio::IAudio*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
				if (!(*it)->playing()){
					(*it)->play3d(cAudio::cVector3(Ogre::Math::RangeRandom(-1,1),Ogre::Math::RangeRandom(-1,1),Ogre::Math::RangeRandom(-1,1)),1.0f,false);
					found = true;
				}
			}
			if (!found){
				//cAudio::IAudio* first = *mSndPtrInstance.begin();
				//first->stop();
				//first->play3d(cAudio::cVector3(0,0,0),1.0f,false);

				cAudio::IAudioManager* iam = SoundManager::getSingleton()->getManager();
				cAudio::IAudio* sndPtr = iam->createFromFile(mLabel,"media\\audio\\"+mLabel);

				
				mSndPtrInstance.push_back(sndPtr);
				sndPtr->play3d(cAudio::cVector3(Ogre::Math::RangeRandom(-1,1),Ogre::Math::RangeRandom(-1,1),Ogre::Math::RangeRandom(-1,1)),1.0f,false);
			}
		}else{
			(*mSndPtrInstance.begin())->play2d(false);
			SoundManager::getSingleton()->addSpeechPtr(*mSndPtrInstance.begin());
		}

#endif
#ifdef IRRKLANG

		//(*mSndPtrInstance.begin())->play3d(cAudio::cVector3(0,0,0),1.0f,false);

		/*bool found = false;
		for (list<irrklang::ISound*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
			if ((*it)->isFinished()){
				/(*it)->setPlayPosition(0);
				(*it)->setIsPaused(false);
				(*it)->
				found = true;
			}
		}*/
		//if (!found){
		irrklang::ISound* snd;
		if (isSpeech){
			snd = SoundManager::getSingleton()->getEngine()->play2D(mSource, false, false, true, true);
			SoundManager::getSingleton()->addSpeechPtr(snd);
		}else{
			snd = SoundManager::getSingleton()->getEngine()->play3D(mSource, irrklang::vec3df(0,0,0), false, false, true, true);
		}
		mSndPtrInstance.push_back(snd);
		//}
		while((*mSndPtrInstance.begin())->isFinished() == true){
			mSndPtrInstance.pop_front();
		}

#endif
	}

	void Sound::mute(){
#ifdef AUDIERE
		mPreMuteVolume = mSndPtr->getVolume();
		mSndPtr->setVolume(0);
#endif
#ifdef CAUDIO
		if (mSndPtrInstance.size() > 0){
			mPreMuteVolume = (*mSndPtrInstance.begin())->getVolume();
			for (list<cAudio::IAudio*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
				(*it)->setVolume(0);
			}
		}else{
			mPreMuteVolume = 1;
		}
#endif
#ifdef IRRKLANG
		if (mSndPtrInstance.size() > 0){
			mPreMuteVolume = (*mSndPtrInstance.begin())->getVolume();
			for (list<irrklang::ISound*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
				(*it)->setVolume(0);
			}
		}else{
			mPreMuteVolume = 1;
		}
#endif

	}
	void Sound::pause(){
#ifdef IRRKLANG
		for (list<irrklang::ISound*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
			(*it)->setIsPaused(true);
		}
#endif
	}
	void Sound::resume(){
#ifdef IRRKLANG
		for (list<irrklang::ISound*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
			(*it)->setIsPaused(false);
		}
#endif
	}
	void Sound::unMute(){
#ifdef AUDIERE
		mSndPtr->setVolume(mPreMuteVolume);
#endif
#ifdef CAUDIO
		for (list<cAudio::IAudio*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
			(*it)->setVolume(mPreMuteVolume);
		}
#endif
#ifdef IRRKLANG
		for (list<irrklang::ISound*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
			(*it)->setVolume(mPreMuteVolume);
		}
#endif
		mPreMuteVolume = FLT_MAX;

	}

}