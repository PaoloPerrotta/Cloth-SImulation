#include "Particle.h"
//class created to manage point, adds some force properties to the single vertex
Particle::Particle(){
    //ctor
    //forceAcum is the resulting force on the object
    forceAcum = new Vector();
    velocity = new Vector();
    normal = new Vector();
    this->dampForce = 5;
    this->dim = 5;
    this->numberOfAdjTriangles = 0;
}

Particle::~Particle(){
    //dtor
}

void Particle::setPosition(float pX, float pY, float pZ, int posI, int posJ){
    //The position relative to the index in the mesh
    this->posI = posI;
    this->posJ = posJ;

    //position variables in the drawing space
    position = new Vector(pX, pY, pZ);
    prevPosition = new Vector(pX, pY, pZ);
}

//reset accumulated force components on the single particle
void Particle::clearForces(){
    forceAcum->x = 0.0f;
    forceAcum->y = 0.0f;
    forceAcum->z = 0.0f;
}

void Particle::addForce(float x, float y, float z){
    forceAcum->x += x;
    forceAcum->y += y;
    forceAcum->z += z;
}

//display particles
void Particle::render(){
    glPointSize((int)dim);

    if(isFixed)
        glColor3f(0, 0, 1);
    else
        glColor3f(1, 1, 1);

    glBegin(GL_POINTS);
        glVertex3f(position->x, position->y, position->z);
    glEnd();
}

//using verlet Intregration to get next position
Vector * Particle::verletIntegrationToPosition(){
    Vector *nextPos = new Vector();

    //delta time not used

    //Verlet corrected
    /*nextPos->x = position->x + (position->x - prevPosition->x) + forceAcum->x * 0.1f * 0.1f;
    nextPos->y = position->y + (position->y - prevPosition->y) + forceAcum->y * 0.1f * 0.1f;
    nextPos->z = position->z + (position->z - prevPosition->z) + forceAcum->z * 0.1f * 0.1f;*/

    //Verlet not corrected
    nextPos->x = (2 * position->x) - prevPosition->x + forceAcum->x * 0.1f * 0.1f;
    nextPos->y = (2 * position->y) - prevPosition->y + forceAcum->y * 0.1f * 0.1f;
    nextPos->z = (2 * position->z) - prevPosition->z + forceAcum->z * 0.1f * 0.1f;

    return nextPos;
}

//Update particle position, checking if it collides with the sphere object
void Particle::update(int sphereRadius, float *objPos, int floorY){
    //updates the particle speed taking into account the force accumulated on it
    velocity->x = forceAcum->x * 0.1f;
    velocity->y = forceAcum->y * 0.1f;
    velocity->z = forceAcum->z * 0.1f;

    float tempX = position->x;
    float tempY = position->y;
    float tempZ = position->z;

    Vector *nextPos = verletIntegrationToPosition();
//if fixed, particle's position is not updated
    if(!isFixed){
        position->x = nextPos->x;
        position->y = nextPos->y;
        position->z = nextPos->z;
    }

    prevPosition->x = tempX;
    prevPosition->y = tempY;
    prevPosition->z = tempZ;
//if new position collides with the sphere position
    checkCollisionWithSphere(sphereRadius, objPos);
//if new position collides with ideal floor
    //checkCollisionWithFloor(floorY);
}


//method used to avoid collision between the cloth and the sphere object
void Particle::checkCollisionWithSphere(int radius, float *objPos){
    float penDist;
    Vector *spherePos = new Vector(objPos[0], objPos[1], objPos[2]);
    Vector *dist = new Vector();
    //distance between sphere center and particle position
    Vector::substract(dist, *position, *spherePos);
    //distance calculated taking count of the particle "size"
    penDist = dist->getLength() - (radius + this->dim);
    //normalizing distance
    dist->normalize();

    Vector *P = new Vector();
    //multiply normalized distance with penDist to obtain (supposed) collision coordinates
    Vector::multiplicationByScalar(P, *dist, penDist);
    //if collision
    if(penDist < 0){
        this->position->x += P->x;
        this->position->y += P->y;
        this->position->z += P->z;
        }
}

//method used to avoid collision between the cloth and the sphere object
void Particle::checkCollisionWithFloor(int floorY){
    //distance between particle and floor
    float floorDist=this->position->y-floorY;
    //if collision
    if(floorDist < 0){
        this->position->y = floorY;
    }
}


//check collision with capsule around an object, unused, method taken from web resources
void Particle::checkCollisionWithCapsule(Vector p0, Vector p1, float radius){
    Vector *A = new Vector();
    Vector::substract(A, p0, p1);

    Vector *D = new Vector();
    Vector::substract(D, p0, *(position));

    float d = Vector::dotProduct(*D, *A);

    if(d < 0){
        d = 0;
    }
    else if(d > A->getLength()){
        d = A->getLength();
    }

    Vector *aD = new Vector();
    Vector::multiplicationByScalar(aD, *A, d);

    Vector *R = new Vector();
    R->x = p0.x + aD->x;
    R->y = p0.y + aD->y;
    R->z = p0.z + aD->z;

    Vector *distHR = new Vector();
    Vector::substract(distHR, *R, *position);

    float b = distHR->getLength();

    if(b < (this->dim + dim)){
        float p = b - (this->dim + dim);

        Vector *N = new Vector();
        N->x = distHR->x;// / b;
        N->y = distHR->y;// / b;
        N->z = distHR->z;// / b;
        N->normalize();

        Vector *P = new Vector();
        Vector::multiplicationByScalar(P, *N, p);

        this->position->x += P->x;
        this->position->y += P->y;
        this->position->z += P->z;
    }
}

