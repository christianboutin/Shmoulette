// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___LOCALSTR_H___
#define ___DEF___LOCALSTR_H___

#include "export.h"

namespace Util{

	class _ShmouletteExport LocalStr{
		private:
			LocalStr();
			static LocalStr* mLocalStr;
		public:
			static LocalStr* getSingleton(){
				if (mLocalStr == NULL){
					mLocalStr = new LocalStr();
				}
				return mLocalStr;
			}
			string getString(string id, string entry="");
	};



}



#endif
