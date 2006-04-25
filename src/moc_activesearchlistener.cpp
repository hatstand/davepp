/****************************************************************************
** Meta object code from reading C++ file 'activesearchlistener.h'
**
** Created: Tue Apr 25 17:04:53 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "activesearchlistener.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'activesearchlistener.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ActiveSearchListener[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      30,   22,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      66,   21,   21,   21, 0x08,
      76,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ActiveSearchListener[] = {
    "ActiveSearchListener\0\0results\0resultArrived(QList<SearchResult*>)\0"
    "timeout()\0socketReadyRead()\0"
};

const QMetaObject ActiveSearchListener::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ActiveSearchListener,
      qt_meta_data_ActiveSearchListener, 0 }
};

const QMetaObject *ActiveSearchListener::metaObject() const
{
    return &staticMetaObject;
}

void *ActiveSearchListener::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ActiveSearchListener))
	return static_cast<void*>(const_cast<ActiveSearchListener*>(this));
    return QObject::qt_metacast(_clname);
}

int ActiveSearchListener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resultArrived(*reinterpret_cast< QList<SearchResult*>(*)>(_a[1])); break;
        case 1: timeout(); break;
        case 2: socketReadyRead(); break;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ActiveSearchListener::resultArrived(QList<SearchResult*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
