/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Tue Apr 25 17:04:35 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      28,   11,   11,   11, 0x08,
      47,   11,   11,   11, 0x08,
      67,   11,   11,   11, 0x08,
      89,   84,   11,   11, 0x08,
     128,   84,   11,   11, 0x08,
     171,  160,   11,   11, 0x08,
     209,   11,   11,   11, 0x08,
     237,  230,   11,   11, 0x08,
     263,  160,   11,   11, 0x08,
     302,   11,   11,   11, 0x08,
     320,   11,   11,   11, 0x08,
     334,   11,   11,   11, 0x08,
     356,  348,   11,   11, 0x08,
     385,   11,   11,   11, 0x08,
     404,   11,   11,   11, 0x08,
     418,   11,   11,   11, 0x08,
     433,   11,   11,   11, 0x08,
     457,  450,   11,   11, 0x08,
     527,  485,   11,   11, 0x08,
     566,   11,   11,   11, 0x08,
     578,   11,   11,   11, 0x08,
     608,  596,   11,   11, 0x08,
     641,   11,   11,   11, 0x08,
     660,   11,   11,   11, 0x08,
     675,   11,   11,   11, 0x08,
     700,  693,   11,   11, 0x08,
     743,   11,   11,   11, 0x08,
     760,   11,   11,   11, 0x08,
     777,   11,   11,   11, 0x08,
     791,   11,   11,   11, 0x08,
     817,  808,   11,   11, 0x08,
     846,  841,   11,   11, 0x08,
     864,  841,   11,   11, 0x08,
     887,  841,   11,   11, 0x08,
     903,  841,   11,   11, 0x08,
     940,  930,   11,   11, 0x08,
     972,   11,   11,   11, 0x08,
    1008,  991,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0newHubClicked()\0deleteHubClicked()\0connectHubClicked()\0"
    "editHubClicked()\0item\0hubListCurrentChanged(Q3ListViewItem*)\0"
    "hubDoubleClick(Q3ListViewItem*)\0item,point\0"
    "hubRightClick(Q3ListViewItem*,QPoint)\0userOptionsClicked()\0action\0"
    "userSortChanged(QAction*)\0userRightClick(Q3ListViewItem*,QPoint)\0"
    "browseUserFiles()\0hideNoSlots()\0hideNoFiles()\0index,p\0"
    "hubTabRightClick(int,QPoint)\0hubTabDisconnect()\0hubTabClose()\0"
    "chatTabClose()\0resortUserList()\0widget\0newPrivateChat(ChatWidget*)\0"
    "search,sizeRestricted,isMin,size,dataType\0"
    "startSearch(QString,bool,bool,int,int)\0configure()\0configureAccept()\0"
    "index,point\0resultsTabRightClick(int,QPoint)\0resultsNewSearch()\0"
    "resultsClose()\0resultsCloseAll()\0,point\0"
    "transferRightClick(Q3ListViewItem*,QPoint)\0transferResume()\0"
    "transferCancel()\0transferTop()\0transferBottom()\0newState\0"
    "serverStateChanged(int)\0user\0userJoined(User*)\0"
    "userInfoChanged(User*)\0userQuit(User*)\0userFileListUpdated(User*)\0"
    "connector\0uploadRequest(ClientConnector*)\0updateStatusText()\0"
    "value,totalSteps\0builderProgress(uint,uint)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
	return static_cast<void*>(const_cast<MainWindow*>(this));
    if (!strcmp(_clname, "Ui::MainWindow"))
	return static_cast<Ui::MainWindow*>(const_cast<MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newHubClicked(); break;
        case 1: deleteHubClicked(); break;
        case 2: connectHubClicked(); break;
        case 3: editHubClicked(); break;
        case 4: hubListCurrentChanged(*reinterpret_cast< Q3ListViewItem*(*)>(_a[1])); break;
        case 5: hubDoubleClick(*reinterpret_cast< Q3ListViewItem*(*)>(_a[1])); break;
        case 6: hubRightClick(*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2])); break;
        case 7: userOptionsClicked(); break;
        case 8: userSortChanged(*reinterpret_cast< QAction*(*)>(_a[1])); break;
        case 9: userRightClick(*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2])); break;
        case 10: browseUserFiles(); break;
        case 11: hideNoSlots(); break;
        case 12: hideNoFiles(); break;
        case 13: hubTabRightClick(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2])); break;
        case 14: hubTabDisconnect(); break;
        case 15: hubTabClose(); break;
        case 16: chatTabClose(); break;
        case 17: resortUserList(); break;
        case 18: newPrivateChat(*reinterpret_cast< ChatWidget*(*)>(_a[1])); break;
        case 19: startSearch(*reinterpret_cast< QString(*)>(_a[1]),*reinterpret_cast< bool(*)>(_a[2]),*reinterpret_cast< bool(*)>(_a[3]),*reinterpret_cast< int(*)>(_a[4]),*reinterpret_cast< int(*)>(_a[5])); break;
        case 20: configure(); break;
        case 21: configureAccept(); break;
        case 22: resultsTabRightClick(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2])); break;
        case 23: resultsNewSearch(); break;
        case 24: resultsClose(); break;
        case 25: resultsCloseAll(); break;
        case 26: transferRightClick(*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]),*reinterpret_cast< const QPoint(*)>(_a[2])); break;
        case 27: transferResume(); break;
        case 28: transferCancel(); break;
        case 29: transferTop(); break;
        case 30: transferBottom(); break;
        case 31: serverStateChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 32: userJoined(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 33: userInfoChanged(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 34: userQuit(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 35: userFileListUpdated(*reinterpret_cast< User*(*)>(_a[1])); break;
        case 36: uploadRequest(*reinterpret_cast< ClientConnector*(*)>(_a[1])); break;
        case 37: updateStatusText(); break;
        case 38: builderProgress(*reinterpret_cast< uint(*)>(_a[1]),*reinterpret_cast< uint(*)>(_a[2])); break;
        }
        _id -= 39;
    }
    return _id;
}
