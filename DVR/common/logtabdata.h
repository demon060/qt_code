#ifndef LOGTABDATA_H
#define LOGTABDATA_H

#include <QObject>
#include "sqlitedatebase.h"
#include "hi_common.h"
#include <QMap>

class DateInfo{
public:
    uint64_t mId;
    int mChn;
    uint mCreateTime;
    QString mType;
    QString mType1;
    QString mCommit;
};


class LogTabData : public QObject,public SqliteDateBase
{
    Q_OBJECT
public:
    static LogTabData * getLogRecoedPoint();
    ~LogTabData();

    bool createTable();     //
    bool insertSignalData(DateInfo &info);
    bool insertMorelData(QList<DateInfo> &infolist);
    bool writeOneData(MLOGTYPE mtype,CLOGTYPE ctype,QString commit,int Chn =-1 );
    bool isTabExists();

private:
    explicit LogTabData(QObject *parent = nullptr);
signals:

public slots:

private:
    static LogTabData *mLogRecord;
    static QMap<CLOGTYPE,QString> mLogType;
    static QMap<MLOGTYPE,QString> mLogMType;
    quint64 mLogId;
};

#endif // LOGTABDATA_H
