#include "LevelLoader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

bool LevelLoader::loadForLevel(int levelId, LevelData& levelData) {
    // 【已修正】使用 QString::number() 来避免编译器在解析 arg() 时产生歧义
    QString filePath = QString("./levels/level_%1.json").arg(QString::number(levelId));
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open level file:" << filePath;
        return false;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Failed to parse JSON or JSON is not an object in file:" << filePath;
        return false;
    }

    QJsonObject rootObj = jsonDoc.object();

    // --- 1. 加载背景图片路径 ---
    if (rootObj.contains("backgroundPath") && rootObj["backgroundPath"].isString()) {
        levelData.backgroundPath = rootObj["backgroundPath"].toString();
    } else {
        qWarning() << "JSON parse error: 'backgroundPath' missing or invalid.";
        return false;
    }

    // --- 2. 加载网格维度 ---
    if (rootObj.contains("gridSize") && rootObj["gridSize"].isObject()) {
        QJsonObject gridSizeObj = rootObj["gridSize"].toObject();
        levelData.gridSize.setWidth(gridSizeObj["cols"].toInt());
        levelData.gridSize.setHeight(gridSizeObj["rows"].toInt());
    } else {
        qWarning() << "JSON parse error: 'gridSize' missing or invalid.";
        return false;
    }

    // --- 3. 加载敌人移动路径点 (网格坐标) ---
    levelData.pathPoints.clear();
    if (rootObj.contains("pathPoints") && rootObj["pathPoints"].isArray()) {
        QJsonArray pathArray = rootObj["pathPoints"].toArray();
        for (const QJsonValue& val : pathArray) {
            QJsonObject pointObj = val.toObject();
            levelData.pathPoints.emplace_back(pointObj["col"].toInt(), pointObj["row"].toInt());
        }
    } else {
        qWarning() << "JSON parse error: 'pathPoints' missing or invalid.";
        return false;
    }

    // --- 4. 加载防御塔基座位置 (网格坐标) ---
    levelData.towerBasePositions.clear();
    if (rootObj.contains("towerBases") && rootObj["towerBases"].isArray()) {
        QJsonArray towerBasesArray = rootObj["towerBases"].toArray();
        for (const QJsonValue& val : towerBasesArray) {
            QJsonObject pointObj = val.toObject();
            levelData.towerBasePositions.emplace_back(pointObj["col"].toInt(), pointObj["row"].toInt());
        }
    } else {
        qWarning() << "JSON parse error: 'towerBases' missing or invalid.";
        return false;
    }

    // --- 5. 加载出怪波次信息 ---
    levelData.waves.clear();
    if (rootObj.contains("waves") && rootObj["waves"].isArray()) {
        QJsonArray wavesArray = rootObj["waves"].toArray();
        for (const QJsonValue& val : wavesArray) {
            QJsonObject waveObj = val.toObject();
            levelData.waves.push_back({
                waveObj["enemyType"].toInt(),
                waveObj["enemyCount"].toInt(),
                waveObj["spawnInterval"].toInt()
            });
        }
    } else {
        qWarning() << "JSON parse error: 'waves' missing or invalid.";
        return false;
    }

    qInfo() << "Level" << levelId << "grid data loaded successfully.";
    return true;
}

