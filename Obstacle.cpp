#include "Obstacle.h"
#include <QPixmap>

// Obstacle (障碍物)
// 负责人: P2-游戏场景与交互开发
// 说明: 构造函数是这个类的核心。它根据一个整数ID来决定加载哪张图片作为障碍物的外观。
//      这种设计使得我们可以在关卡数据中只存一个数字，就能代表各式各样的障碍物。
Obstacle::Obstacle(int typeId, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent) {
    QString imagePath;
    // 根据typeId选择不同的图片资源
    // 实际的路径需要根据你们项目存放美术资源的位置来定
    switch (typeId) {
        case 1:
            imagePath = ":/assets/images/obstacles/书本障碍方块.png";
        break;
        case 2:
            imagePath = ":/assets/images/obstacles/数据线障碍方块.png";
        break;
        case 3:
            imagePath = ":/assets/images/obstacles/文具障碍方块.png";
        break;
        // 可以根据需要添加更多的case来对应不同类型的障碍物
        default:
            // 提供一个默认的图像以防typeId无效
                imagePath = ":/assets/images/obstacles/default_obstacle.png";
        break;
    }
    setPixmap(QPixmap(imagePath));
}
