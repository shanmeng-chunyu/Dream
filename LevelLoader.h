#pragma once // 防止头文件被重复包含

#include <QString>
#include <vector>
#include <QPointF>
#include <QPixmap>
#include "WaveManager.h" // 需要用到 WaveInfo 结构体

// 前向声明 (Forward Declaration)
class GameMap;
class WaveManager;

// 定义一个结构体，用于封装从文件中加载的所有关卡数据
// 这样做的好处是让加载函数的返回更清晰、更规整
struct LevelData {
    QString backgroundPath;
    std::vector<QPointF> pathPoints;
    std::vector<QPointF> towerBasePositions;
    std::vector<WaveInfo> waves;
};

/*
 * 类名: LevelLoader
 * 负责人: P8 - 数据管理与关卡编辑器
 * 说明: 这是一个静态工具类，负责从数据文件（如此处示例的JSON文件）中
 * 读取和解析关卡数据。它将文件内容转换为游戏核心模块可以理解的
 * 数据结构。该类是连接关卡编辑器与游戏主程序的关键枢纽。
 * 设计要点:
 * - 使用静态函数: loadForLevel 是一个静态函数，外部代码无需创建
 * LevelLoader 的实例即可调用，例如 LevelLoader::loadForLevel(1)。
 * - 数据封装: 使用 LevelData 结构体来聚合所有关卡数据，使得函数
 * 接口更加整洁，便于扩展。
 * - 职责单一: 这个类只负责“加载”和“解析”，不参与任何游戏逻辑。
 */
class LevelLoader {
public:
    // 删除构造函数，防止外部意外创建实例
    LevelLoader() = delete;

    // 根据关卡ID加载对应的关卡数据文件
    // levelId: 关卡的编号，例如 1, 2, 3...
    // levelData: 一个输出参数，如果加载成功，此结构体将被填充有关卡数据
    // 返回值: bool, true 表示加载和解析成功，false 表示失败
    static bool loadForLevel(int levelId, LevelData& levelData);
};
