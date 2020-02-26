#ifndef MATERIAL_H
#define MATERIAL_H
//class created to manage materials
class Material{
    public:
        //material light properties
        float ambient[4];
        float difusse[4];
        float specular[4];
        float shininess;

        Material();
        virtual ~Material();
        //setter
        void setAmbient(float x, float y, float z, float e);
        void setDifusse(float x, float y, float z, float e);
        void setSpecular(float x, float y, float z, float e);
        void setShininess(float shine);

    protected:
    private:
};

#endif // MATERIAL_H
