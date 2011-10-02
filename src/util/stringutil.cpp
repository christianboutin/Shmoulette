#include <string>
#include <sstream>
using namespace std;
#include "util\stringutil.h"

//namespace Shmoulette{

string toString(float nb){
	stringstream ss;
	ss << nb;
	return ss.str();
}

std::string toString(double nb){
	stringstream ss;
	ss << nb;
	return ss.str();
}


string toString(int nb){
	stringstream ss;
	ss << nb;
	return ss.str();
}

string toString(unsigned int nb){
	stringstream ss;
	ss << nb;
	return ss.str();
}

string toString(unsigned long nb){
	stringstream ss;
	ss << nb;
	return ss.str();
}

string toString(Ogre::Vector3 v){
	stringstream ss;
	ss<<std::setw(10) << std::setfill(' ') << std::fixed << std::setprecision(3) <<v.x<<",";
	ss<<std::setw(10) << std::setfill(' ') << std::fixed << std::setprecision(3)<<v.y<<",";
	ss<<std::setw(10) << std::setfill(' ') << std::fixed << std::setprecision(3)<<v.z;
	return ss.str();

}
string toString(Ogre::ColourValue v){
	stringstream ss;
	ss<<std::setw(2) << std::setfill('0') << std::hex<<(int)(v.r*255);
	ss<<std::setw(2) << std::setfill('0') << std::hex<<(int)(v.g*255);
	ss<<std::setw(2) << std::setfill('0') << std::hex<<(int)(v.b*255);
	ss<<std::setw(2) << std::setfill('0') << std::hex<<(int)(v.a*255);
	return ss.str();
}

string toString(Ogre::Quaternion v){
	Ogre::Vector3 axis;
	Ogre::Radian rad;

	v.ToAngleAxis(rad, axis);
	Ogre::Degree deg = rad;
	
	stringstream ss;
	ss<<std::setw(8) << std::setfill(' ') << std::fixed << std::setprecision(5) <<axis.x<<",";
	ss<<std::setw(8) << std::setfill(' ') << std::fixed << std::setprecision(5)<<axis.y<<",";
	ss<<std::setw(8) << std::setfill(' ') << std::fixed << std::setprecision(5)<<axis.z<<",";
	ss<<std::setw(8) << std::setfill(' ') << std::fixed << std::setprecision(3)<<deg.valueDegrees();
	return ss.str();
}


string toString(Ogre::Radian r){
	float angle = r.valueDegrees();
	return ts(angle);
}

string toString(PyObject* po){
	return PyString_AsString(PyObject_Str(po));
}

Ogre::Radian radianFromString(string s){
	Ogre::Degree d = (Ogre::Degree)atof(s.c_str());
	Ogre::Radian r = d;
	return r;
}

bool boolFromString(string s){
	for (int j=0; j<s.length(); ++j)
	{
		s[j]=tolower(s[j]);
	}
	if (
		(s == "true")||
		(s == "1")||
		(s == "yes")||
		(s == "y")||
		(s == "t")){
			return true;
	}else if (
		(s == "false")||
		(s == "0")||
		(s == "no")||
		(s == "n")||
		(s == "f")){
			return false;
	}
	throw("BoolFromString, not a bool : "+s);

}

Ogre::Vector2 vector2FromString(string s){
	vector<string>v;

	Tokenize(s, v, ",");
	
	Ogre::Vector2 rv;
	if (v.size() < 2){
		rv.x=0;
		rv.y=0;
		return rv;
	}
	rv.x = atof(v[0].c_str());
	rv.y = atof(v[1].c_str());
	
	return rv;
	
}


Ogre::Vector3 vector3FromString(string s){
	vector<string>v;

	Tokenize(s, v, ",");

	
	
	Ogre::Vector3 rv;
	if (v.size() < 1){
		rv.x=0;
	}else{
		rv.x = atof(v[0].c_str());
	}

	if (v.size() < 2){
		rv.y=0;
	}else{
		rv.y = atof(v[1].c_str());
	}
	if (v.size() < 3){
		rv.z=0;
	}else{
		rv.z = atof(v[2].c_str());
	}
	
	return rv;
	
}

Ogre::Quaternion quaternionFromQString(string s){
	vector<string>v;

	Tokenize(s, v, ",");
	
	Ogre::Quaternion rv;
	if (v.size() < 4){
		rv.FromAngleAxis((Ogre::Radian)0.0f, Ogre::Vector3(0,1,0));
		return rv;
	}

	rv.x = atof(v[0].c_str());
	rv.y = atof(v[1].c_str());
	rv.z = atof(v[2].c_str());
	rv.w = atof(v[3].c_str());

	return rv;
}

Ogre::Quaternion quaternionFromEString(string s){
	vector<string>v;

	Tokenize(s, v, ",");
	
	Ogre::Quaternion rv;
	if (v.size() < 3){
		rv.FromAngleAxis((Ogre::Radian)0.0f, Ogre::Vector3(0,1,0));
		return rv;
	}
	
	throw ("This don't work");
	
	return rv;
}


Ogre::Quaternion quaternionFromString(string s){
	vector<string>v;

	Tokenize(s, v, ",");
	
	Ogre::Quaternion rv;
	if (v.size() < 4){
		rv.FromAngleAxis((Ogre::Radian)0.0f, Ogre::Vector3(0,1,0));
		return rv;

	}

	Ogre::Vector3 axis;
	Ogre::Radian rad;
	Ogre::Degree deg;
	deg = atof(v[3].c_str());
	rad = deg;

	axis.x = atof(v[0].c_str());
	axis.y = atof(v[1].c_str());
	axis.z = atof(v[2].c_str());

	rv.FromAngleAxis(rad, axis);
	
	return rv;
	
}


Ogre::ColourValue colourValueFromString(string s){
	if (s.length() < 6){
		throw("Color Value not long enough");
	}
	Ogre::ColourValue rv;
	rv.r = ((float)_httoi(s.substr(0,2).c_str())/255);
	rv.g = ((float)_httoi(s.substr(2,2).c_str())/255);
	rv.b = ((float)_httoi(s.substr(4,2).c_str())/255);
	if (s.length() >=8){
		rv.a = ((float)_httoi(s.substr(6,2).c_str())/255);
	}else{
		rv.a = 1;
	}

	return rv;

}

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
//}