#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include "SDL.h"
#include "vector3.h"

const int WIDTH = 1440;
const int HEIGHT = 900;
const int FOV = 80;

#define DEG2RAD(x) ((x)*M_PI/180.0)

inline vector3f normal(const vector3f &v1, const vector3f &v2, const vector3f &v3) {
	return (v1-v2).Cross(v2-v3).Normalized();
}

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}

	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	switch (info->vfmt->BitsPerPixel) {
		case 16:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
			break;
		case 24:
		case 32:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			break;
		default:
			fprintf(stderr, "Invalid pixel depth: %d bpp\n", info->vfmt->BitsPerPixel);
			exit(-1);
	} 
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	if (!SDL_SetVideoMode(WIDTH, HEIGHT, info->vfmt->BitsPerPixel, SDL_OPENGL)) {
		fprintf(stderr, "Failed to set video mode: %s", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	SDL_WM_SetCaption("hyperspace", "hyperspace");

	glewInit();

	glShadeModel(GL_SMOOTH);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	const float lightPos[4] = { -50,-50,0,1 };
	const float lightAmbient[4] = { 0,0,0,1 };
	const float lightDiffuse[4] = { 1,1,1,1 };
	const float lightSpecular[4] = { 1,1,1,1 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	const float specular[4] = { 0,0,0,1 };
	const float emission[4] = { 0,0,0,1 };

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);

	{
		float fov = tan(DEG2RAD(FOV / 2.0f));
		float znear = 1.0, zfar = 100.0;
		float left = fov * znear;
		float top = left * HEIGHT/WIDTH;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-left, left, -top, top, znear, zfar);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	const float o = 0.01f;

	const float left = -20.0f;
	const float right = 20.0f;
	const float front = -30.0f;
	const float back = -100.0f;
	const float depth = 10.0f;
	const float centre = 10.0f;

	float c = 0.0f;

	SDL_Event e;
	while (!SDL_PollEvent(&e) || e.type != SDL_KEYDOWN || e.key.keysym.sym != SDLK_ESCAPE) {

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBegin(GL_QUADS);

			for (float n = 0.0f; n < 1.0f; n += o) {
				float k = c+n;

				float ky1 = cos(2*M_PI*k);
				float ky2 = cos(2*M_PI*(k+o));

				float x1 = left + n*(right-left);
				float x2 = x1 + o*(right-left);

				for (float m = 0.0f; m < 1.0f; m += o) {
					float j = c+m;

					float jy1 = sin(2*M_PI*j);
					float jy2 = sin(2*M_PI*(j+o));

					float z1 = front + m*(back-front);
					float z2 = z1 + o*(back-front);

					float front_y1 = ky1 * jy1 * depth - centre;
					float front_y2 = ky2 * jy1 * depth - centre;
					float back_y1 = ky1 * jy2 * depth - centre;
					float back_y2 = ky2 * jy2 * depth - centre;

					vector3f norm(normal(vector3f(x2, front_y2, z1), vector3f(x2, back_y2, z2), vector3f(x1, back_y1, z2)));

					glColor3f(n, m, 0.6f);

					glVertex3f(x2, front_y2, z1);
					glNormal3f(norm.x, norm.y, norm.z);
					glVertex3f(x2 ,back_y2, z2);
					glNormal3f(norm.x, norm.y, norm.z);
					glVertex3f(x1, back_y1, z2);
					glNormal3f(norm.x, norm.y, norm.z);
					glVertex3f(x1, front_y1, z1);
					glNormal3f(norm.x, norm.y, norm.z);
				}
			}

		glEnd();

		c += o;
		if (c > 1.0) c -= 1.0f;
		
		SDL_GL_SwapBuffers();
	}

	SDL_Quit();
	return 0;
}
