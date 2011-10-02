#include "util/pythonextra.h"

namespace Shmoulette{
	namespace PythonExtra{
		void ValidateTuple(std::string context, PyObject* tuple, std::string types){
			if (PyTuple_Size(tuple) != types.length()){
				throw(
					context+
					" Tuple Size Mismatch "+
					ts((int)PyTuple_Size(tuple))+"/"+ts(types.length()));
			}else{
				for (int i=0;i<types.length();i++){
					ValidateType(context+" (var "+ts(i)+")", PyTuple_GetItem(tuple, i), types.substr(i, 1));
				}

			}

		}
		void ValidateType(std::string context, PyObject* var, std::string type){
			string error = "Python Type Error "+context+"\nExpected : ";
			if (type == "l"){
				if (!PyLong_Check(var)){
					error += "Long";
					throw(error);
				}
			}else if (type == "i"){
				if (!PyInt_Check(var)){
					error += "Int";
					throw(error);
				}
			}else if (type == "s"){
				if (!PyString_Check(var)){
					error += "String";
					throw(error);
				}
			}else if ((type == "f") || (type == "d")){
				if (!PyFloat_Check(var) && !PyInt_Check(var)){ // If you expect a float, an int will do
					error += "Float";
					throw(error);
				}
			}else if (type == "b"){
				if (!PyBool_Check(var)){
					error += "Boolean";
					throw(error);
				}
			}else if (type == "x"){
				// X means that any type is good, so skip it
			}else{
				throw("Unrecognized type : "+type+"\nUse 'x' to allow for any type");
			}

		}
	}
}
