#include "App.h"

#ifdef WINDOWS
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
    main(0, nullptr);
}

#endif

int main(int argc, char* argv[])
{
    App app;
    app.run();

    return 0;
}
