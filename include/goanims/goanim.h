// Created with hdefpair (C) 2006 Christian Boutin (http://christianboutin.com)
#ifndef ___DEF___GOANIM_H___
#define ___DEF___GOANIM_H___

namespace Shmoulette{

	class GoAnimParams:public GameElementParams{
	public:
		float mTime;
	};

	namespace GoAnim{
		class _ShmouletteExport GoAnim:public GameElement{
			typedef Shmoulette::GameElement Parent;
			protected:
			GameObject* mGo;
			float mTime;

			public:
				virtual void init(GameObject* go, GoAnimParams& params);
				virtual void init(GameObject* go, XML* xml);
				virtual void init(GameObject* go, PyObject* pyObject);
				//virtual void init(GameObject* go);
				virtual void update(float time);
				virtual bool update(double timeDelta, double parentTime);
				virtual void setTime(float time){mTime = time;}

		};
	}

	class _ShmouletteExport GoAnimFactory{
	private:
		GoAnimFactory();
		static class GoAnimFactory* mSingleton;
	public:
		static GoAnimFactory* getSingleton(){
			if (mSingleton == NULL){
				mSingleton = new GoAnimFactory();
			}
			return mSingleton;
		}
		GoAnim::GoAnim* createGoAnim(GameObject* go, string type, GoAnimParams& params);
		GoAnim::GoAnim* createGoAnim(GameObject* go, XML* xml);
		GoAnim::GoAnim* createGoAnim(GameObject* go, string type, double time, PyObject* params);

	};
}
#endif