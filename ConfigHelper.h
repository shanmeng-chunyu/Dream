#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

inline QString getConfigFile(const QString& fileName) {
    // fileName 传入例如: "levels/level1.json" 或 "tower_data.json"

#ifdef PROJECT_ROOT
    // 1. 开发模式 (CLion): 优先读源码目录，方便修改调试
    QString devPath = QString(PROJECT_ROOT) + "/" + fileName;
    if (QFile::exists(devPath)) {
        return devPath;
    }
#endif

    // 2. 发布模式 (Release): 读取 .exe 同级目录下的文件
    // QCoreApplication::applicationDirPath() 返回的是 Dream.exe 所在的文件夹路径
    QString releasePath = QCoreApplication::applicationDirPath() + "/" + fileName;

    // 检查文件是否存在
    if (QFile::exists(releasePath)) {
        return releasePath;
    }

    // 3. 如果都找不到，打印严重警告（这通常是崩溃的前兆）
    qWarning() << "!!! CRITICAL ERROR: Config file not found !!!";
    fflush(stderr);
    qWarning() << "Missing file:" << releasePath;
    fflush(stderr);
    return "";
}

#endif // CONFIGHELPER_H