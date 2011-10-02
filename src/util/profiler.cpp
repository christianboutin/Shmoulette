#include "stdafx.h"
#include <fcntl.h>
#include "util/profiler.h"

namespace Shmoulette{
	namespace Profiler{
		ProfilerFactory* ProfilerFactory::mSingleton = NULL;

		ProfilerFactory::ProfilerFactory(){
		}

#ifdef PROFILER_ON
		list<ProfilerGroup*>ProfilerFactory::mGroup;

		ProfilerEntry* ProfilerFactory::getProfiler(string s, string group){
			list<ProfilerGroup*>::iterator it;

			for (it = mGroup.begin();it!=mGroup.end();it++){
				if ((*it)->mId == group){
					list<ProfilerEntry*>::iterator eit;
					for (eit = (*it)->mProfilerEntry.begin();eit != (*it)->mProfilerEntry.end();eit++){
						if ((*eit)->getLabel() == s){
							return (*eit);
						}
					}
					ProfilerEntry* pe = new ProfilerEntry(s);
					(*it)->mProfilerEntry.push_back(pe);
					return pe;
				}

			}
			ProfilerGroup* pgroup = new ProfilerGroup();
			pgroup->mId = group;
			ProfilerEntry* pe = new ProfilerEntry(s);
			pgroup->mProfilerEntry.push_back(pe);
			mGroup.push_back(pgroup);
			return pe;
		}

		void ProfilerFactory::generateReport(){
			list<ProfilerGroup*>::iterator it;
			char datebuf[256];
			_strdate(datebuf);
			char timebuf[256];
			_strtime_s(timebuf, 9);
			string datestr;
			datestr = datebuf[6];
			datestr+= datebuf[7];
			datestr+= datebuf[0];
			datestr+= datebuf[1];
			datestr+= datebuf[3];
			datestr+= datebuf[4];
			datestr+= timebuf[0];
			datestr+= timebuf[1];
			datestr+= timebuf[3];
			datestr+= timebuf[4];
			datestr+= timebuf[6];
			datestr+= timebuf[7];

			for (it = mGroup.begin();it!=mGroup.end();it++){
				int file_handle;
				string r_out;


				string filename = "profiler/profiler_"+datestr+"_"+(*it)->mId+".csv";
				file_handle = _open(filename.c_str(), _O_WRONLY|_O_CREAT|_O_TEXT, _S_IREAD|_S_IWRITE);
				if (file_handle == -1){
					throw("Log File Creation Error");
				}
				list<ProfilerEntry*>::iterator eit;
				r_out="";
				for (eit = (*it)->mProfilerEntry.begin();eit != (*it)->mProfilerEntry.end();eit++){
					r_out += (*eit)->getLabel();
					list<unsigned long>::iterator tit;
					list<unsigned long>tList = (*eit)->getTimes();
					for (tit = tList.begin();tit != tList.end();tit++){
						r_out+=","+ts((double)(*tit)/1000000);
					}
					std::stringstream ss;
					ss << std::endl;
					r_out +=  ss.str();
				}
				_write(file_handle, r_out.c_str(), (unsigned int)r_out.size());
				_close(file_handle);
			}
		}
#endif

	}
}