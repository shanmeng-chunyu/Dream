#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

inline QString getConfigFile(const QString& fileName) {
    // fileName 传入类似于 "enemy_data.json" 的文件名（不要带 :/ 前缀了）

#ifdef PROJECT_ROOT
    // 1. 开发模式：优先读取源代码目录下的文件
    // 这样你修改项目里的 json，游戏会立刻读取到
    QString devPath = QString(PROJECT_ROOT) + "/" + fileName;
    if (QFile::exists(devPath)) {
        static bool hasLoggedDev = false;
        if (!hasLoggedDev) {
            qDebug() << "[DevMode] Loading config from Source:" << devPath;
            hasLoggedDev = true;
        }
        return devPath;
    }
#endif

    // 2. 发布模式：读取可执行文件 (.exe) 同级目录下的文件
    // 当你打包游戏时，需要把 json 文件复制到 exe 旁边
    QString releasePath = QCoreApplication::applicationDirPath() + "/" + fileName;
    if (QFile::exists(releasePath)) {
        return releasePath;
    }

    // 3. 如果都找不到，打印错误
    qWarning() << "Config file not found:" << fileName
               << "\nSearched Dev path:" <<
               #ifdef PROJECT_ROOT
                   QString(PROJECT_ROOT) + "/" + fileName
               #else
                   "N/A"
               #endif
               << "\nSearched Release path:" << releasePath;

    return "";
}

#endif // CONFIGHELPER_H