#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"


bool textureMode = true;
bool lightMode = true;
void Texture();
//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����



//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	if (key == 'C') {
		Texture();
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}
GLuint texId;
void Texture() {
	static int k = 0;
	if (k % 2 == 0) {
		RGBTRIPLE* texarray;
		char* texCharArray;
		int texW, texH;
		OpenGL::LoadBMP("tex.bmp", &texW, &texH, &texarray);
		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//���������� �� ��� ��������
		glGenTextures(1, &texId);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, texId);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//�������� ������
		free(texCharArray);
		free(texarray);
		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		k++;
	}
	else {
		RGBTRIPLE* texarray;
		char* texCharArray;
		int texW, texH;
		OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//���������� �� ��� ��������
		glGenTextures(1, &texId);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, texId);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//�������� ������
		free(texCharArray);
		free(texarray);
		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		k++;
	}
}

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//���������� �� ��� ��������
	glGenTextures(2, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


double* findv(double* a, double* b) // ���� ������ �� 2 ������
{
	double* v = new double[3];
	v[0] = a[0] - b[0];
	v[1] = a[1] - b[1];
	return v;
}

double* findvn(double* a, double* b) // ���� ������ ������� �� 2 �������� a � b
{
	double* v = new double[3];
	v[0] = a[1] * b[2] - b[1] * a[2];
	v[1] = -a[0] * b[2] + b[0] * a[2];
	v[2] = a[0] * b[1] - b[0] * a[1];
	double len_v;
	len_v = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / len_v;
	v[1] = v[1] / len_v;
	v[2] = v[2] / len_v;
	return v;
}


void Render(OpenGL* ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);


	////////////////////////////////////////////////////////////////////////////

	double A[] = { 0, 0, 0 };
	double B[] = { -5, 6, 0 };
	double C[] = { -1,11, 0 };
	double D[] = { 4,9, 0 };
	double E[] = { 5,8, 0 };
	double F[] = { 6,4, 0 };
	double G[] = { 7, 16, 0 };
	double H[] = { 13, 10, 0 };
	double W[] = { -0.01, 0, 0 };

	double A1[] = { 0, 0, 2 };
	double B1[] = { -5, 6, 2 };
	double C1[] = { -1,11, 2 };
	double D1[] = { 4,9, 2 };
	double E1[] = { 5,8, 2 };
	double F1[] = { 6,4, 2 };
	double G1[] = { 7, 16, 2 };
	double H1[] = { 13, 10, 2 };


	//����
	float xcos, ysin, tx, ty, angle, radian, x, y, r = 2; 
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_POLYGON);
	glColor4f(0.7, 0.9, 0.7, 1.0);
	glNormal3d(0, 0, 29);
	for (angle = 0.0; angle < 360.0; angle += 2.0){
		radian = angle * (PI / 180.0f);
		xcos = (float)cos(radian);
		ysin = (float)sin(radian);
		x = xcos * r + 10;
		y = ysin * r + 2;
		tx = xcos * 0.5 + 0.5;
		ty = ysin * 0.5 + 0.5;
		glTexCoord2f(tx, ty);
		glVertex3f(x, y, 2);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);

	//����������
	double xx, yy, m1[1000][3], m2[1000][3];
	int k = 0;
	double rr = sqrt(61) / 2;
	for (double i = 0; i <= 360; i += 0.5) {
		xx = rr * cos(i * PI / 180) - 2.5;
		yy = rr * sin(i * PI / 180) + 3;
		if (yy <= -1.2 * xx) {
			m1[k][0] = xx;
			m1[k][1] = yy;
			m1[k][2] = 0;

			m2[k][0] = xx;
			m2[k][1] = yy;
			m2[k][2] = 2;
			k++;
		}
	}
	for (int i = 0; i < k; i++) {
		glBegin(GL_QUADS);
		glColor4f(0.7, 0.9, 0.7, 1.0);
		glTexCoord2d(0, 0);
		glVertex3dv(m1[i]);
		glTexCoord2d(0, 1);
		glVertex3dv(m1[i + 1]);
		glTexCoord2d(1, 1);
		glVertex3dv(m2[i + 1]);
		glTexCoord2d(1 , 0);
		glVertex3dv(m2[i]);
		glEnd();
	}

	//����������
	double xg, yg, m3[1000][3], m4[1000][3];
	int kg = 0;
	double rg = sqrt(104) / 2;
	for (double i = 0; i <= 360; i += 0.5) {
		xg = rg * cos(i * PI / 180) + 12;
		yg = rg * sin(i * PI / 180) + 15;
		if (yg <= -xg + 23) {
			m3[kg][0] = xg;
			m3[kg][1] = yg;
			m3[kg][2] = 0;

			m4[kg][0] = xg;
			m4[kg][1] = yg;
			m4[kg][2] = 2;
			kg++;
		}
	}
	for (int i = 0; i < kg; i++) {
		glBegin(GL_QUADS);
		glColor4f(0.8, 0.6, 1.0, 1.0);
		glTexCoord2d(0, 0);
		glVertex3dv(m3[i]);
		glTexCoord2d(0, 1);
		glVertex3dv(m3[i + 1]);
		glTexCoord2d(1, 1);
		glVertex3dv(m4[i + 1]);
		glTexCoord2d(1, 0);
		glVertex3dv(m4[i]);
		glEnd();

	}

	//���
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(B);
	glTexCoord2d(1, 1);
	glVertex3dv(C);
	glTexCoord2d(1, 0);
	glVertex3dv(D);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(D);
	glTexCoord2d(1, 1);
	glVertex3dv(E);
	glTexCoord2d(1, 0);
	glVertex3dv(F);

	//��� ����������
	for (int i = 0; i < k; i++) {
		glBegin(GL_QUADS);
		glColor4f(0.7, 0.9, 1.0, 1.0);
		glNormal3d(0, 0, -1);
		glTexCoord2d(0, 0);
		glVertex3dv(m1[i]);
		glTexCoord2d(0, 1);
		glVertex3dv(A);
		glTexCoord2d(1, 1);
		glVertex3dv(D);
		glTexCoord2d(1, 0);
		glVertex3dv(C);
		glEnd();
	}

	//��� ����������
	glBegin(GL_POLYGON);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glNormal3d(0, 0, -1);
	glTexCoord2d(4, 9);
	glVertex3dv(D);
	for (int i = 0; i < kg; i++) {
		if (m3[i][0] <= 0) {
			glTexCoord2d(0, 1);
			glVertex3dv(m3[i]);
		}
	}
	glTexCoord2d(5, 8);
	glVertex3dv(E);
	glEnd();
	glBegin(GL_POLYGON);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glNormal3d(0, 0, -1);
	glTexCoord2d(4, 9);
	glVertex3dv(D);
	for (int i = 0; i < kg; i++) {
		if (m3[i][0] >= 0) {
			glTexCoord2d(0, 1);
			glVertex3dv(m3[i]);
		}
	}
	glTexCoord2d(5, 8);
	glVertex3dv(E);
	glEnd();

	//�������
	/*glBegin(GL_QUADS);
	glColor4f(1.0, 0.8, 0.9, 1.0);
	glNormal3dv(findvn(findv(A, B), findv(A, A1)));
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(B);
	glTexCoord2d(1, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 0);
	glVertex3dv(A1);
	glEnd();*/
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.8, 0.9, 1.0);
	glNormal3dv(findvn(findv(B, C), findv(B, B1)));
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(C);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 0);
	glVertex3dv(B1);
	glEnd();
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.8, 0.9, 1.0);
	glNormal3dv(findvn(findv(C, D), findv(C, C1)));
	glTexCoord2d(0, 0);
	glVertex3dv(C);
	glTexCoord2d(0, 1);
	glVertex3dv(D);
	glTexCoord2d(1, 1);
	glVertex3dv(D1);
	glTexCoord2d(1, 0);
	glVertex3dv(C1);
	glEnd();
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.8, 0.9, 1.0);
	glNormal3dv(findvn(findv(D, G), findv(D, D1)));
	glTexCoord2d(0, 0);
	glVertex3dv(D);
	glTexCoord2d(0, 1);
	glVertex3dv(G);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glTexCoord2d(1, 0);
	glVertex3dv(D1);
	glEnd();
	/*glBegin(GL_QUADS);
	glColor4f(0.5, 0.6, 0.3, 1.0);
	glNormal3dv(findvn(findv(G, H), findv(G, G1)));
	glTexCoord2d(0, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 1);
	glVertex3dv(H);
	glTexCoord2d(1, 1);
	glVertex3dv(H1);
	glTexCoord2d(1, 0);
	glVertex3dv(G1);
	glEnd();*/
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.8, 0.9, 1.0);
	glNormal3dv(findvn(findv(H, E), findv(H, H1)));
	glTexCoord2d(0, 0);
	glVertex3dv(H);
	glTexCoord2d(0, 1);
	glVertex3dv(E);
	glTexCoord2d(1, 1);
	glVertex3dv(E1);
	glTexCoord2d(1, 0);
	glVertex3dv(H1);
	glEnd();
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.8, 0.9, 1.0);
	glNormal3dv(findvn(findv(E, F), findv(E, E1)));
	glTexCoord2d(0, 0);
	glVertex3dv(E);
	glTexCoord2d(0, 1);
	glVertex3dv(F);
	glTexCoord2d(1, 1);
	glVertex3dv(F1);
	glTexCoord2d(1, 0);
	glVertex3dv(E1);
	glEnd();
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.8, 0.9, 1.0);
	glNormal3dv(findvn(findv(F, A), findv(F, F1)));
	glTexCoord2d(0, 0);
	glVertex3dv(F);
	glTexCoord2d(0, 1);
	glVertex3dv(A);
	glTexCoord2d(1, 1);
	glVertex3dv(A1);
	glTexCoord2d(1, 0);
	glVertex3dv(F1);
	glEnd();

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//����
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glTexCoord2d(0, 0);
	glVertex3dv(A1);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 0);
	glVertex3dv(D1);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glTexCoord2d(0, 0);
	glVertex3dv(A1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(1, 1);
	glVertex3dv(E1);
	glTexCoord2d(1, 0);
	glVertex3dv(F1);

	//���� ����������
	for (int i = 0; i < k; i++) {
		glBegin(GL_QUADS);
		glColor4f(0.7, 0.9, 1.0, 1.0);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex3dv(m2[i]);
		glTexCoord2d(0, 1);
		glVertex3dv(A1);
		glTexCoord2d(1, 1);
		glVertex3dv(D1);
		glTexCoord2d(1, 0);
		glVertex3dv(C1);
		glEnd();
	}

	//���� ����������
	glBegin(GL_POLYGON);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex3dv(D1);
	for (int i = 0; i < kg; i++) {
		if (m4[i][0] <= 0) {
			glTexCoord2d(1, 0);
			glVertex3dv(m4[i]);
		}
	}
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glEnd();
	glBegin(GL_POLYGON);
	glColor4f(0.7, 0.9, 1.0, 1.0);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex3dv(D1);
	for (int i = 0; i < kg; i++) {
		if (m4[i][0] >= 0) {
			glTexCoord2d(1, 0);
			glVertex3dv(m4[i]);
		}
	}
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glEnd();

	///////////////////////////////////////////////////////////////////////////


   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	ss << "� - ����������� ��������" << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}