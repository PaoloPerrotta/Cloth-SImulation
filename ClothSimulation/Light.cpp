#include "Light.h"
//class created to manage lights
Light::Light(){
    //ctor
}

Light::~Light(){
    //dtor
}

//light position
void Light::setPosition(float x, float y, float z){
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    pos[3] = 0.0f;
}

//light ambient values
void Light::setAmbient(float x, float y, float z){
    ambient[0] = x;
    ambient[1] = y;
    ambient[2] = z;
}

void Light::setDifusse(float x, float y, float z)
{
    difusse[0] = x;
    difusse[1] = y;
    difusse[2] = z;
}

//light specular values
void Light::setSpecular(float x, float y, float z){
    specular[0] = x;
    specular[1] = y;
    specular[2] = z;
}
