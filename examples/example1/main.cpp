#include<Windows.h>
#include<gl\glew.h>
#include<gl\GL.h>
#include<gl\GLU.h>


// glut 来简化窗口操作，和输入操作
#include <GL/freeglut.h>

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <wchar.h>

#include "Font.h"

FTWRAPPER::FontManager *g_fm = NULL;
FTWRAPPER::Font *g_Font = NULL;


void keyfunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void idledisplay()
{
	static float framesPerSecond = 0.0f;       // This will store our fps
	static float lastTime = 0.0f;       // This will hold the time from the last frame
	float currentTime = GetTickCount() * 0.001f;
	++framesPerSecond;

	if (currentTime - lastTime > 1.0f)
	{

		lastTime = currentTime;
		char strFrameRate[255];
		sprintf_s(strFrameRate, "Current Frames Per Second: %d", int(framesPerSecond));
		glutSetWindowTitle(strFrameRate);
		framesPerSecond = 0;
		glutPostRedisplay();
	}

}

void DrawCharacter(int penx, int peny, int size, unsigned long codepoint, int *advance)
{
	if (g_Font == NULL)
		return;
	if (codepoint == 32)
	{
		if (advance)
			*advance = size * 0.5;
		return;
	}
	GLuint texture = g_Font->GetTextureID(codepoint);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	FTWRAPPER::Font::Glyph uv = g_Font->GetGlyph(texture, codepoint);
	float u = uv.right - uv.left; // hori
	float v = uv.bottom - uv.top; // ver

	GLuint vertex;
	float width = glutGet(GLUT_WINDOW_WIDTH);
	float height = glutGet(GLUT_WINDOW_HEIGHT);
	glGenBuffers(1, &vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	if (u > v)
	{
		// width > height, like '——'
		float aspect = v / u;
		GLfloat buf[6][7] = {
				{ penx / width, (peny + size * (1 + aspect) / 2) / height, uv.left, uv.top, 1.0, 0.0, 0.0 },
				{ (penx + size) / width, (peny + size * (1 + aspect) / 2) / height, uv.right, uv.top, 1.0, 0.0, 0.0 },
				{ penx / width, (peny + size * (1 - aspect) / 2) / height, uv.left, uv.bottom, 1.0, 0.0, 0.0 },
				{ (penx + size) / width, (peny + size * (1 + aspect) / 2) / height, uv.right, uv.top, 1.0, 0.0, 0.0 },
				{ (penx + size) / width, (peny + size * (1 - aspect) / 2) / height, uv.right, uv.bottom, 1.0, 0.0, 0.0 },
				{ penx / width, (peny + size * (1 - aspect) / 2) / height, uv.left, uv.bottom, 1.0, 0.0, 0.0 },
		};

		if (advance)
			*advance = size;

		glBufferData(GL_ARRAY_BUFFER, 7 * 6 * sizeof(GLfloat), buf, GL_STATIC_DRAW);

	}
	else
	{
		// height > width, like '|'
		float aspect = u / v;
		GLfloat buf[6][7] = {
				{ (penx) / width, (peny + size) / height, uv.left, uv.top, 1.0, 0.0, 0.0 },
				{ (penx + size * aspect) / width, (peny + size) / height, uv.right, uv.top, 1.0, 0.0, 0.0 },
				{ (penx) / width, peny / height, uv.left, uv.bottom, 1.0, 0.0, 0.0 },
				{ (penx + size * aspect) / width, (peny + size) / height, uv.right, uv.top, 1.0, 0.0, 0.0 },
				{ (penx + size * aspect) / width, peny / height, uv.right, uv.bottom, 1.0, 0.0, 0.0 },
				{ (penx) / width, peny / height, uv.left, uv.bottom, 1.0, 0.0, 0.0 },
		};

		if (advance)
			*advance = size * aspect;

		glBufferData(GL_ARRAY_BUFFER, 7 * 6 * sizeof(GLfloat), buf, GL_STATIC_DRAW);
	}

	glVertexPointer(2, GL_FLOAT, 7 * sizeof(GLfloat), 0);
	glTexCoordPointer(2, GL_FLOAT, 7 * sizeof(GLfloat), (const void*)(2 * sizeof(float)));
	glColorPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), (const void*)(4 * sizeof(float)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteBuffers(1, &vertex);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("gl error in %s %s", __LINE__, __LINE__);
	}
}

void DrawString(int penx, int peny, int size, wchar_t* str)
{
	for (int i = 0; i < wcslen(str); i++)
	{
		int advance = 0;
		DrawCharacter(penx, peny, size, str[i], &advance);
		penx += advance;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* clear the matrix */
	glLoadIdentity();
	/* viewing transformation  */

	if (g_Font)
	{
		wchar_t str[255];
		for (unsigned long i = 0x4e00; i < 0x4e0a; i++)
			str[i - 0x4e00] = i;
		str[0x4e0a - 0x4e00] = 0x0000;
		wchar_t *line = L"Life Is Cool 程澈";
		DrawString(100, 100, g_Font->GetFontSize(), line);
	}

	glFlush();
}

void init(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glShadeModel(GL_FLAT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void InitFont()
{
	g_Font = new FTWRAPPER::Font("tuanzi", "../../media/fonts/tuanzi.ttf", 2048, 50, 1);
	if (g_Font)
	{
		g_Font->AddCodePointRange(33, 255);
		g_Font->AddCodePointRange(28000, 29000);
		g_Font->AddCodePointRange(31000, 33000);
		time_t t = time(NULL);
		g_Font->LoadFont();
		time_t t2 = time(NULL);
		printf("Load time: %d", t2 - t);
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	// 创建一个窗口，返回一个当前窗口的唯一标识
	glutCreateWindow("Hello");

	glewInit();
	init();

	glutDisplayFunc(display);

	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyfunc);

	glutIdleFunc(idledisplay);

	InitFont();
	
	// 只有调用glutMainLoop，对于窗口的渲染才会生效
	glutMainLoop();

	if (g_Font)
		delete g_Font;
	return 0;
}

