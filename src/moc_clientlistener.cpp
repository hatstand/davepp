/****************************************************************************
** Meta object code from reading C++ file 'clientlistener.h'
**
** Created: Tue Apr 25 17:04:41 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "clientlistener.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientlistener.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ClientListener[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      21,   16,   15,   15, 0x05,
      48,   33,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      68,   15,   15,   15, 0x08,
      84,   15,   15,   15, 0x08,
     102,   15,   15,   15, 0x08,
     124,  120,   15,   15, 0x08,
     170,  166,   15,   15, 0x08,
     197,   15,   15,   15, 0x08,
     218,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ClientListener[] = {
    "ClientListener\0\0code\0result(int)\0size,totalSize\0"
    "progress(uint,uint)\0newConnection()\0socketHostFound()\0"
    "socketConnected()\0err\0socketError(QAbstractSocket::SocketError)\0num\0"
    "socketBytesWritten(qint64)\0socketDisconnected()\0socketReadyRead()\0"
};

const QMetaObject ClientListener::staticMetaObject = {
    { &Client::staticMetaObject, qt_meta_stringdata_ClientListener,
      qt_meta_data_ClientListener, 0 }
};

const QMetaObject *ClientListener::metaObject() const
{
    return &staticMetaObject;
}

void *ClientListener::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClientListener))
	return static_cast<void*>(const_cast<ClientListener*>(this));
    return Client::qt_metacast(_clname);
}

int ClientListener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Client::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: result(*reinterpret_cast< int(*)>(_a[1])); break;
        case 1: progress(*reinterpret_cast< uint(*)>(_a[1]),*reinterpret_cast< uint(*)>(_a[2])); break;
        case 2: newConnection(); break;
        case 3: socketHostFound(); break;
        case 4: socketConnected(); break;
        case 5: socketError(*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])); break;
        case 6: socketBytesWritten(*reinterpret_cast< qint64(*)>(_a[1])); break;
        case 7: socketDisconnected(); break;
        case 8: socketReadyRead(); break;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ClientListener::result(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClientListener::progress(uint _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
