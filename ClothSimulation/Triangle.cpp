#include "Triangle.h"

Triangle::Triangle(){
    //ctor
    normal = new Vector();
}

Triangle::~Triangle(){
    //dtor
}

void Triangle::setTriangle(Particle *p1, Particle *p2, Particle *p3){
    particles[0] = p1;
    particles[1] = p2;
    particles[2] = p3;

    //printf("V1: %f, %f\n", particles[0]->position->x, particles[0]->position->z);
    //printf("V2: %f, %f\n", particles[1]->position->x, particles[1]->position->z);
    //printf("V2: %f, %f\n", particles[2]->position->x, particles[2]->position->z);
}

void Triangle::render(int resolution, bool showWireframe, bool flatShade){
    float uOffset = 1.0 / (resolution - 1);
    float vOffset = 1.0 / (resolution - 1);
    //calculate triangle normal
    calculateNormal();
    //Beware of the orientation of the triangles
    if(showWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(flatShade){
        glNormal3f(normal->x, normal->y, normal->z);
    }
    glColor3f(0,1,1);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 3; i++){
            //texture coordinate
            glTexCoord2f(particles[i]->posI * uOffset, (resolution - 1 - particles[i]->posJ) * vOffset);
            if(!flatShade){//if not flashade, single vertex normal are calculated
                glNormal3f(particles[i]->normal->x, particles[i]->normal->y, particles[i]->normal->z);
            }
            glVertex3f(particles[i]->position->x, particles[i]->position->y, particles[i]->position->z);
        }
    glEnd();
}
//triangle normal
void Triangle::calculateNormal(){
    Vector *v1 = new Vector();
    Vector *v2 = new Vector();

    //substract is an static method so we call it with the :: notation
    Vector::substract(v1, *particles[1]->position, *particles[0]->position);
    Vector::substract(v2, *particles[2]->position, *particles[0]->position);

    Vector::crossProduct(normal, *v1, *v2);
}
