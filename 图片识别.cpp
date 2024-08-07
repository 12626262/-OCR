#include"tess_recognition.h"
#include"ScreenShot.h"
#include <opencv2/core/utils/logger.hpp>
#include <graphics.h>


IMAGE icon1, icon2;
char Screen_shot[17] = "cache/result.bmp";
char key = '0';

class button {
public:
    bool state = false; const wchar_t* word; int x_left, x_right, y_dowm, y_up;//��ť����
};


void start(button* obj) {
    //moveWindow();
    // ��ʼ��ͼ�ν���
    HWND hwnd = initgraph(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));//��ȡ��ʼ��������Ϣ

    MoveWindow(hwnd, 0, GetSystemMetrics(SM_CYSCREEN) - 160, 380, 100, false);
    //initgraph(380, 60);//��380 ��60

    // ���Ʊ���ɫ
    setbkcolor(WHITE); // ������ɫ��Ϊ��ɫ
    cleardevice();//ʹ�õ�ǰ����ɫ��ս���

    // ���ư�ť
    setlinecolor(BLACK);     //�߿���Ϊ��ɫ(͸��)
    setfillcolor(WHITE); // �������ɫ��Ϊ��ɫ
    setlinestyle(PS_SOLID | PS_ENDCAP_ROUND, 1);//ʵ�ߣ��˵�ΪԲ�Σ�����Ϊ1
    setlinecolor(WHITE);//����ɫΪ��ɫ
    fillroundrect(obj[0].x_left, obj[0].y_dowm, obj[0].x_right, obj[0].y_up, 10, 10);//���ƽ�ͼ��ť
    fillroundrect(obj[1].x_left, obj[1].y_dowm, obj[1].x_right, obj[1].y_up, 10, 10);//����ͼƬʶ��ť
    fillroundrect(obj[2].x_left, obj[2].y_dowm, obj[2].x_right, obj[2].y_up, 10, 10);//���Ƽ������İ�ť
    fillroundrect(obj[3].x_left, obj[3].y_dowm, obj[3].x_right, obj[3].y_up, 10, 10);//���Ʒ������İ�ť
    fillroundrect(obj[4].x_left, obj[4].y_dowm, obj[4].x_right, obj[4].y_up, 10, 10);//����Ӣ�İ�ť
    loadimage(&icon1, _T("cache/icon1.png"), 40, 40, true);
    loadimage(&icon2, _T("cache/icon2.png"), 40, 40, true);
    putimage(obj[0].x_left + 5, 5, &icon1);
    putimage(obj[1].x_left + 5, 5, &icon2);	// ��ʾ����
    settextcolor(RGB(17, 119, 189)); // ��������ɫ��Ϊ��ɫ
    settextstyle(30, 0, _T("΢���ź�")); // ����������ֺ�
    outtextxy(obj[2].x_left + 15, obj[2].y_dowm + 10, obj[2].word); // ���λ�� x y  L���ַ��� T���ַ�  
    outtextxy(obj[3].x_left + 15, obj[3].y_dowm + 10, obj[3].word);
    outtextxy(obj[4].x_left + 15, obj[4].y_dowm + 10, obj[4].word);
};




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    SetConsoleOutputCP(CP_UTF8);//ʹ�ñ���UTF-8
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);//����opencv������Ϣ
    SetProcessDPIAware();//ˢ��DPI

    HWND Hwnd;//��ȡ���
    LONG_PTR style;//�洢������Ϣ
    button obj[5];
    obj[0] = { false, L"��" ,25,75,0,50 }; obj[1] = { false,L"ͼ",95,145,0,50 };
    obj[2] = { false, L"��" ,165,215,0,50 }; obj[3] = { false,L"��",235,285,0,50 };
    obj[4] = { false,L"Ӣ",305,355,0,50 };
    obj[2].state = true; button* memory = &obj[2];
    start(obj);


    //setfillcolor(RED); // �������ɫ��Ϊ��ɫ
    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//���Ƽ������İ�ť
    settextcolor(RED); // ��������ɫ��Ϊ��ɫ
    //setbkcolor(RED); // ������ɫ��Ϊ��ɫ
    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);



    while (true)
    {
        // ��ȡ���״̬
        MOUSEMSG mouse = GetMouseMsg();
        if (mouse.uMsg == WM_LBUTTONDOWN) {
            if (mouse.x >= obj[0].x_left && mouse.x < obj[0].x_right && mouse.y >= obj[0].y_dowm && mouse.y < obj[0].y_up)
            {

                SavePic(L"cache/Screen.bmp", NULL);//�������ͼ,����Ϊ1.png�ļ�
                srcImage = imread("cache/Screen.bmp");//����һ��������ȡ���ͼƬ
                if (!srcImage.data)
                {
                    return -1;
                }
                cv::namedWindow("srcImage"); // ����һ�����Ե�����С�Ĵ���
                imshow("srcImage", srcImage);//չʾ�������

                // ��ȡ���ھ��
                Hwnd = FindWindowA(NULL, "srcImage");

                // ���ر�����
                style = GetWindowLongPtr(Hwnd, GWL_STYLE);
                style &= ~WS_CAPTION;
                SetWindowLongPtr(Hwnd, GWL_STYLE, style);
                SetWindowPos(Hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//���ô���λ��

                setMouseCallback("srcImage", MouseEvent, 0);//�������ͼƬ��ͼ
                waitKey(0);
                cv::destroyAllWindows();
                recognition(Screen_shot, key);
                //initgraph(0,0);  //���������������  ��600 ��600  

            }

            if (mouse.x >= obj[1].x_left && mouse.x < obj[1].x_right && mouse.y >= obj[1].y_dowm && mouse.y < obj[1].y_up)
            {
                setbkcolor(WHITE); // ������ɫ��Ϊ��ɫ
                cleardevice();//ʹ�õ�ǰ����ɫ��ս���
                // �����ַ������������������û�����
                wchar_t s[100];
                InputBox(s, 100, L"������ͼƬ·��");

                int num = WideCharToMultiByte(CP_OEMCP, NULL, s, -1, NULL, 0, NULL, FALSE);
                char* image_dress = new char[num];
                WideCharToMultiByte(CP_OEMCP, NULL, s, -1, image_dress, num, NULL, FALSE);//��w_char*ת��Ϊchar*
                recognition(image_dress, key);//ʶ��ͼƬ������


                start(obj);
                fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//���Ƽ������İ�ť
                settextcolor(RED); // ��������ɫ��Ϊ��ɫ
                //setbkcolor(RED); // ������ɫ��Ϊ��ɫ
                outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
            }


            if (mouse.x >= obj[2].x_left && mouse.x < obj[2].x_right && mouse.y >= obj[2].y_dowm && mouse.y < obj[2].y_up && obj[2].state == false)//����ƶ������򡱰�ť
            {
                key = '0';

                //setfillcolor(RED); // �������ɫ��Ϊ��ɫ
                fillroundrect(obj[2].x_left, obj[2].y_dowm, obj[2].x_right, obj[2].y_up, 10, 10);
                settextcolor(RED); // ������ɫ��Ϊ��ɫ
                outtextxy(obj[2].x_left + 15, obj[2].y_dowm + 10, obj[2].word);
                obj[2].state = true;
                if (memory->state == true) {
                    //setfillcolor(RGB(242, 242, 242)); // �������ɫ��Ϊ��ɫ
                    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//���ư�ť

                    settextcolor(RGB(17, 119, 189)); // ������ɫ��Ϊ�Ұ�ɫ
                    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
                }memory->state = false; memory = &obj[2];
            }
            if (mouse.x >= obj[3].x_left && mouse.x < obj[3].x_right && mouse.y >= obj[3].y_dowm && mouse.y < obj[3].y_up && obj[3].state == false)
            {
                key = '2';
                //setfillcolor(RED); // �������ɫ��Ϊ��ɫ
                fillroundrect(obj[3].x_left, obj[3].y_dowm, obj[3].x_right, obj[3].y_up, 10, 10);
                settextcolor(RED); // ��������ɫ��Ϊ��ɫ
                outtextxy(obj[3].x_left + 15, obj[3].y_dowm + 10, obj[3].word);
                obj[3].state = true;
                if (memory->state == true) {

                    //setfillcolor(WHITE); // �������ɫ��Ϊ��ɫ

                    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//���ư�ť
                    settextcolor(RGB(17, 119, 189)); // ����������Ϊ��ɫ
                    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
                }memory->state = false; memory = &obj[3];
            }

            if (mouse.x >= obj[4].x_left && mouse.x < obj[4].x_right && mouse.y >= obj[4].y_dowm && mouse.y < obj[4].y_up && obj[4].state == false)
            {
                key = '1';
                //setfillcolor(RED); // �������ɫ��Ϊ��ɫ
                fillroundrect(obj[4].x_left, obj[4].y_dowm, obj[4].x_right, obj[4].y_up, 10, 10);
                settextcolor(RED); // ������Ϊ��ɫ
                outtextxy(obj[4].x_left + 15, obj[4].y_dowm + 10, obj[4].word);
                obj[4].state = true;
                if (memory->state == true) {

                    //setfillcolor(WHITE); // �������ɫ��Ϊ��ɫ

                    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//���ư�ť
                    settextcolor(RGB(17, 119, 189)); // ������Ϊ��ɫ
                    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
                }memory->state = false; memory = &obj[4];
            }

        }
    }

    // �ر�ͼ�ν���
    closegraph();
    return 0;
}
