#include <QApplication>
#include <ui_widget_menu.h>

#include "MainWindow.h"
#include "LevelEditorWidget.h"
#include "widget_choose_level.h"
#include "widget_level_loading.h"
#include "widget_building_list.h"
#include "widget_menu.h"
#include "widget_ingame.h"
#include "widget_pause_menu.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(1024,768);
    w.show();

    //用于调试主菜单的代码
    // widget_menu menu_widget;
    // menu_widget.setWindowTitle("Level3 Main Menu (Menu Debug)");
    // menu_widget.resize(800, 600); // 调整为你认为合适的大小
    // menu_widget.show();


    //用于调试关卡选择界面的代码
    // widget_choose_level choose_level_widget;
    // choose_level_widget.setWindowTitle("Level3 Choose Level (Choose Level Debug)");
    // choose_level_widget.resize(1024, 768); // 调整为你认为合适的大小
    // choose_level_widget.show();

    // --- 用于调试编辑器的新代码 ---
    // LevelEditorWidget editor;
    // editor.setWindowTitle("Level3 Editor (Level Editor Debug)");
    // editor.resize(1024, 768); // 调整为你认为合适的大小
    // editor.show();
    // --- 结束 ---

    //关卡加载界面
    // widget_level_loading level_loading_widget(0);
    // level_loading_widget.show();

    //建造列表界面
    // QVector<QString> n({"InspirationBulb","KnowledgeTree","FishingCatPillow","LiveCoffee"});
    // QVector<QString> t({":/towers/resources/towers/level1/InspirationBulb.png",":/towers/resources/towers/level1/KnowledgeTree.png",":/towers/resources/towers/level1/FishingCatPillow.png",":/towers/resources/towers/level1/LiveCoffee.png"});
    // QVector<QString> p({"100","200","120","80"});
    // widget_building_list build_widget(0,100,0,n,t,p);
    // build_widget.show();

    //暂停菜单栏
    widget_pause_menu pause_menu_widget;
    pause_menu_widget.show();

    //widget_ingame
    // widget_ingame ingame(0);
    // ingame.show();

    return a.exec();
}
