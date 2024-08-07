#pragma once
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <fstream> 
#include<Windows.h>
#include "image_processing.h"
//using namespace std;

void open_text(const wchar_t* filePath) {
	//打开文件
	HINSTANCE result = ShellExecute(NULL, NULL, filePath, NULL, NULL, SW_SHOWNORMAL);
	if ((int)result <= 32) {
		ofstream error_message("cache/result.txt");
		DWORD errorCode = GetLastError();
		error_message << "错误！无法默认打开文件，请联系作者QQ:3433824787" << endl;
		error_message.close();
		system("start cache/result.txt");
	}
}

void recognition(char* image_dress, char key) {
	// 获取窗口句柄如果txt被打开则关闭他
	HWND text = FindWindow(NULL, L"result.txt - 记事本");
	HWND text1 = FindWindow(NULL, L"result - 记事本");

	if (text != NULL) {
		// 发送关闭窗口消息
		SendMessage(text, WM_CLOSE, 0, 0);
		CloseHandle(text);
	}

	if (text!=NULL&&text1 != NULL) {
		SendMessage(text1, WM_CLOSE, 0, 0);
		CloseHandle(text1);
	}

	const wchar_t* filePath = L"cache\\result.txt";
	// 获取当前可执行文件的路径便于打开txt
	wchar_t currentPath[MAX_PATH];
	GetModuleFileName(NULL, currentPath, MAX_PATH);

	// 提取路径中的目录部分
	wchar_t* lastSlash = wcsrchr(currentPath, L'\\');
	if (lastSlash != NULL) {
		*lastSlash = L'\0';
	}

	// 设置当前工作目录
	SetCurrentDirectory(currentPath);

	char* outText; ofstream ofile("cache/result.txt"); bool judg_background = true;//判断背景颜色，true为纯色，false为非纯色
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	//用指定初始化 tesseract-ocr，而不指定 tessdata 路径
	if (key == '0') {
		if (api->Init(NULL, "chi_sim")) {
			ofile << "未检测到对应语言文件，请检查应用程序同级目录是否含有tessdata文件夹，或联系作者QQ:3433824787";
			ofile.close();
			open_text(filePath);
			exit(1);
		}
	}
	if (key == '1') {
		if (api->Init(NULL, "eng")) {
			ofile << "未检测到对应语言文件，请检查应用程序同级目录是否含有tessdata文件夹，或联系作者QQ:3433824787";
			ofile.close();
			open_text(filePath);
			exit(1);
		}
	}
	if (key == '2') {
		if (api->Init(NULL, "chi_tra")) {
			ofile << "未检测到对应语言文件，请检查应用程序同级目录是否含有tessdata文件夹，或联系作者QQ:3433824787";
			ofile.close();
			open_text(filePath);
			exit(1);
		}
	}

	//api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);    //设置页面分割
	api->SetVariable("preserve_interword_spaces", "1");//更新配置信息
	api->SetVariable("language_model_ngram_on", "0");
	api->SetVariable("save_blob_choices", "T");
	api->SetVariable("chop_enable", "T");
	api->SetVariable("use_new_state_cost", "F");
	api->SetVariable("segment_segcost_rating", "F");
	api->SetVariable("enable_new_segsearch", "0");
	api->SetVariable("language_model_ngram_on", "0");
	api->SetVariable("textord_force_make_prop_words", "F");
	api->SetVariable("edges_max_children_per_outline", "40");//end of our part!
	api->SetVariable("Load frequent word dawg.", "0");
	api->SetVariable("load_freq_dawg", "0");
	//api->SetVariable("tessedit_use_lstm", "0");//不使用lstm
	//api->SetVariable("tessedit_ocr_engine_mode", "2");//仅使用传统方法
	//api->SetPageSegMode(PSM_SINGLE_CHAR);
	
	
	// 打开输入图像
	Pix* image = pixRead(image_dress);
	if (image == NULL) {
		outText = NULL;
		ofile << "你输入的图片路径: " << image_dress << " 有误。或者你输入的不是图片的路径。请重新输入吧";
		ofile.close();
		std::ifstream file("cache/result.txt"); // 打开文件
		if (file.is_open()) {
			// 文件打开成功
			// 这里可以对文件进行操作

			file.close(); // 关闭文件
		}
		else {
			// 文件打开失败
			std::cout << "异常，请联系QQ 3433824787！" << std::endl;
			// 可以选择进行错误处理或其他操作
		}

		open_text(filePath);

		//printf("你输入的图片路径: %s 有误。或者你输入的不是图片的路径。请重新输入吧", image_dress);
	}
	else {
		ofile.close();
		ofstream text_result("cache/result.txt", ios::app);

		//------------处理图片
		//cv::Mat denoised; 
		int count_rows = 0;//计算文字行数
		cv::Mat image_original = cv::imread(image_dress);
		cv::Mat image_dispose = cv::imread(image_dress);//读取图片
		cvtColor(image_dispose, image_dispose, cv::COLOR_BGR2GRAY);//灰度
		//GaussianBlur(image_dispose, image_dispose, cv::Size(3, 3), 0);//高斯模糊
		//GaussianBlur(image_dispose, image_dispose, cv::Size(5, 5), 0);//高斯滤波去除小噪点
		//width = image.cols;
		//medianBlur(image_dispose, image_dispose, 5);//中值滤波
		
		//cv::threshold(image_dispose, image_dispose, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
		//adaptiveThreshold(image_dispose, image_dispose, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 127, 20);// 二值化处理

		std::map<uchar, int> colorFreq;

		// 遍历图像的每个像素
		for (int i = 0; i < image_dispose.rows; i++) {
			for (int j = 0; j < image_dispose.cols; j++) {
				// 获取像素值
				uchar pixel = image_dispose.at<uchar>(i, j);

				// 像素值频次加一
				colorFreq[pixel]++;
			}
		}

		// 找到频次最高的像素值和频次
		uchar mostFrequentColor = 0;
		int maxFrequency = 0;

		std::map<uchar, int>::iterator it;
		for (it = colorFreq.begin(); it != colorFreq.end(); ++it) {
			std::pair<int, int> pair = *it;
			int frequency = pair.second;
			if (frequency > maxFrequency) {
				maxFrequency = frequency;
				mostFrequentColor = pair.first;
			}
		}
		//判断背景是不是纯色if成立则不是纯色
		if (maxFrequency < image_dispose.total() / 2) {
			judg_background = false;
			cv::threshold(image_dispose, image_dispose, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);//二值化
			int black_pixels = countNonZero(image_dispose);//获取图片非黑色的像素点
			int white_pixels = image_dispose.total() - black_pixels;
			if (black_pixels < white_pixels) {//将图片转为白底黑字
				bitwise_not(image_dispose, image_dispose);
			}
		}
		else if (mostFrequentColor <= 120) {//将图片转为白底黑字
			bitwise_not(image_dispose, image_dispose);
		}
		if (judg_background)copyMakeBorder(image_dispose, image_dispose, 15, 15, 15, 15, cv::BORDER_CONSTANT, mostFrequentColor <= 120 ? cv::Scalar(255 - mostFrequentColor) : cv::Scalar(mostFrequentColor));//添加白色像素为10的边框
		else copyMakeBorder(image_dispose, image_dispose, 15, 15, 15, 15, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));//添加白色像素为10的边框


		cv::Mat* img;

		img = detect(image_dispose, count_rows);
		if (img != NULL) {
			for (int i = count_rows; i >= 0; i--) {

				//调整图片大小，经过实验，英文和中文更改的大小有区别
				if (key == '0') {
					int mult = img[i].cols;
					while (mult % 5 != 0)mult++;
					if (img[i].cols % 5 != 0)resize(img[i], img[i], cv::Size(), (double)mult / img[i].cols, (double)mult / img[i].cols, cv::INTER_NEAREST);
					if (img[i].cols < 650 && img[i].cols>600)resize(img[i], img[i], cv::Size(), 655.00 / img[i].cols, 655.00 / img[i].cols, cv::INTER_NEAREST);
					if (img[i].cols < 100 && img[i].cols>500)resize(img[i], img[i], cv::Size(), 130.00 / img[i].cols, 130.00 / img[i].cols, cv::INTER_NEAREST);
					/*if (img[i].cols < 1100 && img[i].cols>1000)resize(img[i], img[i], cv::Size(), a / img[i].cols, a / img[i].cols, cv::INTER_NEAREST);
					if (img[i].cols < 1200 && img[i].cols>1100)resize(img[i], img[i], cv::Size(), 1000.00 / img[i].cols, 1000.00 / img[i].cols, cv::INTER_NEAREST);
					if (img[i].cols < 800 && img[i].cols>700)resize(img[i], img[i], cv::Size(), 800.00 / img[i].cols, 800.00 / img[i].cols, cv::INTER_NEAREST);
					if (img[i].cols < 300 && img[i].rows < 30)resize(img[i], img[i], cv::Size(), 435.00 / img[i].cols, 435.00 / img[i].cols, cv::INTER_NEAREST);
					if (img[i].cols < 600 && img[i].cols>500)resize(img[i], img[i], cv::Size(), 620.00 / img[i].cols, 620.00 / img[i].cols, cv::INTER_NEAREST);
					if (img[i].cols < 300 && img[i].rows < 30 && img[i].cols>70)resize(img[i], img[i], cv::Size(), 330.00 / img[i].cols, 330.00 / img[i].cols, cv::INTER_NEAREST);*/
				}

				if (judg_background)copyMakeBorder(image_dispose, image_dispose, 15, 15, 15, 15, cv::BORDER_CONSTANT, mostFrequentColor <= 120 ? cv::Scalar(255 - mostFrequentColor) : cv::Scalar(mostFrequentColor));//添加白色像素为10的边框
				else copyMakeBorder(image_dispose, image_dispose, 15, 15, 15, 15, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));//添加白色像素为10的边框 
				
				std::string imagename = "cache/img" + std::to_string(i) + ".bmp";
				const char* imagenamePtr = imagename.c_str();
				cv::imwrite(imagenamePtr, img[i]);
				image = pixRead(imagenamePtr);
				api->SetImage(image);
				outText = api->GetUTF8Text();

				text_result << outText;//将结果输出到文件
				
				std::remove(imagenamePtr);//删除分割的图片文件
			}
		}

		text_result.close();
		std::ifstream file("cache/result.txt"); // 打开文件
		if (file.is_open()) {
			// 文件打开成功
			file.close(); // 关闭文件
		}
		else {
			// 文件打开失败
			std::cout << "异常，请联系QQ 3433824787！" << std::endl;
			// 可以选择进行错误处理或其他操作
		}
		//system("TASKKILL /F /IM cache/result.txt");
		open_text(filePath);

		strcpy(image_dress, "cache/result.bmp");
		//-----------图片处理完成


		imwrite("cache/result.bmp", image_original);  //保存原始图片

		delete[]img;
		if (count_rows != 0)delete[] outText;
	}

	// 释放内存
	api->End();

	delete api;
	pixDestroy(&image);

}

