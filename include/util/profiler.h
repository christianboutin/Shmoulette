#ifndef PROFILER_H
#define PROFILER_H
namespace Shmoulette{
		#include "OgreTimer.h"
		namespace Profiler{
//#define PROFILER_ON
//#define PROFILER_STRICT




#ifdef PROFILER_ON
			#define MACRO_PROFILER_GET(name, group) Shmoulette::Profiler::ProfilerFactory::getSingleton()->getProfiler(name, group)

			class _ShmouletteExport ProfilerEntry{
				string mLabel;
				Ogre::Timer* mTimer;
				list<unsigned long> mTime;
				bool mStarted;
			public:
				ProfilerEntry(string s){
					mLabel = s;
					mTimer=new Ogre::Timer();
					mStarted=false;
				}
				void start(){
#ifdef PROFILER_STRICT
					if (mStarted) throw("Profiler already started"+mLabel);
#endif
					mTimer->reset();
					mStarted = true;
				}
				void end(){
#ifdef PROFILER_STRICT
					if (!mStarted) throw("Profiler not started "+mLabel);
#else
					if (!mStarted) mTimer->reset();
#endif
					mTime.push_back(mTimer->getMicrosecondsCPU());
					mStarted = false;
				}
				list<unsigned long> getTimes(){return mTime;}
				string getLabel(){return mLabel;}
			};
			class _ShmouletteExport ProfilerGroup{
				public:
				string mId;
				list<ProfilerEntry*>mProfilerEntry;

			};
			

			class _ShmouletteExport ProfilerFactory{
				private:
					static list<ProfilerGroup*>mGroup;
					static ProfilerFactory* mSingleton;
					ProfilerFactory();
				public:
					static ProfilerFactory* getSingleton(){
						if (mSingleton == NULL){
							mSingleton = new ProfilerFactory();
						}
						return mSingleton;

					}
					static ProfilerEntry* getProfiler(string s, string group);
					static void generateReport();
			};

#else
			#define MACRO_PROFILER_GET(name, group) Shmoulette::Profiler::ProfilerFactory::getSingleton()->getProfiler("","")

			class ProfilerEntry{
			public:
				void start(){}
				void end(){}
			};
			
			class ProfilerFactory{
				private:
					static ProfilerFactory* mSingleton;
					ProfilerEntry* mDummyEntry;
					ProfilerFactory();
				public:
					static ProfilerFactory* getSingleton(){
						if (mSingleton == NULL){
							mSingleton = new ProfilerFactory();
							mSingleton->mDummyEntry = new ProfilerEntry();
						}
						return mSingleton;

					}
					static ProfilerEntry* getProfiler(string s, string group){return mSingleton->mDummyEntry;}
					static void generateReport(){}
			};

#endif

		};
};


#endif