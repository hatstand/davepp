/****************************************************************************
** Meta object code from reading C++ file 'client.h'
**
** Created: Tue Apr 25 17:04:43 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "client.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Client[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,    8,    7,    7, 0x05,
      41,   35,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,    7,    7,    7, 0x09,
      81,    7,    7,    7, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_Client[] = {
    "Client\0\0newState\0stateChanged(int)\0speed\0speedChanged(quint64)\0"
    "socketReadyRead()\0speedTimerTimeout()\0"
};

const QMetaObject Client::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Client,
      qt_meta_data_Client, 0 }
};

const QMetaObject *Client::metaObject() const
{
    return &staticMetaObject;
}

void *Client::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Client))
	return static_cast<void*>(const_cast<Client*>(this));
    return QObject::qt_metacast(_clname);
}

int Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stateChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 1: speedChanged(*reinterpret_cast< quint64(*)>(_a[1])); break;
        case 2: socketReadyRead(); break;
        case 3: speedTimerTimeout(); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Client::stateChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Client::speedChanged(quint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
