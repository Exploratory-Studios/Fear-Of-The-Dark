#define BOOST_NO_CXX11_SCOPED_ENUMS
#define BOOST_NO_SCOPED_ENUMS

#include <IMainGame.h>

#include "App.h"

#ifdef WINDOWS // Windows needs a very particular main function for some reason. Just let it do its thing
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
	main(0, nullptr);
}

#endif

int main(int argc, char** argv) {
	App app;
	app.run();
	return 0;
}
