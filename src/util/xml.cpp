#include "stdafx.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <malloc.h>
#include <vector>
#include <list>
#include <string>
using namespace std;
#include "util\stringutil.h"
#include "main.h"

namespace Shmoulette{
	int XML::mTempFileNumber = 0;

#define XML_LOG


	
vector<TI_XML_USAGE>XML::mTiXmlUsage;
list<XmlDocumentCache*>XML::sTiXmlDocumentCache;

void XML::AddInstance(TiXmlDocument* tid){
	if (tid == NULL) return;
	vector<TI_XML_USAGE>::iterator it;
	for (it = mTiXmlUsage.begin();it != mTiXmlUsage.end();it++){
		if ((*it).mTiDoc == tid){
			(*it).mNum++;
			return;
		}
	}
	TI_XML_USAGE tix;
	tix.mTiDoc = tid;
	tix.mNum=1;
	mTiXmlUsage.push_back(tix);
}
void XML::setAttrib(string attrib, string value, bool create){
	TiXmlElement* elem = mTiXmlFirstChild->ToElement();
	if (create || elem->Attribute(attrib)!=NULL){
		elem->SetAttribute(attrib, value);
	}
}
void XML::setAttrib(string elem, string attrib, string value, bool create){
	extractElem(elem).setAttrib(attrib, value, create);
}
void XML::insertElemBefore(string elem){
	TiXmlNode* new_elem = new TiXmlElement(elem);
	mTiXmlFirstChild->Parent()->InsertBeforeChild(mTiXmlFirstChild, *new_elem);
}

void XML::insertElemBefore(string beforeWhat, string elem){
	extractElem(beforeWhat).insertElemBefore(elem);
}

TiXmlElement* XML::addElem(string elem){
	TiXmlElement* new_elem = new TiXmlElement(elem);
	TiXmlElement* rv = mTiXmlFirstChild->InsertEndChild(*new_elem)->ToElement();
	delete new_elem;

	return rv;

}

void XML::RemoveInstance(TiXmlDocument* tid){
	if (tid == NULL) return;
	vector<TI_XML_USAGE>::iterator it;
	for (it = mTiXmlUsage.begin();it != mTiXmlUsage.end();it++){
		if ((*it).mTiDoc == tid){
			(*it).mNum--;
			if ((*it).mNum == 0){
				mTiXmlUsage.erase(it);
			}
			return;
		}
	}
}

void XML::addVariables(list<XML_VAR>var, bool overwrite){
	list<XML_VAR>::iterator it;
	for (it = var.begin();it != var.end();it++){
		addVariable((*it).name, (*it).value, overwrite);
	}
}

string XML::getVariable(string name){
	list<XML_VAR>::iterator it;
	for (it = mVariable.begin();it != mVariable.end();it++){
		if ((*it).name == name){
			return (*it).value;
		}
	}
	throw("Variable : "+name+" not found.");
}

void XML::addVariable(string name, string value, bool overwrite){
	list<XML_VAR>::iterator it;
	for (it = mVariable.begin();it != mVariable.end();it++){
		if ((*it).name == name){
			if (overwrite){
				(*it).value = value;
				return;
			}else{
				return;
			}
		}
	}
	XML_VAR var;
	var.name = name;
	var.value = value;
	mVariable.push_back(var);
}

void XML::parseVariables(){
	TiXmlNode* node;
	
	if (mTiXmlDoc != NULL){
		node = mTiXmlDoc->FirstChild();
	}else{
		node = mTiXmlFirstChild;
	}
	if (node == NULL) return;

	while(node->Type() != TiXmlNode::ELEMENT){
		node = node->NextSibling();
	}

	node = node->FirstChild("variable");

	

#if 0
	if (mTiXmlDoc != NULL){
		node = mTiXmlDoc->FirstChild()->FirstChild("variable");
	}else{
		node = mTiXmlFirstChild->FirstChild("variable");
		if (node != NULL){
			TiXmlElement* elem = node->ToElement();
			string label = elem->Value();
			if (label != "variable"){
				node = node->NextSibling("variable");
			}

		/*if (node->Type() != TiXmlNode::ELEMENT){
			node = node->NextSibling("variable");
		}else{
			TiXmlElement* elem = node->ToElement();
			string label = elem->Value();
			if (label != "variable"){
				node = node->NextSibling("variable");
			}
		}*/
		}
	}
#endif
	
	if (node != NULL){
		do{
			TiXmlElement* elem = node->ToElement();
			const char* name = elem->Attribute("name");
			const char* value = elem->Attribute("value");
			if (name == NULL || value == NULL){
				throw("Variable badly formatted in XML"+(string)elem->FirstAttribute()->Value());
			}

			addVariable(name, value);
		}while((node = node->NextSibling("variable")) != NULL);
	}
/*		if (digger_list[current_digger].label == "##anything##" || digger_list[current_digger].label == ""){
			node = node->FirstChild();
		}else{
			node = node->FirstChild(digger_list[current_digger].label);



	XMLIterator it(this);
	it.setElemName("variable");
	it.gotoZero();
	while(it.gotoNext() == true){
		string label;
		string value;
		label = it.getElem().getString("label");
		value = it.getElem().getString("value");
		addVariable(label, value);
	}*/
}

XML::XML(string filename, string context){
	list<XmlDocumentCache*>::iterator cit;
	for (cit = sTiXmlDocumentCache.begin();cit != sTiXmlDocumentCache.end();cit++){
		if ((*cit)->filename == filename && (*cit)->context == context){
			setContent((*cit)->ptr);
			AddInstance((*cit)->ptr);
			return;
		}
	}

	File f(filename, context);
	if (f.error() != FILE_OK){
		error = XML_FILE_NOT_FOUND;
		throw("XML Not Found : "+filename+" in context "+context);
	}else{
		error = XML_OK;
		mTiXmlDoc = new TiXmlDocument();
		AddInstance(mTiXmlDoc);
		setContent(f.getStrContent());
		XmlDocumentCache* cache = new XmlDocumentCache();
		cache->filename = filename;
		cache->context = context;
		cache->ptr = mTiXmlDoc;
		sTiXmlDocumentCache.push_back(cache);
		AddInstance(mTiXmlDoc);
	}
}

XML::XML(){
	error = XML_OK;
	mTiXmlDoc = new TiXmlDocument();
	AddInstance(mTiXmlDoc);
//	raw_content = "";
}
XML::XML(string content){
	error = XML_OK;
	mTiXmlDoc = new TiXmlDocument();
	AddInstance(mTiXmlDoc);
	setContent(content);
	
}


XML::XML(TiXmlNode* tid){
	error = XML_OK;
	mTiXmlDoc = NULL;
	mTiXmlFirstChild = tid;
	parseVariables();
}

XML::XML(const XML& xml){
	mTiXmlDoc = xml.mTiXmlDoc;
	//list<XML_VAR> varlist;
	//varlist = xml.getVariables();
	addVariables(xml.mVariable);
	AddInstance(mTiXmlDoc);
	error = xml.error;
	mTiXmlFirstChild = xml.mTiXmlFirstChild;
	parseVariables();

}

XML::~XML(){
	RemoveInstance(mTiXmlDoc);
}

string XML::getContent(){
	if (mTiXmlDoc != NULL){
		TiXmlPrinter printer;
		mTiXmlDoc->Accept( &printer );
		return (printer.CStr());
	}else{
		TiXmlPrinter printer;
		mTiXmlFirstChild->Accept( &printer );
		return (printer.CStr());
	}
}

void XML::setContent(TiXmlDocument* tid){
	mTiXmlDoc = tid;
	mTiXmlFirstChild = mTiXmlDoc->FirstChild();
	parseVariables();
}

void XML::setContent(TiXmlNode* tin){
	error = XML_OK;
	mTiXmlDoc = NULL;
	mTiXmlFirstChild = tin;
	parseVariables();
}


void XML::setContent(string content_ptr){
	if (mTiXmlDoc != NULL){
		mTiXmlDoc->Parse(content_ptr.c_str());
		if (mTiXmlDoc->Error() && content_ptr!=""){
			Util::Log(content_ptr);
			throw("XML parsing error "+content_ptr.substr(0,360)+" [...]\n"+mTiXmlDoc->ErrorDesc());
		}
		mTiXmlFirstChild = mTiXmlDoc->FirstChild();
		parseVariables();
	}else{
		Util::Log(content_ptr);
		throw("Setting content on derived nodes is forbidden");
	}
}

string XML::getString(char* elem, char*att){
	XML xml2 = extractElem(elem);
	string rv = xml2.getString(att);

	error = xml2.getError();
	return rv;
}

string XML::getString(string att){
	TiXmlNode* tin = mTiXmlFirstChild;
	if (tin == NULL){
		error = XML_ELEMENT_NOT_FOUND;
		return "XML_ELEMENT_NOT_FOUND";
	}
	TiXmlElement* tie = tin->ToElement();
	if (tie == NULL){
		error = XML_ELEMENT_EMPTY;
		return "XML_ELEMENT_EMPTY";
	}

	
	const char* c = tie->Attribute(att.c_str());
	if (c == NULL){
		error = XML_ATTRIBUTE_NOT_FOUND;
		return "XML_ATTRIBUTE_NOT_FOUND";
	}
	string rv;
	string s = c;
	bool hasVariables = false;
	while (s.find("~") != string::npos){
		hasVariables = true;
		string::size_type s1 = s.find("~");
		if (s1 != 0){
			rv += s.substr(0,s1);
		}
		string sub = s.substr(s1+1);
		string::size_type s2 = sub.find("~");
		string var = s.substr(s1+1, s2);
		rv += getVariable(var);
		s = s.substr(s1+s2+2);
	}
	rv += s;
	if (hasVariables == true && (rv.find("+") != string::npos || rv.find("-") != string::npos)){
		int j=0;
		float result=0;
		int operation = 1;

		for (int i=0;i<rv.length();i++){
			if (rv[i]=='+' || rv[i]=='-'){
				float value = atof(rv.substr(j,i-j).c_str());
				result = result + (operation*value);
				operation = rv[i]=='+'?1:-1;
				j = i+1;
			}
		}
		float value = atof(rv.substr(j,(rv.length())-j).c_str());
		result = result + (operation*value);
		rv = ts(result);
	}


	error = XML_OK;
	return rv;
}


string XML::getString(string elem, string att){
	return (string)getString((char*)elem.c_str(), (char*)att.c_str());
}


int XML::getInt(char* elem, char*att){
	XML xml2 = extractElem(elem);
	int rv = xml2.getInt(att);
	error = xml2.getError();
	return rv;
}

int XML::getInt(char*att){
	string tmp = getString(att);
	if (getError() != XML_OK) return INT_MAX;
	return atoi(tmp.c_str());
}


float XML::getFloat(string att){
	string tmp = getString(att);
	if (error != XML_OK){
		return XML_NO_VALUE;
	}
	string s = tmp;
	return (float)atof(tmp.c_str());
}


float XML::getFloat(char* elem, char*att){
	XML xml2 = extractElem(elem);
	float rv = xml2.getFloat(att);
	error = xml2.getError();
	return rv;
}

double XML::getDouble(char*att){
	string tmp = getString(att);
	if (error != XML_OK){
		return XML_NO_VALUE;
	}
	return (float)atof(tmp.c_str());
}


double XML::getDouble(char* elem, char*att){
	XML xml2 = extractElem(elem);
	return xml2.getDouble(att);
}

void XML::createElem(string parent, string elemName){
	XML xml2 = extractElem(parent);
	xml2.addElem(elemName);

}
// Returns a new XML that contains the requested extracted elment

typedef struct TAG_XML_DIGGER{
	int index;
	string label;
}XML_DIGGER;

TiXmlNode* XML::getNode(string s){
	//TiXmlPrinter printer;

	vector<XML_DIGGER>digger_list;
	vector<string>tokenized;
	Tokenize(s, tokenized, ">");
	vector<string>::iterator it;
	for (it = tokenized.begin();it != tokenized.end();it++){
		vector<string>token;
		Tokenize((*it), token, "<");
		XML_DIGGER digger;
		digger.label = token[0];
		if (token.size() > 1){
			digger.index = atoi(token[1].c_str());
		}else{
			digger.index = 1;
		}
		digger_list.push_back(digger);
	}


	/*int index = 0;
	while(index <= s.length()){
		XML_DIGGER digger;
		while(index < s.length() && s[index] != '>' && s[index] != '<'){
			digger.label.push_back(s[index]);
			index++;
		};
		if (s[index] == '<'){
			index++;
			string index_str;
			while(index < s.length() && s[index] != '>'){
				index_str.push_back(s[index]);
				index++;
			}
			digger.index = atoi(index_str.c_str());
		}else{
			digger.index = 1;
		}
		digger_list.push_back(digger);
		index++;
	}*/

	int current_digger=0;
	TiXmlNode* node;
	if (mTiXmlDoc != NULL){
		node = mTiXmlDoc;
		if (digger_list[current_digger].label == "##anything##" || digger_list[current_digger].label == ""){
			node = node->FirstChild();
		}else{
			node = node->FirstChild(digger_list[current_digger].label);
		}
	}else{
		node = mTiXmlFirstChild;
		if (digger_list[current_digger].label != "##anything##" &&
			digger_list[current_digger].label != node->Value()){
			node = node->NextSibling(digger_list[current_digger].label);
		}
	}

	if (node != NULL){
		while(current_digger < digger_list.size()){
			//TiXmlPrinter tmpa;
			//node->Accept( &tmpa);

			if (current_digger != 0){
				if (digger_list[current_digger].label == "##anything##" || digger_list[current_digger].label == ""){
					node = node->FirstChild();
				}else{
					node = node->FirstChild(digger_list[current_digger].label);
				}
			}

			if (node == NULL)
				break;
			else{
				//TiXmlPrinter tmpb;
				//node->Accept( &tmpb);
			}

			for (int index_digger = 1;index_digger < digger_list[current_digger].index;index_digger++){
				if (digger_list[current_digger].label == "##anything##" || digger_list[current_digger].label == ""){
					node = node->NextSibling();
				}else{
					node = node->NextSibling(digger_list[current_digger].label);
				}
				if (node == NULL) break;
			}
			current_digger++;
		}
	}
	return node;
}

XML XML::extractElem(string s){
	TiXmlNode* node = getNode(s);
	if (node == NULL){
		error = XML_ELEMENT_NOT_FOUND;
		XML erv("");
		return erv;
	}
	//node->Accept( &printer );
	
	XML rv(node); //printer.CStr()
	rv.addVariables(mVariable);
	error = XML_OK;
	return rv;
}

void XML::destroyElem(string elem){
	XML xml = extractElem(elem);
	if (getError() == XML_OK){
		TiXmlNode* child = xml.mTiXmlFirstChild;
		TiXmlNode* parent = child->Parent();
		parent->RemoveChild(child);
	}
}

XML XML::extractElem(char* c){
	string s = c;
	return extractElem(s);
}

void XML::append(string branch, XML* xml){
	TiXmlDocument ti;
	string content = xml->getContent();
	ti.Parse(content.c_str());
	TiXmlNode* node = ti.FirstChild();
	if (node) mTiXmlDoc->FirstChildElement(branch)->InsertEndChild(*node);
}

	XMLIterator::XMLIterator(XML* xml){
		mCurrent = 0;
		mXml = xml;
		mElemName="";
		//mElemContent = NULL;
	};
	void XMLIterator::setElemName(string name, bool iterateAllSiblings){
		Util::Log("XML Iterator started using : "+name, 6);
		mElemName = name;
		mFirstNode = mXml->getNode(name);
		mCurrentNode = mFirstNode;
		//string s = name.substr(name.rfind(">"));
		//mNodeName = s.substr(0,s.find("<"));
			//mNodeName = s;
		if (iterateAllSiblings == true){
			mNodeName = "";
		}else{
			if (mFirstNode != NULL){
				mNodeName = mFirstNode->Value();
			}
		}

	}
	void XMLIterator::gotoZero(){
		mCurrent = 0;
		mCurrentNode = mFirstNode;
	}
	bool XMLIterator::gotoNext(){
		if (mCurrentNode==NULL){
			return false;
		}else{
			mElemContent.setContent(mCurrentNode);
			if (mNodeName == ""){
				mCurrentNode = mCurrentNode->NextSiblingElement();
			}else{
				mCurrentNode = mCurrentNode->NextSiblingElement(mNodeName);
			}
			return true;
		}
		/*mCurrent++;
		mElemContent = mXml->extractElem(mElemName+"<"+ts(mCurrent));
		//mElemContent = mXml->extractElem(mElemName, mCurrent);
		if (mXml->getError() == XML_OK){
			Util::Log("Extracting elem : "+ts(mCurrent), 6);
			Util::Log("Elem has content : "+mElemContent.getContent(),7);
			return true;
		}else{
			Util::Log("XML Element Not Found "+mElemName+" ("+ts(mCurrent)+")", 6);
			return false;
		}*/
	}
	void XMLIterator::destroyElem(){
		mXml->destroyElem(mElemName+"<"+ts(mCurrent));
	}
	XML XMLIterator::getElem(){ return mElemContent; }

}


