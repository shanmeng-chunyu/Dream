#include <QApplication>
#include <QTimer>
#include <QStringList>
#include <QIcon>

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

    a.setWindowIcon(QIcon(":/background/resources/background/title.png"));

    // (调试用的关卡编辑器保持注释状态)
    LevelEditorWidget editor;
    editor.resize(1024, 768);
    editor.setWindowTitle("关卡编辑器 [调试模式]");


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

    QVector<QString> g_loadingTips;
    g_loadingTips.push_back("正在唤醒记忆...");
    g_loadingTips.push_back("很外向，说悄悄话都要用音响。");
    g_loadingTips.push_back("爱究竟源自何处呢？就这样扪心自问着。");
    g_loadingTips.push_back("梦境正在被侵蚀，请守护好你最后的精神稳定度。");
    g_loadingTips.push_back("无论是论文、回忆还是噩梦，它们都只是你梦中的一部分。别怕，天就快亮了。");
    g_loadingTips.push_back("梦境是现实的倒影。你在烦恼什么，就会梦见什么。");
    g_loadingTips.push_back("那些无穷无尽的压力，是否让你喘不过气？");
    g_loadingTips.push_back("孤独和遗憾总是结伴而来。");
    g_loadingTips.push_back("最终的恐惧源于你自己。战胜它，才能迎来真正的黎明。");
    g_loadingTips.push_back("不要低估陪伴的力量，它能让你身边的防线更加坚固。");
    g_loadingTips.push_back("温暖的回忆拥有强大的力量，它能让时间短暂停止。");
    g_loadingTips.push_back("有时候，一杯咖啡、一个抱枕，就是对抗焦虑的最好武器。");
    g_loadingTips.push_back("有一颗紧紧依靠着你的心，一瞬间落空");
    g_loadingTips.push_back("清除障碍物可以获得额外资源。");
    g_loadingTips.push_back("出售防御塔可以立即返还其总花费（包括升级费用）的 70%。");
    g_loadingTips.push_back("防御塔总是优先攻击范围内的敌人；只有当没有敌人在范围内时，它们才会攻击障碍物。");
    g_loadingTips.push_back("知识古树发射的子弹可以造成范围伤害，非常适合对付成群的敌人。");
    g_loadingTips.push_back("续命咖啡是一座光环塔，它能减慢范围内敌人的移动速度，并提升周围友方防御塔的攻击速度。");
    g_loadingTips.push_back("朋友的陪伴也是一座光环塔，它可以提升周围友方防御塔的攻击力。");
    g_loadingTips.push_back("深夜电台发射的子弹可以穿透路径上的一排敌人，适合放在长直道上。");
    g_loadingTips.push_back("摸鱼猫猫枕和温暖的记忆可以使敌人眩晕，暂时阻止其前进。");
    g_loadingTips.push_back("PRE会嘲讽你所有的防御塔，使它们不顾一切地优先攻击自己。");

    // 设置你想要的加载时长 (3000ms = 3秒)
    const int LOADING_DURATION_MS = 8000;

    /**
     * @brief 这是一个新的辅助 Lambda，它负责：
     * 1. 创建和显示加载界面
     * 2. 连接加载界面的 finished() 信号
     * 3. 在加载完成后，才真正加载并切换到游戏窗口
     */
    auto showLoadingAndStart = [&](int levelIndex,    // 关卡索引 (0, 1, 2)
                                   const QString& levelPath, // 关卡路径
                                   QWidget* fromWindow) // 从哪个窗口切换过来 (chooser 或 editor)
    {
        // 1. 创建加载界面
        //    (我们使用 levelIndex % 3 来确保主题索引总是 0, 1, 2 之一)
        widget_level_loading *loader = new widget_level_loading(
            levelIndex % 3,
            g_loadingTips,
            LOADING_DURATION_MS,
            nullptr // 必须为 nullptr，使其成为一个独立的窗口
        );
        loader->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除

        // 2. 关键：连接 loader 的 finished 信号
        QObject::connect(loader, &widget_level_loading::finished, &a,
            // 当加载界面播放完毕后，执行这个 Lambda：
            [&, levelPath, loader]() {

            // 3. (A) 真正开始加载关卡
            if (!w.startLevelFromSource(levelPath, true))
            {
                // 如果加载失败 (例如 json 文件损坏)
                loader->close(); // 关闭加载界面
                switchWindow(nullptr, &levelChooser); // 退回到关卡选择界面
                return;
            }

            // 3. (B) 加载成功，切换到游戏窗口
            switchWindow(loader, &w);
            loader->close(); // 切换后关闭 (并自动删除) 加载界面
        });

        // 4. 立即从 "fromWindow" 切换到 "loader"
        switchWindow(fromWindow, loader);
    };

    // --- 信号槽连接 ---

    // (保留) 从关卡选择器 -> 开始游戏
    QObject::connect(&levelChooser, &widget_choose_level::level1, &a, [&]()
                     { showLoadingAndStart(0, "levels/level1.json", &levelChooser); });
    QObject::connect(&levelChooser, &widget_choose_level::level2, &a, [&]()
                     { showLoadingAndStart(1, "levels/level2.json", &levelChooser); });
    QObject::connect(&levelChooser, &widget_choose_level::level3, &a, [&]() {
        editor.loadLevelForEditing(":/levels/levels/level3.json");
        switchWindow(&levelChooser, &editor);
    });

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

    QObject::connect(&editor, &LevelEditorWidget::levelEditingFinished, &a, [&](const QString& savedLevelPath)
    {
        showLoadingAndStart(2, savedLevelPath, &editor);
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