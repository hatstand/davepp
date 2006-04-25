/****************************************************************************
** Meta object code from reading C++ file 'clientconnector.h'
**
** Created: Tue Apr 25 17:04:40 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "clientconnector.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientconnector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ClientConnector[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      32,   17,   16,   16, 0x05,
      52,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      66,   16,   16,   16, 0x08,
      84,   16,   16,   16, 0x08,
     106,  102,   16,   16, 0x08,
     152,  148,   16,   16, 0x08,
     179,   16,   16,   16, 0x08,
     200,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ClientConnector[] = {
    "ClientConnector\0\0size,totalSize\0progress(uint,uint)\0infoChanged()\0"
    "socketHostFound()\0socketConnected()\0err\0"
    "socketError(QAbstractSocket::SocketError)\0num\0"
    "socketBytesWritten(qint64)\0socketDisconnected()\0sendSomeData()\0"
};

const QMetaObject ClientConnector::staticMetaObject = {
    { &Client::staticMetaObject, qt_meta_stringdata_ClientConnector,
      qt_meta_data_ClientConnector, 0 }
};

const QMetaObject *ClientConnector::metaObject() const
{
    return &staticMetaObject;
}

void *ClientConnector::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClientConnector))
	return static_cast<void*>(const_cast<ClientConnector*>(this));
    return Client::qt_metacast(_clname);
}

int ClientConnector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Client::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: progress(*reinterpret_cast< uint(*)>(_a[1]),*reinterpret_cast< uint(*)>(_a[2])); break;
        case 1: infoChanged(); break;
        case 2: socketHostFound(); break;
        case 3: socketConnected(); break;
        case 4: socketError(*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])); break;
        case 5: socketBytesWritten(*reinterpret_cast< qint64(*)>(_a[1])); break;
        case 6: socketDisconnected(); break;
        case 7: sendSomeData(); break;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ClientConnector::progress(uint _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClientConnector::infoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
