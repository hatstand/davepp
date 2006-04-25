/****************************************************************************
** Meta object code from reading C++ file 'resultswidget.h'
**
** Created: Tue Apr 25 17:04:49 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "resultswidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'resultswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ResultsWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      57,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     104,   96,   14,   14, 0x0a,
     153,  140,   14,   14, 0x08,
     202,   14,   14,   14, 0x08,
     226,  221,   14,   14, 0x08,
     252,   14,   14,   14, 0x08,
     267,  261,   14,   14, 0x08,
     295,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ResultsWidget[] = {
    "ResultsWidget\0\0search,sizeRestricted,isMin,size,dataType\0"
    "startSearch(QString,bool,bool,int,int)\0results\0"
    "resultArrived(QList<SearchResult*>)\0item,pos,col\0"
    "contextMenuRequested(Q3ListViewItem*,QPoint,int)\0downloadSelected()\0"
    "item\0download(Q3ListViewItem*)\0search()\0index\0"
    "sizeRestrictionChanged(int)\0browseFiles()\0"
};

const QMetaObject ResultsWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ResultsWidget,
      qt_meta_data_ResultsWidget, 0 }
};

const QMetaObject *ResultsWidget::metaObject() const
{
    return &staticMetaObject;
}

void *ResultsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ResultsWidget))
	return static_cast<void*>(const_cast<ResultsWidget*>(this));
    if (!strcmp(_clname, "Ui::UIResultsWidget"))
	return static_cast<Ui::UIResultsWidget*>(const_cast<ResultsWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ResultsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startSearch(*reinterpret_cast< QString(*)>(_a[1]),*reinterpret_cast< bool(*)>(_a[2]),*reinterpret_cast< bool(*)>(_a[3]),*reinterpret_cast< int(*)>(_a[4]),*reinterpret_cast< int(*)>(_a[5])); break;
        case 1: resultArrived(*reinterpret_cast< QList<SearchResult*>(*)>(_a[1])); break;
        case 2: contextMenuRequested(*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2]),*reinterpret_cast< int(*)>(_a[3])); break;
        case 3: downloadSelected(); break;
        case 4: download(*reinterpret_cast< Q3ListViewItem*(*)>(_a[1])); break;
        case 5: search(); break;
        case 6: sizeRestrictionChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 7: browseFiles(); break;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ResultsWidget::startSearch(QString _t1, bool _t2, bool _t3, int _t4, int _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
