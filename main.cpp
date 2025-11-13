#include <QApplication>
#include <QTimer>
#include <QStringList>

#include "LevelEditorWidget.h"
#include "MainWindow.h"
#include "widget_choose_level.h"

namespace
{
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
    //调试关卡编辑界面
    // LevelEditorWidget editor;
    //
    // editor.resize(1024, 768);
    // editor.setWindowTitle("关卡编辑器 [调试模式]");
    //
    // editor.show();

    MainWindow w;
    w.resize(1024, 768);

    widget_choose_level levelChooser;
    levelChooser.setWindowTitle(QStringLiteral("Choose Level"));
    levelChooser.resize(1024, 768);

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

    QObject::connect(&levelChooser, &widget_choose_level::level1, &a, [&]()
                     { startLevel(QStringLiteral("levels/level1.json")); });
    QObject::connect(&levelChooser, &widget_choose_level::level2, &a, [&]()
                     { startLevel(QStringLiteral("levels/level2.json")); });
    QObject::connect(&levelChooser, &widget_choose_level::level3, &a, [&]()
                     { startLevel(QStringLiteral("levels/level3.json")); });
    QObject::connect(&levelChooser, &widget_choose_level::back, &a, [&]()
                     {
                         levelChooser.hide();
                         if (w.isVisible())
                         {
                             focusWindow(&w);
                         }
                         else
                         {
                             a.quit();
                         } });

    QObject::connect(&w, &MainWindow::levelSelectionRequested, &levelChooser, [&]()
                     { focusWindow(&levelChooser); });

    const QString cmdCandidate = commandLineLevelCandidate();
    const QString envCandidate = QString::fromLocal8Bit(qgetenv("DREAM_LEVEL_PATH")).trimmed();
    const bool hasExplicitLevel = !cmdCandidate.isEmpty() || !envCandidate.isEmpty();

    if (hasExplicitLevel)
    {
        w.show();
    }
    else
    {
        QTimer::singleShot(0, &levelChooser, [&]()
                           { focusWindow(&levelChooser); });
    }

    return a.exec();
}
