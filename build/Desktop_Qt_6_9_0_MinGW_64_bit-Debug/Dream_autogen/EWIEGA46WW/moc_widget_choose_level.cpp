/****************************************************************************
** Meta object code from reading C++ file 'widget_choose_level.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../widget_choose_level.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget_choose_level.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19widget_choose_levelE_t {};
} // unnamed namespace

template <> constexpr inline auto widget_choose_level::qt_create_metaobjectdata<qt_meta_tag_ZN19widget_choose_levelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "widget_choose_level",
        "level1",
        "",
        "level2",
        "level3",
        "back"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'level1'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'level2'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'level3'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'back'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<widget_choose_level, qt_meta_tag_ZN19widget_choose_levelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject widget_choose_level::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19widget_choose_levelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19widget_choose_levelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19widget_choose_levelE_t>.metaTypes,
    nullptr
} };

void widget_choose_level::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<widget_choose_level *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->level1(); break;
        case 1: _t->level2(); break;
        case 2: _t->level3(); break;
        case 3: _t->back(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (widget_choose_level::*)()>(_a, &widget_choose_level::level1, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (widget_choose_level::*)()>(_a, &widget_choose_level::level2, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (widget_choose_level::*)()>(_a, &widget_choose_level::level3, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (widget_choose_level::*)()>(_a, &widget_choose_level::back, 3))
            return;
    }
}

const QMetaObject *widget_choose_level::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *widget_choose_level::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19widget_choose_levelE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int widget_choose_level::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void widget_choose_level::level1()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void widget_choose_level::level2()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void widget_choose_level::level3()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void widget_choose_level::back()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
