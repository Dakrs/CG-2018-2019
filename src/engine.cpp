#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "../headers/Point.h"
#include "../headers/Figure.h"
#include "../headers/tinyxml2.h"


using namespace tinyxml2;
using namespace std;

#define 	GLUT_WHEEL_UP 		3
#define 	GLUT_WHEEL_DOWN 	4

//variaveis globais
int WINDOW_X = 800;
int WINDOW_Y = 800;
int X_POS = WINDOW_X/2;
int Y_POS = WINDOW_Y/2;
long building = 0;


float angleA = 0;
float angleB = 0;
float radius = 5;
GLenum viewMode = GL_LINE;


vector<Figure*> figs;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void build_figure(string path){

  string line;
  string delimiter = " ";
  float x,y,z;
  ifstream myfile(path);
  Figure * f;
  if (myfile.is_open())
  {
	vector<Point*> * v = new vector<Point*>();

    while (getline(myfile,line)){
		size_t pos = 0;
  		string token;

		pos = line.find(delimiter);
		token = line.substr(0, pos);
		x = atof(token.c_str());
		line.erase(0, pos + delimiter.length());

		pos = line.find(delimiter);
		token = line.substr(0, pos);
		y = atof(token.c_str());
		line.erase(0, pos + delimiter.length());

		pos = line.find(delimiter);
		token = line.substr(0, pos);
		z = atof(token.c_str());
		line.erase(0, pos + delimiter.length());

		v->push_back(new Point(x,y,z));
    }
	f = new Figure(v);
	figs.push_back(f);
    myfile.close();
  }
  else cout << "error" << endl;
}

void draw_figures(){
	vector<Figure*>::iterator it;
	for(it = figs.begin(); it != figs.end(); it++){
		(*it)->draw(&building);
	}
}

void renderScene(void) {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(radius * cos(angleB) * cos(angleA), radius * sin(angleB),radius * cos(angleB) * sin(angleA),
					0.0,0.0,0.0,
				0.0f,1.0f,0.0f);
	// End of frame

	glPolygonMode(GL_FRONT,viewMode);
	glColor3f(1,1,1);

	draw_figures();

	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

	switch (c) {
			case 'w':
			    angleB += M_PI / 6;
				glutPostRedisplay();
				break;
			case 's':
				angleB -= M_PI / 6;
			    glutPostRedisplay();
			case 'a':
			    angleA -= M_PI / 6;
				glutPostRedisplay();
				break;
			case 'd':
				angleA += M_PI / 6;
			    glutPostRedisplay();
			break;
			case 'q':
			    angleB += M_PI / 6;
				angleA -= M_PI / 6;
				glutPostRedisplay();
				break;
			case 'e':
				angleB += M_PI / 6;
				angleA += M_PI / 6;
			    glutPostRedisplay();
			break;
			case 'z':
				angleB -= M_PI / 6;
				angleA -= M_PI / 6;
			    glutPostRedisplay();
			break;
			case 'x':
				angleB -= M_PI / 6;
				angleA += M_PI / 6;
			    glutPostRedisplay();
			break;
			case '1':
				radius -= 1;
			    glutPostRedisplay();
			break;
			case '2':
				radius += 1;
			    glutPostRedisplay();
			break;
			case 'j':
				viewMode = GL_LINE;
			    glutPostRedisplay();
			break;
			case 'k':
				viewMode = GL_POINT;
			    glutPostRedisplay();
			break;
			case 'l':
			    viewMode = GL_FILL;
				glutPostRedisplay();
				break;
	        case 'm':
	            building += 18;
	            glutPostRedisplay();
	            break;
			default:
			    break;
	}

}


void processSpecialKeys(int key, int xx, int yy) {

}

void regPos(int x, int y){
	X_POS = x;
	Y_POS = y;
}



void mouse(int button, int state, int x, int y)
{
	// Used for wheels, has to be up
	if (state == GLUT_UP )	{

		if ( button == GLUT_WHEEL_UP ){
			radius *= 0.95;
		}
		else if( button == GLUT_WHEEL_DOWN ){
			radius *= 1.05;
		}
		glutPostRedisplay();
	}
}



void activeMotion(int x, int y){
	angleA += ((float)(x - X_POS)/WINDOW_X)*M_PI;
	angleB += ((float)(y - Y_POS)/WINDOW_Y)*M_PI;
	regPos(x,y);
	glutPostRedisplay();
}

string mergePath(string path, string prog){



	int i ;
	int flag = 0;
	for( i = path.length() - 1 ; i>=0 && !flag; i-- )
		if(path[i] == '/') flag = 1;

	std::string pathFull = path.substr (0,i+2);     // "think"

	string pathProg = pathFull + prog;
	cout << pathProg << endl;

	return pathProg;


}

void parseXML(string  f_path){
    XMLDocument xmlDoc;
    XMLElement *element;

    if (!(xmlDoc.LoadFile(f_path.c_str()))) {

        element = xmlDoc.FirstChildElement();
        for (element = element->FirstChildElement(); element; element = element->NextSiblingElement()) {
            string figura = element->Attribute("file");

						cout << (figura) <<endl; // Gets model's vertexes

						string file2 = mergePath(f_path,figura);

						build_figure(file2);
        }

    }
    else {
        cout << "Ficheiro não encontrado! " << endl;
    }
}

int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WINDOW_X,WINDOW_Y);
	glutCreateWindow("CG@DI-UM");




	glClearColor(0,0,0,1) ;
	glClear(GL_COLOR_BUFFER_BIT);




	if(argc < 2){
	     cout << "Input not given!" << endl;
	     return 0;
	}

	else parseXML(argv[1]); // Read XML File
// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glutPassiveMotionFunc(regPos);
	glutMotionFunc(activeMotion);
	glutMouseFunc(mouse);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
