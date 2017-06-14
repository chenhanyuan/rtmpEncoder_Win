#ifndef _CRTMPSTREAM_H_
#define _CRTMPSTREAM_H_

#include "winsock2.h"  

#ifdef WIN32     
#include <windows.h>  
#pragma comment(lib,"WS2_32.lib")   
#pragma comment(lib,"winmm.lib")  
#endif 

#include "rtmp.h"
#include "amf.h"
#include "rtmp_sys.h"
#include "Common.h"

class CRtmpStream
{
public:
	CRtmpStream();
	~CRtmpStream();
	static void InitSocket();
	static void CleanupSockets();

	int InitRtmp();//rtmp初始化
public:
	bool ConnectToServer(char *url);
	void DisconnectToServer();
	bool SendAMFPacket(LPRTMPMetadata lpMetaData);
	bool SendPacket(unsigned int nTagType,unsigned char *data,unsigned int size,unsigned int nTimestamp);  
	bool SendVideoSpsPpsPacket(unsigned char *pps,int pps_len,unsigned char * sps,int sps_len);
	bool SendH264Packet(unsigned char *data,unsigned int size,int bIsKeyFrame,unsigned int nTimeStamp);

public://file referent 可读取h264文件进行发送
	bool SendH264File(char *filename);
	bool Geth264FileSpsPpsData(unsigned char *fileData, int fileLength, LPRTMPMetadata pMetaData);
	//bool parseSpsData(LPRTMPMetadata pMetaData);
	int ReadOneNaluFromBuf(NaluUnit &nalu, unsigned char *data, int dataLength);//h264数据获取每一个Nalu，可以处理文件也可以处理缓存

public://H264 缓存数据的操作
	bool putH264BufferToRtmpStream(unsigned char *h264Buffer, unsigned int h264size);
public:
	RTMP *rtmp;
	RTMPMetadata metaData;
	unsigned char *h264FileBuffer;
	int h264FileLength;
};

#endif