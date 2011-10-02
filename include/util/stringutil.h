// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___STRINGUTIL_H___
#define ___DEF___STRINGUTIL_H___

#include "export.h"
#include "ogre.h"
#include "httoi.h"
#include "python.h"

#define toStr toString
#define tS toString
#define ts toString

_ShmouletteExport std::string toString(bool nb);
_ShmouletteExport std::string toString(float nb);
_ShmouletteExport std::string toString(double nb);
_ShmouletteExport std::string toString(int nb);
_ShmouletteExport std::string toString(unsigned int nb);
_ShmouletteExport std::string toString(unsigned long nb);
_ShmouletteExport std::string toString(Ogre::Vector3 v);
_ShmouletteExport std::string toString(Ogre::ColourValue v);
_ShmouletteExport std::string toString(Ogre::Quaternion v);
_ShmouletteExport Ogre::Vector3 vector3FromString(string s);
_ShmouletteExport Ogre::ColourValue colourValueFromString(string s);
_ShmouletteExport Ogre::Quaternion quaternionFromString(string s);
_ShmouletteExport Ogre::Quaternion quaternionFromQString(string s);
_ShmouletteExport void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ");

_ShmouletteExport string toString(Ogre::Radian r);
_ShmouletteExport Ogre::Radian radianFromString(string s);
_ShmouletteExport Ogre::Vector2 vector2FromString(string s);
_ShmouletteExport bool boolFromString(string s);
_ShmouletteExport string toString(PyObject* po);

#endif
