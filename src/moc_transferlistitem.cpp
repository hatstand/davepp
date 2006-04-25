/****************************************************************************
** Meta object code from reading C++ file 'transferlistitem.h'
**
** Created: Tue Apr 25 17:04:51 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "transferlistitem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'transferlistitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_TransferListItem[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   18,   17,   17, 0x08,
      55,   48,   17,   17, 0x08,
      88,   73,   17,   17, 0x08,
     114,   17,   17,   17, 0x08,
     132,   17,   17,   17, 0x08,
     152,  147,   17,   17, 0x08,
     168,   17,   17,   17, 0x08,
     194,  188,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TransferListItem[] = {
    "TransferListItem\0\0state\0clientStateChanged(int)\0result\0"
    "clientResult(int)\0size,totalSize\0clientProgress(uint,uint)\0"
    "clientDestroyed()\0doNextQueued()\0user\0userQuit(User*)\0"
    "clientInfoChanged()\0speed\0clientSpeedChanged(quint64)\0"
};

const QMetaObject TransferListItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TransferListItem,
      qt_meta_data_TransferListItem, 0 }
};

const QMetaObject *TransferListItem::metaObject() const
{
    return &staticMetaObject;
}

void *TransferListItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TransferListItem))
	return static_cast<void*>(const_cast<TransferListItem*>(this));
    if (!strcmp(_clname, "Q3ListViewItem"))
	return static_cast<Q3ListViewItem*>(const_cast<TransferListItem*>(this));
    return QObject::qt_metacast(_clname);
}

int TransferListItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clientStateChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 1: clientResult(*reinterpret_cast< int(*)>(_a[1])); break;
        case 2: clientProgress(*reinterpret_cast< uint(*)>(_a[1]),*reinterpret_cast< uint(*)>(_a[2])); break;
        case 3: clientDestroyed(); break;
        case 4: doNextQueued(); break;
        case 5: userQuit(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 6: clientInfoChanged(); break;
        case 7: clientSpeedChanged(*reinterpret_cast< quint64(*)>(_a[1])); break;
        }
        _id -= 8;
    }
    return _id;
}
