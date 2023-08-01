#define _CRT_SECURE_NO_WARNINGS
#include <gl/glut.h>
#include <cmath>
#include <stdio.h>
#include "Scene.h"
#include "utils.h"
#include "Board.h"
#include "Cube.h"

namespace CubeGame
{
    using namespace GraphUtils;

    Scene::Scene(float xStep, float zStep) {
        this->xStep = xStep;
        this->zStep = zStep;
        // �������� ������� ������ Cube � ���������� ��� � ������ �����
        shapes.push_back(new Board(N * xStep, 0.1, M * xStep, 0.0, 0.0, 0.0, diffGray, ambiGray, specGray));
        int k = 0;
        for (int i = 0; i < M - 1; i++) {
            for (int j = 0; j < N; j++) {
                cubes[k] = new Cube(0.39, 0.39, 0.39, allocX(j), 0.20, allocZ(i), diffYellow, ambiWhite, specYellow);
                shapes.push_back(cubes[k]);
                k++;
            }
        }
        cubes[k] = new Cube(0.39, 0.39, 0.39, allocX(0), 0.20, allocZ(M - 1), diffYellow, ambiWhite, specYellow);
        shapes.push_back(cubes[k]);
        k++;
        cubes[k] = new Cube(0.39, 0.39, 0.39, allocX(1), 0.20, allocZ(M - 1), diffYellow, ambiWhite, specYellow);
        shapes.push_back(cubes[k]);
        k++;
        cubes[k] = new Cube(0.39, 0.39, 0.39, allocX(2), 0.20, allocZ(M - 1), diffYellow, ambiWhite, specYellow);
        shapes.push_back(cubes[k]);
        Initialize();
    }

    Scene::~Scene() {
        // ��������� �� ������:
        for (size_t i = 0; i < shapes.size(); i++)
        {
            delete shapes[i];
        }
    }

    void Scene::fillingArr() {
        int k = 1;
        for (int i = 0; i < M - 1; i++)
        {
            for (int j = 0; j < N; j++)
            {
                cubeArr[i][j] = k;
                k += 1;

            }
        }
        for (int j = 0; j < M - 1; j++)
        {
            cubeArr[M - 1][j] = k;
            k += 1;
        }
        cubeArr[3][3] = 0;

    }

    // ������������� ������� ������ cubeArr � ���������� x
    float Scene::allocX(int i)
    {
        return  xStep * i - (N - 1) * xStep / 2;
    }

    // ������������� ������� ������ cubeArr � ���������� z
    float Scene::allocZ(int i)
    {
        return  zStep * i - (M - 1) * zStep / 2;
    }

    void Scene::allocateCubes()
    {
        for (int i = 0; i < M - 1; i++) {
            for (int j = 0; j < N; j++) {
                if (cubeArr[i][j] > 0)
                {
                    cubes[cubeArr[i][j] - 1]->setCoords(allocX(j), 0.20, allocZ(i));
                }
            }
        }
        cubes[cubeArr[M - 1][0] - 1]->setCoords(allocX(0), 0.20, allocZ(3));
        cubes[cubeArr[M - 1][1] - 1]->setCoords(allocX(1), 0.20, allocZ(3));
        cubes[cubeArr[M - 1][2] - 1]->setCoords(allocX(2), 0.20, allocZ(3));
    }

    bool Scene::moveCubes(int xFrom, int zFrom, int xTo, int zTo)
     {
        // �������� ��������� ����������:
        if (xFrom < 0 || zFrom < 0 || xFrom >= N || zFrom >= M || cubeArr[zFrom][xFrom] == 0)
        {
            return false;
        }
        if (xTo < 0 || zTo < 0 || xTo >= N || zTo >= M || cubeArr[zTo][xTo] > 0)
        {
            return false;
        }
        if (xFrom == xTo && zFrom == zTo)
        {
            return false;
        }
        if (xFrom != xTo && zFrom != zTo)
        {
            return false;
        }
        if (xFrom - xTo > 1 || xTo - xFrom > 1 || zFrom - zTo > 1 || zTo - zFrom > 1)
        {
            return false;
        }
        if (cubes[cubeArr[zFrom][xFrom] - 1]->getYCenter() < 0.2)
        {
            return false;
        }

        // ����������:
        cubes[cubeArr[zFrom][xFrom] - 1]->setXCenter(allocX(xTo));
        cubes[cubeArr[zFrom][xFrom] - 1]->setZCenter(allocZ(zTo));
        // �������� ��� � ��� ������ fields:
        cubeArr[zTo][xTo] = cubeArr[zFrom][xFrom];
        cubeArr[zFrom][xFrom] = 0;

        // �������� �� ���������� ����
        checkFinish();

        return true;
    }



    void Scene::Initialize() {
        fillingArr(); // ��������� ����� ������������ �����

        GraphUtils::shuffle((int*)cubeArr, 3 * 5);
        allocateCubes();
        angleX = -10;
        angleY = 30;
        WorldDistZ = -3;
        time = 0;
        minute = 0;
        hour = 0;
        count = 0;
        finish = false;
    }

    bool Scene::findNearest(int x, int y, int& x1, int& z1)
    {
        int viewport[4];
        int iMin = -1, jMin = -1;
        double mvMatrix[16], projMatrix[16];
        double minDist = 2000;

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {

                // ����� x, y, z ��������� ����:
                double wx = allocX(j);
                double wy = 0.20;
                double wz = allocZ(i);

                // ���������� ����� viewport �������� ������� ���������:
                glGetIntegerv(GL_VIEWPORT, viewport);

                // ���������� ������ ��������� ���������:
                glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
                glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

                // ����� x, y, z ����������, �� �����������:
                double dx, dy, dz;

                // �������� ���������� �����, �� ��� ������������ �������� ���:
                gluProject(wx, wy, wz, mvMatrix, projMatrix, viewport, &dx, &dy, &dz);
                dy = viewport[3] - dy - 1; // dy ��������� ������������
                double d = (x - dx) * (x - dx) + (y - dy) * (y - dy); // ������� ������
                if (d < minDist) // ������� ������� ���
                {
                    minDist = d;
                    iMin = i;
                    jMin = j;
                }
            }
        }
        if (minDist < 1000) // ������� ���������� ���
        {
            x1 = jMin;
            z1 = iMin;
            return true;
        }
        else
        {
            return false;
        }
    }

    void Scene::on_paint() {
        char text[128]; // ����� �������, 

        if (finish)
        {
            glClearColor(0.0f, 0.4f, 0.1f, 1.0f);
            sprintf(text, "Complite!. Time: %d hour. %d minute. %d sec. |F2 - Restart game | Esc - Exit |", hour, minute, time);
        }
        else
        {
            glClearColor(0.5f, 0.4f, 1.0f, 1.0f);
            sprintf(text, "F2 - Restart game | Esc - Exit | Time: %d hour. %d minute. %d sec.", hour, minute, time);
        }

        glViewport(0, 0, width, height);
        // ���������� ��������� �������� � ������� �����:
        float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // ���� �������� ��������� 
        float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���� ��������� ��������� 
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };// ���� ������������ �����������
        float lightPosition[] = { 0.5f, 1.0f, 2.0f, 0.0f };// ������������ ������� �����

        // ������������ ��������� ������� ����� 0:
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // ��� ����������� ������, ����� ��������������� ������������ ��������:
        glOrtho(0, 1, 0, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor3f(1, 1, 0); // ������ �����
        drawString(GLUT_BITMAP_HELVETICA_18, text, 0.01, 0.95);
        glPopMatrix();
     
        //����� ������ � �������� ��������:
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, width / height, 1, 100);
        // �������� ������������� ������
        glMatrixMode(GL_MODELVIEW);     
        glLoadIdentity();
        glTranslatef(0, 0, WorldDistZ);	      // ������ � ������� ��������� ��������� �� WorldDistZ,
        glRotatef(angleX, 0.0f, 1.0f, 0.0f);  // ���� ���������� �� �� Oy
        glRotatef(angleY, 1.0f, 0.0f, 0.0f);  // ���� ���������� �� �� Ox
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHTING);
        // ������ ������� ����� # 0 
        glEnable(GL_LIGHT0);

        for (int i = 0; i < shapes.size(); i++)
        {
            shapes[i]->draw();

            if (i > 0) // ���������� �����
            {
                int cubeNumber = i;
                float cubeX = shapes[i]->getXCenter();
                float cubeY = shapes[i]->getYCenter();
                float cubeZ = shapes[i]->getZCenter();

                char textCube[15];
                sprintf(textCube, "%d", cubeNumber);

                drawText(textCube, cubeX, cubeY + 0.2, cubeZ);
            }
        }

        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glFlush();
        glutSwapBuffers();
    }

    void Scene::on_size(int width, int height) {
        this->width = width;
        if (height == 0)
            height = 1;
        this->height = height;
    }

    void Scene::on_special(int key, int x, int y)
    {
        switch (key) {
        case GLUT_KEY_UP:   // ����������
            if (WorldDistZ == -1.7)
            {
                break;
            }
            if (count == 8) {
                break;
            }
            WorldDistZ += 0.1;
            count++;
            break;
        case GLUT_KEY_DOWN: // ���������
            WorldDistZ -= 0.1;
            count--;
            break;
        case GLUT_KEY_F2:   // ���� ���
            Initialize();
            break;
        }
    }

    void Scene::on_mouse(int button, int state, int x, int y)
    {
        // �������� ������ ���������� ����:
        mouseX = x;
        mouseY = y;
        if ((state == GLUT_UP)) // ������ ������
        {
            this->button = -1;  // ���� ������ �� ���������
            return;
        }
        this->button = button;  // �������� ���������� ��� ������
        if (finish)
        {
            return;
        }
        // �������� ���� ��� �����������:
    }

    // ���������� ����, ���'������ � ������������ ���� � ���������� �������
    void Scene::on_motion(int x, int y)
    {
        switch (button)
        {
        case 0: // ��� ������ - ����������� �����
            if (finish)
                break;
            if (findNearest(x, y, xTo, zTo))
            {
                moveCubes(xFrom, zFrom, xTo, zTo);
                xFrom = xTo;
                zFrom = zTo;
            }
            break;
        case 2: // ����� ������ - ��������� �����
            angleX += x - mouseX;
            angleY += y - mouseY;
            mouseX = x;
            mouseY = y;
            break;
        }
    }

    int tick = 0; // ��������, �������� ����� ��������� ���� 25 ��

    // ���������� ��䳿 �� �������
    void Scene::on_timer(int value)
    {
        tick++;
        if (tick >= 40) // ���������� �������� �������
        {
            if (!finish)// ������� �����������, ���� ��� �� ��������
            {
                time++;
                if (time == 60) {
                    minute++;
                    time = 0;
                    if (minute == 60) {
                        hour++;
                        minute = 0;
                    }
                }
            }
            tick = 0;   // ������� ��������
        }
        on_paint();     // ��������� ������������� ����
    }

    void Scene::checkFinish() {
        bool correctOrder = true;
        int k = 1;
        for (int i = 0; i < M - 1; i++) {
            for (int j = 0; j < N; j++) {
                if (cubeArr[i][j] != k) {
                    correctOrder = false;
                    break;
                }
                k++;
            }
            if (!correctOrder) {
                break;
            }
        }
            for (int j = 0; j < 3; j++) {
                if (cubeArr[3][j] != k) {
                    correctOrder = false;
                    break;
                }
                k++;
            }
        if (correctOrder) {
            finish = true;
        }
    }

}