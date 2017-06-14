#ifndef _USBCAMERA_H_
#define _USBCAMERA_H_

#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

class UsbCamera
{
public:
	UsbCamera();
	~UsbCamera();
public:

	cv::VideoCapture cap;
	//cv::VideoCapture cap("bike.avi"); 可直接读取文件
	cv::Mat frame;
	bool Init();
	bool SetUsbCamera(int nWidth, int nHeight);//设置分辨率等信息
	bool ReadCameraData(unsigned char *rgbData, unsigned int &rgbSize);

	bool saveBmpImage(IplImage &pFrame);//usb camera 获取出来的数据是rgb 24位 
};


#endif