//
// Created by Fisher on 10/25/24.
//


#include "PluginManager.h"

#include <EmuInstance.h>
#include <QDir>
#include <QDir>
#include <QMenu>
#include <QPainter>
#include <QDebug>
#include <X11/Xlib.h>
#include <QList>

#include "dylib.hpp"
using namespace melonDS;
using namespace melonDS::Platform;
PluginManager::PluginManager(EmuInstance* emuInst)
{
    Log(LogLevel::Info, "init 1\n");
    emuInstance = emuInst;

    QDir dir = QCoreApplication::applicationDirPath() + "/plugins/";
    if (!dir.exists()) {
        Log(LogLevel::Info, "creating plugins dir\n");
        if (!dir.mkpath(".")) Log(LogLevel::Info, "creating plugins dir failed\n");;
    }
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    for (int i = 0; i < files.count(); i++)
    {
        Log(LogLevel::Info, "loading file: %s\n", files.at(i).toStdString().c_str());
        plugins.append(new Plugin(emuInstance, dir.absolutePath().toStdString().c_str(), files.at(i).toStdString().c_str()));
    }
}

void PluginManager::ndsInit() {
    emuInstance->getNDS()->ARM9.PM9step = &ARM9step;
    emuInstance->getNDS()->ARM9.PM_ptr = this;

    emuInstance->getNDS()->ARM7.PM7step = &ARM7step;
    emuInstance->getNDS()->ARM7.PM_ptr = this;

    Log(LogLevel::Info, "inited nds: %p\n", emuInstance->getNDS());
    Log(LogLevel::Info, "inited nds arm9: %p\n", &(emuInstance->getNDS()->ARM9));
}


void PluginManager::clickTopScreen(int x, int y)
{
    for (int i = 0; i < plugins.size(); ++i) {
        Plugin* plugin = plugins.at(i);
        if (plugin->enabled) plugin->clickTopScreen(x, y);
    }
}

void PluginManager::clickBottomScreen(int x, int y)
{
    for (int i = 0; i < plugins.size(); ++i) {
        Plugin* plugin = plugins.at(i);
        if (plugin->enabled) plugin->clickBottomScreen(x, y);
    }
}



void PluginManager::frame(){
    for (int i = 0; i < plugins.size(); ++i) {
        Plugin* plugin = plugins.at(i);
        if (plugin->enabled) plugin->frame();
    }
}

void PluginManager::draw(int screenIdx, QPainter* painter){
    for (int i = 0; i < plugins.size(); ++i) {
        Plugin* plugin = plugins.at(i);
        if (plugin->enabled) plugin->draw(screenIdx, painter);
    }
}



void ARM9step(void* self, unsigned int addr){
    PluginManager* pm = (PluginManager*)self;
    for (int i = 0; i < pm->plugins.size(); ++i) {
        Plugin* plugin = pm->plugins.at(i);
        if (plugin->enabled) plugin->ARM9step(addr);
    }
}

void ARM7step(void* self, unsigned int addr){
    PluginManager* pm = (PluginManager*)self;
    for (int i = 0; i < pm->plugins.size(); ++i) {
        Plugin* plugin = pm->plugins.at(i);
        if (plugin->enabled) plugin->ARM7step(addr);
    }
}



Plugin::Plugin(EmuInstance* emuInstance, const char* dir, const char* fileName)
{
    enabled = false;
    lib = new dylib(dir, fileName, dylib::no_filename_decorations);
    name = lib->get_variable<char*>("name");
    lib->get_variable<EmuInstance*>("emu") = emuInstance;
    auto init = lib->get_function<void()>("init");
    init();


    frame = lib->get_function<void()>("frame");
    draw = lib->get_function<void(int, QPainter*)>("draw");
    onEnable = lib->get_function<void()>("onEnable");
    onDisable = lib->get_function<void()>("onDisable");
    clickTopScreen = lib->get_function<void(int, int)>("clickTopScreen");
    clickBottomScreen = lib->get_function<void(int, int)>("clickBottomScreen");
    ARM9step = lib->get_function<void(unsigned int)>("ARM9step");
    ARM7step = lib->get_function<void(unsigned int)>("ARM7step");
}

void Plugin::registerSignal(QMenu* menu, MainWindow* mainWindow)
{
    checkAction = menu->addAction(name);
    checkAction->setCheckable(true);
}

void Plugin::toggleEnabled(bool checked)
{
    if (checked)
    {
        enabled = true;
        Log(LogLevel::Info, "enabling plugin\n");
        onEnable();
    }
    else
    {
        enabled = false;
        onDisable();
    }
}
