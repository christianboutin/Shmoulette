// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___XML_H___
#define ___DEF___XML_H___

//#define MAX_PATH 4096
#include "util.h"
#include "stringutil.h"
#include "..\..\..\tinyxml\tinyxml.h"
#include "export.h"

namespace Shmoulette{

//#define USING_TINY_XML

typedef enum TAG_XML_ERROR{
	XML_OK,
	XML_ELEMENT_NOT_FOUND,
	XML_ATTRIBUTE_NOT_FOUND,
	XML_FORMAT_ERROR,
	XML_ELEMENT_EMPTY,
	XML_ELEM_NOT_CLOSED,
	XML_FILE_NOT_FOUND
}XML_ERR;

typedef struct TAG_TI_XML_USAGE{
	TiXmlDocument* mTiDoc;
	int mNum;
}TI_XML_USAGE;

typedef struct TAG_XML_VAR{
	string name;
	string value;
}XML_VAR;

#define XML_NO_VALUE FLT_MAX

using namespace std;

class _ShmouletteExport XmlDocumentCache{
	public:
	string filename;
	string context;
	TiXmlDocument* ptr;
};

class _ShmouletteExport XML{
	//char* raw_content;
	TiXmlDocument* mTiXmlDoc;
	TiXmlNode* mTiXmlFirstChild;
	//string raw_content;
	static int mTempFileNumber;
	bool doNotDeleteTiXml;
	static vector<TI_XML_USAGE>mTiXmlUsage;
	static void AddInstance(TiXmlDocument* tid);
	static void RemoveInstance(TiXmlDocument* tid);

	static list<XmlDocumentCache*> sTiXmlDocumentCache;

	list<XML_VAR>mVariable;
	/*list<XML_VAR> getVariables(){
		return mVariable;
	}*/

	double resolveEquation(string s);
	void parseVariables();
	XML_ERR error;
	public:
		TiXmlDocument* getTiXmlDoc(){return mTiXmlDoc;} // Use with care.
		
		XML();
		XML(string content);
		XML(TiXmlNode* tid);
		XML( const XML &xml);
		XML(string filename, string context);

		string getVariable(string label);
		void addVariable(string label, string content, bool overwrite=false);
		void addVariables(list<XML_VAR>var, bool overwrite=false);

		void setAttrib(string attrib, string value, bool create=true);
		void setAttrib(string elem, string attrib, string value, bool create=true);
		~XML();
		void setContent(string content_ptr);
		void setContent(TiXmlDocument* tid);
		void setContent(TiXmlNode* tin);
		string getContent();
		std::string getString(char* elem, char* att);
		std::string getString(std::string elem, string att);
		std::string getString(string att);
		int getInt(char* elem, char* att);
		int getInt(char* att);
		float getFloat(char* elem, char* att);
		float getFloat(string att);
		double getDouble(char* elem, char* att);
		double getDouble(char* att);
		XML_ERR getError(){return error;}
		XML extractElem(char* elem);
		XML extractElem(string elem);
		TiXmlNode* getNode(string elem);
		void destroyElem(string elem);

		void append(string branch, XML* xml);

		void insertElemBefore(string elemName);
		TiXmlElement* addElem(string elemName);
		void createElem(string parent, string elemName);
		void insertElemBefore(string beforeWhat, string elemName);
		//XML XML::extractElem(string elem, int nb);
		string getFirstChildName(){
			TiXmlElement* tie = mTiXmlFirstChild->ToElement();//mTiXmlDoc->FirstChildElement();
			if (tie == NULL){
				error = XML_ELEMENT_NOT_FOUND;
				return "XML_ELEMENT_NOT_FOUND";
			}
			return tie->Value();
		}

};


class _ShmouletteExport XMLIterator{
	XML* mXml;
	int mCurrent;
	string mElemName;
	XML mElemContent;
	TiXmlNode* mFirstNode;
	TiXmlNode* mCurrentNode;
	string mNodeName;

	public:
		XMLIterator(XML* xml);
		void setElemName(string name, bool iterateAllSiblings = false);
		void gotoZero();
		bool gotoNext();
		void destroyElem();
		XML getElem();

};

}

#endif
