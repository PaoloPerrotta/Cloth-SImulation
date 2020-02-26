/*  Paolo Nicola Perrotta - N97/266
	grafica computazionale e laboratorio
	Anno accademico 2018/2019
	Mass-Spring Model for cloth simulation
*/

#include <stdlib.h>
#include <windows.h>
#include <SOIL.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "Wire.h"
#include "Light.h"
#include "Material.h"

//define
#define MAX_RESOLUTION 60
#define MIN_RESOLUTION 20
#define RESOLUTION_DELTA 5

#define WIRE_SIZE 600

#define SPHERE_RADIUS 100
#define SPHERE_SLICES 20

#define MIN_PARTICLES_PER_HAIR 1
#define MAX_PARTICLES_PER_HAIR 10

#define REFRESH_RATE 60


//init support objects
Wire *myWire;

Light *dirLight;

Material *material;

//texture
static GLuint texnames[2];

//toggle variables showing/hiding elements with their state
int showParticles = 1;
int showTexture = 1;
int showWireframe = 0;
int flatShade = 0;
int lightEnabled = 1;
int figure = 0;
bool isPaused = false;

int floorY= -500;
int resolution = MIN_RESOLUTION;
//flag mouse
float rotationX=0.0;
float rotationY=0.0;
float prevX=0.0;
float prevY=0.0;
bool mouseDown=false;

//flag fixed fall
bool flag1=true;
bool flag2=true;
bool flag3=true;
bool flag4=true;

//gravity
float gravityForce = 10.0f;
Vector *gravityDirection;
//hook law
float ks = 20;
float kd = 0.2f;
float particleRadius = 5;
//window, perspective data
float fovy = 45.0;
float dNear = 100;
float dFar = 2000;
//camera
float global_ambient[4] = {0.0, 0.0, 0.0, 0.0};
float cameraEye[3] = {0.0, 0.0, 1000.0};
float cameraLookAt[4] = {0.0, 0.0, 0.0, 1.0};
float cameraUp[4] = {0.0, 1.0, 0.0};
float viewRotation[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
//light
float lightRotation[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
//object poition
float objPos[3] = {-50.f, 0.0f, +50.f};

float intensity = 1.0f;

//init texture, not utilized
GLvoid initTexture(char *imagefilename, int pos, bool isMipmap) {
    glBindTexture(GL_TEXTURE_2D, texnames[pos]);
    GLsizei imageWidth, imageHeight, components;

    unsigned char *image;
    image = SOIL_load_image(imagefilename, &imageWidth, &imageHeight, &components, SOIL_LOAD_RGBA);
    if(image){
        if(!isMipmap) {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            GLsizei sWidth=256, sHeight=256;
            unsigned *sImage = (unsigned *)malloc( sWidth*sHeight*4*sizeof( unsigned ) );
            gluScaleImage( GL_RGBA, imageWidth, imageHeight, GL_UNSIGNED_BYTE, image, sWidth, sHeight, GL_UNSIGNED_BYTE, sImage );
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sWidth, sHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, sImage);
        } else {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
    }
    else
        printf("errore caricamento texture\n");
    SOIL_free_image_data(image);

    glEnable( GL_TEXTURE_2D );
}

//init perspective
void reshape(int w, int h){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (GLdouble)w/(GLdouble)h, dNear, dFar);
    glViewport(0, 0, w, h);
}

//draw routine
void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	//update light pos
	glMultMatrixf(lightRotation);
    glLightfv(GL_LIGHT0, GL_POSITION, dirLight->pos);

	//update camera pos
	glLoadIdentity();
    gluLookAt(cameraEye[0], cameraEye[1], cameraEye[2], cameraLookAt[0], cameraLookAt[1], cameraLookAt[2], cameraUp[0], cameraUp[1], cameraUp[2]);

    glMultMatrixf(viewRotation);

	glRotatef(rotationX,1,0,0);
	glRotatef(rotationY,0,1,0);
	//cloth surface render
	myWire->renderTriangles(showWireframe,flatShade);
	//toggle lights
	if(lightEnabled){
		glEnable(GL_LIGHTING);
	}
	if(showParticles){
		myWire->renderParticles();
	}

	glLightfv(GL_LIGHT0,GL_DIFFUSE,dirLight->difusse);

	//toggle texture & materials
	if(showTexture){
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
	}else{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_COLOR_MATERIAL);
	}

	//toggle flatshade
	if(flatShade){
		glShadeModel(GL_FLAT);
	}else{
		glShadeModel(GL_SMOOTH);
	}

	//create sphere
	glPushMatrix();
		glTranslatef(objPos[0],objPos[1],objPos[2]);
		glColor3f(0.6,0.0,0.0);
		glutSolidSphere(SPHERE_RADIUS,SPHERE_SLICES,SPHERE_SLICES);
	glPopMatrix();
    //stop vertex
    myWire->particles[resolution-1][resolution-1].isFixed=flag1;//flag1
    myWire->particles[0][0].isFixed=flag2;
    myWire->particles[resolution-1][0].isFixed=flag3;//flag3
    myWire->particles[0][resolution-1].isFixed=flag4;
    glColor3f(0,1,1);
	glutSwapBuffers();
}


void init(void){
	rotationX = 0;
	rotationY = 0;
	//create a vector to simulate gravity, pointing down
	gravityDirection = new Vector(0,-1,0);

	//init light pos
	dirLight = new Light();
	dirLight->setPosition(100,100,100);
	dirLight->setAmbient(0,0,0);
	dirLight->setDifusse(0.8f,0.8f,0.8f);
	dirLight->setSpecular(0.0f,0.8f,0.0f);

	//init material
	material = new Material();
    material->setAmbient(0, 0, 0, 1);
    material->setDifusse(0.7f, 0.7f, 0.7f, 1.0f);
    material->setSpecular(0.01f, 0.01f, 0.01f, 1.0f);
	material->setShininess(32);

	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1,1,1,1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	//missing textures and blind

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glLightfv(GL_LIGHT0, GL_POSITION, dirLight->pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,   dirLight->ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR,  dirLight->specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,   dirLight->difusse);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

//mouse button callback
void mouse(int button, int state, int x , int y){
	if(button == GLUT_LEFT_BUTTON && state== GLUT_DOWN){
		mouseDown = true;
		prevX = x-rotationY;
		prevY = y-rotationX;
	}else{
		mouseDown = false;
	}
	//scrollup - down
    if(button == 3)
        objPos[1]=objPos[1]+1;
    if(button == 4)
        objPos[1]=objPos[1]-1;

    glutPostRedisplay();
}

//mouse motion callback
void mouseMotion(int x, int y){
    if(mouseDown){
        rotationX = y - prevY;
        rotationY = x - prevX;
        glutPostRedisplay();
    }
}

//init a wire class, paremeters are specified by global variables
void initWire(){
    myWire = new Wire(resolution, WIRE_SIZE);
}

//keyboard callback
void key(unsigned char key, int x, int y){

    if(key == 'x') cameraEye[0]-= 50;
    if(key == 'X') cameraEye[0]+= 50;
    if(key == 'y') cameraEye[1]-= 50;
    if(key == 'Y') cameraEye[1]+= 50;
    if(key == 'z') cameraEye[2]-= 50;
    if(key == 'Z') cameraEye[2]+= 50;
    if(key == 'r' || key == 'R') showParticles = !showParticles;
    if(key == 't' || key == 'T') showTexture = !showTexture;
    if(key == 'q' || key == 'Q') showWireframe = !showWireframe;
    if(key == 'e' || key == 'E') flatShade = !flatShade;
    //if(key == 'e' || key == 'E') flatShade = !flatShade;
    //object movement
    if(key == 'w' || key == 'W') objPos[0]=objPos[0]-1;
    if(key == 's' || key == 'S') objPos[0]=objPos[0]+1;
    if(key == 'a' || key == 'A') objPos[2]=objPos[2]+1;
    if(key == 'd' || key == 'D') objPos[2]=objPos[2]-1;

    //wire vertex fix
    if(key == '1') flag1=!flag1;
    if(key == '2') flag2=!flag2;
    if(key == '3') flag3=!flag3;
    if(key == '4') flag4=!flag4;

    //wire density (+ or -)
    if(key == '+'){
       if(resolution < MAX_RESOLUTION){
            resolution += RESOLUTION_DELTA;
            initWire();
       }
    }
    if(key == '-'){
        if(resolution > MIN_RESOLUTION){
            resolution -= RESOLUTION_DELTA;
            initWire();
        }
    }
    glutPostRedisplay();
}

//keyboard special callback
void specialKey(int key, int x, int yy){
//gravity vector
	switch (key) {
		case GLUT_KEY_UP   : gravityDirection->y = 1; break;
		case GLUT_KEY_DOWN : gravityDirection->y = -1; break;
		case GLUT_KEY_F1 : gravityDirection->y = 0; break;
		case GLUT_KEY_LEFT : gravityDirection->x = -1; break;
		case GLUT_KEY_RIGHT :gravityDirection->x = 1; break;
		case GLUT_KEY_F2 :gravityDirection->x = 0; break;
	}
}

//timer func, updates graphycs elements
void update(int i){
    if(isPaused == false){
        myWire->update(SPHERE_RADIUS, gravityForce, *gravityDirection, ks, kd, particleRadius, objPos,floorY);
        glutPostRedisplay();
    }
    glutTimerFunc(60, update, 1);
}

//main func
int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowPosition(10, 10);
    glutInitWindowSize(1300, 700);
    glutCreateWindow("ClothSimulation");

    init();
    initWire();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //mouse press
    glutMouseFunc(mouse);
    //keyboard
    glutKeyboardFunc(key);
    glutSpecialFunc(specialKey);
    //timer updating wire elements
    glutTimerFunc(60, update, 10);
    //mouse motion
    glutMotionFunc(mouseMotion);


    glutMainLoop();

    return(EXIT_SUCCESS);
}

