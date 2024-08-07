#include<opencv2/opencv.hpp>
#include<iostream>

//using namespace cv;
using namespace std;

//������ת�������Ͻǵ�x
double count_x(cv::RotatedRect& p1) {
    if (p1.angle >= -10 && p1.angle <= 10)return p1.center.x - p1.size.width / 2 * std::cos(p1.angle * CV_PI / 180) + p1.size.height / 2 * std::sin(p1.angle * CV_PI / 180);
    return p1.center.x - p1.size.width / 2 * std::cos(p1.angle * CV_PI / 180) - p1.size.height / 2 * std::sin(p1.angle * CV_PI / 180);
}

////������ת�������Ͻǵ�y
double count_y(cv::RotatedRect& p1) {
    if (p1.angle >= -10 && p1.angle <= 10)return p1.center.y - p1.size.width / 2 * std::sin(p1.angle * CV_PI / 180) - p1.size.height / 2 * std::cos(p1.angle * CV_PI / 180);
    return p1.center.y - p1.size.width / 2 * std::sin(p1.angle * CV_PI / 180) + p1.size.height / 2 * std::cos(p1.angle * CV_PI / 180);
}


//�γ���������
cv::Mat preprocess(cv::Mat gray)
{
    //1.Sobel���ӣ�x�������ݶ�
    Sobel(gray, gray, CV_8U, 1, 0, 3);
    //imshow("3", gray);

    //2.��ֵ��
    //�����������������Ǵ����ǻ�ɫ�Ķ�ֵ�����ݺ��ң����º���ķָ��޷�����
    //Ŀǰ���õ���ֵ��ֵ�����Ե�ǰ��ƷЧ�����У��ܷ�ͨ�û�������
    threshold(gray, gray, 127, 255, cv::THRESH_TOZERO);
    //imshow("4", gray);

    //3.���ͺ͸�ʴ�������趨
    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(24, 9));
    //���Ƹ߶����ÿ��Կ��������е����ͳ̶ȣ�����3��4������������ǿ,��Ҳ�����©��
    cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(24, 4));

    //4.����һ�Σ�������ͻ��
    dilate(gray, gray, element2);
    //imshow("0", gray);

    //5.��ʴһ�Σ�ȥ��ϸ�ڣ�����ߵȡ�����ȥ��������ֱ����
    erode(gray, gray, element1);
    //imshow("1", gray);

    //6.�ٴ����ͣ�����������һЩ
    dilate(gray, gray, element2);
    //imshow("2", gray);

    return gray;
}


//�洢������������
vector<cv::RotatedRect> findTextRegion(cv::Mat img)
{
    vector<cv::RotatedRect> rects_coord;
    //1.��������
    vector<vector<cv::Point>> contours;//�洢ͼ���е�������
    vector<cv::Vec4i> hierarchy;//�洢�����Ĳ㼶��Ϣ��
    findContours(img, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));



    //2.ɸѡ��Щ���С��
    for (int i = 0; i < contours.size(); i++)
    {
        //���㵱ǰ���������
        double area = cv::contourArea(contours[i]);

        //���С��140��ȫ��ɸѡ��
        /*if (area < 250)
            continue;*/

            //�������ƣ����ý�С��approxPolyDP�����д��о�
        double epsilon = 0.001 * arcLength(contours[i], true);
        cv::Mat approx;
        approxPolyDP(contours[i], approx, epsilon, true);

        //�ҵ���С���Σ��þ��ο����з���
        cv::RotatedRect rect_min = minAreaRect(contours[i]);

        //����ߺͿ�
        int m_width = rect_min.boundingRect().width;
        int m_height = rect_min.boundingRect().height;


        //ɸѡ��Щ̫ϸ�ľ��Σ����±��
        if (m_height > m_width * 1.2)
            continue;
        if (m_width <= 30 && m_height > m_width * 0.82)//�ж�����{} ȥ������
            continue;
        if (m_height < 10)  //ȥ��δ���������
            continue;
        //cout <<"��:" << m_height <<"��:" << " " << m_width << endl;
        //����������rect��ӵ�rects������
        rects_coord.push_back(rect_min);

    }
    return rects_coord;
}

//���־��δ��µ��ϴ���������
bool cmp(cv::RotatedRect& p1, cv::RotatedRect& p2) {
    double y1 = count_y(p1);
    double y2 = count_y(p2);
    double x1 = count_x(p1);
    double x2 = count_x(p2);

    if (fabs(y1 - y2) < 5)return x1 < x2;
    else return y1 > y2;
}

//�ü�ͼƬ
cv::Mat* detect(cv::Mat img, int& count)
{
    cv::Mat gray = img.clone();
    //img.copyTo(gray);
    count = 0;
    //cvtColor(img, gray, CV_BGR2GRAY);
    //imshow("����ǰ", gray);


    //2.��̬ѧ�任��Ԥ�����õ����Բ��Ҿ��ε�����
    cv::Mat dilation = preprocess(gray);
    //imshow("bbb", gray);

    //3.���Һ�ɸѡ��������
    vector<cv::RotatedRect> rects;
    vector<cv::Point2f> textrect;//�洢ÿ�����ֵ����Ͻ�����
    vector<pair<double, double>>lenth;//�洢�����Ϣ��
    rects = findTextRegion(dilation);

    //cout << endl;
    sort(rects.begin(), rects.end(), cmp);

    ////���ԣ��������
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

    //��һ�����ε���Ϣ
    if (rects.size() == 0)
    {
        return 0;
    }
    double text_width, text_height;
    text_width = rects[0].size.width;
    text_height = rects[0].size.height;

    if (text_width > 10 || text_height > 10) {
        if (text_width < text_height)swap(text_width, text_height);//ʹ���ֵĿ���Զ���ڸ�
    }
    cv::Point2f text_point(count_x(rects[0]), count_y(rects[0]));
    pair<double, double>p(text_width, text_height);
    textrect.push_back(text_point);
    lenth.push_back(p);
    count = 0;//�����ж�����

    //ͬһ�кϲ�
    //�������boundingRect()��size�ĸ߲�ͬ�����ܵ��²ü�������ֹ֣�����б�����
    for (int i = 0; i < rects.size(); i++)
    {
        if (i != rects.size() - 1) {
            text_width = rects[i].size.width;
            text_height = rects[i].size.height;
            double text_width1 = rects[i + 1].size.width;
            double text_height1 = rects[i + 1].size.height;
            if (text_width > 10 || text_height > 10) {
                if (text_width < text_height)swap(text_width, text_height);//ʹ���ֵĿ���Զ���ڸ�
            }
            if (text_width1 > 10 || text_height1 > 10) {
                if (text_width1 < text_height1)swap(text_width1, text_height1);//ʹ���ֵĿ���Զ���ڸ�
            }

            double y1 = count_y(rects[i]);
            double y2 = count_y(rects[i + 1]);
            double x1 = count_x(rects[i]);
            double x2 = count_x(rects[i + 1]);
            if (fabs(y2 - y1) < 5) {
                lenth[count].first = fabs(x2 - textrect[count].x) + text_width1;
                lenth[count].second = max(text_height, text_height);

                //���ԣ�
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

    //���ԣ������߿�
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
    //�Ѻϲ����ÿһ�е�����ΪͼƬ���
    cv::Mat* text_part = new cv::Mat[count + 1];
    for (int i = count; i >= 0; i--) {
        text_part[i] = cv::Mat(lenth[i].second + 15, lenth[i].first + 15, CV_8UC3);
        cv::Rect rect(textrect[i].x - 5, textrect[i].y - 5, lenth[i].first, lenth[i].second + 9);//ע�����Ͻǵ�����������½�ҪС��
        //cout << lenth[i].first << endl;
        text_part[i] = img(rect).clone();
        //cv::imshow("Image " + std::to_string(i), text_part[i]);//���ԣ���ʾ�����ͼ��
    }

    //���ԣ����������Ϣ
    //imshow("img", img);
    //cv::namedWindow("Image");
    //cv::setMouseCallback("Image", onMouse); 
    //while (true)
    //{
    //    cv::imshow("Image", img);

    //    int key = cv::waitKey(1);
    //    if (key == 27) // ���� ESC ���˳�ѭ��
    //        break;
    //}
    //cv::waitKey(0);
    return text_part;
}