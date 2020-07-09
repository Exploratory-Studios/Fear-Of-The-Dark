#ifndef APP_H
#define APP_H

#include <IMainGame.h>
#include <ScreenList.h>

class MainScreen;

class App : public GLEngine::IMainGame
{
    public:
        App();
        virtual ~App();

        virtual void onInit() override;
        virtual void addScreens() override;
        virtual void onExit() override;

    private:
        std::unique_ptr<MainScreen> m_mainScreen;
};

#endif // APP_H
