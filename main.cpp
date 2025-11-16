#include <QApplication>
#include <QTimer>
#include <QStringList>

#include "LevelEditorWidget.h"
#include "MainWindow.h"
#include "widget_choose_level.h"
#include "widget_level_loading.h"
#include "widget_building_list.h"
#include "widget_menu.h"
#include "widget_ingame.h"
#include "widget_pause_menu.h"
#include "widget_reference_book.h" // 确保包含了图鉴的头文件

namespace
{
    // ... (commandLineLevelCandidate 函数保持不变) ...
    QString commandLineLevelCandidate()
    {
        const QStringList args = QCoreApplication::arguments();
        for (int i = 1; i < args.size(); ++i)
        {
            const QString &arg = args.at(i);
            if (arg.startsWith(QStringLiteral("--level="), Qt::CaseInsensitive))
            {
                return arg.section('=', 1).trimmed();
            }
            if (arg.compare(QStringLiteral("--level"), Qt::CaseInsensitive) == 0 && i + 1 < args.size())
            {
                return args.at(i + 1).trimmed();
            }
        }
        return {};
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // (调试用的关卡编辑器保持注释状态)
    /*
    LevelEditorWidget editor;
    editor.resize(1024, 768);
    editor.setWindowTitle("关卡编辑器 [调试模式]");
    editor.show();
    */

    // --- 窗口实例化 ---
    MainWindow w; // 游戏主窗口
    w.resize(1024, 768);

    widget_choose_level levelChooser; // 关卡选择
    levelChooser.setWindowTitle(QStringLiteral("Choose Level"));
    levelChooser.resize(1024, 768);

    widget_menu menu; // 主菜单
    menu.resize(1024, 768);

    // 步骤 1：(新增) 实例化图鉴窗口
    widget_reference_book refBook;
    refBook.resize(1024, 768);

    // --- 辅助 Lambda (保持不变) ---
    auto focusWindow = [](QWidget *widget)
    {
        if (!widget)
        {
            return;
        }
        widget->show();
        widget->raise();
        widget->activateWindow();
    };

    auto startLevel = [&](const QString &candidatePath)
    {
        // ... (内容保持不变) ...
        if (!w.startLevelFromSource(candidatePath, true))
        {
            focusWindow(&levelChooser);
            return;
        }

        levelChooser.hide();
        if (!w.isVisible())
        {
            w.show();
        }
        w.raise();
        w.activateWindow();
    };

    // --- 信号槽连接 ---

    // (保留) 从关卡选择器 -> 开始游戏
    QObject::connect(&levelChooser, &widget_choose_level::level1, &a, [&]()
                     { startLevel(QStringLiteral("levels/level1.json")); });
    QObject::connect(&levelChooser, &widget_choose_level::level2, &a, [&]()
                     { startLevel(QStringLiteral("levels/level2.json")); });
    QObject::connect(&levelChooser, &widget_choose_level::level3, &a, [&]()
                     { startLevel(QStringLiteral("levels/level3.json")); });

    // (保留) 从关卡选择器 -> 返回主菜单
    QObject::connect(&levelChooser, &widget_choose_level::back, &a, [&]()
                     {
                         levelChooser.hide();
                         focusWindow(&menu);
                     });

    // (保留) 从游戏窗口 -> 返回关卡选择器
    QObject::connect(&w, &MainWindow::levelSelectionRequested, &levelChooser, [&]()
                     { focusWindow(&levelChooser); });

    // <--- 新增连接：从游戏窗口 -> 返回主菜单 --->
    QObject::connect(&w, &MainWindow::mainMenuRequested, &a, [&]() {
        // 'w' 已经在 onReturnToMainMenu 中隐藏了
        focusWindow(&menu);
    });

    // --- 主菜单的三个按钮连接 ---

    // 1. (保留) 主菜单 -> 关卡选择器
    QObject::connect(&menu, &widget_menu::choose_level, &a, [&]()
                     {
                         menu.hide();
                         focusWindow(&levelChooser);
                     });

    // 2. 步骤 2：(新增) 主菜单 -> 图鉴
    QObject::connect(&menu, &widget_menu::reference_book, &a, [&]()
                     {
                         menu.hide();
                         focusWindow(&refBook);
                     });

    // 3. (保留) 主菜单 -> 退出游戏
    QObject::connect(&menu, &widget_menu::exit_game, &a, &QApplication::quit);


    // --- 图鉴的返回按钮连接 ---

    // 步骤 3：(新增) 从图鉴 -> 返回主菜单
    QObject::connect(&refBook, &widget_reference_book::back_to_menu, &a, [&]()
                     {
                         refBook.hide();
                         focusWindow(&menu);
                     });


    // --- 启动逻辑 (保持不变) ---
    const QString cmdCandidate = commandLineLevelCandidate();
    const QString envCandidate = QString::fromLocal8Bit(qgetenv("DREAM_LEVEL_PATH")).trimmed();
    const bool hasExplicitLevel = !cmdCandidate.isEmpty() || !envCandidate.isEmpty();

    if (hasExplicitLevel)
    {
        w.show();
    }
    else
    {
        // 默认启动主菜单
        QTimer::singleShot(0, &menu, [&]()
                           { focusWindow(&menu); });
    }

    return a.exec();
}