/****************************************************************************
** Meta object code from reading C++ file 'server.h'
**
** Created: Tue Apr 25 17:04:33 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "server.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'server.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Server[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,    8,    7,    7, 0x05,
      40,   35,    7,    7, 0x05,
      58,   35,    7,    7, 0x05,
      81,   35,    7,    7, 0x05,
     115,   97,    7,    7, 0x05,
     157,  149,    7,    7, 0x05,
     172,   35,    7,    7, 0x05,
     205,  195,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
     237,    7,    7,    7, 0x08,
     255,    7,    7,    7, 0x08,
     277,  273,    7,    7, 0x08,
     319,    7,    7,    7, 0x08,
     341,  337,    7,    7, 0x08,
     368,    7,    7,    7, 0x08,
     389,    7,    7,    7, 0x08,
     417,  410,    7,    7, 0x08,
     443,  437,    7,    7, 0x08,
     469,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Server[] = {
    "Server\0\0newState\0stateChanged(int)\0user\0userJoined(User*)\0"
    "userInfoChanged(User*)\0userQuit(User*)\0from,message,priv\0"
    "chatMessage(QString,QString,bool)\0message\0error(QString)\0"
    "fileListUpdated(User*)\0connector\0uploadRequest(ClientConnector*)\0"
    "socketHostFound()\0socketConnected()\0err\0"
    "socketError(QAbstractSocket::SocketError)\0socketReadyRead()\0num\0"
    "socketBytesWritten(qint64)\0socketDisconnected()\0udpsocketReadyRead()\0"
    "result\0listenerResult(int)\0state\0listenerStateChanged(int)\0"
    "fileListUpdateFinished()\0"
};

const QMetaObject Server::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Server,
      qt_meta_data_Server, 0 }
};

const QMetaObject *Server::metaObject() const
{
    return &staticMetaObject;
}

void *Server::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Server))
	return static_cast<void*>(const_cast<Server*>(this));
    return QObject::qt_metacast(_clname);
}

int Server::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stateChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 1: userJoined(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 2: userInfoChanged(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 3: userQuit(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 4: chatMessage(*reinterpret_cast< QString(*)>(_a[1]),*reinterpret_cast< QString(*)>(_a[2]),*reinterpret_cast< bool(*)>(_a[3])); break;
        case 5: error(*reinterpret_cast< QString(*)>(_a[1])); break;
        case 6: fileListUpdated(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 7: uploadRequest(*reinterpret_cast< ClientConnector*(*)>(_a[1])); break;
        case 8: socketHostFound(); break;
        case 9: socketConnected(); break;
        case 10: socketError(*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])); break;
        case 11: socketReadyRead(); break;
        case 12: socketBytesWritten(*reinterpret_cast< qint64(*)>(_a[1])); break;
        case 13: socketDisconnected(); break;
        case 14: udpsocketReadyRead(); break;
        case 15: listenerResult(*reinterpret_cast< int(*)>(_a[1])); break;
        case 16: listenerStateChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 17: fileListUpdateFinished(); break;
        }
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void Server::stateChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Server::userJoined(User * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Server::userInfoChanged(User * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Server::userQuit(User * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Server::chatMessage(QString _t1, QString _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Server::error(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Server::fileListUpdated(User * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Server::uploadRequest(ClientConnector * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
