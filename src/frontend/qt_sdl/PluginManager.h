//
// Created by Fisher on 10/25/24.
//



#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "dylib.hpp"
#include <QMenu>
#include <QPainter>
#include <QList>


#include "NDS.h"

class ScreenLayout;
class ScreenPanel;
class EmuInstance;
class MainWindow;



class Plugin : public QWidget
{
    Q_OBJECT

    public:
    void toggleEnabled(bool checked);
    void registerSignal(QMenu*, MainWindow*);

    void (*frame)();
    void (*instr)();
    void (*onKeyPress)(QKeyEvent*);
    void (*onKeyRelease)(QKeyEvent*);
    void (*draw)(int, QPainter*);
    void (*onEnable)();
    void (*onDisable)();
    void (*clickTopScreen)(int, int);
    void (*clickBottomScreen)(int, int);
    void (*ARM9step)(unsigned int);
    void (*ARM9read)(unsigned int, unsigned char);
    void (*ARM9write)(unsigned int, unsigned char, void*);
    void (*ARM7step)(unsigned int);


    Plugin(EmuInstance*, const char*, const char*);
    bool enabled;
    dylib* lib;
    QAction* checkAction;

    char* name;
};


class PluginManager  {
public:
    explicit PluginManager(EmuInstance*);
    void ndsInit();
    EmuInstance* emuInstance;

    void clickTopScreen(int x, int y);
    void clickBottomScreen(int x, int y);
    void onKeyPress(QKeyEvent* event);
    void onKeyRelease(QKeyEvent* event);

    void frame();
    void draw(int, QPainter*);

    QList<Plugin*> plugins = {};
};

void ARM9step(void*, unsigned int);
void ARM9read(void*, unsigned int, unsigned char);
void ARM9write(void*, unsigned int, unsigned char, void*);
void ARM7step(void*, unsigned int);
void ARM7read(void*, unsigned int, unsigned char);
void ARM7write(void*, unsigned int, unsigned char, void*);

#endif //PLUGINMANAGER_H
