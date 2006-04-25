/****************************************************************************
** Meta object code from reading C++ file 'hubwidget.h'
**
** Created: Tue Apr 25 17:04:39 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "hubwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hubwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HubWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      18,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   10,   10,   10, 0x0a,
      66,   11,   10,   10, 0x0a,
     115,   97,   10,   10, 0x08,
     155,  149,   10,   10, 0x08,
     181,  173,   10,   10, 0x08,
     196,   10,   10,   10, 0x08,
     214,   10,   10,   10, 0x08,
     230,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HubWidget[] = {
    "HubWidget\0\0widget\0newPrivateChat(ChatWidget*)\0disconnectPressed()\0"
    "privateChatClosed(ChatWidget*)\0from,message,priv\0"
    "chatMessage(QString,QString,bool)\0state\0stateChanged(int)\0message\0"
    "error(QString)\0userJoined(User*)\0userQuit(User*)\0sendPressed()\0"
};

const QMetaObject HubWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_HubWidget,
      qt_meta_data_HubWidget, 0 }
};

const QMetaObject *HubWidget::metaObject() const
{
    return &staticMetaObject;
}

void *HubWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HubWidget))
	return static_cast<void*>(const_cast<HubWidget*>(this));
    if (!strcmp(_clname, "Ui::UIHubWidget"))
	return static_cast<Ui::UIHubWidget*>(const_cast<HubWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int HubWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newPrivateChat(*reinterpret_cast< ChatWidget*(*)>(_a[1])); break;
        case 1: disconnectPressed(); break;
        case 2: privateChatClosed(*reinterpret_cast< ChatWidget*(*)>(_a[1])); break;
        case 3: chatMessage(*reinterpret_cast< QString(*)>(_a[1]),*reinterpret_cast< QString(*)>(_a[2]),*reinterpret_cast< bool(*)>(_a[3])); break;
        case 4: stateChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 5: error(*reinterpret_cast< QString(*)>(_a[1])); break;
        case 6: userJoined(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 7: userQuit(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 8: sendPressed(); break;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void HubWidget::newPrivateChat(ChatWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
