#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <QString>
#include "GameMap.h"
#include "WaveManager.h"
#include "Player.h"

class LevelLoader {
public:
    static bool loadLevel(const QString& filePath, GameMap& map, WaveManager& waveManager, Player& player);
};

#endif // LEVELLOADER_H
