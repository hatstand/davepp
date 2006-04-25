/****************************************************************************
** Meta object code from reading C++ file 'davetabwidget.h'
**
** Created: Tue Apr 25 17:04:44 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "davetabwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'davetabwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_DaveTabBar[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   12,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DaveTabBar[] = {
    "DaveTabBar\0\0index,\0tabRightClicked(int,QPoint)\0"
};

const QMetaObject DaveTabBar::staticMetaObject = {
    { &QTabBar::staticMetaObject, qt_meta_stringdata_DaveTabBar,
      qt_meta_data_DaveTabBar, 0 }
};

const QMetaObject *DaveTabBar::metaObject() const
{
    return &staticMetaObject;
}

void *DaveTabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DaveTabBar))
	return static_cast<void*>(const_cast<DaveTabBar*>(this));
    return QTabBar::qt_metacast(_clname);
}

int DaveTabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: tabRightClicked(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2])); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DaveTabBar::tabRightClicked(int _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_DaveTabWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      22,   15,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DaveTabWidget[] = {
    "DaveTabWidget\0\0index,\0tabRightClicked(int,QPoint)\0"
};

const QMetaObject DaveTabWidget::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_DaveTabWidget,
      qt_meta_data_DaveTabWidget, 0 }
};

const QMetaObject *DaveTabWidget::metaObject() const
{
    return &staticMetaObject;
}

void *DaveTabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DaveTabWidget))
	return static_cast<void*>(const_cast<DaveTabWidget*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int DaveTabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: tabRightClicked(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2])); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DaveTabWidget::tabRightClicked(int _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
