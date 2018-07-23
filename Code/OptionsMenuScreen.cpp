#include "OptionsMenuScreen.h"

OptionsMenuScreen::OptionsMenuScreen(GLEngine::Window* window) : m_window(window)
{
    //ctor
}

OptionsMenuScreen::~OptionsMenuScreen()
{
    //dtor
}

int OptionsMenuScreen::getNextScreenIndex() const {
    return m_screenIndex;
}

int OptionsMenuScreen::getPreviousScreenIndex() const {
    return m_screenIndex;
}

void OptionsMenuScreen::build() {

}

void OptionsMenuScreen::destroy() {

}

void OptionsMenuScreen::onEntry() {

}

void OptionsMenuScreen::onExit() {

}

void OptionsMenuScreen::update() {

}

void OptionsMenuScreen::draw() {

}
