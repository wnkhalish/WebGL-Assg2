#include <stdlib.h>
#include <windows.h>
#include <glut.h>
#include <stdio.h>
#include <cmath>
#include <GL/GL.H>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <glut.h>
#include <cstdlib>
#include <iomanip>

using namespace std;

float v[1262][3];
int f[1500][3];

static float Xangle = 150.0, Yangle = 0.0, Zangle = 0.0;
float angle = 0.0;
float lx = 0.0f, lz = -1.0f;
float x = 0.0f, z = 5.0f;
float g_rotation;

GLfloat light_diffuse[4] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_specular[4] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
#define X .525731112119133606
#define Z .850650808352039932
#define Y 1
#define K .5
#define checkImageWidth 64
#define checkImageHeight 64
#define subImageWidth 16
#define subImageHeight 16
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
static GLubyte subImage[subImageHeight][subImageWidth][4];

static GLuint texName;
void makeCheckImages(void)
{
    int i, j, c;

    for (i = 0; i < checkImageHeight; i++) {
        for (j = 0; j < checkImageWidth; j++) {
            c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
            checkImage[i][j][0] = (GLubyte)c;
            checkImage[i][j][1] = (GLubyte)c;
            checkImage[i][j][2] = (GLubyte)c;
            checkImage[i][j][3] = (GLubyte)255;
        }
    }
    for (i = 0; i < subImageHeight; i++) {
        for (j = 0; j < subImageWidth; j++) {
            c = ((((i & 0x4) == 0) ^ ((j & 0x4)) == 0)) * 255;
            subImage[i][j][0] = (GLubyte)c;
            subImage[i][j][1] = (GLubyte)0;
            subImage[i][j][2] = (GLubyte)0;
            subImage[i][j][3] = (GLubyte)255;
        }
    }
}

static GLfloat idata[12][3] = {
 {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
 {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
 {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};
static GLuint iindices[20][3] = {
 {1,4,0}, {4,9,0}, {4,9,5}, {8,5,4}, {1,8,4},
 {1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
 {3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
 {10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
};

static GLfloat cdata[8][3] = {
    {-Y, -Y, -K}, {Y, -Y, -K}, {Y, Y, -K}, {-Y, Y, -K},
    {-Y, -Y, K }, {Y, -Y, K }, {Y, Y, K }, {-Y, Y, K }
};
static GLuint cindices[12][3] = {
    {5, 4, 0},{1, 5, 0}, {6, 5, 1},{2, 6, 1},
    {7, 6, 2},{3, 7, 2}, {4, 7, 3},{0, 4, 3},
    {6, 7, 4},{5, 6, 4}, {1, 0, 3},{2, 1, 3}
};

static GLfloat tdata[4][3] = {
    {Y,  Y,  Y}, { -Y, Y, -Y}, {Y, -Y, -Y}, {-Y, -Y, Y}
};

static GLuint tindices[4][3] = {
    {1, 2, 3},
    {0, 3, 2},
    {0, 1, 3},
    {0, 2, 1}
};

GLfloat d1[3], d2[3], n[3];

void normalize(float v[3]) {
    GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}
void normCrossProd(float u[3], float v[3], float out[3]) {
    out[0] = u[1] * v[2] - u[2] * v[1];
    out[1] = u[2] * v[0] - u[0] * v[2];
    out[2] = u[0] * v[1] - u[1] * v[0];
    normalize(out);
}
void icoNormVec(int i) {
    for (int k = 0; k < 3; k++) {
        d1[k] = idata[iindices[i][0]][k] - idata[iindices[i][1]][k];
        d2[k] = idata[iindices[i][1]][k] - idata[iindices[i][2]][k];
    }
    normCrossProd(d1, d2, n);
    glNormal3fv(n);
}

void cuNormVec(int i) {
    for (int k = 0; k < 3; k++) {
        d1[k] = cdata[cindices[i][0]][k] - cdata[cindices[i][1]][k];
        d2[k] = cdata[cindices[i][1]][k] - cdata[cindices[i][2]][k];
    }
    normCrossProd(d1, d2, n);
    glNormal3fv(n);
}
void teNormVec(int i) {
    for (int k = 0; k < 3; k++) {
        d1[k] = tdata[tindices[i][0]][k] - tdata[tindices[i][1]][k];
        d2[k] = tdata[tindices[i][1]][k] - tdata[tindices[i][2]][k];
    }
    normCrossProd(d1, d2, n);
    glNormal3fv(n);
}
void init() {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = { 20.0, 20.0, 20.0, 20.0 };
    glLightfv(GL_FRONT, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_FRONT, GL_SPECULAR, light_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    makeCheckImages();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}
void idle() {
    glutPostRedisplay();
}
class Coord {
public:
    float x;
    float y;
    float z;
}
a3d, b3d, c3d, original;

Coord norm(Coord a3d, Coord b3d, Coord c3d) {
    Coord d3D_1, d3D_2, cross;
    d3D_1.x = b3d.x - a3d.x;
    d3D_1.y = b3d.y - a3d.y;
    d3D_1.z = b3d.z - a3d.z;

    d3D_2.x = c3d.x - b3d.x;
    d3D_2.y = c3d.y - b3d.y;
    d3D_2.z = c3d.z - b3d.z;

    cross.x = d3D_1.y * d3D_2.z - d3D_1.z * d3D_2.y;
    cross.y = d3D_1.z * d3D_2.x - d3D_1.x * d3D_2.z;
    cross.z = d3D_1.x * d3D_2.y - d3D_1.y * d3D_2.x;
    double l;

    l = sqrt(cross.x * cross.x + cross.y * cross.y + cross.z * cross.z);
    cross.x /= l;
    cross.y /= l;
    cross.z /= l;
    return cross;
}
//============================================Nut=====================================================
void drawFlatNut() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glTranslatef(0, 0, -90);
    glScalef(4.0f, 4.0f, 4.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 1500; i++) {
        a3d.x = v[f[i][0]][0];
        a3d.y = v[f[i][0]][1];
        a3d.z = v[f[i][0]][2];
        b3d.x = v[f[i][1]][0];
        b3d.y = v[f[i][1]][1];
        b3d.z = v[f[i][1]][2];
        c3d.x = v[f[i][2]][0];
        c3d.y = v[f[i][2]][1];
        c3d.z = v[f[i][2]][2];

        glVertex3f(a3d.x, a3d.y, a3d.z);
        glVertex3f(b3d.x, b3d.y, b3d.z);
        glVertex3f(c3d.x, c3d.y, c3d.z);
        Coord response = norm(a3d, b3d, c3d);

        glNormal3f(response.x, response.y, response.z);
        glPopMatrix();
    }

    glEnd();
    glutSwapBuffers();
}
void drawGouraudNut() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glTranslatef(0, 0, -90);
    glScalef(4.0f, 4.0f, 4.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);

    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 1500; i++) {
        a3d.x = v[f[i][0]][0];
        a3d.y = v[f[i][0]][1];
        a3d.z = v[f[i][0]][2];
        b3d.x = v[f[i][1]][0];
        b3d.y = v[f[i][1]][1];
        b3d.z = v[f[i][1]][2];
        c3d.x = v[f[i][2]][0];
        c3d.y = v[f[i][2]][1];
        c3d.z = v[f[i][2]][2];

        glNormal3f(a3d.x, a3d.y, a3d.z);
        glVertex3f(a3d.x, a3d.y, a3d.z);
        glNormal3f(b3d.x, b3d.y, b3d.z);
        glVertex3f(b3d.x, b3d.y, b3d.z);
        glNormal3f(c3d.x, c3d.y, c3d.z);
        glVertex3f(c3d.x, c3d.y, c3d.z);
        glPopMatrix();
    }

    glEnd();
    glutSwapBuffers();
}
void drawFlatTextureNut() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glTranslatef(0, 0, -90);
    glScalef(4.0f, 4.0f, 4.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glShadeModel(GL_FLAT);
    glBindTexture(GL_TEXTURE_2D, texName);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 1500; i++) {
        a3d.x = v[f[i][0]][0];
        a3d.y = v[f[i][0]][1];
        a3d.z = v[f[i][0]][2];
        b3d.x = v[f[i][1]][0];
        b3d.y = v[f[i][1]][1];
        b3d.z = v[f[i][1]][2];
        c3d.x = v[f[i][2]][0];
        c3d.y = v[f[i][2]][1];
        c3d.z = v[f[i][2]][2];

        glTexCoord2f(0.0, 0.0);
        glVertex3f(a3d.x, a3d.y, a3d.z);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(b3d.x, b3d.y, b3d.z);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(c3d.x, c3d.y, c3d.z);
        Coord response = norm(a3d, b3d, c3d);

        glNormal3f(response.x, response.y, response.z);
        glPopMatrix();
    }

    glEnd();
    glutSwapBuffers();
}
void drawGouraudTextureNut() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glTranslatef(0, 0, -90);
    glScalef(4.0f, 4.0f, 4.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glShadeModel(GL_FLAT);
    glBindTexture(GL_TEXTURE_2D, texName);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 1500; i++) {
        a3d.x = v[f[i][0]][0];
        a3d.y = v[f[i][0]][1];
        a3d.z = v[f[i][0]][2];
        b3d.x = v[f[i][1]][0];
        b3d.y = v[f[i][1]][1];
        b3d.z = v[f[i][1]][2];
        c3d.x = v[f[i][2]][0];
        c3d.y = v[f[i][2]][1];
        c3d.z = v[f[i][2]][2];


        glNormal3f(a3d.x, a3d.y, a3d.z);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(a3d.x, a3d.y, a3d.z);

        glNormal3f(b3d.x, b3d.y, b3d.z);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(b3d.x, b3d.y, b3d.z);
        glNormal3f(c3d.x, c3d.y, c3d.z);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(c3d.x, c3d.y, c3d.z);
        glPopMatrix();
    }

    glEnd();
    glutSwapBuffers();
}
//============================================Pyramid=====================================================
void drawFlatPyramid(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; i++) {
        teNormVec(i);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&tdata[tindices[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&tdata[tindices[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&tdata[tindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawGouraudPyramid(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    /* gluLookAt(x, 1.0f, z,
         x + lx, 1.0f, z + lz,
         0.0f, 1.0f, 0.0f);*/
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; i++) {
        glNormal3fv(&tdata[tindices[i][0]][0]);
        glVertex3fv(&tdata[tindices[i][0]][0]);
        glNormal3fv(&tdata[tindices[i][1]][0]);
        glVertex3fv(&tdata[tindices[i][1]][0]);
        glNormal3fv(&tdata[tindices[i][2]][0]);
        glVertex3fv(&tdata[tindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawFlatTexturePyramid(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);
    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glShadeModel(GL_FLAT);
    glBindTexture(GL_TEXTURE_2D, texName);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; i++) {
        teNormVec(i);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&tdata[tindices[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&tdata[tindices[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&tdata[tindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawGouraudTexturePyramid(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);
    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glShadeModel(GL_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, texName);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; i++) {
        glTexCoord2f(0.0, 0.0);
        glNormal3fv(&tdata[tindices[i][0]][0]);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&tdata[tindices[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glNormal3fv(&tdata[tindices[i][1]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&tdata[tindices[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glNormal3fv(&tdata[tindices[i][2]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&tdata[tindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
//============================================Cube=====================================================
void drawFlatCube(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 12; i++) {
        cuNormVec(i);
        glVertex3fv(&cdata[cindices[i][0]][0]);
        glVertex3fv(&cdata[cindices[i][1]][0]);
        glVertex3fv(&cdata[cindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawGouraudCube(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 12; i++) {
        glNormal3fv(&cdata[cindices[i][0]][0]);
        glVertex3fv(&cdata[cindices[i][0]][0]);
        glNormal3fv(&cdata[cindices[i][1]][0]);
        glVertex3fv(&cdata[cindices[i][1]][0]);
        glNormal3fv(&cdata[cindices[i][2]][0]);
        glVertex3fv(&cdata[cindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawFlatTextureCube(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, texName);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 12; i++) {
        cuNormVec(i);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&cdata[cindices[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&cdata[cindices[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&cdata[cindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawGouraudTextureCube(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, texName);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 12; i++) {
        glTexCoord2f(0.0, 0.0);
        glNormal3fv(&cdata[cindices[i][0]][0]);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&cdata[cindices[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glNormal3fv(&cdata[cindices[i][1]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&cdata[cindices[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glNormal3fv(&cdata[cindices[i][2]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&cdata[cindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
//============================================Icosahedron=====================================================
void drawFlatIcosahedron(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        icoNormVec(i);
        glVertex3fv(&idata[iindices[i][0]][0]);
        glVertex3fv(&idata[iindices[i][1]][0]);
        glVertex3fv(&idata[iindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawGouraudIcosahedron(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        glNormal3fv(&idata[iindices[i][0]][0]);
        glVertex3fv(&idata[iindices[i][0]][0]);
        glNormal3fv(&idata[iindices[i][1]][0]);
        glVertex3fv(&idata[iindices[i][1]][0]);
        glNormal3fv(&idata[iindices[i][2]][0]);
        glVertex3fv(&idata[iindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawFlatTextureIcosahedron(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);

    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, texName);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        icoNormVec(i);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&idata[iindices[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&idata[iindices[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&idata[iindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void drawGouraudTextureIcosahedron(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glLoadIdentity();
    gluLookAt(x, 0.0f, z,
        x + lx, 0.0f, z + lz,
        0.0f, 1.0f, 0.0f);
    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, texName);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        glTexCoord2f(0.0, 0.0);
        glNormal3fv(&idata[iindices[i][0]][0]);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&idata[iindices[i][0]][0]);
        glTexCoord2f(0.0, 1.0);
        glNormal3fv(&idata[iindices[i][1]][0]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&idata[iindices[i][1]][0]);
        glTexCoord2f(1.0, 1.0);
        glNormal3fv(&idata[iindices[i][2]][0]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&idata[iindices[i][2]][0]);
    }
    glEnd();
    glFlush();
    glEnd();
    glutSwapBuffers();

}
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 1.0, 8000.0);
    glMatrixMode(GL_MODELVIEW);

}

void processNormalKeys(unsigned char key, int x, int y) {

    switch (key) {
        //============================================Diffuse Light=====================================================
    case 'i':
        light_diffuse[0] += 0.1;
        glColor4fv(light_diffuse);
        glutPostRedisplay();
        break;
    case 'o':
        light_diffuse[1] += 0.1;
        glColor4fv(light_diffuse);
        glutPostRedisplay();
        break;
    case 'p':
        light_diffuse[2] += 0.1;
        glColor4fv(light_diffuse);
        glutPostRedisplay();
        break;
    case 'I':
        light_diffuse[0] -= 0.1;
        glColor4fv(light_diffuse);
        glutPostRedisplay();
        break;
    case 'O':
        light_diffuse[1] -= 0.1;
        glColor4fv(light_diffuse);
        glutPostRedisplay();
        break;
    case 'P':
        light_diffuse[2] -= 0.1;
        glColor4fv(light_diffuse);
        glutPostRedisplay();
        break;
        //============================================Specular Light=====================================================
    case 'j':
        light_specular[0] += 0.1;
        glColor4fv(light_specular);
        glutPostRedisplay();
        break;
    case 'k':
        light_specular[1] += 0.1;
        glColor4fv(light_specular);
        glutPostRedisplay();
        break;
    case 'l':
        light_specular[2] += 0.1;
        glColor4fv(light_specular);
        glutPostRedisplay();
        break;
    case 'J':
        light_specular[0] -= 0.1;
        glColor4fv(light_specular);
        glutPostRedisplay();
        break;
    case 'K':
        light_specular[1] -= 0.1;
        glColor4fv(light_specular);
        glutPostRedisplay();
        break;
    case 'L':
        light_specular[2] -= 0.1;
        glColor4fv(light_specular);
        glutPostRedisplay();
        break;
        //============================================Ambient Light=====================================================
    case 'b':
        light_ambient[0] += 0.1;
        glColor4fv(light_ambient);
        glutPostRedisplay();
        break;
    case 'n':
        light_ambient[1] += 0.1;
        glColor4fv(light_ambient);
        glutPostRedisplay();
        break;
    case 'm':
        light_ambient[2] += 0.1;
        glColor4fv(light_ambient);
        glutPostRedisplay();
        break;
    case 'B':
        light_ambient[0] -= 0.1;
        glColor4fv(light_ambient);
        glutPostRedisplay();
        break;
    case 'N':
        light_ambient[1] -= 0.1;
        glColor4fv(light_ambient);
        glutPostRedisplay();
        break;
    case 'M':
        light_ambient[2] -= 0.1;
        glColor4fv(light_ambient);
        glutPostRedisplay();
        break;
        //============================================Rotate Object=====================================================
    case 'x':
        Xangle += 5.0;
        if (Xangle > 360.0) Xangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'X':
        Xangle -= 5.0;
        if (Xangle < 0.0) Xangle += 360.0;
        glutPostRedisplay();
        break;
    case 'y':
        Yangle += 5.0;
        if (Yangle > 360.0) Yangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'Y':
        Yangle -= 5.0;
        if (Yangle < 0.0) Yangle += 360.0;
        glutPostRedisplay();
        break;
    case 'z':
        Zangle += 5.0;
        if (Zangle > 360.0) Zangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'Z':
        Zangle -= 5.0;
        if (Zangle < 0.0) Zangle += 360.0;
        glutPostRedisplay();
        break;
    case ',':
        glBindTexture(GL_TEXTURE_2D, texName);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 12, 44, subImageWidth,
            subImageHeight, GL_RGBA,
            GL_UNSIGNED_BYTE, subImage);
        glutPostRedisplay();
        break;
    case '.':
        glBindTexture(GL_TEXTURE_2D, texName);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
            checkImageHeight, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, checkImage);
        glutPostRedisplay();
        break;
        //===================================== Flat Shading Key ===================================
    case 'q':
        glutDisplayFunc(drawFlatPyramid);
        glutIdleFunc(idle);
        break;
    case 'w':
        glutDisplayFunc(drawFlatCube);
        glutIdleFunc(idle);
        break;
    case 'e':
        glutDisplayFunc(drawFlatIcosahedron);
        glutIdleFunc(idle);
        break;
    case 'r':
        glutDisplayFunc(drawFlatNut);
        glutIdleFunc(idle);
        break;
        //===================================== Gouraud Shading Key ===================================
    case 'Q':
        glutDisplayFunc(drawGouraudPyramid);
        glutIdleFunc(idle);
        break;
    case 'W':
        glutDisplayFunc(drawGouraudCube);
        glutIdleFunc(idle);
        break;
    case 'E':
        glutDisplayFunc(drawGouraudIcosahedron);
        glutIdleFunc(idle);
        break;
    case 'R':
        glutDisplayFunc(drawGouraudNut);
        glutIdleFunc(idle);
        break;
        //===================================== Texture Mapping Key ===================================
    case 'a':
        glutDisplayFunc(drawFlatTexturePyramid);
        glutIdleFunc(idle);
        break;
    case 's':
        glutDisplayFunc(drawFlatTextureCube);
        glutIdleFunc(idle);
        break;
    case 'd':
        glutDisplayFunc(drawFlatTextureIcosahedron);
        glutIdleFunc(idle);
        break;
    case 'f':
        glutDisplayFunc(drawFlatTextureNut);
        glutIdleFunc(idle);
        break;
        //===================================== Gouraud Texture Key ===================================
    case 'A':
        glutDisplayFunc(drawGouraudTexturePyramid);
        glutIdleFunc(idle);
        break;
    case 'S':
        glutDisplayFunc(drawGouraudTextureCube);
        glutIdleFunc(idle);
        break;
    case 'D':
        glutDisplayFunc(drawGouraudTextureIcosahedron);
        glutIdleFunc(idle);
        break;
    case 'F':
        glutDisplayFunc(drawGouraudTextureNut);
        glutIdleFunc(idle);
        break;

    default:
        break;
    }
}

void processSpecialKeys(int key, int xx, int yy) {

    float fraction = 0.1f;

    switch (key) {
    case GLUT_KEY_LEFT:
        angle -= 0.01f;
        lx = sin(angle);
        lz = -cos(angle);
        break;
    case GLUT_KEY_RIGHT:
        angle += 0.01f;
        lx = sin(angle);
        lz = -cos(angle);
        break;
    case GLUT_KEY_UP:
        x += lx * fraction;
        z += lz * fraction;
        break;
    case GLUT_KEY_DOWN:
        x -= lx * fraction;
        z -= lz * fraction;
        break;
    }
}

void callNutObj(void) {
    int k = 0, m = 0;
    char ch;
    float aa, bb, cc;

    ifstream in("nut.obj", ios::in);

    string line;
    while (getline(in, line)) {
        if (line.substr(0, 2) == "v ") {
            istringstream vert(line.substr(2));

            vert >> aa;
            vert >> bb;
            vert >> cc;
            v[k][0] = (float)aa;
            v[k][1] = (float)bb;
            v[k][2] = (float)cc;

            k++;
        }
        else if (line.substr(0, 2) == "f ") {
            int a, b, c, d;
            int A, B, C, D;
            int a1, b1, c1;

            const char* chh = line.c_str();
            sscanf_s(chh, "f %i/%i/%i %i/%i/%i %i/%i/%i", &a, &A, &a1, &b, &B, &b1, &c, &C, &c1);

            f[m][0] = a - 1;
            f[m][1] = b - 1;
            f[m][2] = c - 1;
            m++;
        }
    }


}



int main(int argc, char* argv) {
    //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    //glutInitWindowPosition(0, 0);
    //glutInitWindowSize(1200, 640);
    //glutCreateWindow("Assignment 2");
    HWND console = GetConsoleWindow();
    RECT ConsoleRect;
    GetWindowRect(console, &ConsoleRect); // include windows.h
    MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 500, 800, TRUE);
    glutInit(&argc, &argv);
    callNutObj();
    init();

    //============================================DrawFunction=====================================================
    //glutDisplayFunc(drawFlatNut);
    //glutDisplayFunc(drawGouraudNut);
    //glutDisplayFunc(drawFlatTextureNut);
    //glutDisplayFunc(drawGouraudTextureNut);

    //glutDisplayFunc(drawFlatPyramid);
    //glutDisplayFunc(drawGouraudPyramid);
    //glutDisplayFunc(drawFlatTexturePyramid);
    //glutDisplayFunc(drawGouraudTexturePyramid);

    //glutDisplayFunc(drawFlatCube);
    //glutDisplayFunc(drawGouraudCube);
    //glutDisplayFunc(drawFlatTextureCube);
    //glutDisplayFunc(drawGouraudTextureCube);

    //glutDisplayFunc(drawFlatIcosahedron);
    //glutDisplayFunc(drawGouraudIcosahedron);
    //glutDisplayFunc(drawFlatTextureIcosahedron);
    //glutDisplayFunc(drawGouraudTextureIcosahedron);

    int choice, choiceNut, choicePyramid, choiceCube, choiceIco;


    printf("============================MENU==========================\n\n");
    printf("Use ARROW KEYS  to move in the scene.\n");
    printf("\n");
    printf("==========================================================\n\n");
    printf("There will be 4 objects used in this project:\n1. Pyramid\n2. Cube\n3. Icosahedron\n4. Nut\n");

    printf("\n===============Refer to this Key Functions===============\n");
    //printf("\nShading and texture object keys:\n ");
    printf("\nPress the following keys to change the object shading\n ");
    printf("(Flat/Gauraud) :\n ");
    printf("[q/Q] Pyramid \n ");
    printf("[w/W] Cube \n ");
    printf("[e/E] Icosahedron\n ");
    printf("[r/R] Nut \n ");

    printf("\nPress the following keys to change the object texture: \n ");
    printf("(Texture Mapping/Gauraud Texture) :\n ");
    printf("[a/A] Pyramid \n ");
    printf("[s/S] Cube \n ");
    printf("[d/D] Icosahedron\n ");
    printf("[f/F] Nut \n ");

    printf("\nPress the following keys to move the object:\n ");
    printf("[x/X] Rotate Object (x, -x)\n ");
    printf("[y/Y] Rotate Object (y, -y)\n ");
    printf("[z/Z] Rotate Object (z, -z)\n ");

    printf("\nAdjust Light percentages:\n ");
    printf("[i / o / p] + Diffuse Light(R, G, B)\n ");
    printf("[I / O / P] - Diffuse Light(R, G, B)\n ");
    printf("[j / k / l] + Specular Light(R, G, B)\n ");
    printf("[J / K / L] - Specular Light(R, G, B)\n ");
    printf("[b / n / m] + Ambient Light(R, G, B)\n ");
    printf("[B / N / M] - Ambient Light(R, G, B)\n ");

    printf("\n==========================================================\n\n");
    printf("Press 1 to start or 2 to exit => ");
    scanf_s("%d", &choice);

    switch (choice)
    {
    case 1:

        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(1200, 640);
        glutInitWindowPosition(0, 0);
        glutCreateWindow(" Output ");
        init();
        glutDisplayFunc(drawFlatPyramid); //initial object displayed
        break;


    case 2:
        break;

    default:
        return 0;
    }

    glutReshapeFunc(reshape);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    glutIdleFunc(idle);
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    return 0;


}