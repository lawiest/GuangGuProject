/****************************************************************************
** Meta object code from reading C++ file 'ribbonwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ribbonwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ribbonwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RibbonWindow_t {
    QByteArrayData data[23];
    char stringdata0[340];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RibbonWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RibbonWindow_t qt_meta_stringdata_RibbonWindow = {
    {
QT_MOC_LITERAL(0, 0, 12), // "RibbonWindow"
QT_MOC_LITERAL(1, 13, 5), // "about"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 12), // "optionsTheme"
QT_MOC_LITERAL(4, 33, 8), // "QAction*"
QT_MOC_LITERAL(5, 42, 13), // "optionsDialog"
QT_MOC_LITERAL(6, 56, 18), // "includingAnimation"
QT_MOC_LITERAL(7, 75, 21), // "setTitleGroupsVisible"
QT_MOC_LITERAL(8, 97, 14), // "maximizeToggle"
QT_MOC_LITERAL(9, 112, 19), // "minimizationChanged"
QT_MOC_LITERAL(10, 132, 9), // "minimized"
QT_MOC_LITERAL(11, 142, 11), // "optionsFont"
QT_MOC_LITERAL(12, 154, 3), // "act"
QT_MOC_LITERAL(13, 158, 13), // "setDPIToggled"
QT_MOC_LITERAL(14, 172, 26), // "setNativeScrollBarsToggled"
QT_MOC_LITERAL(15, 199, 23), // "setNativeDialogsToggled"
QT_MOC_LITERAL(16, 223, 28), // "showQuickAccessCustomizeMenu"
QT_MOC_LITERAL(17, 252, 6), // "QMenu*"
QT_MOC_LITERAL(18, 259, 4), // "menu"
QT_MOC_LITERAL(19, 264, 28), // "switchQuickAccessBarPosition"
QT_MOC_LITERAL(20, 293, 21), // "showRibbonContextMenu"
QT_MOC_LITERAL(21, 315, 18), // "QContextMenuEvent*"
QT_MOC_LITERAL(22, 334, 5) // "event"

    },
    "RibbonWindow\0about\0\0optionsTheme\0"
    "QAction*\0optionsDialog\0includingAnimation\0"
    "setTitleGroupsVisible\0maximizeToggle\0"
    "minimizationChanged\0minimized\0optionsFont\0"
    "act\0setDPIToggled\0setNativeScrollBarsToggled\0"
    "setNativeDialogsToggled\0"
    "showQuickAccessCustomizeMenu\0QMenu*\0"
    "menu\0switchQuickAccessBarPosition\0"
    "showRibbonContextMenu\0QContextMenuEvent*\0"
    "event"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RibbonWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x09 /* Protected */,
       3,    1,   85,    2, 0x09 /* Protected */,
       5,    0,   88,    2, 0x09 /* Protected */,
       6,    1,   89,    2, 0x09 /* Protected */,
       7,    1,   92,    2, 0x09 /* Protected */,
       8,    0,   95,    2, 0x09 /* Protected */,
       9,    1,   96,    2, 0x09 /* Protected */,
      11,    1,   99,    2, 0x09 /* Protected */,
      13,    1,  102,    2, 0x09 /* Protected */,
      14,    1,  105,    2, 0x09 /* Protected */,
      15,    1,  108,    2, 0x09 /* Protected */,
      16,    1,  111,    2, 0x09 /* Protected */,
      19,    0,  114,    2, 0x09 /* Protected */,
      20,    2,  115,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, 0x80000000 | 4,   12,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 21,   18,   22,

       0        // eod
};

void RibbonWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RibbonWindow *_t = static_cast<RibbonWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->about(); break;
        case 1: _t->optionsTheme((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: _t->optionsDialog(); break;
        case 3: _t->includingAnimation((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setTitleGroupsVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->maximizeToggle(); break;
        case 6: _t->minimizationChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->optionsFont((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 8: _t->setDPIToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setNativeScrollBarsToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->setNativeDialogsToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->showQuickAccessCustomizeMenu((*reinterpret_cast< QMenu*(*)>(_a[1]))); break;
        case 12: _t->switchQuickAccessBarPosition(); break;
        case 13: _t->showRibbonContextMenu((*reinterpret_cast< QMenu*(*)>(_a[1])),(*reinterpret_cast< QContextMenuEvent*(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject RibbonWindow::staticMetaObject = {
    { &Qtitan::RibbonMainWindow::staticMetaObject, qt_meta_stringdata_RibbonWindow.data,
      qt_meta_data_RibbonWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RibbonWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RibbonWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RibbonWindow.stringdata0))
        return static_cast<void*>(this);
    return Qtitan::RibbonMainWindow::qt_metacast(_clname);
}

int RibbonWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Qtitan::RibbonMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
