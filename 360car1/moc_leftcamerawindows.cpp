/****************************************************************************
** Meta object code from reading C++ file 'leftcamerawindows.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "leftcamerawindows.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'leftcamerawindows.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LeftCameraWindows_t {
    QByteArrayData data[7];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LeftCameraWindows_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LeftCameraWindows_t qt_meta_stringdata_LeftCameraWindows = {
    {
QT_MOC_LITERAL(0, 0, 17), // "LeftCameraWindows"
QT_MOC_LITERAL(1, 18, 15), // "updateLeftImage"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 11), // "CaptureView"
QT_MOC_LITERAL(4, 47, 11), // "frameOffset"
QT_MOC_LITERAL(5, 59, 21), // "Start_warpPerspective"
QT_MOC_LITERAL(6, 81, 9) // "readFrame"

    },
    "LeftCameraWindows\0updateLeftImage\0\0"
    "CaptureView\0frameOffset\0Start_warpPerspective\0"
    "readFrame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LeftCameraWindows[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   35,    2, 0x08 /* Private */,
       5,    0,   38,    2, 0x08 /* Private */,
       6,    1,   39,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,

       0        // eod
};

void LeftCameraWindows::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LeftCameraWindows *_t = static_cast<LeftCameraWindows *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateLeftImage(); break;
        case 1: _t->CaptureView((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->Start_warpPerspective(); break;
        case 3: _t->readFrame((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LeftCameraWindows::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LeftCameraWindows::updateLeftImage)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject LeftCameraWindows::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LeftCameraWindows.data,
      qt_meta_data_LeftCameraWindows,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LeftCameraWindows::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LeftCameraWindows::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LeftCameraWindows.stringdata0))
        return static_cast<void*>(const_cast< LeftCameraWindows*>(this));
    return QWidget::qt_metacast(_clname);
}

int LeftCameraWindows::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void LeftCameraWindows::updateLeftImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
