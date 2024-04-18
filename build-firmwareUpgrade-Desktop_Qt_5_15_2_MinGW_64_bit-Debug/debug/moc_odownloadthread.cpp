/****************************************************************************
** Meta object code from reading C++ file 'odownloadthread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../firmwareUpgrade/thread/odownloadthread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'odownloadthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ODownloadThread_t {
    QByteArrayData data[17];
    char stringdata0[269];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ODownloadThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ODownloadThread_t qt_meta_stringdata_ODownloadThread = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ODownloadThread"
QT_MOC_LITERAL(1, 16, 12), // "sendFileList"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 31), // "QList<QPair<QString,QString> >*"
QT_MOC_LITERAL(4, 62, 22), // "oDownloadStatusMessage"
QT_MOC_LITERAL(5, 85, 13), // "oDownloadRate"
QT_MOC_LITERAL(6, 99, 23), // "parseStatusFileFinished"
QT_MOC_LITERAL(7, 123, 8), // "File_LNS"
QT_MOC_LITERAL(8, 132, 19), // "receiveCheckedFiles"
QT_MOC_LITERAL(9, 152, 15), // "checkedFileList"
QT_MOC_LITERAL(10, 168, 27), // "rcvStatusCodeAndMessageSlot"
QT_MOC_LITERAL(11, 196, 10), // "statusCode"
QT_MOC_LITERAL(12, 207, 12), // "totalFileNum"
QT_MOC_LITERAL(13, 220, 13), // "statusMessage"
QT_MOC_LITERAL(14, 234, 5), // "error"
QT_MOC_LITERAL(15, 240, 12), // "errorMessage"
QT_MOC_LITERAL(16, 253, 15) // "parseStatusFile"

    },
    "ODownloadThread\0sendFileList\0\0"
    "QList<QPair<QString,QString> >*\0"
    "oDownloadStatusMessage\0oDownloadRate\0"
    "parseStatusFileFinished\0File_LNS\0"
    "receiveCheckedFiles\0checkedFileList\0"
    "rcvStatusCodeAndMessageSlot\0statusCode\0"
    "totalFileNum\0statusMessage\0error\0"
    "errorMessage\0parseStatusFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ODownloadThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       5,    2,   55,    2, 0x06 /* Public */,
       6,    1,   60,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   63,    2, 0x0a /* Public */,
      10,    5,   66,    2, 0x0a /* Public */,
      16,    0,   77,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    2,    2,
    QMetaType::Void, 0x80000000 | 7,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QStringList,    9,
    QMetaType::Void, QMetaType::UShort, QMetaType::UShort, QMetaType::QString, QMetaType::Bool, QMetaType::QString,   11,   12,   13,   14,   15,
    QMetaType::Void,

       0        // eod
};

void ODownloadThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ODownloadThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendFileList((*reinterpret_cast< QList<QPair<QString,QString> >*(*)>(_a[1]))); break;
        case 1: _t->oDownloadStatusMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->oDownloadRate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->parseStatusFileFinished((*reinterpret_cast< File_LNS(*)>(_a[1]))); break;
        case 4: _t->receiveCheckedFiles((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 5: _t->rcvStatusCodeAndMessageSlot((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< unsigned short(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        case 6: _t->parseStatusFile(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ODownloadThread::*)(QList<QPair<QString,QString>> * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ODownloadThread::sendFileList)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ODownloadThread::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ODownloadThread::oDownloadStatusMessage)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ODownloadThread::*)(int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ODownloadThread::oDownloadRate)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ODownloadThread::*)(File_LNS );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ODownloadThread::parseStatusFileFinished)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ODownloadThread::staticMetaObject = { {
    QMetaObject::SuperData::link<MyThread::staticMetaObject>(),
    qt_meta_stringdata_ODownloadThread.data,
    qt_meta_data_ODownloadThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ODownloadThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ODownloadThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ODownloadThread.stringdata0))
        return static_cast<void*>(this);
    return MyThread::qt_metacast(_clname);
}

int ODownloadThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MyThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ODownloadThread::sendFileList(QList<QPair<QString,QString>> * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ODownloadThread::oDownloadStatusMessage(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ODownloadThread::oDownloadRate(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ODownloadThread::parseStatusFileFinished(File_LNS _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
