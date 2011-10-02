#include "main.h"
#include "game.h"


GameCore game;
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ){
	remove("log.txt");
	AllocConsole();
	Util::Log("START");
    try {
        game.go();
	} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
	} catch( string s){
		char buffer[256];
		GetCurrentDirectoryA(256,buffer);
		string s2 = s+"\nCurrent Directory : "+buffer;
		MessageBox( NULL, s2.c_str(), "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	} catch( char* s){
		MessageBox( NULL, (LPCSTR)s, "Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
	Util::Log("[[[EXITING]]]");

	FreeConsole();
	Util::Log("[[[POST_CONSOLE]]]");

    return 0;
}
