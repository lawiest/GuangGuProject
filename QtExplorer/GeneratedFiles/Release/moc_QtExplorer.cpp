/****************************************************************************
** Meta object code from reading C++ file 'QtExplorer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../QtExplorer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtExplorer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MenuItemWidget_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MenuItemWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MenuItemWidget_t qt_meta_stringdata_MenuItemWidget = {
    {
QT_MOC_LITERAL(0, 0, 14) // "MenuItemWidget"

    },
    "MenuItemWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MenuItemWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void MenuItemWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject MenuItemWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MenuItemWidget.data,
      qt_meta_data_MenuItemWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MenuItemWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MenuItemWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MenuItemWidget.stringdata0))
        return static_cast<void*>(const_cast< MenuItemWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int MenuItemWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_QtExplorer_t {
    QByteArrayData data[8];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtExplorer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtExplorer_t qt_meta_stringdata_QtExplorer = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QtExplorer"
QT_MOC_LITERAL(1, 11, 11), // "closeWidget"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 21), // "ShowFullOrNomalWindow"
QT_MOC_LITERAL(4, 46, 12), // "AddImageSlot"
QT_MOC_LITERAL(5, 59, 14), // "AddTerrainSlot"
QT_MOC_LITERAL(6, 74, 16), // "AddPlaceNameSlot"
QT_MOC_LITERAL(7, 91, 15) // "AddObliqueImage"

    },
    "QtExplorer\0closeWidget\0\0ShowFullOrNomalWindow\0"
    "AddImageSlot\0AddTerrainSlot\0"
    "AddPlaceNameSlot\0AddObliqueImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtExplorer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    0,   46,    2, 0x0a /* Public */,
       5,    0,   47,    2, 0x0a /* Public */,
       6,    0,   48,    2, 0x0a /* Public */,
       7,    0,   49,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QtExplorer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QtExplorer *_t = static_cast<QtExplorer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { bool _r = _t->closeWidget();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: _t->ShowFullOrNomalWindow(); break;
        case 2: _t->AddImageSlot(); break;
        case 3: _t->AddTerrainSlot(); break;
        case 4: _t->AddPlaceNameSlot(); break;
        case 5: _t->AddObliqueImage(); break;
        default: ;
        }
    }
}

const QMetaObject QtExplorer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtExplorer.data,
      qt_meta_data_QtExplorer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QtExplorer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtExplorer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QtExplorer.stringdata0))
        return static_cast<void*>(const_cast< QtExplorer*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtExplorer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
