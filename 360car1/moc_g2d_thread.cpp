/****************************************************************************
** Meta object code from reading C++ file 'g2d_thread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "g2d_thread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'g2d_thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_G2DThread_t {
    QByteArrayData data[6];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_G2DThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_G2DThread_t qt_meta_stringdata_G2DThread = {
    {
QT_MOC_LITERAL(0, 0, 9), // "G2DThread"
QT_MOC_LITERAL(1, 10, 10), // "caleFinish"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 8), // "copyData"
QT_MOC_LITERAL(4, 31, 11), // "frameOffset"
QT_MOC_LITERAL(5, 43, 15) // "cleanFrameSpeed"

    },
    "G2DThread\0caleFinish\0\0copyData\0"
    "frameOffset\0cleanFrameSpeed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_G2DThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   32,    2, 0x0a /* Public */,
       5,    0,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt,    4,
    QMetaType::Void,

       0        // eod
};

void G2DThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        G2DThread *_t = static_cast<G2DThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->caleFinish((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 1: _t->copyData((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 2: _t->cleanFrameSpeed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (G2DThread::*_t)(unsigned int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&G2DThread::caleFinish)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject G2DThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_G2DThread.data,
      qt_meta_data_G2DThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *G2DThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *G2DThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_G2DThread.stringdata0))
        return static_cast<void*>(const_cast< G2DThread*>(this));
    return QThread::qt_metacast(_clname);
}

int G2DThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void G2DThread::caleFinish(unsigned int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
