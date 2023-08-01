#ifndef Scene_h
#define Scene_h

#include "Shape.h"
#include "Cube.h"
#include <vector>

namespace CubeGame
{

    const int M = 4, N = 4;         // ������� ����� �� ������� ����

    class Scene
    {
        std::vector<Shape*> shapes; // "�������" ����� ��������� �� �������� ���
        float xStep, zStep;         // ³������ �� ������
        float width, height;        // ������ ����
        float mouseX, mouseY;       // ������ ���������� �������
        float angleX, angleY;       // �������� ��� �������� �����
        float WorldDistZ;           // ��������� �� Z
        int button;                 // ��������� ������
        bool finish;                // ������ ����, �� ��� ���������
        int time;                   // �������� ��� � ��������
        int minute;
        int hour;
        int count;
        Cube* cubes[N*M];
        int cubeArr[M][N];
        int xFrom, zFrom;     
        int xTo, zTo;    
    public:
        Scene(float xStep, float yStep);
        ~Scene();
        void on_paint();                                   // ��������� ����
        void on_size(int width, int height);               // ������ ����
        void on_motion(int x, int y);                      // �� ��� �������� ������
        void on_mouse(int button, int state, int x, int y);// ������ ������
        void on_timer(int value);                          // ������ (��� fps � ��������� ���������� �����
        void on_special(int key, int x, int y);
    private:
        void Initialize();                                 // �-� �����������
        void allocateCubes();
        bool moveCubes(int xFrom, int zFrom, int xTo, int zTo);
        bool findNearest(int x, int z, int& x1, int& z1);
        void fillingArr();
        void checkFinish();
        float allocX(int x);
        float allocZ(int z);
    };

}

#endif