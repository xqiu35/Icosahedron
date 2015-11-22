// Draw an Icosahedron
// ECE4893/8893 Project 4
// Xiaofei Qiu

#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>

using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

/***************************** 3D point struct *******************************/

typedef struct point3d
{
  GLfloat x;
  GLfloat y;
  GLfloat z;

  point3d():x(0.0),y(0.0),z(0.0){}				// constructor
  point3d(GLfloat a,GLfloat b, GLfloat c):x(a),y(b),z(c){}	// cons
  point3d(const point3d& p){x=p.x;y=p.y;z=p.z;}			// copy con

  point3d operator+(const point3d& rhs) const{return point3d(x+rhs.x,y+rhs.y,z+rhs.z);} 	// + operator
  point3d operator/(const GLfloat& i) const{return point3d(x/i,y/i,z/i);}			// / operator

  void setXYZ(const GLfloat* v)	// set point using array 
  {
    x = v[0];
    y = v[1];
    z = v[2];
  }

  void normalize()		// normalize vector
  {
	GLfloat l = sqrt(x*x+y*y+z*z);
	x = x/l;
	y = y/l;
	z = z/l;
  }

}Point3D;

/********************** RGB struct, used to store color value *************************/

typedef struct rgb
{
	GLfloat r,g,b;
	rgb():r(0),g(0),b(0){}
	rgb(const rgb& _rgb):r(_rgb.r),g(_rgb.g),b(_rgb.b){}
	rgb(const GLfloat& red,const GLfloat& green,const GLfloat& blue):r(red),g(green),b(blue){}
}RGB;

/************************** Trangle struct contains Vertex and RGB color *********************/

typedef struct triangle		// triangle
{
  Point3D vertex[3];		// has 3 vertex
  RGB color;			// triangle color

  void setVertex(const GLint& v0,const GLint& v1,const GLint& v2)	// setters
  {
    this->vertex[0].setXYZ(vdata[v0]);
    this->vertex[1].setXYZ(vdata[v1]);
    this->vertex[2].setXYZ(vdata[v2]);
  }
 
  void setVertex(const Point3D& vertex0,const Point3D& vertex1, const Point3D& vertex2)
  {
    this->vertex[0]=vertex0;
    this->vertex[1]=vertex1;
    this->vertex[2]=vertex2;
  }

}Triangle;

/******************************* Icosahedron, contains muti traiangles ****************************/

typedef struct icosahedron	// icosahedron
{
  Triangle* triangle;		// to be convinient for this lab, use dynamic allocation
  ~icosahedron(){delete [] triangle;}
}Icosahedron;

/******************************* Variables and functions ***************************************/

int testNumber; 		// Global variable indicating which test number is desired
int depth;
int num_faces = 20;		// var num_faces, default to 20 faces
Icosahedron icosahedron;	

// routines
void GLinit(int argc, char** argv);			// used for init GLUT stuff.....
void reshape(int w, int h);				// callback reshape function
void callback(int arg);					// timer call back for re-display
void drawIcos();					// main draw function
void icosInit(Icosahedron&, const int& dep = 0);	// init icos
void extend(Icosahedron&);				// extend function used for extending icos

// tests
void Test1();
void Test2();

/******************************** Main function **************************************************/

int main(int argc, char** argv)
{
	

	if (argc < 2)
	{
		std::cout << "Usage: icosahedron testnumber" << endl;
      		exit(1);
    	}
  	
	testNumber = atol(argv[1]);	// set test number;	

	if(testNumber > 4)
	{
		if(argv[2])
		{
			depth = atol(argv[2]);  
		}
		else
		{
			cout<<"Please specify the depth\n";
			exit(1);
		}
	}

	GLinit(argc,argv);	    		         	 // init window & disp parameters 
  	glutCreateWindow("Icosahedron");			 // create window
	glClearColor(0.0,0.0,0.0,0.0);					// set background color
	glShadeModel(GL_FLAT);						// set flat shading
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_LINE_SMOOTH);
	switch(testNumber)
	{
		case 1: icosInit(icosahedron);glutDisplayFunc(Test1);break;
    		case 2: icosInit(icosahedron);glutDisplayFunc(Test2);break;
    		case 3: icosInit(icosahedron,1);glutDisplayFunc(Test1);break;
    		case 4: icosInit(icosahedron,1);glutDisplayFunc(Test2);break;
    		case 5: icosInit(icosahedron,depth);glutDisplayFunc(Test1);break;
    		case 6: icosInit(icosahedron,depth);glutDisplayFunc(Test2);break;
  	}
	glutReshapeFunc(reshape);
	glutTimerFunc(100.0,callback,0);
	glutMainLoop();

  	return 0;
}


void GLinit(int argc, char** argv)
{

	glutInit(&argc,argv);						// init glu
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);	// use double buffer and RGB color
	glutInitWindowSize(500,500);					// init window size
	glutInitWindowPosition(500,500);				// window posi
}

void reshape(int w, int h)
{
	glViewport(0,0, (GLsizei)w, (GLsizei)h);
  	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
  	glOrtho(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)w, (GLdouble)-w);
  	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void callback(int arg)
{
	glutPostRedisplay();
	glutTimerFunc(100,callback,0);
}

void drawIcos()
{
	
  	for(int i = 0; i<num_faces; i++)			// draw faces
  	{
		//set triangle color
  		glColor3f(icosahedron.triangle[i].color.r, icosahedron.triangle[i].color.g, icosahedron.triangle[i].color.b);
  		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//	glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);			
    		
		glVertex3f(icosahedron.triangle[i].vertex[0].x,	// first vertex of triangle
               		   icosahedron.triangle[i].vertex[0].y,
               		   icosahedron.triangle[i].vertex[0].z);
    		
		glVertex3f(icosahedron.triangle[i].vertex[1].x,	// second vertex of triangle
               		   icosahedron.triangle[i].vertex[1].y,
               		   icosahedron.triangle[i].vertex[1].z);
    		
		glVertex3f(icosahedron.triangle[i].vertex[2].x,	// third vertex of triangle
               		   icosahedron.triangle[i].vertex[2].y,
               	   	   icosahedron.triangle[i].vertex[2].z);
  		glEnd();


		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glColor3f(1.0, 1.0, 1.0);
		glLineWidth(1.0);
		glBegin(GL_TRIANGLES);
		glVertex3f(icosahedron.triangle[i].vertex[0].x,
              		   icosahedron.triangle[i].vertex[0].y,
              		   icosahedron.triangle[i].vertex[0].z);
    		
		glVertex3f(icosahedron.triangle[i].vertex[1].x,
               		   icosahedron.triangle[i].vertex[1].y,	
               		   icosahedron.triangle[i].vertex[1].z);
    	  	
		glVertex3f(icosahedron.triangle[i].vertex[2].x,
               		   icosahedron.triangle[i].vertex[2].y,
               		   icosahedron.triangle[i].vertex[2].z);
  		glEnd();
  	}
}

void icosInit(Icosahedron& icos,const int& dep)
{	
  	icos.triangle = new Triangle[20];			// init 20 faces
  	for(int i = 0; i<20; i++)
 	{
		icos.triangle[i].setVertex(tindices[i][0],tindices[i][1],tindices[i][2]);	// init vertex of each triangle(face)
		icos.triangle[i].color = RGB((GLfloat)cos(i),(GLfloat)sin(i),(GLfloat)tan(i));	// set triangle color
	}

	if(dep!=0)
	{
		for(int i=0;i<dep;i++)
		{
			extend(icos);	// extend icos
		}
	}	
}

void extend(Icosahedron& icos)
{
	int num_triangle = num_faces;
	num_faces = num_faces * 4;					// each extention will have 4 times num_faces

	Triangle* temp_triangles = new Triangle[num_faces];		// alloc mem for faces

	Point3D vertex01;
	Point3D vertex02;
	Point3D vertex12;
	
	Point3D vertex0;
	Point3D vertex1;
	Point3D vertex2;
	
	for(int i=0;i<num_triangle;i++)					// each triangle will be devided into 4 small triangles
	{
		vertex0 = icosahedron.triangle[i].vertex[0];	
		vertex1 = icosahedron.triangle[i].vertex[1];	
		vertex2 = icosahedron.triangle[i].vertex[2];

		vertex01 = (vertex0 + vertex1)/2;			// find mid point
		vertex02 = (vertex0 + vertex2)/2;
		vertex12 = (vertex1 + vertex2)/2;

		vertex01.normalize();					// normalize
		vertex02.normalize();
		vertex12.normalize();

		temp_triangles[i*4].setVertex(vertex0,vertex01,vertex02);		// set vertex of triangles(faces)
		temp_triangles[i*4+1].setVertex(vertex01,vertex02,vertex12);
		temp_triangles[i*4+2].setVertex(vertex1,vertex01,vertex12);
		temp_triangles[i*4+3].setVertex(vertex2,vertex12,vertex02);
	}

	delete [] icos.triangle;					// delete original faces

	icos.triangle = temp_triangles;					// icos extended

	for(int i=0;i<num_faces;i++)
	{
		// set triangle color
		icos.triangle[i].color = RGB(sin(i),cos((GLfloat)i/num_faces),sin((GLfloat)i/num_faces));
	}
}

void Test1()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 250.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  	glTranslatef(250, 250, 250);
	glScalef(200.0, 200.0, 200.0);
  	drawIcos();
	glutSwapBuffers();
}

void Test2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 250.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glTranslatef(250, 250, 250);

	glScalef(200.0, 200.0, 200.0);
	static GLfloat rotX = 0;
	static GLfloat rotY = 0;

	glRotatef(rotX, 1.0, 0.0, 0.0);
 	glRotatef(rotY, 0.0, 1.0, 0.0);

	rotX += 1;
	rotY += 1;
	drawIcos();
	glutSwapBuffers();
}
