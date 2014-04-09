#include <cstdio>
#include <string>
#include <cstdlib>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>
#include<iostream>
#include <math.h>
#include <time.h>
#define WIDTH 1080
#define HEIGHT 720
#define XTOP 100
#define YTOP 100
#define DTRANS 1

using namespace std;
class CIRCLE{
public:
double x;
double z;
};

CIRCLE circle;
int total_score=0;
double rot = 0,theta, thetaXZ;               

double distanceX=0, distanceY=4, distanceZ=-30;
int power=1;
double factor=10;
double fov=60.0;
double aspect_ratio=(double)WIDTH/(double)HEIGHT;
double angleX=0, angleY=4, angleZ=-4;
float farlimit=-400;
double eyeX=0,eyeY=0,eyeZ=0,lookX=0,lookY=0,lookZ=-1,upX=0,upY=1,upZ=0;
double red=0.0,green=0.0,blue=0.0;
double ring_radius=10,ball_radius=2;
double ringX=0, ringY=100, ringZ=farlimit;
int flag_hit=0;
double velocityX, velocityY, velocityZ, velocityV;
void draw_wallandbase();
void draw_ball(double x,double y, double z);
void draw_circle(double x, double y, double z,double radius) ;
void idle();
void show_bar();
void draw_arrow();
void checkBasket();
double length = sqrt(pow(angleX,2) + pow(angleY,2) + pow(angleZ,2));
bool flag_power=true,incpower=true,up=true;
static GLuint fl,wall,ball;
int flag_basket=0;
int load_bmp(string a,GLuint &texName)
{
	int width,height;
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;
	// Open the file
	FILE * file = fopen(a.c_str(),"rb");
	if (!file)                              {printf("Image could not be opened\n"); return 0;}
	if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
    	printf("Not a correct BMP file\n");
    	return false;
	}
	if ( header[0]!='B' || header[1]!='M' )
	{
    printf("Not a correct BMP file\n");
    return 0;
	}
	// Read ints from the byte array
	dataPos    = *(int*)&(header[0x0A]);	
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way
	// Create a buffer
	data = new unsigned char [imageSize]; 
	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);
	//Everything is in memory now, the file can be closed
	fclose(file);
	// Create one OpenGL texture
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                   GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                   GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
}

void Render_text(int x, int y,int z, string String)
{
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1.0f, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE); 
	 glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(4.0);
    glRasterPos3f(x,y,z);
    glColor3f(0.309804 , 0.284314 , 0.9);
    for (int i=0; i<String.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String[i]);
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

int calculate_score()
{
	int basket;
	int center_difference = sqrt( pow(ringX-distanceX,2) + pow(ringY-distanceY,2) + pow(ringZ-distanceZ,2) );
	if ( center_difference <= ring_radius -  ball_radius) 
	{
		total_score+= 10;
		return 1;
	}
	else
		return 0;
}

int flag_collision=0;
void collision()
{
	float time=fabs(DTRANS/velocityZ);
	if (distanceY<=-100 && distanceZ>farlimit )
	{
		distanceY-=velocityY*time - 0.5*9.8*time*time;
	    velocityY=-1*velocityY;
	    velocityY=velocityY*(3.0/4.0) ;
	}			
	
	else if ( distanceZ <= farlimit && distanceY > -100 && distanceY <200 )
	{
		flag_basket = calculate_score();
		if(flag_basket==0)
		{
			velocityZ=-1*velocityZ; 
			flag_collision=1;
			distanceZ+=DTRANS;
			velocityZ=velocityZ*(3.0/4.0) ;
		}
	}
}
void trajectory_in_y()
{	
	if(distanceY >-100)
	{
		float time=fabs(DTRANS/velocityY);
		distanceY-=DTRANS;
		velocityX=0;
		velocityZ=0;
		velocityY= velocityY-9.8*time;
		distanceZ=ringZ;
		distanceX=ringX;
		glutPostRedisplay();
	}
	return;
}
void trajectory()
{
	double time;
	time=fabs(DTRANS/velocityZ);
	distanceX-=velocityX*time;
	velocityY=velocityY -(9.8)*time;
	distanceY+= velocityY*time - 0.5*9.8*time*time;
	if(flag_collision==1) 
		distanceZ+=DTRANS;
	else
		distanceZ-=DTRANS;                              //
	collision();
	if (flag_basket==1)
	{
		trajectory_in_y();
	}		
	
	glutPostRedisplay();
}

void defaultfunction()
{
	flag_hit=0;
    distanceZ=-30;
    distanceY=4;
    distanceX=0;
	eyeX=0;eyeY=0;eyeZ=0;lookX=0;lookY=0;lookZ=-1;upX=0;upY=1;upZ=0;
	angleX=0;
	angleY=4;
	angleZ=4;
	power=1;
	flag_collision=0;
	flag_basket=0;
	incpower=true;up=true;
}

void display()
{
	static int counterZ=distanceZ, counterY= distanceY;
	glClearColor(red,green,blue,0.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,aspect_ratio, 1, 1000);
	gluLookAt(eyeX,eyeY,eyeZ,lookX,lookY,lookZ,upX,upY,upZ);

	draw_arrow();
	show_bar();
	draw_wallandbase();
	draw_circle(ringX,ringY,ringZ,ring_radius);
	draw_ball(distanceX, distanceY, distanceZ);
	counterZ=distanceZ;
	counterY=distanceY;
	if(flag_hit==1)
	{ 
	  if(counterZ>=farlimit && counterY > -100  && counterZ <5)
	    {
			if(flag_basket==1 )
			{
				trajectory_in_y();	
				counterY=distanceY;
			}
			else
			{
				trajectory();
				if(flag_collision==1) 
					counterZ+=DTRANS;
				else
					counterZ-=DTRANS;
					counterY=distanceY;
			}
		}
	    else
	    {
	        counterZ=-30;
	        defaultfunction();
	    }
	}
	char string[64];
    sprintf(string,"Score = %d",total_score);
	Render_text(100,600,-50,string);
	glFlush();
}

int max(int a, int b)
{
	if(a>b) return a;
	else return b;
}

int min(int a, int b){
	if(a<b) return a;
	else return b;
}

void calc_angle()
{
	angleX= ((ringX-distanceX )*4)/(sqrt( 2*(pow(distanceX-ringX,2) + pow(distanceZ-ringZ,2) )));
	angleZ= ((ringZ-distanceZ )*4)/(sqrt( 2*(pow(distanceX-ringX,2) + pow(distanceZ-ringZ,2) )));
}

void keyboard(unsigned char key,int x, int y)
{
    length=sqrt(pow(angleX,2) + pow(angleY,2) + pow(angleZ,2));
	theta = asin(angleY/length);
	switch(key)
	{
	    case 'X':	calc_angle(); angleY=4;
       				distanceX+=DTRANS; eyeX+=DTRANS; lookX+=DTRANS;   break;
		case 'x':   calc_angle(); angleY=4;
					distanceX-=DTRANS; eyeX-=DTRANS; lookX-=DTRANS;   break;

		case 'Y': 	distanceY+=DTRANS; eyeY+=DTRANS; lookY+=DTRANS;   break;
		case 'y':   distanceY-=DTRANS; eyeY-=DTRANS; lookY-=DTRANS;   break;
		case 'Z':	eyeZ+=DTRANS;
		         	lookZ+=DTRANS;
		         	distanceZ+=DTRANS; 
			        break;
		case 'z':   eyeZ-=DTRANS;
			    	lookZ-=DTRANS;
			    	distanceZ-=DTRANS;
				    break;
		case 'H':			        
		case 'h':	flag_hit=1;
                    incpower=false;
					checkBasket();
					float magnitude, tilt_angle;
					velocityY=power*factor*sin(theta);
					velocityV=power*factor*cos(theta);                  
					magnitude=sqrt(pow(angleX,2)+pow(angleZ,2));
					tilt_angle=acos(-1*angleX/magnitude);		
					velocityX=velocityV*cos(tilt_angle);   
					velocityZ=velocityV*sin(tilt_angle); 
  				    break;
        case '0':   exit(0);
	}
	glutPostRedisplay();
}

void checkBasket()
{
    double Ui=power*factor;
    double Dist_xz=sqrt(pow(ringX-distanceX,2)+pow(ringZ-distanceZ,2));
    double Sy=fabs(ringY-distanceY);
    double distSq=pow(Dist_xz,2);
    double uiSq=pow(Ui,2);
    double a=4.9*(distSq/uiSq);
    double b=Dist_xz;
    double c=Sy+a;
    double det=pow(b,2)-4*a*c;
    if(det>=0)
    {
        double tanT=(sqrt(det)-b)/(2*a);
        double accr8T=atan(tanT);
        if(fabs(accr8T-theta)<=0.2){
            theta=accr8T;
        }
    }
}

template <class TYPE>
void controller(TYPE key,int x, int y){
	float magnitude, tilt_angle;
	length=sqrt(pow(angleX,2) + pow(angleY,2) + pow(angleZ,2));
	theta = asin(angleY/length);
	magnitude=sqrt(pow(angleX,2)+pow(angleZ,2));
	tilt_angle=acos(angleX/magnitude);
	switch(key)
	{
		case GLUT_KEY_UP :  angleY=4*sqrt(2)*sin(theta+0.1);
							magnitude=4*sqrt(2)*cos(theta+0.1);
							theta+=0.1;
							angleX=magnitude*cos(tilt_angle);
							angleZ=magnitude*sin(tilt_angle);
							break;
		case GLUT_KEY_DOWN :  angleY=4*sqrt(2)*sin(theta-0.1);
							magnitude=4*sqrt(2)*cos(theta-0.1);
							theta-=0.1;
							angleX=magnitude*cos(tilt_angle);
							angleZ=magnitude*sin(tilt_angle);
							break;
	    case GLUT_KEY_RIGHT:  power=min(power+1,10);  break;
	    case GLUT_KEY_LEFT:  power=max(power-1,1); break;
	}
	glutPostRedisplay();
}


void Resize(int w, int h) {
    glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspect_ratio=(double)w / (double)h;
	gluPerspective(fov,aspect_ratio, 1.0, 1000.0);
}

void init(){
	glEnable(GL_DEPTH_TEST);
	load_bmp("floor.bmp",fl);
	load_bmp("plank.bmp",wall);
	load_bmp("ball.bmp",ball);
}

int main (int argc, char ** argv)
{
	  glutInit(&argc,argv);
	  int mode=GLUT_RGBA | GLUT_DEPTH;
	  glutInitDisplayMode(mode);
	  glutInitWindowPosition(XTOP,YTOP);
	  glutInitWindowSize(WIDTH,HEIGHT);
	  int window = glutCreateWindow("Graphics Project");
	  glutDisplayFunc(display);
	  glutKeyboardFunc(keyboard);
	  glutSpecialFunc(controller);
	  glutReshapeFunc(Resize);
	  glutIdleFunc(idle);
	  gluPerspective(70, 1, 1, 100);
	  glMatrixMode(GL_MODELVIEW);
	  glLoadIdentity();
	 
	  init();
	  glutMainLoop();
}

void idle(){
  static GLuint PreviousClock=glutGet(GLUT_ELAPSED_TIME);
  static GLuint Clock=glutGet(GLUT_ELAPSED_TIME);
  static GLfloat deltaT;
  Clock = glutGet(GLUT_ELAPSED_TIME);
  deltaT=Clock-PreviousClock;
  if (deltaT < 150) {return;} 
  else {PreviousClock=Clock;}
  if (incpower){
        if(up){power+=1;if(power==10)up=false;}
        else {power-=1;if(power==1)up=true;}
  }
  glutPostRedisplay();
}


void draw_ball(double x,double y, double z)
{   glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(x,y,z);
    GLUquadric *quadratic;
		quadratic = gluNewQuadric();
glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, ball);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    gluQuadricTexture(quadratic,1);
			gluSphere(quadratic,ball_radius,32,32);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
}

void draw_arrow()
{
	glBegin(GL_LINES);
	glVertex3f(distanceX, distanceY, distanceZ); // from
	glVertex3f(distanceX+angleX, distanceY+angleY, distanceZ+angleZ);// to  
	glEnd(); 
}

void draw_circle(double x, double y, double z,double radius) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, z+radius);
    static const int circle_points = 100;
    static const double angle = 2.0f * 3.1416f / circle_points;
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin(GL_POLYGON);  
    glColor3f(1.0,0.0,0.5);
    double angle1=0.0;
    glVertex3f(radius * cos(0.0) , 0, radius * sin(0.0));
    int i;
    for (i=0; i<circle_points; i++)
    {       
        glVertex3f(radius * cos(angle1), 0, radius *sin(angle1));
        angle1 += angle;
    }
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopMatrix();
}

void show_bar()
{
	glBegin(GL_QUADS);
	int i,j;
	switch(power)
	{
	case 10 :  	glColor3f(0.6875,0.1875,0.375); 
			glVertex3f(155,150+45,farlimit);glVertex3f(175,150+45,farlimit);glVertex3f(175,150+47,farlimit);glVertex3f(155,150+47,farlimit);
			

	case 9: 	glColor3f(1,0,0 ); 
			glVertex3f(155,150+40,farlimit);glVertex3f(175,150+40,farlimit);glVertex3f(175,150+42,farlimit);glVertex3f(155,150+42,farlimit);

	case 8 :	glColor3f(1,0.2695,0); 
	glVertex3f(155,150+35,farlimit);glVertex3f(175,150+35,farlimit);glVertex3f(175,150+37,farlimit);glVertex3f(155,150+37,farlimit);

	case 7 :	glColor3f(1,0.625,0.4765); 
	glVertex3f(155,150+30,farlimit);glVertex3f(175,150+30,farlimit);glVertex3f(175,150+32,farlimit);glVertex3f(155,150+32,farlimit);

	case 6 :	glColor3f( 1 , 1 , 0); 
	glVertex3f(155,150+25,farlimit);glVertex3f(175,150+25,farlimit);glVertex3f(175,150+27,farlimit);glVertex3f(155,150+27,farlimit);

	case 5 :	glColor3f(0.0137255 , 1.0,0.0137255); 
	glVertex3f(155,150+20,farlimit);glVertex3f(175,150+20,farlimit);glVertex3f(175,150+22,farlimit);glVertex3f(155,150+22,farlimit);

	case 4 :	glColor3f(0.184314 , 0.309804 ,0.184314); 
	glVertex3f(155,150+15,farlimit);glVertex3f(175,150+15,farlimit);glVertex3f(175,150+17,farlimit);glVertex3f(155,150+17,farlimit);

	case 3 :	glColor3f( 0.402 , 0.47059 , 0.847059); 
	glVertex3f(155,150+10,farlimit);glVertex3f(175,150+10,farlimit);glVertex3f(175,150+12,farlimit);glVertex3f(155,150+12,farlimit);

	case 2 :
	glColor3f(0.309804 , 0.284314 , 0.9); 
	glVertex3f(155,150+5,farlimit);glVertex3f(175,150+5,farlimit);glVertex3f(175,150+7,farlimit);glVertex3f(155,150+7,farlimit);

	case 1:
	glColor3f(0.013 , 0.12 , 1.0); 
	glVertex3f(155,150+0,farlimit);glVertex3f(175,150+0,farlimit);glVertex3f(175,150+2,farlimit);glVertex3f(155,150+2,farlimit);
	
}
	glEnd();
	
}

void draw_wallandbase()
{   glEnable(GL_TEXTURE_2D);
   	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   	glBindTexture(GL_TEXTURE_2D, fl);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-150,-100,0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-150,-100,farlimit);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(150,-100,farlimit);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(150,-100,0);
		
	glEnd();
	glDisable(GL_TEXTURE_2D);
	    glEnable(GL_TEXTURE_2D);
   	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   	glBindTexture(GL_TEXTURE_2D, wall);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-150,-100,farlimit);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-150,200,farlimit);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(150,200,farlimit);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(150,-100,farlimit);
	glEnd();
	glDisable(GL_TEXTURE_2D);

}


