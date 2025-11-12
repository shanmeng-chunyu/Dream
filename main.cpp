#include <QApplication>
#include <ui_widget_menu.h>

#include "MainWindow.h"
#include "LevelEditorWidget.h"
#include "widget_choose_level.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(1024,768);
    w.show();

    //用于调试主菜单的代码
    // Ui::widget_menu menu_ui;
    // QWidget menu_widget;
    // menu_ui.setupUi(&menu_widget);
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

    return a.exec();
}
