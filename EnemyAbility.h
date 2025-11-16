#ifndef ENEMY_ABILITY_H
#define ENEMY_ABILITY_H
#include <QString>
#include <QStringList>
#include <QVector>

struct AbilitySpec {
    QString name;        // 例: "SplitOnDeath"
    QStringList tokens;  // 例: ["SmallBug","2"]
};

inline AbilitySpec parseAbility(const QString& s) {
    AbilitySpec sp;
    int p = s.indexOf(':');
    if (p < 0) { sp.name = s.trimmed(); return sp; }
    sp.name = s.left(p).trimmed();
    QString rest = s.mid(p+1).trimmed();
    for (const auto& part : rest.split(',', Qt::SkipEmptyParts)) sp.tokens << part.trimmed();
    return sp;
}

#endif // ENEMY_ABILITY_H
