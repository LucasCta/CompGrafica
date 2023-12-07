#define _CRT_SECURE_NO_WARNINGS
#define PI 3.141592654

// #include <windows.h>
#include "RgbImage.h"
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

// char *filenameTexMetal1 = "./metalTexture1.bmp";
char *filenameTexMetal1 = "./metal10.bmp";
char *filenameTexGrass1 = "./tgrass.bmp";

GLuint _textureIdMetal1;
GLuint _textureIdSphere;
GLuint _textureIdCylinder;
GLuint _textureIdGrass;
GLUquadric *quadSphere;
GLUquadric *quadCylinder;

bool textureOn = true;

bool redisplay = false;
float sitAngle = 0.0;
float layHeight = 0.0;
int walkCycle = 0;
int walkStart = 0;
int walkEnd = false;
float walkDistance = 0;

float diameterCylinder = 0.3;
float diameterSphere = 0.4;
float sizeArm = 4.5;
float sizeForearm = 3.0;
float sizeHand = 2.0;
float sizeClampPart = 1.0;
float diameterBase = 2.0;
float heightBase = 0.5;

float eyeDistance = 20.0;
float eyeX;
float eyeY;
float eyeZ;
float viewAngleX = 0.0;
float viewAngleZ = 15.0;

float angleArm = 90.0;
float angleForearm = 90.0;
float angleHand = 0.0;
float angleClampZ = 0.0;
float angleClampY = 0.0;

float angleLegs[4] = {90.0, 90.0, 90.0, 90.0};
float angleBackLegs[4] = {0.0, 0.0, 0.0, 0.0};

// makes the image into a texture, and returns the id of the texture
GLuint loadTexture(char *filename) {
  GLuint textureId;

  RgbImage theTexMap(filename); // Image with texture

  glGenTextures(1, &textureId);            // Make room for our texture
  glBindTexture(GL_TEXTURE_2D, textureId); // Tell OpenGL which texture to edit
                                           // Map the image to the texture
  glTexImage2D(GL_TEXTURE_2D,              // Always GL_TEXTURE_2D
               0,                          // 0 for now
               GL_RGB,                     // Format OpenGL uses for image
               theTexMap.GetNumCols(),     // Width
               theTexMap.GetNumRows(),     // Height
               0,                          // The border of the image
               GL_RGB, // GL_RGB, because pixels are stored in RGB format
               GL_UNSIGNED_BYTE, // GL_UNSIGNED_BYTE, because pixels are stored
                                 // as unsigned numbers
               theTexMap.ImageData()); // The actual pixel data
  return textureId;                    // Returns the id of the texture
}

void initRendering(void) {
  quadSphere = gluNewQuadric();
  quadCylinder = gluNewQuadric();
  _textureIdGrass = loadTexture(filenameTexGrass1);
  _textureIdMetal1 = loadTexture(filenameTexMetal1);
  _textureIdCylinder = _textureIdMetal1;
  _textureIdSphere = _textureIdMetal1;
}

void handleKeypress(unsigned char key, int x, int y) {
  switch (key) {
  case 27: // Escape key
    exit(0);
  case 'w': // Increase view angle z axis
    if (viewAngleZ < 180)
      viewAngleZ += 3;
    glutPostRedisplay();
    break;
  case 's': // Decrease view angle z axis
    if (viewAngleZ > 0)
      viewAngleZ -= 3;
    glutPostRedisplay();
    break;
  case 'a': // Decrease view angle x axis
    if (viewAngleX > 0)
      viewAngleX -= 3;
    glutPostRedisplay();
    break;
  case 'd': // Increase view angle x axis
    if (viewAngleX < 180)
      viewAngleX += 3;
    glutPostRedisplay();
    break;
  case 'q':
    if (eyeDistance < 30)
      eyeDistance += 1.0;
    glutPostRedisplay();
    break;
  case 'e':
    if (eyeDistance > 10)
      eyeDistance -= 1.0;
    glutPostRedisplay();
    break;
  case 't': // Use texture or not
    textureOn = !textureOn;
    glutPostRedisplay();
    break;
  case '1': // Increase arm angle
    angleArm += 3;
    if (angleArm >= 360)
      angleArm = 0;
    glutPostRedisplay();
    break;
  case '2': // Decrease arm angle
    angleArm -= 3;
    if (angleArm <= 0)
      angleArm = 360;
    glutPostRedisplay();
    break;
  case '3': // Increase forearm angle
    if (angleForearm < 90)
      angleForearm += 3;
    glutPostRedisplay();
    break;
  case '4': // Decrease forearm angle
    if (angleForearm > -90)
      angleForearm -= 3;
    glutPostRedisplay();
    break;
  case '5': // Increase clamp angle y axis
    if (angleClampY < 60)
      angleClampY += 3;
    glutPostRedisplay();
    break;
  case '6': // Decrease clamp angle y axis
    if (angleClampY > 0)
      angleClampY -= 3;
    glutPostRedisplay();
    break;
  }
}

enum action { nothing, walking, sitting, layingdown };
int state = nothing;
int want = nothing;

void handleKeypress2(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_RIGHT:
    if (want == walking)
      want = nothing;
    else
      want = walking;
    glutPostRedisplay();
    break;
  case GLUT_KEY_LEFT:
    want = nothing;
    glutPostRedisplay();
    break;
  case GLUT_KEY_UP:
    if (want == sitting)
      want = nothing;
    else
      want = sitting;
    glutPostRedisplay();
    break;
  case GLUT_KEY_DOWN:
    if (want == layingdown)
      want = nothing;
    else
      want = layingdown;
    glutPostRedisplay();
    break;
  }
}

void handleResize(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (float)w / (float)h, 1.0, 100.0);
}

void drawCylinder(float diameter, float lenght) {
  if (textureOn) {
    glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluQuadricTexture(quadCylinder, 1);
  } else
    gluQuadricTexture(quadCylinder, 0);
  gluCylinder(quadCylinder, diameter, diameter, lenght, 40.0, lenght * 30.0);
}

void drawCone(float diameter, float lenght) {
  if (textureOn) {
    glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluQuadricTexture(quadCylinder, 1);
  } else
    gluQuadricTexture(quadCylinder, 0);
  gluCylinder(quadCylinder, diameter, 0, lenght, 40.0, lenght * 30.0);
}

void drawDisk(float diameterInner, float diameterOuter) {
  if (textureOn) {
    glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluQuadricTexture(quadCylinder, 1);
  } else
    gluQuadricTexture(quadCylinder, 0);
  gluDisk(quadCylinder, diameterInner, diameterOuter, 40.0, 30.0);
}

void drawSphere(float diameter) {
  if (textureOn) {
    glBindTexture(GL_TEXTURE_2D, _textureIdSphere);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluQuadricTexture(quadSphere, 1);
  } else
    gluQuadricTexture(quadSphere, 0);
  gluSphere(quadSphere, diameter, 40.0, 40.0);
}

void drawLeg(float angleBackLeg, float angleThigh) {

  glRotatef(angleBackLeg, 0.0f, 1.0f, 0.0f);

  // draws the arm
  drawCylinder(diameterCylinder, sizeArm);

  // move to forearm referential
  glTranslatef(0.0f, 0.0f, sizeArm + diameterSphere / 5);
  glRotatef(angleThigh, 0.0f, 1.0f, 0.0f);

  // draws the forearm
  drawSphere(diameterSphere);
  glTranslatef(0.0f, 0.0f, diameterSphere / 5);
  drawCylinder(diameterCylinder, sizeForearm);

  // move to clamp referential
  glTranslatef(0.0f, 0.0f, sizeForearm + diameterSphere / 5);

  // draws the clamp sphere
  drawSphere(diameterSphere);
}

void drawLegs() {
  // legs referential
  glRotatef(90, 1.0f, 0.0f, 0.0f);
  glRotatef(270, 0.0f, 0.0f, 1.0f);

  // draws first leg
  glPushMatrix();
  glTranslatef(-0.5f, 1.0f, 1.5f);
  glRotatef(-45, 0.0f, 1.0f, 0.0f);
  drawLeg(angleBackLegs[0], angleLegs[0]);
  glPopMatrix();

  // draws second leg
  glPushMatrix();
  glTranslatef(-0.5f, -1.0f, 1.5f);
  glRotatef(-45, 0.0f, 1.0f, 0.0f);
  drawLeg(angleBackLegs[1], angleLegs[1]);
  glPopMatrix();

  // draws third leg
  glPushMatrix();
  glTranslatef(-7.0f, -1.0f, 1.5f);
  glRotatef(-45, 0.0f, 1.0f, 0.0f);
  drawLeg(angleBackLegs[2], angleLegs[2]);
  glPopMatrix();

  // draws fourth leg
  glPushMatrix();
  glTranslatef(-7.0f, 1.0f, 1.5f);
  glRotatef(-45, 0.0f, 1.0f, 0.0f);
  drawLeg(angleBackLegs[3], angleLegs[3]);
  glPopMatrix();
}

void drawArm() {
  // move to arm referential
  glTranslatef(0.0f, 0.3f, 0.0f);
  glRotatef(angleArm, 0.0f, 0.0f, 1.0f);

  // draws the arm
  drawCylinder(diameterCylinder, sizeArm);

  // move to forearm referential
  glTranslatef(0.0f, 0.0f, sizeArm + diameterSphere / 5);
  glRotatef(angleForearm, 0.0f, 1.0f, 0.0f);

  // draws the forearm
  drawSphere(diameterSphere);
  glTranslatef(0.0f, 0.0f, diameterSphere / 5);
  drawCylinder(diameterCylinder, sizeForearm);

  // move to clamp referential
  glTranslatef(0.0f, 0.0f, sizeForearm + diameterSphere / 5);
  glRotatef(angleClampZ, 0.0f, 0.0f, 1.0f);

  // draws the clamp sphere
  drawSphere(diameterSphere);
  glTranslatef(0.0f, 0.0f, diameterSphere / 2);

  glRotatef(120, 0.0f, 0.0f, 1.0f);

  glPushMatrix();

  glRotatef(angleClampY + 60, 0.0f, 1.0f, 0.0f);

  drawCylinder(diameterCylinder / 3, sizeClampPart);
  glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
  drawSphere(diameterSphere / 3);

  glTranslatef(0.0f, 0.0f, diameterSphere / 15);
  glRotatef(-60, 0.0f, 1.0f, 0.0f);

  drawCylinder(diameterCylinder / 3, sizeClampPart);
  glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
  drawSphere(diameterSphere / 3);

  glTranslatef(0.0f, 0.0f, diameterSphere / 15);
  glRotatef(-60, 0.0f, 1.0f, 0.0f);
  drawCone(diameterCylinder / 3, sizeClampPart);

  glPopMatrix();

  glRotatef(300, 0.0f, 0.0f, 1.0f);

  glPushMatrix();

  glRotatef(-angleClampY - 60, 0.0f, 1.0f, 0.0f);

  drawCylinder(diameterCylinder / 3, sizeClampPart);
  glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
  drawSphere(diameterSphere / 3);

  glTranslatef(0.0f, 0.0f, diameterSphere / 15);
  glRotatef(60, 0.0f, 1.0f, 0.0f);

  drawCylinder(diameterCylinder / 3, sizeClampPart);
  glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
  drawSphere(diameterSphere / 3);

  glTranslatef(0.0f, 0.0f, diameterSphere / 15);
  glRotatef(60, 0.0f, 1.0f, 0.0f);
  drawCone(diameterCylinder / 3, sizeClampPart);

  glPopMatrix();

  glRotatef(120, 0.0f, 0.0f, 1.0f);

  glPushMatrix();

  glRotatef(-angleClampY - 60, 0.0f, 1.0f, 0.0f);

  drawCylinder(diameterCylinder / 3, sizeClampPart);
  glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
  drawSphere(diameterSphere / 3);

  glTranslatef(0.0f, 0.0f, diameterSphere / 15);
  glRotatef(60, 0.0f, 1.0f, 0.0f);

  drawCylinder(diameterCylinder / 3, sizeClampPart);
  glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
  drawSphere(diameterSphere / 3);

  glTranslatef(0.0f, 0.0f, diameterSphere / 15);
  glRotatef(60, 0.0f, 1.0f, 0.0f);
  drawCone(diameterCylinder / 3, sizeClampPart);

  glPopMatrix();
}

void drawFloor() {
  glBindTexture(GL_TEXTURE_2D, _textureIdGrass);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glNormal3f(0.0, 0.0, 1.0);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-150.0f, -150.0f, -7.5f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(150.0f, -150.0f, -7.5f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(150.0f, 150.0f, -7.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-150.0f, 150.0f, -7.5f);
  glEnd();
}

void drawScene(void) {

  glClearColor(0.0f, 0.8f, 1.0f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_TEXTURE_2D);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  eyeX = eyeDistance * cos(viewAngleZ * PI / 180) * cos(viewAngleX * PI / 180);
  eyeY = eyeDistance * cos(viewAngleZ * PI / 180) * sin(viewAngleX * PI / 180);
  eyeZ = eyeDistance * sin(viewAngleZ * PI / 180);
  if (viewAngleZ < 90)
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  else
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);

  // grass color
  glColor3f(0.0f, 1.0f, 0.0f);

  // Iluminação
  GLfloat luzAmbiente[4] = {0.3, 0.3, 0.2, 10.0};
  GLfloat luzDifusa[4] = {0.7, 0.7, 11.7, 1.0};
  GLfloat luzEspecular[4] = {1.0, 2.0, 2.0, 1.0};
  GLfloat posicaoLuz[4] = {80.0, 0.0, 60.0, 0.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
  glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
  glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
  glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);

  GLfloat especularidade[4] = {1.0, 1.0, 1.0, 1.0};
  glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);

  // desenha o chão
  glMateriali(GL_FRONT, GL_SHININESS, 1);
  glTranslatef(0.0f, -walkDistance, 0.0f);
  drawFloor();
  glTranslatef(0.0f, walkDistance, 0.0f);

  // aumenta o brilho para desenhar o robo
  glMateriali(GL_FRONT, GL_SHININESS, 15);

  // translate forward
  glTranslatef(0.0f, 3.5f, 0.0f);

  // robot color
  glColor3f(1.0f, 0.6f, 0.0f);

  switch (state) {
  case nothing:
    if (want != nothing) {
      state = want;
      if (state == walking)
        walkStart = 0;
      redisplay = true;
    }
    break;
  case sitting:
    if (want == sitting) {
      if (sitAngle <= 30) {
        sitAngle += 1;
        for (int i = 0; i < 2; i++) {
          angleBackLegs[i] += 0.2f;
          angleLegs[i] -= 1;
        }
        for (int i = 2; i < 4; i++) {
          angleBackLegs[i] -= 1.2f;
          angleLegs[i] += 1.7f;
        }
      }
    } else {
      if (fabs(sitAngle) < 0.1)
        state = nothing;
      else {
        sitAngle -= 1;
        for (int i = 0; i < 2; i++) {
          angleBackLegs[i] -= 0.2f;
          angleLegs[i] += 1;
        }
        for (int i = 2; i < 4; i++) {
          angleBackLegs[i] += 1.2f;
          angleLegs[i] -= 1.7f;
        }
      }
    }
    glRotatef(sitAngle, 1.0f, 0.0f, 0.0f);
    redisplay = true;
    break;
  case walking:
    walkDistance += 0.1f;
    if (walkDistance > 70)
      walkDistance = 0;
    if (!walkEnd) {
      if (walkStart < 20) {
        angleBackLegs[1] -= 0.5f;
        angleBackLegs[3] -= 0.5f;
        angleLegs[1] += 2.0f;
        angleLegs[3] += 2.0f;
        walkStart++;
      }
      if (walkCycle < 20) {
        angleBackLegs[0] -= 1;
        angleBackLegs[2] -= 1;
        angleLegs[0] -= 1.5f;
        angleLegs[2] -= 1.5f;
      } else if (walkCycle < 40) {
        angleBackLegs[0] += 0.5;
        angleBackLegs[2] += 0.5;
        angleLegs[0] += 3.5f;
        angleLegs[2] += 3.5f;
      } else if (walkCycle < 60) {
        angleBackLegs[0] += 2;
        angleBackLegs[2] += 2;
        angleLegs[0] -= 2.0f;
        angleLegs[2] -= 2.0f;
      } else if (walkCycle < 80) {
        angleBackLegs[0] -= 1.5f;
        angleBackLegs[2] -= 1.5f;
      }
      if (walkCycle < 20) {
        angleBackLegs[1] += 2;
        angleBackLegs[3] += 2;
        angleLegs[1] -= 2.0f;
        angleLegs[3] -= 2.0f;
      } else if (walkCycle < 40) {
        angleBackLegs[1] -= 1.5f;
        angleBackLegs[3] -= 1.5f;
      } else if (walkCycle < 60) {
        angleBackLegs[1] -= 1;
        angleBackLegs[3] -= 1;
        angleLegs[1] -= 1.5f;
        angleLegs[3] -= 1.5f;
      } else if (walkCycle < 80) {
        angleBackLegs[1] += 0.5;
        angleBackLegs[3] += 0.5;
        angleLegs[1] += 3.5f;
        angleLegs[3] += 3.5f;
      }
      if (++walkCycle == 80) {
        walkCycle = 0;
        if (want != walking)
          walkEnd = true;
      }
    } else {
      angleBackLegs[1] += 0.5f;
      angleBackLegs[3] += 0.5f;
      angleLegs[1] -= 2.0f;
      angleLegs[3] -= 2.0f;
      if (--walkStart == 0) {
        state = nothing;
        walkEnd = false;
      }
    }
    redisplay = true;
    break;
  case layingdown:
    if (want == layingdown) {
      if (layHeight <= 2) {
        layHeight += 0.05f;
        for (int i = 0; i < 4; i++) {
          angleBackLegs[i] -= 0.5f;
          angleLegs[i] += 1.75f;
        }
      }
      if (angleForearm < 120)
        angleForearm += 0.25f;
    } else {
      if (fabs(layHeight) < 0.1)
        state = nothing;
      else {
        layHeight -= 0.05f;
        for (int i = 0; i < 4; i++) {
          angleBackLegs[i] += 0.5f;
          angleLegs[i] -= 1.75f;
        }
        if (angleForearm > 90)
          angleForearm -= 1;
      }
    }
    glTranslatef(0.0f, 0.0f, -layHeight * 2);
    redisplay = true;
    break;
  }

  // drawsArm
  glPushMatrix();
  drawArm();
  glPopMatrix();

  // draws dogs body and legs
  glPushMatrix();
  glTranslatef(0.0f, 1.0f, 0.0f);
  glRotatef(90, 1.0f, 0.0f, 0.0f);
  drawDisk(0.0f, diameterBase);
  // ajustando as normais
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 10.0f);
  glRotatef(180, 0.0f, 1.0f, 0.0f);
  drawDisk(0.0f, diameterBase);
  glPopMatrix();
  drawCylinder(diameterBase, 10);
  drawLegs();
  glPopMatrix();

  glPopMatrix();
  glutSwapBuffers();

  if (redisplay)
    glutPostRedisplay();
  redisplay = false;
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Robo Garra");

  initRendering();
  glutDisplayFunc(drawScene);
  glutKeyboardFunc(handleKeypress);
  glutSpecialFunc(handleKeypress2);
  glutReshapeFunc(handleResize);

  glutMainLoop();
  return 0;
}
