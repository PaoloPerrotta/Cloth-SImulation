#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include "Vector.h"
//class created to manage point, adds some force properties to the single vertex
#ifndef PARTICLE_H
#define PARTICLE_H

class Particle{
    public:
        float mass;
        //coordinates of the particle inside the full wire
        int posI, posJ;
        //particle "size"
        float dim;

        //box to avoid internal collision
        float boxRadius;

        int numberOfAdjTriangles;
        int adjTriangles[6];
        int dampForce;
        //position
        Vector *position;
        Vector *prevPosition;
        //accumulated force on the particle
        Vector *forceAcum;
        //particle speed
        Vector *velocity;
        //if the particle position is fixed
        bool isFixed;
        //normal on the particle
        Vector *normal;

        Particle();
        virtual ~Particle();
        void setPosition(float pX, float pY, float pZ, int posI, int posJ);
        void clearForces();
        //adds a force to the total accumulated forces on the particle
        void addForce(float x, float y, float z);
        void render();
        Vector *verletIntegrationToVelocity();
        Vector *verletIntegrationToPosition();
        void update(int sphereRadius, float *objPos, int floorY);
        void checkCollisionWithSphere(int radius, float *objPos);
        void checkCollisionWithCapsule(Vector p0, Vector p1, float radius);
        void checkCollisionWithFloor(int floorY);

    protected:
    private:
};

#endif // PARTICLE_H
