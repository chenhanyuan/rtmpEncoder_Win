// RtmpEncoder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "CRtmpStream.h"
#include "UsbCamera.h"
#include "CEncoder.h"

//#define SendH264FILE
//#define SaveH264FILE
int nWidth = 1280;
int nHeight = 720;
int main(int argc,char *argv[])
{
	unsigned char *rgbData = (unsigned char *)malloc(nWidth*nHeight*3);
	unsigned int rgbSize = 0;

	/*usb Camera*/
	UsbCamera usbCamera;
	usbCamera.Init();
	usbCamera.SetUsbCamera(nWidth, nHeight);


	/*Rtmp Protocol*/
	CRtmpStream::InitSocket();
	CRtmpStream rtmpStream;
	rtmpStream.InitRtmp();

	rtmpStream.ConnectToServer("rtmp://218.207.195.169/live/data");

#ifdef SendH264FILE
	DebugPrint("Connect Success");
	if (argc < 2)
	{
		printf("argument error\n");
		system("pause");
	}
	nWidth = atoi(argv[1]);
	nHeight = atoi(argv[2]);

	rtmpStream.SendH264File(argv[3]);

#else
	/*Encode*/
	CEncoder Encode;
	CEncoder::avInit();
	Encode.AV_InitCodec(CODEC_ID_H264, nWidth, nHeight);

	int outbuf_size = 900000;  
	uint8_t * outbuf= (uint8_t*)malloc(outbuf_size); 
	uint8_t *yuv_buff = (uint8_t *) malloc((nWidth * nHeight * 3) / 2);

	//for (int i=0; i<500; i++)
	enum AVPixelFormat srcFormat, dstFormat;

	srcFormat = AV_PIX_FMT_BGR24;//camera获取的是BGR24的数据

	dstFormat = AV_PIX_FMT_YUV420P;


#ifdef SaveH264FILE
	FILE *f=NULL;   
	char * filename = "myData.h264";  
	f = fopen(filename, "ab");  
	if (!f)  
	{  
		//TRACE( "could not open %s\n", filename);  
		getchar();  
		exit(1);  
	}
#endif
	while(1)
	{
		AVPacket avpkt = {0};
		usbCamera.ReadCameraData(rgbData, rgbSize);

		avpkt.data = outbuf;  
		avpkt.size = outbuf_size; 
		int rec = Encode.Sws_ScaleHandle(&avpkt, rgbData, srcFormat, yuv_buff, dstFormat);
 
		if (rec == 1)
		{
			rtmpStream.putH264BufferToRtmpStream(avpkt.data, avpkt.size);
		}
#ifdef SaveH264FILE
		if (rec == 1)  
		{  
			fwrite(avpkt.data, 1, avpkt.size, f);  
			printf("size : %d\n", avpkt.size);
		}
		//DebugPrint("Encode size : %d", avpkt.size);
#endif
	}
#endif
	return 0;
}
