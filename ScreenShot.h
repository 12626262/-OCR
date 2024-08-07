#pragma once
#include <opencv2/opencv.hpp>
#include<string>
#include<atlimage.h>
#include <WinNls.h>
#include<Windows.h>


using namespace std;
using namespace cv;

Mat srcImage;
Rect roirect;
Point startPoint;
Point endPoint;
//name:保存的文件名
//hWnd:要截图的窗口句柄,NULL表示对桌面截图
bool SavePic(wstring name, HWND hWnd) {
	//SetProcessDPIAware();//刷新DPI
	HDC hDc = NULL;
	hWnd = (hWnd == NULL) ? GetDesktopWindow() : hWnd; //判断窗口句柄是否为NULL,如果为NULL则默认获取桌面DC
	hDc = GetDC(hWnd); //获取DC
	if (hDc == NULL) return false;
	int bitOfPix = GetDeviceCaps(hDc, BITSPIXEL); //获取DC像素的大小
	int width = GetDeviceCaps(hDc, HORZRES);  //获取DC宽度
	int hight = GetDeviceCaps(hDc, VERTRES);  //获取DC高度
	
	
	CImage image;
	image.Create(width, hight, bitOfPix); //为图像类创建与窗口DC相同大小的DC
	BitBlt(image.GetDC(), 0, 0, width, hight, hDc, 0, 0, SRCCOPY); //将窗口DC图像复制到image
	image.Save(name.data(), Gdiplus::ImageFormatBMP); //保存为bmp格式图片文件
	image.ReleaseDC(); //释放DC
	ReleaseDC(hWnd, hDc); //释放DC资源
}
// 鼠标事件的回调函数
void MouseEvent(int event, int x, int y, int flags, void*)
{
	// 鼠标左键按下
	if (event == EVENT_LBUTTONDOWN)
	{
		startPoint = Point(x, y);
	}
	// 鼠标移动并且左键被按下
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
	{
		endPoint = Point(x, y);
		Mat tempImage = srcImage.clone();
		rectangle(tempImage, startPoint, endPoint, Scalar(255, 100, 0), 1, 8, 0);
		imshow("srcImage", tempImage);
	}
	// 鼠标左键抬起
	else if (event == EVENT_LBUTTONUP)
	{
		endPoint = Point(x, y);
		roirect.width = abs(endPoint.x - startPoint.x);
		roirect.height = abs(endPoint.y - startPoint.y);
		if (roirect.width > 0 && roirect.height > 0)
		{
			roirect.x = min(startPoint.x, endPoint.x);
			roirect.y = min(startPoint.y, endPoint.y);
			Mat roiMat = srcImage(roirect);
			imshow("ROI", roiMat);
			imwrite("cache/result.bmp", roiMat);
		}
	}
}
