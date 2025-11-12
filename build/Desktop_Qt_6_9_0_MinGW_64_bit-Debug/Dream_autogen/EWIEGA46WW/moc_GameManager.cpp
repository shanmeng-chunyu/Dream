/****************************************************************************
** Meta object code from reading C++ file 'GameManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../GameManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GameManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11GameManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto GameManager::qt_create_metaobjectdata<qt_meta_tag_ZN11GameManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GameManager",
        "onSpawnEnemy",
        "",
        "type",
        "std::vector<QPointF>",
        "absolutePath",
        "onNewBullet",
        "Tower*",
        "tower",
        "Enemy*",
        "target",
        "onEnemyReachedEnd",
        "enemy",
        "onEnemyDied",
        "onBulletHitTarget",
        "Bullet*",
        "bullet",
        "onObstacleDestroyed",
        "Obstacle*",
        "obstacle",
        "resourceValue",
        "updateGame"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onSpawnEnemy'
        QtMocHelpers::SlotData<void(const QString &, const std::vector<QPointF> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Slot 'onNewBullet'
        QtMocHelpers::SlotData<void(Tower *, Enemy *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 }, { 0x80000000 | 9, 10 },
        }}),
        // Slot 'onEnemyReachedEnd'
        QtMocHelpers::SlotData<void(Enemy *)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 12 },
        }}),
        // Slot 'onEnemyDied'
        QtMocHelpers::SlotData<void(Enemy *)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 12 },
        }}),
        // Slot 'onBulletHitTarget'
        QtMocHelpers::SlotData<void(Bullet *)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Slot 'onObstacleDestroyed'
        QtMocHelpers::SlotData<void(Obstacle *, int)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 19 }, { QMetaType::Int, 20 },
        }}),
        // Slot 'updateGame'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GameManager, qt_meta_tag_ZN11GameManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GameManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11GameManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11GameManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11GameManagerE_t>.metaTypes,
    nullptr
} };

void GameManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GameManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onSpawnEnemy((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::vector<QPointF>>>(_a[2]))); break;
        case 1: _t->onNewBullet((*reinterpret_cast< std::add_pointer_t<Tower*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Enemy*>>(_a[2]))); break;
        case 2: _t->onEnemyReachedEnd((*reinterpret_cast< std::add_pointer_t<Enemy*>>(_a[1]))); break;
        case 3: _t->onEnemyDied((*reinterpret_cast< std::add_pointer_t<Enemy*>>(_a[1]))); break;
        case 4: _t->onBulletHitTarget((*reinterpret_cast< std::add_pointer_t<Bullet*>>(_a[1]))); break;
        case 5: _t->onObstacleDestroyed((*reinterpret_cast< std::add_pointer_t<Obstacle*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 6: _t->updateGame(); break;
        default: ;
        }
    }
}

const QMetaObject *GameManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11GameManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GameManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
