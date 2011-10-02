#include "main.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


namespace Util{
	class ProfileEntry{
		public:
		string label;
		Ogre::Timer* sTimer;
	};

	
	bool aboutEqual(float a, float b){
		return floor(a*1000) == floor(b*1000);
	}

#define USE_OGRE_LOG
//#define LOG_ON

	static Ogre::Timer* sTimer = NULL;
	static int gLogLevel = 5;
	static vector<ProfileEntry*> sProfileEntry;
	
	void deprecated_Profile(string marker, int level){
		vector<ProfileEntry*>::iterator it;
		for (it = sProfileEntry.begin();it!=sProfileEntry.end();it++){
			if ((*it)->label == marker){
				unsigned long timer = (*it)->sTimer->getMillisecondsCPU();

				/*int file_handle;*/
				string r_out;
				/*file_handle = _open("profiler.txt", _O_RDWR|_O_CREAT|_O_TEXT|_O_APPEND, _S_IREAD|_S_IWRITE);
				if (file_handle == -1){
					throw("Log File Creation Error");
				}*/
				r_out = "PR > ";
				std::stringstream ss;
				ss << std::setw(50) << marker << " " << std::setw(20) << timer << std::endl;
				
				r_out +=ss.str();

				/*_write(file_handle, r_out.c_str(), (unsigned int)r_out.size());
				_close(file_handle);*/
				Util::Log(r_out, level);
				
				delete (*it)->sTimer;
				delete (*it);
				sProfileEntry.erase(it);
				return;
			}
		}

		ProfileEntry* pi = new ProfileEntry();
		pi->sTimer = new Ogre::Timer();
		pi->sTimer->reset();
		pi->label = marker;
		sProfileEntry.push_back(pi);


		/*
		if (sTimer == NULL){
			sTimer = new Ogre::Timer();
			sTimer->reset();
		}
		if (marker != ""){
			unsigned long timer = sTimer->getMillisecondsCPU();

			int file_handle;
			string r_out;
			file_handle = _open("profiler.txt", _O_RDWR|_O_CREAT|_O_TEXT|_O_APPEND, _S_IREAD|_S_IWRITE);
			if (file_handle == -1){
				throw("Log File Creation Error");
			}
			r_out = "";
			std::stringstream ss;
			ss << std::setw(50) << marker << " " << std::setw(20) << timer << std::endl;
			
			r_out +=ss.str();

			_write(file_handle, r_out.c_str(), (unsigned int)r_out.size());
			_close(file_handle);
		}
		sTimer->reset();*/

	}

	void LogRaiseLevel(){
		if (gLogLevel<20){
			gLogLevel++;
		}
		Util::Log("Changing Log Level To : "+ts(gLogLevel),-1);
	}
	void LogLowerLevel(){
		if (gLogLevel>0){
			gLogLevel--;
		}
		Util::Log("Changing Log Level To : "+ts(gLogLevel),-1);
	}

	void Log(string out, int level){
	#ifndef LOG_ON
		return;
	#else
		if (level <= Util::gLogLevel){
				string r_out;

				r_out = "";
				for (int i=0;i<level;i++){
					r_out += " ";
				}
				
				std::stringstream ss;
				struct tm *pTime;
				time_t ctTime; time(&ctTime);
				pTime = localtime( &ctTime );
				ss << std::setw(2) << std::setfill('0') << pTime->tm_hour
						<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
						<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec 
						<< ": " << out << std::endl;

				
				r_out +=ss.str();
				//r_out +=" : "+out+"\n";

				_cprintf(out.c_str());
				_cprintf("\n");

			
				#ifdef USE_OGRE_LOG
					Ogre::LogManager::getSingleton().logMessage(r_out);
				#else
					int file_handle;
					file_handle = _open("log.txt", _O_RDWR|_O_CREAT|_O_TEXT|_O_APPEND, _S_IREAD|_S_IWRITE);
					if (file_handle == -1){
						throw("Log File Creation Error");
					}
					_write(file_handle, r_out.c_str(), (unsigned int)r_out.size());
					_close(file_handle);
				#endif
			}
	#endif
	}
	int stringNbLines(string str){
		int rv = 1;
		size_t i;
		string nStr = str;
		i = nStr.find('\n');
		while (i != string::npos){
			rv++;
			nStr = nStr.substr(i+1);
			i = nStr.find('\n');
		}
		return rv;
	}

}