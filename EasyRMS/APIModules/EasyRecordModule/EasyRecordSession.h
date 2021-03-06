/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
/*
    File:       EasyRecordSession.h
    Contains:   HLS
*/
#include "QTSS.h"
#include "OSRef.h"
#include "StrPtrLen.h"
#include "ResizeableStringFormatter.h"
#include "MyAssert.h"

#include "SourceInfo.h"
#include "OSArrayObjectDeleter.h"
#include "EasyRTSPClientAPI.h"
#include "EasyRecordAPI.h"

#include "TimeoutTask.h"

#ifndef __EASY_RECORD_SESSION__
#define __EASY_RECORD_SESSION__

#define QTSS_MAX_URL_LENGTH	512

class EasyRecordSession : public Task
{
    public:
        EasyRecordSession(StrPtrLen* inSourceID);
        virtual ~EasyRecordSession();
        
        //加载模块配置
        static void Initialize(QTSS_ModulePrefsObject inPrefs);

		virtual SInt64	Run();
        // ACCESSORS

        OSRef*          GetRef()            { return &fRef; }
        OSQueueElem*    GetQueueElem()      { return &fQueueElem; }
	
        StrPtrLen*      GetSessionID()     { return &fHLSSessionID; }
		QTSS_Error		ProcessData(int _chid, int mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo);
		QTSS_Error		HLSSessionStart(char* rtspUrl, UInt32 inTimeout);
		QTSS_Error		HLSSessionRelease();
		char*			GetHLSURL();
		char*			GetSourceURL();

		void RefreshTimeout()	{ fTimeoutTask.RefreshTimeout(); }

		//统计
		SInt64          GetTotalPlayTime()		const { return fTotalPlayTime; }
		SInt64			GetNumPacketsReceived() const { return fNumPacketsReceived; }
		SInt64			GetNumBytesReceived()	const { return fNumBytesReceived; }
		UInt32			GetLastStatBitrate()	const { return fLastStatBitrate; }

		const char*		TimeToString(UInt64 inTime);//return string with formatYYYYMMDDhhmmss
		bool			TryCreateNewRecord();
   
public:
		static UInt32	sM3U8Version;
		static Bool16	sAllowCache;
		static UInt32	sTargetDuration;
		static UInt32	sPlaylistCapacity;
		static char*	sHTTPRootDir;
		static char*	sOSSBucketName;
		static char*	sOSSEndpoint;
		static UInt32	sOSSPort;
		static char*	sOSSAccessKeyID;
		static char*	sOSSAccessKeySecret;
		static UInt32	sRecordDuration;
		static char*	sLocalRecordPath;
		static UInt32	sRecordToWhere;	//0-OSS 1-local

    private:

        //HLSSession列表由EasyRecordModule的sHLSSessionMap维护  
        OSRef       fRef;
        StrPtrLen   fHLSSessionID;
		char		fHLSURL[QTSS_MAX_URL_LENGTH];
		char		fSourceURL[QTSS_MAX_URL_LENGTH];
        OSQueueElem fQueueElem; 

		//RTSPClient Handle
		Easy_RTSP_Handle	fRTSPClientHandle;
		//HLS Handle
		Easy_Record_Handle fRecordHandle;
		
		//TS timestamp ms，自定义时间戳
		int tsTimeStampMSsec;

		
		//统计
		SInt64          fPlayTime;				//起始的时间
		SInt64			fLastStatPlayTime;		//上一次统计的时间

        SInt64          fTotalPlayTime;			//总共播放时间

        SInt64			fNumPacketsReceived;	//收到的数据包的数量
		SInt64			fLastNumPacketsReceived;//上一次统计收到的数据包数量

        SInt64			fNumBytesReceived;		//收到的数据总量
        SInt64			fLastNumBytesReceived;	//上一次统计收到的数据总量

		UInt32			fLastStatBitrate;		//最后一次统计得到的比特率

		UInt64			fLastRecordTime;		//上次启动新录像的时间
		OSMutex         fRecordMutex;

	protected:
		TimeoutTask		fTimeoutTask;
};

#endif

