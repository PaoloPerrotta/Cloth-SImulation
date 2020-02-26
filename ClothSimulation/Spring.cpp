#include "Spring.h"

Spring::Spring(Particle *p1, Particle *p2, float normalLength){
    //ctor
    //Particles connected by this spring
    this->p1 = p1;
    this->p2 = p2;
    //Hook coef
    this->ks = 20;
    //elasticity coef
    this->kd = 0.2f;
    //resting distance of the spring
    this->normalLength = normalLength;
}

Spring::~Spring(){
    //dtor
}

void Spring::update(float ks, float kd){
    //update spring coef
    this->ks = ks;
    this->kd = kd;
    //updates the forces acting on the spring
    applySpringForce();
}

//render spring lines
void Spring::render(){
    glColor3f(0, 1, 1);
    glBegin(GL_LINES);
        glVertex3f(p1->position->x, p1->position->y, p1->position->z);
        glVertex3f(p2->position->x, p2->position->y, p2->position->z);
    glEnd();
}

//updates the forces acting on the spring
void Spring::applySpringForce(){
    //The distance between both particles in the spring
    float ex = p2->position->x - p1->position->x;
    float ey = p2->position->y - p1->position->y;
    float ez = p2->position->z - p1->position->z;

    //Vector *springVector = new Vector();
    //Vector::substract(springVector, *(p1->position), *(p2->position));
    //vector length
    float length = sqrt(pow(ex, 2) + pow(ey, 2) + pow(ez, 2));

    //Normalize the vector
    ex = ex / length;
    ey = ey / length;
    ez = ez / length;

    //Calculate Vi scalars
    float v1 = ex * p1->velocity->x + ey * p1->velocity->y + ez * p1->velocity->z;
    float v2 = ex * p2->velocity->x + ey * p2->velocity->y + ez * p2->velocity->z;

    //calculate spring force, Elasticity (Hook's law) + Damping force
    float fSD = -ks * (normalLength - this->getLength()) - kd * (v1 - v2);

    //The vector to apply
    float fx = fSD * ex;
    float fy = fSD * ey;
    float fz = fSD * ez;
    //Apply the resulting force to both ends of the spring, with the opposite direction
    p1->addForce(fx, fy, fz);
    p2->addForce(-fx, -fy, -fz);
}

//spring length
float Spring::getLength(){
    Vector *dist = new Vector();
    Vector::substract(dist, *(p1->position), *(p2->position));
    return dist->getLength();
}
