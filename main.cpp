#include <QApplication>
#include "MainWindow.h"
#include "LevelEditorWidget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    // w.show();

    // --- 用于调试编辑器的新代码 ---
    // LevelEditorWidget editor;
    //  editor.setWindowTitle("Level3 Editor (Level Editor Debug)");
    //  editor.resize(1024, 768); // 调整为你认为合适的大小
    //  editor.show();
    // --- 结束 ---

    return a.exec();
}
