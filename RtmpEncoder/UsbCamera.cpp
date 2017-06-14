#include "StdAfx.h"
#include "UsbCamera.h"
//#include <windows.h>
//#include <windef.h>
//#include <Winuser.h>
//#include <WinGDI.h>
#include <atlimage.h>
#include "Common.h"

UsbCamera::UsbCamera()
{

}
UsbCamera::~UsbCamera()
{

}

bool UsbCamera::Init()
{
	if(!cap.open(0))
		return false;
	for(int i=0;i<50;i++)
		cap>>frame;
	return true;
}

bool UsbCamera::SetUsbCamera(int nWidth, int nHeight)
{
	int width = cap.get(CV_CAP_PROP_FRAME_WIDTH );
	int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT );
	std::cout<< "width:" << width << std::endl;
	std::cout<< "height:" << height << std::endl;
	cap.set(CV_CAP_PROP_FRAME_WIDTH,nWidth);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,nHeight);
	width = cap.get(CV_CAP_PROP_FRAME_WIDTH );
	height = cap.get(CV_CAP_PROP_FRAME_HEIGHT );
	std::cout<< "width:" << width << std::endl;
	std::cout<< "height:" << height << std::endl;
	return true;
}

bool UsbCamera::ReadCameraData(unsigned char *rgbData, unsigned int &rgbSize)
{
	IplImage pFrame;  

	//cap>>frame;
	cv::waitKey(25);//帧率控制
	cap.read(frame);
	pFrame = frame;
	//cvShowImage("video", &pFrame);
	memcpy(rgbData, pFrame.imageData, pFrame.imageSize);
	rgbSize = pFrame.imageSize;



	//循环读取
	/*double fps;
	char string[50];  // 用于存放帧率的字符串
	double t = 0;
	IplImage pFrame;  
	bool stop = false;
	while(!stop)
	{
		//cap>>frame;
		t = (double)cv::getTickCount();
		cv::waitKey(25);//帧率控制
		cap.read(frame);
		pFrame = frame;
		cvShowImage("video", &pFrame);
		saveBmpImage(pFrame);
		if(cv::waitKey(25) >=0)
			stop = true;
	}*/


	return true;
}

bool UsbCamera::saveBmpImage(IplImage &pFrame)
{
	BITMAPFILEHEADER bf;    
	bf.bfType      = 0x4D42;                  
	//BM    
	bf.bfSize      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pFrame.imageSize;     
	bf.bfReserved1 = 0;     
	bf.bfReserved2 = 0;     
	bf.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);     
	//位图信息头    
	BITMAPINFOHEADER bi;    
	bi.biSize          = sizeof(BITMAPINFOHEADER);    
	bi.biWidth         = 1280;
	bi.biHeight        = -720; 
	bi.biPlanes        = 1;    
	bi.biBitCount      = 24; 
	bi.biCompression   = BI_RGB;
	bi.biSizeImage     = pFrame.imageSize;    
	bi.biXPelsPerMeter = 0;    
	bi.biYPelsPerMeter = 0;    
	bi.biClrUsed       = 0;    
	bi.biClrImportant  = 0;    
	//位图数据;    
	char path[16]={0};
	static int i=0;
	sprintf(path, "screen%d.bmp", i++);
	FILE* f = fopen(path,"wb");    
	fwrite((char*)&bf,sizeof(BITMAPFILEHEADER),1,f);    
	fwrite((char*)&bi,sizeof(BITMAPINFOHEADER),1,f);    
	fwrite(pFrame.imageData, pFrame.imageSize,1,f);    
	fclose(f); 

	return true;
}

