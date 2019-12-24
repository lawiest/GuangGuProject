/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[76];
    char stringdata0[1029];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 10), // "AddTerrain"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 8), // "AddImage"
QT_MOC_LITERAL(4, 32, 9), // "AddVector"
QT_MOC_LITERAL(5, 42, 12), // "AddPlaceName"
QT_MOC_LITERAL(6, 55, 15), // "AddObliqueImage"
QT_MOC_LITERAL(7, 71, 15), // "AddModelElement"
QT_MOC_LITERAL(8, 87, 11), // "std::string"
QT_MOC_LITERAL(9, 99, 7), // "strPath"
QT_MOC_LITERAL(10, 107, 1), // "x"
QT_MOC_LITERAL(11, 109, 1), // "y"
QT_MOC_LITERAL(12, 111, 1), // "z"
QT_MOC_LITERAL(13, 113, 8), // "AddModel"
QT_MOC_LITERAL(14, 122, 15), // "PlaceNameChange"
QT_MOC_LITERAL(15, 138, 4), // "Test"
QT_MOC_LITERAL(16, 143, 14), // "ThrowingEnable"
QT_MOC_LITERAL(17, 158, 9), // "AddServer"
QT_MOC_LITERAL(18, 168, 17), // "AddOsgbServerLoop"
QT_MOC_LITERAL(19, 186, 13), // "AddServerSlot"
QT_MOC_LITERAL(20, 200, 14), // "HeightAnalysis"
QT_MOC_LITERAL(21, 215, 16), // "DistanceAnalysis"
QT_MOC_LITERAL(22, 232, 12), // "AreaAnalysis"
QT_MOC_LITERAL(23, 245, 17), // "OPInRangeAnalysis"
QT_MOC_LITERAL(24, 263, 19), // "AddLonAndLatElement"
QT_MOC_LITERAL(25, 283, 17), // "AddCompassElement"
QT_MOC_LITERAL(26, 301, 19), // "LineOfSightAnalysis"
QT_MOC_LITERAL(27, 321, 22), // "TerrainProfileAnalysis"
QT_MOC_LITERAL(28, 344, 19), // "OsgbProfileAnalysis"
QT_MOC_LITERAL(29, 364, 17), // "RemoveAllAnalysis"
QT_MOC_LITERAL(30, 382, 14), // "RemoveAnalysis"
QT_MOC_LITERAL(31, 397, 27), // "MultipleLineOfSightAnalysis"
QT_MOC_LITERAL(32, 425, 19), // "AddBillBoardElement"
QT_MOC_LITERAL(33, 445, 22), // "AddGeoBillBoardElement"
QT_MOC_LITERAL(34, 468, 13), // "RadarAnalysis"
QT_MOC_LITERAL(35, 482, 14), // "VolumeAnalysis"
QT_MOC_LITERAL(36, 497, 18), // "DistanceAnalysis3D"
QT_MOC_LITERAL(37, 516, 21), // "AreaMeasureAnalysis3D"
QT_MOC_LITERAL(38, 538, 11), // "EditElement"
QT_MOC_LITERAL(39, 550, 8), // "OnFinish"
QT_MOC_LITERAL(40, 559, 42), // "std::vector<GeoStar::Kernel::..."
QT_MOC_LITERAL(41, 602, 8), // "vecPoint"
QT_MOC_LITERAL(42, 611, 13), // "OnSelectEvent"
QT_MOC_LITERAL(43, 625, 32), // "GeoStar::Kernel::Globe::Element*"
QT_MOC_LITERAL(44, 658, 6), // "oldEle"
QT_MOC_LITERAL(45, 665, 6), // "newEle"
QT_MOC_LITERAL(46, 672, 12), // "AddFlyPoints"
QT_MOC_LITERAL(47, 685, 8), // "StartFly"
QT_MOC_LITERAL(48, 694, 7), // "StopFly"
QT_MOC_LITERAL(49, 702, 8), // "PauseFly"
QT_MOC_LITERAL(50, 711, 10), // "RestartFly"
QT_MOC_LITERAL(51, 722, 7), // "BackFly"
QT_MOC_LITERAL(52, 730, 9), // "FastSpeed"
QT_MOC_LITERAL(53, 740, 11), // "ReduceSpeed"
QT_MOC_LITERAL(54, 752, 11), // "SaveFlyPath"
QT_MOC_LITERAL(55, 764, 11), // "LoadFlyPath"
QT_MOC_LITERAL(56, 776, 12), // "ReSetFlyPath"
QT_MOC_LITERAL(57, 789, 13), // "BeginRotation"
QT_MOC_LITERAL(58, 803, 19), // "ReduceRotationSpeed"
QT_MOC_LITERAL(59, 823, 17), // "FastRotationSpeed"
QT_MOC_LITERAL(60, 841, 12), // "StopRotation"
QT_MOC_LITERAL(61, 854, 12), // "StartFreeFly"
QT_MOC_LITERAL(62, 867, 11), // "StopFreeFly"
QT_MOC_LITERAL(63, 879, 8), // "Marathon"
QT_MOC_LITERAL(64, 888, 10), // "fullScreen"
QT_MOC_LITERAL(65, 899, 7), // "checked"
QT_MOC_LITERAL(66, 907, 14), // "stateStatusBar"
QT_MOC_LITERAL(67, 922, 5), // "state"
QT_MOC_LITERAL(68, 928, 16), // "zoomSliderScroll"
QT_MOC_LITERAL(69, 945, 5), // "value"
QT_MOC_LITERAL(70, 951, 21), // "showRibbonContextMenu"
QT_MOC_LITERAL(71, 973, 6), // "QMenu*"
QT_MOC_LITERAL(72, 980, 4), // "menu"
QT_MOC_LITERAL(73, 985, 18), // "QContextMenuEvent*"
QT_MOC_LITERAL(74, 1004, 5), // "event"
QT_MOC_LITERAL(75, 1010, 18) // "showCompanyWebSite"

    },
    "MainWindow\0AddTerrain\0\0AddImage\0"
    "AddVector\0AddPlaceName\0AddObliqueImage\0"
    "AddModelElement\0std::string\0strPath\0"
    "x\0y\0z\0AddModel\0PlaceNameChange\0Test\0"
    "ThrowingEnable\0AddServer\0AddOsgbServerLoop\0"
    "AddServerSlot\0HeightAnalysis\0"
    "DistanceAnalysis\0AreaAnalysis\0"
    "OPInRangeAnalysis\0AddLonAndLatElement\0"
    "AddCompassElement\0LineOfSightAnalysis\0"
    "TerrainProfileAnalysis\0OsgbProfileAnalysis\0"
    "RemoveAllAnalysis\0RemoveAnalysis\0"
    "MultipleLineOfSightAnalysis\0"
    "AddBillBoardElement\0AddGeoBillBoardElement\0"
    "RadarAnalysis\0VolumeAnalysis\0"
    "DistanceAnalysis3D\0AreaMeasureAnalysis3D\0"
    "EditElement\0OnFinish\0"
    "std::vector<GeoStar::Kernel::GsRawPoint3D>\0"
    "vecPoint\0OnSelectEvent\0"
    "GeoStar::Kernel::Globe::Element*\0"
    "oldEle\0newEle\0AddFlyPoints\0StartFly\0"
    "StopFly\0PauseFly\0RestartFly\0BackFly\0"
    "FastSpeed\0ReduceSpeed\0SaveFlyPath\0"
    "LoadFlyPath\0ReSetFlyPath\0BeginRotation\0"
    "ReduceRotationSpeed\0FastRotationSpeed\0"
    "StopRotation\0StartFreeFly\0StopFreeFly\0"
    "Marathon\0fullScreen\0checked\0stateStatusBar\0"
    "state\0zoomSliderScroll\0value\0"
    "showRibbonContextMenu\0QMenu*\0menu\0"
    "QContextMenuEvent*\0event\0showCompanyWebSite"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      58,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  304,    2, 0x09 /* Protected */,
       3,    0,  305,    2, 0x09 /* Protected */,
       4,    0,  306,    2, 0x09 /* Protected */,
       5,    0,  307,    2, 0x09 /* Protected */,
       6,    0,  308,    2, 0x09 /* Protected */,
       7,    4,  309,    2, 0x09 /* Protected */,
      13,    0,  318,    2, 0x09 /* Protected */,
      14,    0,  319,    2, 0x09 /* Protected */,
      15,    0,  320,    2, 0x09 /* Protected */,
      16,    0,  321,    2, 0x09 /* Protected */,
      17,    0,  322,    2, 0x09 /* Protected */,
      18,    0,  323,    2, 0x09 /* Protected */,
      19,    0,  324,    2, 0x09 /* Protected */,
      20,    0,  325,    2, 0x09 /* Protected */,
      21,    0,  326,    2, 0x09 /* Protected */,
      22,    0,  327,    2, 0x09 /* Protected */,
      23,    0,  328,    2, 0x09 /* Protected */,
       7,    0,  329,    2, 0x09 /* Protected */,
      24,    0,  330,    2, 0x09 /* Protected */,
      25,    0,  331,    2, 0x09 /* Protected */,
      26,    0,  332,    2, 0x09 /* Protected */,
      27,    0,  333,    2, 0x09 /* Protected */,
      28,    0,  334,    2, 0x09 /* Protected */,
      29,    0,  335,    2, 0x09 /* Protected */,
      30,    0,  336,    2, 0x09 /* Protected */,
      31,    0,  337,    2, 0x09 /* Protected */,
      32,    0,  338,    2, 0x09 /* Protected */,
      33,    0,  339,    2, 0x09 /* Protected */,
      34,    0,  340,    2, 0x09 /* Protected */,
      35,    0,  341,    2, 0x09 /* Protected */,
      36,    0,  342,    2, 0x09 /* Protected */,
      37,    0,  343,    2, 0x09 /* Protected */,
      38,    0,  344,    2, 0x09 /* Protected */,
      39,    1,  345,    2, 0x09 /* Protected */,
      42,    2,  348,    2, 0x09 /* Protected */,
      46,    0,  353,    2, 0x09 /* Protected */,
      47,    0,  354,    2, 0x09 /* Protected */,
      48,    0,  355,    2, 0x09 /* Protected */,
      49,    0,  356,    2, 0x09 /* Protected */,
      50,    0,  357,    2, 0x09 /* Protected */,
      51,    0,  358,    2, 0x09 /* Protected */,
      52,    0,  359,    2, 0x09 /* Protected */,
      53,    0,  360,    2, 0x09 /* Protected */,
      54,    0,  361,    2, 0x09 /* Protected */,
      55,    0,  362,    2, 0x09 /* Protected */,
      56,    0,  363,    2, 0x09 /* Protected */,
      57,    0,  364,    2, 0x09 /* Protected */,
      58,    0,  365,    2, 0x09 /* Protected */,
      59,    0,  366,    2, 0x09 /* Protected */,
      60,    0,  367,    2, 0x09 /* Protected */,
      61,    0,  368,    2, 0x09 /* Protected */,
      62,    0,  369,    2, 0x09 /* Protected */,
      63,    0,  370,    2, 0x09 /* Protected */,
      64,    1,  371,    2, 0x09 /* Protected */,
      66,    1,  374,    2, 0x09 /* Protected */,
      68,    1,  377,    2, 0x09 /* Protected */,
      70,    2,  380,    2, 0x09 /* Protected */,
      75,    0,  385,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Double, QMetaType::Double, QMetaType::Double,    9,   10,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 40,   41,
    QMetaType::Void, 0x80000000 | 43, 0x80000000 | 43,   44,   45,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   65,
    QMetaType::Void, QMetaType::Int,   67,
    QMetaType::Void, QMetaType::Int,   69,
    QMetaType::Void, 0x80000000 | 71, 0x80000000 | 73,   72,   74,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->AddTerrain(); break;
        case 1: _t->AddImage(); break;
        case 2: _t->AddVector(); break;
        case 3: _t->AddPlaceName(); break;
        case 4: _t->AddObliqueImage(); break;
        case 5: _t->AddModelElement((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 6: _t->AddModel(); break;
        case 7: _t->PlaceNameChange(); break;
        case 8: _t->Test(); break;
        case 9: _t->ThrowingEnable(); break;
        case 10: _t->AddServer(); break;
        case 11: _t->AddOsgbServerLoop(); break;
        case 12: _t->AddServerSlot(); break;
        case 13: _t->HeightAnalysis(); break;
        case 14: _t->DistanceAnalysis(); break;
        case 15: _t->AreaAnalysis(); break;
        case 16: _t->OPInRangeAnalysis(); break;
        case 17: _t->AddModelElement(); break;
        case 18: _t->AddLonAndLatElement(); break;
        case 19: _t->AddCompassElement(); break;
        case 20: _t->LineOfSightAnalysis(); break;
        case 21: _t->TerrainProfileAnalysis(); break;
        case 22: _t->OsgbProfileAnalysis(); break;
        case 23: _t->RemoveAllAnalysis(); break;
        case 24: _t->RemoveAnalysis(); break;
        case 25: _t->MultipleLineOfSightAnalysis(); break;
        case 26: _t->AddBillBoardElement(); break;
        case 27: _t->AddGeoBillBoardElement(); break;
        case 28: _t->RadarAnalysis(); break;
        case 29: _t->VolumeAnalysis(); break;
        case 30: _t->DistanceAnalysis3D(); break;
        case 31: _t->AreaMeasureAnalysis3D(); break;
        case 32: _t->EditElement(); break;
        case 33: _t->OnFinish((*reinterpret_cast< const std::vector<GeoStar::Kernel::GsRawPoint3D>(*)>(_a[1]))); break;
        case 34: _t->OnSelectEvent((*reinterpret_cast< GeoStar::Kernel::Globe::Element*(*)>(_a[1])),(*reinterpret_cast< GeoStar::Kernel::Globe::Element*(*)>(_a[2]))); break;
        case 35: _t->AddFlyPoints(); break;
        case 36: _t->StartFly(); break;
        case 37: _t->StopFly(); break;
        case 38: _t->PauseFly(); break;
        case 39: _t->RestartFly(); break;
        case 40: _t->BackFly(); break;
        case 41: _t->FastSpeed(); break;
        case 42: _t->ReduceSpeed(); break;
        case 43: _t->SaveFlyPath(); break;
        case 44: _t->LoadFlyPath(); break;
        case 45: _t->ReSetFlyPath(); break;
        case 46: _t->BeginRotation(); break;
        case 47: _t->ReduceRotationSpeed(); break;
        case 48: _t->FastRotationSpeed(); break;
        case 49: _t->StopRotation(); break;
        case 50: _t->StartFreeFly(); break;
        case 51: _t->StopFreeFly(); break;
        case 52: _t->Marathon(); break;
        case 53: _t->fullScreen((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 54: _t->stateStatusBar((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 55: _t->zoomSliderScroll((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 56: _t->showRibbonContextMenu((*reinterpret_cast< QMenu*(*)>(_a[1])),(*reinterpret_cast< QContextMenuEvent*(*)>(_a[2]))); break;
        case 57: _t->showCompanyWebSite(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &RibbonWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return RibbonWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RibbonWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 58)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 58;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 58)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 58;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
