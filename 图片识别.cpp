#include"tess_recognition.h"
#include"ScreenShot.h"
#include <opencv2/core/utils/logger.hpp>
#include <graphics.h>


IMAGE icon1, icon2;
char Screen_shot[17] = "cache/result.bmp";
char key = '0';

class button {
public:
    bool state = false; const wchar_t* word; int x_left, x_right, y_dowm, y_up;//按钮坐标
};


void start(button* obj) {
    //moveWindow();
    // 初始化图形界面
    HWND hwnd = initgraph(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));//获取初始化窗口信息

    MoveWindow(hwnd, 0, GetSystemMetrics(SM_CYSCREEN) - 160, 380, 100, false);
    //initgraph(380, 60);//宽380 高60

    // 绘制背景色
    setbkcolor(WHITE); // 将背景色设为白色
    cleardevice();//使用当前背景色清空界面

    // 绘制按钮
    setlinecolor(BLACK);     //边框设为白色(透明)
    setfillcolor(WHITE); // 将填充颜色设为白色
    setlinestyle(PS_SOLID | PS_ENDCAP_ROUND, 1);//实线，端点为圆形，像素为1
    setlinecolor(WHITE);//线颜色为蓝色
    fillroundrect(obj[0].x_left, obj[0].y_dowm, obj[0].x_right, obj[0].y_up, 10, 10);//绘制截图按钮
    fillroundrect(obj[1].x_left, obj[1].y_dowm, obj[1].x_right, obj[1].y_up, 10, 10);//绘制图片识别按钮
    fillroundrect(obj[2].x_left, obj[2].y_dowm, obj[2].x_right, obj[2].y_up, 10, 10);//绘制简体中文按钮
    fillroundrect(obj[3].x_left, obj[3].y_dowm, obj[3].x_right, obj[3].y_up, 10, 10);//绘制繁体中文按钮
    fillroundrect(obj[4].x_left, obj[4].y_dowm, obj[4].x_right, obj[4].y_up, 10, 10);//绘制英文按钮
    loadimage(&icon1, _T("cache/icon1.png"), 40, 40, true);
    loadimage(&icon2, _T("cache/icon2.png"), 40, 40, true);
    putimage(obj[0].x_left + 5, 5, &icon1);
    putimage(obj[1].x_left + 5, 5, &icon2);	// 显示背景
    settextcolor(RGB(17, 119, 189)); // 将字体颜色设为蓝色
    settextstyle(30, 0, _T("微软雅黑")); // 调整字体和字号
    outtextxy(obj[2].x_left + 15, obj[2].y_dowm + 10, obj[2].word); // 输出位置 x y  L是字符串 T是字符  
    outtextxy(obj[3].x_left + 15, obj[3].y_dowm + 10, obj[3].word);
    outtextxy(obj[4].x_left + 15, obj[4].y_dowm + 10, obj[4].word);
};




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    SetConsoleOutputCP(CP_UTF8);//使用编码UTF-8
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);//消除opencv配置信息
    SetProcessDPIAware();//刷新DPI

    HWND Hwnd;//获取句柄
    LONG_PTR style;//存储窗口信息
    button obj[5];
    obj[0] = { false, L"截" ,25,75,0,50 }; obj[1] = { false,L"图",95,145,0,50 };
    obj[2] = { false, L"简" ,165,215,0,50 }; obj[3] = { false,L"繁",235,285,0,50 };
    obj[4] = { false,L"英",305,355,0,50 };
    obj[2].state = true; button* memory = &obj[2];
    start(obj);


    //setfillcolor(RED); // 将填充颜色设为红色
    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//绘制简体中文按钮
    settextcolor(RED); // 将字体颜色设为黑色
    //setbkcolor(RED); // 将背景色设为红色
    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);



    while (true)
    {
        // 获取鼠标状态
        MOUSEMSG mouse = GetMouseMsg();
        if (mouse.uMsg == WM_LBUTTONDOWN) {
            if (mouse.x >= obj[0].x_left && mouse.x < obj[0].x_right && mouse.y >= obj[0].y_dowm && mouse.y < obj[0].y_up)
            {

                SavePic(L"cache/Screen.bmp", NULL);//对桌面截图,保存为1.png文件
                srcImage = imread("cache/Screen.bmp");//设置一个变量获取这个图片
                if (!srcImage.data)
                {
                    return -1;
                }
                cv::namedWindow("srcImage"); // 创建一个可以调整大小的窗口
                imshow("srcImage", srcImage);//展示这个窗口

                // 获取窗口句柄
                Hwnd = FindWindowA(NULL, "srcImage");

                // 隐藏标题栏
                style = GetWindowLongPtr(Hwnd, GWL_STYLE);
                style &= ~WS_CAPTION;
                SetWindowLongPtr(Hwnd, GWL_STYLE, style);
                SetWindowPos(Hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//设置窗口位置

                setMouseCallback("srcImage", MouseEvent, 0);//鼠标对这个图片截图
                waitKey(0);
                cv::destroyAllWindows();
                recognition(Screen_shot, key);
                //initgraph(0,0);  //绘制文字输出界面  宽600 高600  

            }

            if (mouse.x >= obj[1].x_left && mouse.x < obj[1].x_right && mouse.y >= obj[1].y_dowm && mouse.y < obj[1].y_up)
            {
                setbkcolor(WHITE); // 将背景色设为白色
                cleardevice();//使用当前背景色清空界面
                // 定义字符串缓冲区，并接收用户输入
                wchar_t s[100];
                InputBox(s, 100, L"请输入图片路径");

                int num = WideCharToMultiByte(CP_OEMCP, NULL, s, -1, NULL, 0, NULL, FALSE);
                char* image_dress = new char[num];
                WideCharToMultiByte(CP_OEMCP, NULL, s, -1, image_dress, num, NULL, FALSE);//将w_char*转换为char*
                recognition(image_dress, key);//识别图片的文字


                start(obj);
                fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//绘制简体中文按钮
                settextcolor(RED); // 将字体颜色设为黑色
                //setbkcolor(RED); // 将背景色设为红色
                outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
            }


            if (mouse.x >= obj[2].x_left && mouse.x < obj[2].x_right && mouse.y >= obj[2].y_dowm && mouse.y < obj[2].y_up && obj[2].state == false)//鼠标移动到“简”按钮
            {
                key = '0';

                //setfillcolor(RED); // 将填充颜色设为红色
                fillroundrect(obj[2].x_left, obj[2].y_dowm, obj[2].x_right, obj[2].y_up, 10, 10);
                settextcolor(RED); // 将背景色设为红色
                outtextxy(obj[2].x_left + 15, obj[2].y_dowm + 10, obj[2].word);
                obj[2].state = true;
                if (memory->state == true) {
                    //setfillcolor(RGB(242, 242, 242)); // 将填充颜色设为白色
                    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//绘制按钮

                    settextcolor(RGB(17, 119, 189)); // 将背景色设为灰白色
                    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
                }memory->state = false; memory = &obj[2];
            }
            if (mouse.x >= obj[3].x_left && mouse.x < obj[3].x_right && mouse.y >= obj[3].y_dowm && mouse.y < obj[3].y_up && obj[3].state == false)
            {
                key = '2';
                //setfillcolor(RED); // 将填充颜色设为红色
                fillroundrect(obj[3].x_left, obj[3].y_dowm, obj[3].x_right, obj[3].y_up, 10, 10);
                settextcolor(RED); // 将字线条色设为红色
                outtextxy(obj[3].x_left + 15, obj[3].y_dowm + 10, obj[3].word);
                obj[3].state = true;
                if (memory->state == true) {

                    //setfillcolor(WHITE); // 将填充颜色设为白色

                    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//绘制按钮
                    settextcolor(RGB(17, 119, 189)); // 将字线条设为蓝色
                    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
                }memory->state = false; memory = &obj[3];
            }

            if (mouse.x >= obj[4].x_left && mouse.x < obj[4].x_right && mouse.y >= obj[4].y_dowm && mouse.y < obj[4].y_up && obj[4].state == false)
            {
                key = '1';
                //setfillcolor(RED); // 将填充颜色设为红色
                fillroundrect(obj[4].x_left, obj[4].y_dowm, obj[4].x_right, obj[4].y_up, 10, 10);
                settextcolor(RED); // 将字设为红色
                outtextxy(obj[4].x_left + 15, obj[4].y_dowm + 10, obj[4].word);
                obj[4].state = true;
                if (memory->state == true) {

                    //setfillcolor(WHITE); // 将填充颜色设为白色

                    fillroundrect(memory->x_left, memory->y_dowm, memory->x_right, memory->y_up, 10, 10);//绘制按钮
                    settextcolor(RGB(17, 119, 189)); // 将字设为蓝色
                    outtextxy(memory->x_left + 15, memory->y_dowm + 10, memory->word);
                }memory->state = false; memory = &obj[4];
            }

        }
    }

    // 关闭图形界面
    closegraph();
    return 0;
}
