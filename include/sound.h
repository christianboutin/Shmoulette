// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___SOUND_H___
#define ___DEF___SOUND_H___

//#define AUDIERE
//#define CAUDIO
#define IRRKLANG

#ifdef IRRKLANG
	#include "..\..\irrKlang\include\irrKlang.h"
#endif

#ifdef AUDIERE
	#include "..\..\audiere\include\audiere.h"
#endif

#ifdef CAUDIO
	#include <iostream>
	#include <math.h>

	//Include IAudioManager so we can easily work with cAudio
	#include "../../cAudio\include/IAudioManager.h"
	//Include IAudio so we can create cAudio objects
	#include "../../cAudio\include/IAudio.h"
	//Include The cAudio vector class
	#include "../../cAudio\include/cVector3.h"
	//Include our version of Sleep to free CPU usage
	#include "../../cAudio\include/cAudioSleep.h"
#endif

#include <list>
#include <string>

#include "export.h"
#include "util\file.h"


namespace Shmoulette{

	//static		audiere::AudioDevicePtr device(audiere::OpenDevice());
	//static		audiere::OutputStreamPtr fx(audiere::OpenSound(device, "media\\audio\\212121_SOUNDDOGS_la.mp3", false));

	class _ShmouletteExport Sound{

#ifdef AUDIERE
		audiere::SoundEffect* mSndPtr;
		audiere::SampleSource* mSmpPtr;
#endif
#ifdef CAUDIO
		list<cAudio::IAudio*> mSndPtrInstance;
#endif
#ifdef IRRKLANG
		irrklang::ISoundSource* mSource;
		list<irrklang::ISound*> mSndPtrInstance;
#endif

		std::string mLabel;
		double mLength;

		float mPreMuteVolume;

		Ogre::Node* mNode;
		
		public:
			std::string getLabel(){return mLabel;}
			Sound(std::string filename, bool isStreamed = false);
			void setNode(Ogre::Node* node){mNode = node;}
			void play(bool isSpeech = false);
			void pause();
			void resume();
			
			void stop(){
#ifdef AUDIERE
				mSndPtr->stop();
#endif
#ifdef CAUDIO
				for (list<cAudio::IAudio*>::iterator it = mSndPtrInstance.begin();it!= mSndPtrInstance.end();it++){
					if ((*it)->playing()){
						(*it)->stop();
					}
				}
#endif
			}

			void mute();
			void unMute();
			double getLength(){
				return mLength;

			};
			
	};

	/*typedef struct TAG_SOUND_LIBRARY{
		std::string filename;
		//audiere::OutputStreamPtr ptr;
		audiere::SoundEffect* ptr;
	}SOUND_LIBRARY;*/

	class _ShmouletteExport SoundManager{
#ifdef AUDIERE
		audiere::AudioDevice* mDevice;
		audiere::OutputStreamPtr mMusic;
		double mSpeechFadeLength;
#endif
#ifdef CAUDIO
		cAudio::IAudioManager* mAudioManager;
		cAudio::IListener* mListener;
		list<cAudio::IAudio*> mSpeechPtr;
		cAudio::IAudio* mMusic;
#endif
#ifdef IRRKLANG
		irrklang::ISoundEngine* mEngine;
		irrklang::ISound* mMusic;
		list<irrklang::ISound*> mSpeechPtr;
#endif


		std::list<pair<Sound*, int>>mSndPtr;

		Ogre::Node* mListenerNode;

		//std::list<SOUND_LIBRARY>mSndLib;

		SoundManager();
		static SoundManager* mSingleton;
		bool mMusicFadeOut;
		double mMusicFadeOutDepth;
		
	public:
		static SoundManager* getSingleton(){
			if (mSingleton == NULL){
				mSingleton = new SoundManager();
				mSingleton->mListenerNode = NULL;
			}
			return mSingleton;
		}
		
#ifdef AUDIERE
		audiere::AudioDevice* getDevice(){return mDevice;}
		void speechFade(double length){mSpeechFadeLength += length;}
#endif
#ifdef CAUDIO
		cAudio::IAudioManager* getManager(){return mAudioManager;}
		void addSpeechPtr(cAudio::IAudio* ptr){mSpeechPtr.push_back(ptr);}
#endif
#ifdef IRRKLANG
		irrklang::ISoundEngine* getEngine(){return mEngine;}
		void addSpeechPtr(irrklang::ISound* ptr){mSpeechPtr.push_back(ptr);}
#endif
		void setListenerNode(Ogre::Node* node){mListenerNode = node;}
		~SoundManager();
		Sound* createSound(std::string filename, bool stream = false);
		void destroySound(Sound* ptr);
		void startMusic(std::string filename, bool repeat = false);
		void stopMusic();
		void pause();
		void resume();

		void muteAll();
		void unMuteAll();
		void update(float time);

		void musicFadeOut(double length = 4);
	};

}

#endif
