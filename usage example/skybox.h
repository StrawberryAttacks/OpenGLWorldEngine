#ifndef SKYBOX_H
#define SKYBOX_H

#include "transformational.h"
#include "simpleobject.h"

class SkyBox : public Transformational
{
public:
    SkyBox(float width, const QImage &texture);
    ~SkyBox();

    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions, bool usingTextures = true);

    void Rotate(const QQuaternion &r);

    void Translate(const QVector3D &t);

    void Scale(const float &s);

    void SetGlobalTransform(const QMatrix4x4 &q);


private:

    SimpleObject *cube;

};

#endif // SKYBOX_H
