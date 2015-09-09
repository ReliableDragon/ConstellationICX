#include <windows.h>   // use as needed for your system
#include <gl/Gl.h>
#include <gl/glu.h>
#include "glut.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

void LoadConst(const char *fileName);
void MouseClick(int button, int state, int x, int y);
void KeyPress(unsigned char key, int x, int y);

float X_MAX = 640;
float Y_MAX = 480;
char constName[100];
bool constIsOrion = true;
int stars[100][5]; //x, y, brightness, up or down, tics till increment
int numStars;
long int spinStep = 0;
int constMidX, constMidY;
bool leftDown = false, rightDown = false, upDown = false, downDown = false;
float shipX = -10.0, shipY = Y_MAX/2;
const GLubyte testArray[48] = { 0xff, 0x81, 0xff,
								0xff, 0x81, 0xff,
								0xff, 0x81, 0xff,
								0x3f, 0x00, 0xfe,
								0x3e, 0x00, 0x7c,
								0x3e, 0x3c, 0x7c,
								0x3e, 0x3c, 0x7c,
								0x3f, 0x3c, 0x3f,
								0x1f, 0xff, 0xf8,
								0x1f, 0xff, 0xf8,
								0x03, 0xff, 0xc0,
								0x03, 0xff, 0xc0,
								0x03, 0xff, 0xc0,
								0x03, 0xff, 0xc0,
								0x03, 0xff, 0xc0,
								0x03, 0xff, 0xc0};
//GLfloat backgroundVerts[] = {-1, -1, 0,
//							-1, 1, 0,
//							1, 1, 0,
//							1, -1, 0};
//GLubyte backgroundInds[] = {0, 1, 2,
//							0, 2, 3};

//<<<<<<<<<<<<<<<<<<<<<<< myInit >>>>>>>>>>>>>>>>>>>>
 void myInit(void)
 {
    glClearColor( 0.0, 0.0, 0.0, 0.0 );      // set the bg color
	glMatrixMode( GL_PROJECTION );           // set up appropriate matrices- to be explained later
	glLoadIdentity();                      // to be explained later
	gluOrtho2D( 0.0, X_MAX, 0.0, Y_MAX );// to be explained
}

//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
// the redraw function
void myDisplay(void)
{
    glColor3ub(255, 255, 255);      // set the drawing color
 	glPointSize(3.0);		          //set the point size 
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen 
	//glVertexPointer(3, GL_FLOAT, 0, backgroundVerts);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, backgroundInds);


	glBegin(GL_POINTS);       // draw some points 
	for (int i = 0; i < numStars; i++) {
		
		glColor3ub(255 - stars[i][2], 255 - stars[i][2], 255 - stars[i][2]);
		if (stars[i][4] == 0) {
			stars[i][4] = 2;
			if (stars[i][3] == 1) {
				stars[i][2]++;
				if (stars[i][2] >= 255) {
					stars[i][3] = 0;
				}
			}
			else {
				stars[i][2]--;
				if (stars[i][2] <= 0) {
					stars[i][3] = 1;
				}
			}
		}
		else
		{
			stars[i][4]--;
		}

		int xrot = (stars[i][0] - constMidX)*cos(0.0005 * spinStep) - (stars[i][1] - constMidY)*sin(0.0005 * spinStep) + X_MAX/2; 
		int yrot = (stars[i][0] - constMidX)*sin(0.0005 * spinStep) + (stars[i][1] - constMidY)*cos(0.0005 * spinStep) + Y_MAX/2;
		//int x = rand() % 100;
		//if (x > 5) {
		glVertex2i(xrot, yrot);
		//}

	}
		spinStep++;
		//glColor3ub(255, 0, 0);
		//glVertex2i(X_MAX/2, Y_MAX/2);
	glEnd();	
		glColor3ub(0,255,0);
		glRasterPos2i((int)shipX, (int)shipY);
		glBitmap(24, 16, 0.0, 0.0, 10.0, 0.0, testArray);

	glFlush();		                 // send all output to display 

}

void idle() {
	if (upDown)
		shipY += 0.08;
	if (downDown)
		shipY -= 0.08;
	if (rightDown)
		shipX += 0.08;
	if (leftDown)
		shipX -= 0.08;
	glutPostRedisplay();
}


//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
void main(int argc, char **argv)
{
	LoadConst("orion.txt");

	glutInit(&argc, argv);          // initialize the toolkit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // set the display mode
	glutInitWindowSize(X_MAX,Y_MAX);     // set the window size
	glutInitWindowPosition(100, 150); // set the window position on the screen
	glutCreateWindow(constName); // open the screen window(with its exciting title)
	glutDisplayFunc(myDisplay);     // register the redraw function
	glutMouseFunc(MouseClick);
	glutKeyboardFunc(KeyPress);
	myInit();                   
	glutIdleFunc(idle);
	glutMainLoop(); 		     // go into a perpetual loop
}

void LoadConst(const char *fileName) {
	int currIndex = 0;
	FILE *inFile;
	inFile = fopen(fileName, "r");
	char currChar = (char)fgetc(inFile);
	while(currChar != '\n') {
		if (currIndex < 100) {
			constName[currIndex] = currChar;
		}
		currChar = (char)fgetc(inFile);
		currIndex++;
	}
	while(currIndex < 100) {
		constName[currIndex] = NULL;
		currIndex++;
	}

	numStars = 0;
	int nextStarNumValue = fgetc(inFile);
	while((char)nextStarNumValue != '\n' ) {
		numStars *= 10;
		numStars += (nextStarNumValue - '0');
		nextStarNumValue = fgetc(inFile);
	}

	int starCounter = 0;
	int starXYTotal = 0;
	int starXYValue = fgetc(inFile);
	int constXMin = X_MAX, constYMin = Y_MAX, constXMax = 0, constYMax = 0;
	bool isX = true;
	while (starCounter < numStars) {
		stars[starCounter][0] = 0;
		stars[starCounter][1] = 0;
		stars[starCounter][2] = rand() % 255;
		stars[starCounter][3] = rand() % 2;
		stars[starCounter][4] = 0;
		while((char)starXYValue != '\n' && (char)starXYValue != EOF) {
			if ((char)starXYValue == ' ') {
				isX = false;
			}
			else
			{
				if (isX) {
					stars[starCounter][0] *= 10;
					stars[starCounter][0] += starXYValue - '0';
				}
				else {
					stars[starCounter][1] *= 10;
					stars[starCounter][1] += starXYValue - '0';
				}
			}
			starXYValue = fgetc(inFile);
		}
		if ((char)starXYValue != EOF)
			starXYValue = fgetc(inFile);

		if (stars[starCounter][0] > constXMax)
			constXMax = stars[starCounter][0];
		if (stars[starCounter][1] > constYMax)
			constYMax = stars[starCounter][1];
		if (stars[starCounter][0] < constXMin)
			constXMin = stars[starCounter][0];
		if (stars[starCounter][1] < constYMin)
			constYMin = stars[starCounter][1];
		starCounter++;
		isX = true;
	}

	constMidX = (constXMax + constXMin) / 2;
	constMidY = (constYMax + constYMin) / 2;
}


void MouseClick(int button, int state, int x, int y) {
	if (constIsOrion && state == GLUT_DOWN) {
		LoadConst("perseus.txt");
		constIsOrion = false;
	}
	else if (state == GLUT_DOWN) {
		LoadConst("orion.txt");
		constIsOrion = true;
	}
	glutSetWindowTitle(constName);

}

void KeyPress(unsigned char key, int x, int y) {
	upDown = false;
	leftDown = false;
	rightDown = false;
	downDown = false;
	switch(key) {
	case 'w':
		upDown = true;
		break;
	case 's':
		downDown = true;
		break;
	case 'a':
		leftDown = true;
		break;
	case 'd':
		rightDown = true;
		break;
	//default:
	//	upDown = false;
	//	leftDown = false;
	//	rightDown = false;
	//	downDown = false;
	}
}