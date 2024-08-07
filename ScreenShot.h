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
//name:������ļ���
//hWnd:Ҫ��ͼ�Ĵ��ھ��,NULL��ʾ�������ͼ
bool SavePic(wstring name, HWND hWnd) {
	//SetProcessDPIAware();//ˢ��DPI
	HDC hDc = NULL;
	hWnd = (hWnd == NULL) ? GetDesktopWindow() : hWnd; //�жϴ��ھ���Ƿ�ΪNULL,���ΪNULL��Ĭ�ϻ�ȡ����DC
	hDc = GetDC(hWnd); //��ȡDC
	if (hDc == NULL) return false;
	int bitOfPix = GetDeviceCaps(hDc, BITSPIXEL); //��ȡDC���صĴ�С
	int width = GetDeviceCaps(hDc, HORZRES);  //��ȡDC���
	int hight = GetDeviceCaps(hDc, VERTRES);  //��ȡDC�߶�
	
	
	CImage image;
	image.Create(width, hight, bitOfPix); //Ϊͼ���ഴ���봰��DC��ͬ��С��DC
	BitBlt(image.GetDC(), 0, 0, width, hight, hDc, 0, 0, SRCCOPY); //������DCͼ���Ƶ�image
	image.Save(name.data(), Gdiplus::ImageFormatBMP); //����Ϊbmp��ʽͼƬ�ļ�
	image.ReleaseDC(); //�ͷ�DC
	ReleaseDC(hWnd, hDc); //�ͷ�DC��Դ
}
// ����¼��Ļص�����
void MouseEvent(int event, int x, int y, int flags, void*)
{
	// ����������
	if (event == EVENT_LBUTTONDOWN)
	{
		startPoint = Point(x, y);
	}
	// ����ƶ��������������
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
	{
		endPoint = Point(x, y);
		Mat tempImage = srcImage.clone();
		rectangle(tempImage, startPoint, endPoint, Scalar(255, 100, 0), 1, 8, 0);
		imshow("srcImage", tempImage);
	}
	// ������̧��
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
