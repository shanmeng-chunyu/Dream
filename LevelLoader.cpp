#include "LevelLoader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug> // 用于在出现错误时打印调试信息

bool LevelLoader::loadForLevel(int levelId, LevelData& levelData) {
    // 1. 根据关卡ID构造文件路径
    // 我们约定将关卡文件放在Qt资源系统里，路径为 ":/levels/level_X.json"
    QString filePath = QString(":/levels/level_%1.json").arg(levelId);

    // 2. 使用 QFile 打开文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open level file:" << filePath;
        return false;
    }

    // 3. 读取文件内容并解析JSON
    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Failed to parse JSON or JSON is not an object in file:" << filePath;
        return false;
    }

    QJsonObject rootObj = jsonDoc.object();

    // 4. 从JSON对象中提取数据并填充到 LevelData 结构体中
    // --- 加载背景图片路径 ---
    if (rootObj.contains("backgroundPath") && rootObj["backgroundPath"].isString()) {
        levelData.backgroundPath = rootObj["backgroundPath"].toString();
    } else {
        qWarning() << "JSON parse error: 'backgroundPath' not found or not a string.";
        return false;
    }

    // --- 加载敌人移动路径点 ---
    if (rootObj.contains("pathPoints") && rootObj["pathPoints"].isArray()) {
        QJsonArray pathArray = rootObj["pathPoints"].toArray();
        for (const QJsonValue& val : pathArray) {
            QJsonObject pointObj = val.toObject();
            levelData.pathPoints.emplace_back(pointObj["x"].toDouble(), pointObj["y"].toDouble());
        }
    } else {
        qWarning() << "JSON parse error: 'pathPoints' not found or not an array.";
        return false;
    }

    // --- 加载防御塔基座位置 ---
    if (rootObj.contains("towerBases") && rootObj["towerBases"].isArray()) {
        QJsonArray towerBasesArray = rootObj["towerBases"].toArray();
        for (const QJsonValue& val : towerBasesArray) {
            QJsonObject pointObj = val.toObject();
            levelData.towerBasePositions.emplace_back(pointObj["x"].toDouble(), pointObj["y"].toDouble());
        }
    } else {
        qWarning() << "JSON parse error: 'towerBases' not found or not an array.";
        return false;
    }

    // --- 加载出怪波次信息 ---
    if (rootObj.contains("waves") && rootObj["waves"].isArray()) {
        QJsonArray wavesArray = rootObj["waves"].toArray();
        for (const QJsonValue& val : wavesArray) {
            QJsonObject waveObj = val.toObject();
            WaveInfo waveInfo;
            waveInfo.enemyType = waveObj["enemyType"].toInt();
            waveInfo.enemyCount = waveObj["enemyCount"].toInt();
            waveInfo.spawnInterval = waveObj["spawnInterval"].toInt();
            levelData.waves.push_back(waveInfo);
        }
    } else {
        qWarning() << "JSON parse error: 'waves' not found or not an array.";
        return false;
    }

    qInfo() << "Level" << levelId << "data loaded successfully.";
    return true;
}
