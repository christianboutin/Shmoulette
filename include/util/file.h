// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___FILE_H___
#define ___DEF___FILE_H___

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include "export.h"
#include "list"

namespace Shmoulette{

typedef enum TAG_FILE_MODE{
	FILE_READ,
	FILE_WRITE
}FILE_MODE;

typedef enum TAG_FILE_ERROR{
	FILE_OK,
	FILE_NOT_FOUND,
	DESTINATION_FILE_ALREADY_EXISTS
}FILE_ERROR;

class FileLoadCallback{
public:
	virtual void fileLoadStart(std::string name)=0;
	virtual void fileLoadEnd(std::string name)=0;
};

class _ShmouletteExport FileCacheEntry{
	public:
		std::string mFilename;
		std::string mContext;
		void* mContent;
		size_t mSize;
};

class _ShmouletteExport File{
	void* mContent;
	static FILE_ERROR fileError;
	int mHandle;
	int mLength;
	std::string mFilename;
	std::string mContext;
	std::string mCompleteFilename;
	__time64_t mTimeStamp;
	
	//static list<File*> mCache;
	static std::list<FileCacheEntry*> mFileCache;
	static FileLoadCallback* mCallback;
	
	void init(char* filename, std::string context, FILE_MODE file_mode = FILE_READ, bool cache = true);
	public:
	static void flushCash();
	static bool exists(std::string filename,std::string context="");
	static void File::rename(char* oldFilename, char* newFilename, char* oldContext=NULL, char* newContext=NULL);
	void write(char* buffer);
	void write(std::string buffer);

	void wipeAndWrite(char* buffer);
	bool isNewer(File* file);
	File(std::string str, std::string context="", FILE_MODE mode = FILE_READ);
	File(char* filename, char* context, FILE_MODE file_mode = FILE_READ);
	~File();
	
	static void setLoadCallback(FileLoadCallback* callback){mCallback = callback;}

	int getLength(){return mLength;}
	void* content(){return mContent;}
	std::string getStrContent(){
		if (mContent == NULL){
			throw ("Requesting string content of non-existent file "+mFilename);
		}
		std::string rv;
		rv = (char*)mContent;
		return rv;
	}
	FILE_ERROR error(){return fileError;}
};

class _ShmouletteExport FileIterator{
	intptr_t mSystemPtr;
	struct _finddata_t mFindData;
	std::string mPattern;
	std::string mContext;
	FILE_ERROR mError;

public:
	FILE_ERROR error(){return mError;}
	FileIterator(std::string pattern, std::string context="");
	File* getFirst();
	File* getNext();
	std::string getFirstFilename();
	std::string getNextFilename();
};

}

#endif
