#ifndef LIGHT_H
#define LIGHT_H
//class created to manage lights
class Light{
    public:
        //light properties
        float pos[4];
        float ambient[3];
        float difusse[3];
        float specular[3];
        //setter
        void setPosition(float x, float y, float z);
        void setAmbient(float x, float y, float z);
        void setDifusse(float x, float y, float z);
        void setSpecular(float x, float y, float z);

        Light();
        virtual ~Light();
    protected:
    private:
};

#endif // LIGHT_H
