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

    auto switchWindow = [](QWidget *from, QWidget *to)
    {
        if (!from || !to)
        {
            return;
        }

        // 1. 从 "from" 窗口获取当前的几何信息和窗口状态
        const QRect geometry = from->geometry();
        const Qt::WindowStates state = from->windowState();

        // 2. 隐藏 "from" 窗口
        from->hide();

        // 3. 将状态应用到 "to" 窗口
        if (state.testFlag(Qt::WindowFullScreen))
        {
            // 如果是全屏，则全屏显示 "to" 窗口
            to->setWindowState(Qt::WindowFullScreen);
            to->showFullScreen();
        }
        else if (state.testFlag(Qt::WindowMaximized))
        {
            // 如果是最大化，则最大化显示
            to->setWindowState(Qt::WindowMaximized);
            to->showMaximized();
        }
        else
        {
            // 否则，是普通窗口，恢复其精确的几何位置和大小
            to->setGeometry(geometry);
            to->setWindowState(Qt::WindowNoState); // 确保移除了任何最大化/最小化/全屏标志
            to->showNormal();
        }

        // 4. (可选但推荐) 确保新窗口在前台
        to->raise();
        to->activateWindow();
    };

    auto startLevel = [&](const QString &candidatePath)
    {
        // ... (内容保持不变) ...
        if (!w.startLevelFromSource(candidatePath, true))
        {
            focusWindow(&levelChooser); // 失败时保持旧逻辑
            return;
        }

        // 【修改】使用新的 switchWindow 函数
        switchWindow(&levelChooser, &w);

        // 【删除】以下逻辑已由 switchWindow 处理
        // levelChooser.hide();
        // if (!w.isVisible())
        // {
        //     w.show();
        // }
        // w.raise();
        // w.activateWindow();
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
                         switchWindow(&levelChooser, &menu); // <-- 修改
                     });

    // (保留) 从游戏窗口 -> 返回关卡选择器
    QObject::connect(&w, &MainWindow::levelSelectionRequested, &levelChooser, [&]()
                      {
                          switchWindow(&w, &levelChooser); // <-- 修改
                      });
    // <--- 新增连接：从游戏窗口 -> 返回主菜单 --->
    QObject::connect(&w, &MainWindow::mainMenuRequested, &a, [&]() {
        switchWindow(&w, &menu); // <-- 修改
    });

    // --- 主菜单的三个按钮连接 ---

    // 1. (保留) 主菜单 -> 关卡选择器
    QObject::connect(&menu, &widget_menu::choose_level, &a, [&]()
                     {
                         switchWindow(&menu, &levelChooser); // <-- 修改
                     });

    // 2. 步骤 2：(新增) 主菜单 -> 图鉴
    QObject::connect(&menu, &widget_menu::reference_book, &a, [&]()
                     {
                         switchWindow(&menu, &refBook); // <-- 修改
                     });

    // 3. (保留) 主菜单 -> 退出游戏
    QObject::connect(&menu, &widget_menu::exit_game, &a, &QApplication::quit);


    // --- 图鉴的返回按钮连接 ---

    // 步骤 3：(新增) 从图鉴 -> 返回主菜单
    QObject::connect(&refBook, &widget_reference_book::back_to_menu, &a, [&]()
                     {
                         switchWindow(&refBook, &menu); // <-- 修改
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