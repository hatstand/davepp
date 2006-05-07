#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef CAMBRIDGE

static const QString GETLISTLEN = "$TEGListLen";
static const QString LISTLEN = "$SILtLen";
static const QString MYNICK = "$NYMick";
static const QString MAXEDOUT = "$XAMedOut";
static const QString SUPPORTS = "$PUSports";
static const QString FILELENGTH = "$LIFeLength";
static const QString GET = "$TEG";
static const QString SEND = "$NESd";
static const QString CANCELED = "$NACceled";
static const QString DIRECTION = "$RIDection";
static const QString GETZBLOCK = "$TEGZBlock";
static const QString UGETZBLOCK = "$EGUtZBlock";
static const QString UGETBLOCK = "$EGUtBlock";

#else

static const QString GETLISTLEN = "$GetListLen";
static const QString LISTLEN = "$ListtLen";
static const QString MYNICK = "$MyNick";
static const QString MAXEDOUT = "$MaxedOut";
static const QString SUPPORTS = "$Supports";
static const QString FILELENGTH = "$FileLength";
static const QString GET = "$Get";
static const QString SEND = "$Send";
static const QString CANCELED = "$Canceled";
static const QString DIRECTION = "$Direction";
static const QString GETZBLOCK = "$GetZBlock";
static const QString UGETZBLOCK = "$UGetZBlock";
static const QString UGETBLOCK = "$UGetBlock";

#endif

#endif
