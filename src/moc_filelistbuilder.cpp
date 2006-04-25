/****************************************************************************
** Meta object code from reading C++ file 'filelistbuilder.h'
**
** Created: Tue Apr 25 17:04:58 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "filelistbuilder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filelistbuilder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_FileListBuilder[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      29,   17,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_FileListBuilder[] = {
    "FileListBuilder\0\0value,total\0progress(uint,uint)\0"
};

const QMetaObject FileListBuilder::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_FileListBuilder,
      qt_meta_data_FileListBuilder, 0 }
};

const QMetaObject *FileListBuilder::metaObject() const
{
    return &staticMetaObject;
}

void *FileListBuilder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FileListBuilder))
	return static_cast<void*>(const_cast<FileListBuilder*>(this));
    return QThread::qt_metacast(_clname);
}

int FileListBuilder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: progress(*reinterpret_cast< uint(*)>(_a[1]),*reinterpret_cast< uint(*)>(_a[2])); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void FileListBuilder::progress(uint _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
