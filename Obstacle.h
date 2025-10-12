#pragma once

#include <QGraphicsPixmapItem>

// Obstacle (障碍物)
// 负责人: P2-游戏场景与交互开发
// 说明: 这是一个代表地图上静态障碍物的类。它继承自QGraphicsPixmapItem以便于在场景中显示。
//      障碍物是静态的，因此它没有复杂的逻辑，也不需要像动态对象那样每帧更新。
//      它的创建和放置由GameManager在加载地图时统一处理。
class Obstacle : public QGraphicsPixmapItem {
public:
    // 构造函数接收一个障碍物类型ID和一个可选的父项
    // 实际项目中, 你可以根据typeId加载不同的障碍物图片
    explicit Obstacle(int typeId, QGraphicsItem* parent = nullptr);

    // Obstacle没有自己的update逻辑, 所以不需要声明或实现update()函数
};
