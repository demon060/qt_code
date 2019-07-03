#ifndef RECORD_H
#define RECORD_H

#include <QThread>
#include <QTimer>
#include <QList>
#include <QMutex>
#include "common/sample_common_sys.h"
#include "settings/settings.h"

class Record : public QThread
{
    Q_OBJECT
public:
    explicit Record(QObject *parent = nullptr);
    void RecordExit();

    void setRecordSrc(Sample_Common_Vpss *vpss);
    void setRecordSrc(Sample_Common_Vpss &vpss);
    void setRecordStatus(VI_CHN ViChn,bool start);
    HI_S32 startRecordChn(VI_CHN Chn,VIDEO_NORM_E enNorm);
    HI_S32 stopRecordChn(VI_CHN Chn);

    HI_BOOL addChnToRecord(VENC_CHN VencChn);
    HI_BOOL deleteChnFromRecord(VENC_CHN VencChn);
//    HI_BOOL CreatNewFile(VENC_CHN VencChn);
//    HI_BOOL Save_file_Stop(VENC_CHN VencChn);
    HI_S32 getFileSize(VENC_CHN VencChn);
    void checkFileSize();

protected:
    virtual void run();

private:
    void Init();
    HI_S32 setRecordAttr(VI_CHN ViChnCnt,PIC_SIZE_E enSize,SAMPLE_RC_E enRcMode,HI_U32 u32BitRate,HI_FR32 frmRate,HI_U32 u32Profile);
    HI_S32 startRecordChn(VI_CHN ViChnCnt, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, HI_U32 u32BitRate, HI_FR32 frmRate, HI_U32 u32Profile);

signals:
    void createNewFile(VENC_CHN VencChn);

public slots:
    HI_BOOL onCreatNewFileSlot(VENC_CHN VencChn);
    HI_BOOL onSaveFileStopSlot(VENC_CHN VencChn);
    void onTimeHander();
    void onVencAttrChangedSlot(VI_CHN Chn,HI_U32 stream);
    void onVencStatusChanged(VI_CHN Chn,bool start);
    void onViStatusChangedSlot(VI_CHN Chn,HI_BOOL status);


private:
    const HI_CHAR *VENC_PATH = "/mnt/sda1/venc";
    const HI_U32 MAXSIZE = 1024*1024*20;
    const VPSS_CHN m_VencBindVpss = VPSS_CHN0;   //主码流绑定到通道0

    typedef struct{
        VENC_CHN Venc_Chn;
        HI_S32 VencFd;
        FILE *pFile;
        int framnum;
    } Venc_Data;

    VencSet *m_VencSet;
    VIDEO_NORM_E m_enNorm;
    PAYLOAD_TYPE_E m_enType;
    PIC_SIZE_E m_enSize;
    HI_BOOL m_Venc_Run;
    QList<Venc_Data> m_VencChnPara;
    HI_S32 m_maxfd;
    QMutex m_file_mutex;
    Sample_Common_Vpss m_Vpss;
    Sample_Common_Venc *m_pVenc[VENC_MAX_CHN_NUM];
    QMap<QString,bool> m_VencStatus;
};

#endif // RECORD_H
