// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___PYTHONEXTRA_H___
#define ___DEF___PYTHONEXTRA_H___

#include "python.h"
#include "string"
using namespace std;
#include "util/stringutil.h"

namespace Shmoulette{
	namespace PythonExtra{
		void ValidateTuple(std::string context, PyObject* tuple, std::string types);
		void ValidateType(std::string context, PyObject* var, std::string type);
		
	}
}

#endif
