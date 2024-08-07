#include<opencv2/opencv.hpp>
#include<iostream>

//using namespace cv;
using namespace std;

//计算旋转矩形左上角的x
double count_x(cv::RotatedRect& p1) {
    if (p1.angle >= -10 && p1.angle <= 10)return p1.center.x - p1.size.width / 2 * std::cos(p1.angle * CV_PI / 180) + p1.size.height / 2 * std::sin(p1.angle * CV_PI / 180);
    return p1.center.x - p1.size.width / 2 * std::cos(p1.angle * CV_PI / 180) - p1.size.height / 2 * std::sin(p1.angle * CV_PI / 180);
}

////计算旋转矩形左上角的y
double count_y(cv::RotatedRect& p1) {
    if (p1.angle >= -10 && p1.angle <= 10)return p1.center.y - p1.size.width / 2 * std::sin(p1.angle * CV_PI / 180) - p1.size.height / 2 * std::cos(p1.angle * CV_PI / 180);
    return p1.center.y - p1.size.width / 2 * std::sin(p1.angle * CV_PI / 180) + p1.size.height / 2 * std::cos(p1.angle * CV_PI / 180);
}


//形成文字轮廓
cv::Mat preprocess(cv::Mat gray)
{
    //1.Sobel算子，x方向求梯度
    Sobel(gray, gray, CV_8U, 1, 0, 3);
    //imshow("3", gray);

    //2.二值化
    //待解决，如果背景不是纯白是灰色的二值化内容很乱，导致后面的分割无法进行
    //目前采用低阈值二值化，对当前样品效果还行，能否通用化待考究
    threshold(gray, gray, 127, 255, cv::THRESH_TOZERO);
    //imshow("4", gray);

    //3.膨胀和腐蚀操作核设定
    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(24, 9));
    //控制高度设置可以控制上下行的膨胀程度，例如3比4的区分能力更强,但也会造成漏检
    cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(24, 4));

    //4.膨胀一次，让轮廓突出
    dilate(gray, gray, element2);
    //imshow("0", gray);

    //5.腐蚀一次，去掉细节，表格线等。这里去掉的是竖直的线
    erode(gray, gray, element1);
    //imshow("1", gray);

    //6.再次膨胀，让轮廓明显一些
    dilate(gray, gray, element2);
    //imshow("2", gray);

    return gray;
}


//存储文字轮廓坐标
vector<cv::RotatedRect> findTextRegion(cv::Mat img)
{
    vector<cv::RotatedRect> rects_coord;
    //1.查找轮廓
    vector<vector<cv::Point>> contours;//存储图像中的轮廓。
    vector<cv::Vec4i> hierarchy;//存储轮廓的层级信息。
    findContours(img, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));



    //2.筛选那些面积小的
    for (int i = 0; i < contours.size(); i++)
    {
        //计算当前轮廓的面积
        double area = cv::contourArea(contours[i]);

        //面积小于140的全部筛选掉
        /*if (area < 250)
            continue;*/

            //轮廓近似，作用较小，approxPolyDP函数有待研究
        double epsilon = 0.001 * arcLength(contours[i], true);
        cv::Mat approx;
        approxPolyDP(contours[i], approx, epsilon, true);

        //找到最小矩形，该矩形可能有方向
        cv::RotatedRect rect_min = minAreaRect(contours[i]);

        //计算高和宽
        int m_width = rect_min.boundingRect().width;
        int m_height = rect_min.boundingRect().height;


        //筛选那些太细的矩形，留下扁的
        if (m_height > m_width * 1.2)
            continue;
        if (m_width <= 30 && m_height > m_width * 0.82)//判断例如{} 去掉竖线
            continue;
        if (m_height < 10)  //去掉未截完的文字
            continue;
        //cout <<"高:" << m_height <<"宽:" << " " << m_width << endl;
        //符合条件的rect添加到rects集合中
        rects_coord.push_back(rect_min);

    }
    return rects_coord;
}

//文字矩形从下到上从左到右排序
bool cmp(cv::RotatedRect& p1, cv::RotatedRect& p2) {
    double y1 = count_y(p1);
    double y2 = count_y(p2);
    double x1 = count_x(p1);
    double x2 = count_x(p2);

    if (fabs(y1 - y2) < 5)return x1 < x2;
    else return y1 > y2;
}

//裁剪图片
cv::Mat* detect(cv::Mat img, int& count)
{
    cv::Mat gray = img.clone();
    //img.copyTo(gray);
    count = 0;
    //cvtColor(img, gray, CV_BGR2GRAY);
    //imshow("处理前", gray);


    //2.形态学变换的预处理，得到可以查找矩形的轮廓
    cv::Mat dilation = preprocess(gray);
    //imshow("bbb", gray);

    //3.查找和筛选文字区域
    vector<cv::RotatedRect> rects;
    vector<cv::Point2f> textrect;//存储每行文字的左上角坐标
    vector<pair<double, double>>lenth;//存储宽高信息。
    rects = findTextRegion(dilation);

    //cout << endl;
    sort(rects.begin(), rects.end(), cmp);

    ////调试：输出坐标
    //for (int i = 0; i < rects.size(); i++) {
    //     cout << "x:" << count_x(rects[i]) << " " << "y:" << count_y(rects[i]) << endl;
    //     cout << rects[i].angle << endl;
    // }
    //cout << endl;
    //for (int i = 0; i < rects.size(); i++) {
    //    cv::Point2f P[4];
    //    rects[i].points(P);
    //    for (int j = 0; j <= 3; j++) {
    //        cout << P[j] << " ";
    //    }cout << endl;
    //}

    //第一个矩形的信息
    if (rects.size() == 0)
    {
        return 0;
    }
    double text_width, text_height;
    text_width = rects[0].size.width;
    text_height = rects[0].size.height;

    if (text_width > 10 || text_height > 10) {
        if (text_width < text_height)swap(text_width, text_height);//使文字的宽永远大于高
    }
    cv::Point2f text_point(count_x(rects[0]), count_y(rects[0]));
    pair<double, double>p(text_width, text_height);
    textrect.push_back(text_point);
    lenth.push_back(p);
    count = 0;//计算有多少行

    //同一行合并
    //待解决：boundingRect()和size的高不同。可能导致裁剪的奇奇怪怪，考虑斜边情况
    for (int i = 0; i < rects.size(); i++)
    {
        if (i != rects.size() - 1) {
            text_width = rects[i].size.width;
            text_height = rects[i].size.height;
            double text_width1 = rects[i + 1].size.width;
            double text_height1 = rects[i + 1].size.height;
            if (text_width > 10 || text_height > 10) {
                if (text_width < text_height)swap(text_width, text_height);//使文字的宽永远大于高
            }
            if (text_width1 > 10 || text_height1 > 10) {
                if (text_width1 < text_height1)swap(text_width1, text_height1);//使文字的宽永远大于高
            }

            double y1 = count_y(rects[i]);
            double y2 = count_y(rects[i + 1]);
            double x1 = count_x(rects[i]);
            double x2 = count_x(rects[i + 1]);
            if (fabs(y2 - y1) < 5) {
                lenth[count].first = fabs(x2 - textrect[count].x) + text_width1;
                lenth[count].second = max(text_height, text_height);

                //调试：
                /*cout <<endl<< textrect[count].x << " " << textrect[count].y << endl;
                cout << x1 << " " << y1 << " " << x2 << " " << y2 << " "<<lenth[count].first;*/
            }
            else {
                count++;
                text_point.x = x2, text_point.y = y2;
                textrect.push_back(text_point);
                p.first = text_width1; p.second = text_height1;
                lenth.push_back(p);
            }
        }
    }

    //调试：画出边框
    //cout << endl;
    //for (int i = 0; i < rects.size(); i++)
    //{
    //    cv::Point2f P[4];
    //    rects[i].points(P);
    //    
    //    for (int j = 0; j <= 3; j++)
    //    {
    //        //cout << P[i];
    //        line(img, P[j], P[(j + 1) % 4], cv::Scalar(0, 255, 0), 1);
    //    }//cout<< endl;
    //}


    //cout << endl;
    //把合并后的每一行单独作为图片输出
    cv::Mat* text_part = new cv::Mat[count + 1];
    for (int i = count; i >= 0; i--) {
        text_part[i] = cv::Mat(lenth[i].second + 15, lenth[i].first + 15, CV_8UC3);
        cv::Rect rect(textrect[i].x - 5, textrect[i].y - 5, lenth[i].first, lenth[i].second + 9);//注意左上角的纵坐标比左下角要小，
        //cout << lenth[i].first << endl;
        text_part[i] = img(rect).clone();
        //cv::imshow("Image " + std::to_string(i), text_part[i]);//调试，显示处理后图像
    }

    //调试：获得坐标信息
    //imshow("img", img);
    //cv::namedWindow("Image");
    //cv::setMouseCallback("Image", onMouse); 
    //while (true)
    //{
    //    cv::imshow("Image", img);

    //    int key = cv::waitKey(1);
    //    if (key == 27) // 按下 ESC 键退出循环
    //        break;
    //}
    //cv::waitKey(0);
    return text_part;
}