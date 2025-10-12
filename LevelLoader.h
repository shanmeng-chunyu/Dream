#pragma once

#include <QString>
#include <vector>
#include <QPoint>  // 使用 QPoint 来存储 (col, row) 整数对
#include <QSize>   // 使用 QSize 来存储 (cols, rows)
#include "WaveManager.h"

// 定义一个结构体，用于封装从文件中加载的所有关卡数据
// 已更新为网格布局
struct LevelData {
    QString backgroundPath;
    QSize gridSize; // 网格维度
    std::vector<QPoint> pathPoints; // 网格坐标
    std::vector<QPoint> towerBasePositions; // 网格坐标
    std::vector<WaveInfo> waves;
};

/*
 * 类名: LevelLoader
 * 负责人: P8 - 数据管理与关卡编辑器
 * 说明: (已更新) 静态工具类，负责从基于网格的JSON文件中读取和
 * 解析关卡数据。
 */
class LevelLoader {
public:
    LevelLoader() = delete; // 静态类，禁止实例化

    // 根据关卡ID加载对应的关卡数据文件
    // levelData: 输出参数，将被填充为解析后的网格化关卡数据
    // 返回值: bool, true 表示加载和解析成功
    static bool loadForLevel(int levelId, LevelData& levelData);
};

