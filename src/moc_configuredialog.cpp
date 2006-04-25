/****************************************************************************
** Meta object code from reading C++ file 'configuredialog.h'
**
** Created: Tue Apr 25 17:04:54 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "configuredialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'configuredialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ConfigureDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      26,   16,   16,   16, 0x0a,
      33,   16,   16,   16, 0x08,
      44,   16,   16,   16, 0x08,
      58,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ConfigureDialog[] = {
    "ConfigureDialog\0\0accept()\0show()\0shareAdd()\0shareRemove()\0"
    "shareEdit()\0"
};

const QMetaObject ConfigureDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ConfigureDialog,
      qt_meta_data_ConfigureDialog, 0 }
};

const QMetaObject *ConfigureDialog::metaObject() const
{
    return &staticMetaObject;
}

void *ConfigureDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConfigureDialog))
	return static_cast<void*>(const_cast<ConfigureDialog*>(this));
    if (!strcmp(_clname, "Ui::UIConfigureDialog"))
	return static_cast<Ui::UIConfigureDialog*>(const_cast<ConfigureDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ConfigureDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: show(); break;
        case 2: shareAdd(); break;
        case 3: shareRemove(); break;
        case 4: shareEdit(); break;
        }
        _id -= 5;
    }
    return _id;
}
