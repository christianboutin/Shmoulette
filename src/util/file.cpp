#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
//#include <darksdk.h>
#include <string>
#include "util\file.h"
#include <sys\stat.h>
#include <string.h>
#include "windows.h"
#include <exception>
namespace Shmoulette{

FILE_ERROR File::fileError = FILE_OK;
FileLoadCallback* File::mCallback = NULL;

std::list<FileCacheEntry*> File::mFileCache;


File::File(std::string str, std::string context, FILE_MODE mode){
	init((char*)str.c_str(), context, mode);
}

void File::init(char* filename, std::string context, FILE_MODE mode, bool cache){
	FileCacheEntry* cacheEntry;
	using namespace std;
	char dirBackup[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, dirBackup);
	string path = context;

	mFilename = filename;
	mContext = context;
	mCompleteFilename = mContext+(std::string)"/"+mFilename;

	/*if (cache && mode == FILE_READ){
		list<FileCacheEntry*>::iterator it;
		for (it = mFileCache.begin();it != mFileCache.end();it++){
			if ((*it)->mFilename == filename && (*it)->mContext == context){
				mContent = malloc((*it)->mSize);
				memcpy(mContent, (*it)->mContent, (*it)->mSize);
				mHandle = -1;
				fileError = FILE_OK;
				return;
			}
		}
	}*/


	try{
		SetCurrentDirectory((LPCSTR)path.c_str());
	}catch (exception e){
		int g=0;
	}
	char dirGLUGLU[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, dirGLUGLU);
	
	mContent = NULL;
	switch (mode){
		case FILE_READ:
			mHandle = ::_open(filename, _O_BINARY|_O_RDONLY);
		break;
		case FILE_WRITE:
			::DeleteFile(filename);
			mHandle = ::_open(filename, _O_BINARY|_O_WRONLY|_O_CREAT, _S_IREAD | _S_IWRITE);
		break;
	}
	SetCurrentDirectory(dirBackup);	

	if (mHandle == -1){
		fileError = FILE_NOT_FOUND;
		mContent = NULL;

		//throw((std::string)("File not found : ")+(std::string)(filename));
		return;
	}else{
		mLength = _filelength(mHandle);
		struct _stat stat;
		::_fstat(mHandle, &stat);
		mTimeStamp = stat.st_mtime;
	}

	if (mCallback != NULL){
		mCallback->fileLoadStart(filename);
	}


	if (mode == FILE_READ){
		mContent = malloc(mLength+1);//_filelength(mHandle )+1);
		::_read(mHandle , mContent, mLength);//_filelength(mHandle ));
		char* ptr = (char*)mContent;
		ptr[mLength] = '\0';
		//free(mContent);
		if (cache){
			cacheEntry = new FileCacheEntry();
			cacheEntry->mFilename = filename;
			cacheEntry->mContext = context;
			cacheEntry->mSize = mLength+1;
			cacheEntry->mContent = malloc(cacheEntry->mSize);
			memcpy(cacheEntry->mContent, mContent, cacheEntry->mSize);

			mFileCache.push_back(cacheEntry);
		}
	}else{
		mContent = NULL;
	}
	fileError = FILE_OK;
	if (mCallback != NULL){
		mCallback->fileLoadEnd(filename);
	}
}

File::File(char* filename, char* context, FILE_MODE mode){
	init(filename, context, mode);
}

void File::write(char* buffer){
	::_write(mHandle, buffer, (unsigned int)strlen(buffer));
}

bool File::isNewer(File* dest){
	if (mTimeStamp > dest->mTimeStamp){
		return true;
	}
	return false;
}

void File::wipeAndWrite(char* buffer){
	if (mHandle != -1){
		_close(mHandle);

		::DeleteFile((LPCSTR)mCompleteFilename.c_str());
		mHandle = ::_open(mCompleteFilename.c_str(), _O_BINARY|_O_WRONLY|_O_CREAT, _S_IREAD | _S_IWRITE);
		write(buffer);
	}else{
		throw("You can't wipeAndWrite a cached file.  This should never happen");
	}

}

void File::write(std::string buffer){
	::_write(mHandle, buffer.c_str(), buffer.length());
}


void File::rename(char* oldFilename, char* newFilename, char* oldContext, char* newContext){
	std::string completeOldPathname;
	completeOldPathname = oldContext;
	completeOldPathname+="\\\\";
	completeOldPathname+=oldFilename;

	std::string completeNewPathname;
	completeNewPathname = newContext;
	completeNewPathname+="\\\\";
	completeNewPathname+=newFilename;

	if (!exists(oldFilename, oldContext)){
		fileError = FILE_NOT_FOUND;
		return;
	}
	if (exists(newFilename, newContext)){
		fileError = DESTINATION_FILE_ALREADY_EXISTS;
		return;
	}

	::rename(completeOldPathname.c_str(), completeNewPathname.c_str());
}

bool File::exists(std::string filename,std::string context){
	int handle;
	std::string path = context;
	handle = ::_open((path+(std::string)"\\"+filename).c_str(), _O_BINARY|_O_RDONLY);
	if (handle == -1){
		return false;
	}else{
		_close(handle);
		return true;
	}
}

File::~File(){
	if (mHandle != -1)
		_close(mHandle);
	if (mContent != NULL){
		free(mContent);
	}
}

FileIterator::FileIterator(std::string pattern, std::string context){
	mPattern = pattern;
	mContext = context;
}

File* FileIterator::getFirst(){
	std::string filename = mContext + "\\"+mPattern;
	mSystemPtr = _findfirst(filename.c_str(), &mFindData);
	if (mSystemPtr == -1){
		mError = FILE_NOT_FOUND;
		return NULL;
	}
	mError = FILE_OK;
	return new File(mFindData.name, mContext, FILE_READ);
}
File* FileIterator::getNext(){
	if (_findnext(mSystemPtr, &mFindData) == -1){
		mError = FILE_NOT_FOUND;
		return NULL;
	}else{
		mError = FILE_OK;
		return new File(mFindData.name, mContext, FILE_READ);
	}
}

std::string FileIterator::getFirstFilename(){
	std::string filename = mContext + "\\"+mPattern;
	mSystemPtr = _findfirst(filename.c_str(), &mFindData);
	
	if (mSystemPtr == -1){
		mError = FILE_NOT_FOUND;
		return "FILE_NOT_FOUND";
	}
	mError = FILE_OK;
	return mFindData.name;

}

std::string FileIterator::getNextFilename(){
	if (_findnext(mSystemPtr, &mFindData) == -1){
		mError = FILE_NOT_FOUND;
		return "";
	}else{
		mError = FILE_OK;
		return mFindData.name;
	}
}



}