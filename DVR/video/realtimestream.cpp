#include "realtimestream.h"
#include <QDebug>


RealTimeStream::RealTimeStream(QObject *parent) : QThread(parent)
{

}

RealTimeStream::~RealTimeStream()
{
    mRun = false;
    wait();
    qDebug()<<"exit RealTimeStream thread";
}


void RealTimeStream::run()
{
    HI_S32 i = 0;
    int maxFd;
    struct timeval TimeoutVal;
    fd_set read_fds;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    int VencChn;
    int ViChn = 0;
    QList<uint>start;
    QList<uint>end;

    mRun = true;

    FILE* vencfile = fopen("realtimeStream.h264","wb");
    if(!vencfile){
        qDebug()<<"open file error";
        return;
    }

    start.append(0);
    start.append(0);
    start.append(0);
    start.append(0);
    end.append(1565308799);
    end.append(1565308799);
    end.append(1565308799);
    end.append(1565308799);


    mStreamPro.setTime(start,end,VIDEO_NORMAL);
    if(!mStreamPro.startVenc(ViChn,/*REALTIME2*/LOCALMODE2)){
        return;
    }

    maxFd = mStreamPro.getVencFd();
    VencChn = mStreamPro.getVencChn();
    qDebug()<<"fd:"<<maxFd<<" VencChn:"<<VencChn;
    while (mRun) {

        FD_ZERO(&read_fds);
        FD_SET(maxFd, &read_fds);

        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxFd + 1, &read_fds, nullptr, nullptr, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
//            continue;
        }
        else
        {


            {
                if (FD_ISSET(maxFd, &read_fds))
                {
                    /*******************************************************
                    step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));
                    s32Ret = HI_MPI_VENC_Query(VencChn, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }
                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:					 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {						SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                     *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (nullptr == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(VencChn, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = nullptr;
                        SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }
                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/

                    s32Ret = Sample_Common_Venc::SAMPLE_COMM_VENC_SaveStream(PT_H264, vencfile, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = nullptr;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.6 : release stream
                     *******************************************************/
                    s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = nullptr;
                        break;
                    }
                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = nullptr;
                }
            }
        }
        usleep(20000);
    }

    fclose(vencfile);

    mStreamPro.stopVenc(ViChn);
}