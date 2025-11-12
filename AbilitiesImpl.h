#ifndef ABILITIES_IMPL_H
#define ABILITIES_IMPL_H
#include <QString>
#include <QVector>

struct RageConfig { bool enabled=false; double threshold=0.0; double speedMul=1.0; };
struct SplitConfig { bool enabled=false; QString childType; int count=0; };
struct BossSpawnConfig { bool destroyAllTowersOnSpawn=false; };

struct EnemyAbilities {
    RageConfig rage;
    SplitConfig split;
    BossSpawnConfig boss;
};

#endif // ABILITIES_IMPL_H
