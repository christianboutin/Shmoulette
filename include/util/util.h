// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___UTIL_H___
#define ___DEF___UTIL_H___

#include <string>
using namespace std;
namespace Util{

	void Log(string out, int level=5);
	void LogRaiseLevel();
	void LogLowerLevel();

	void deprecated_Profile(string marker="", int level=5);
	bool aboutEqual(float a, float b);
	int stringNbLines(string str);

}





#endif
