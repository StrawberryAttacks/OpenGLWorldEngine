#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "simpleobject.h"
#include "materiallibrary.h"

#include <QVector>

#include "transformational.h"

#include <QFile>
#include <QFileInfo>

class Object3D : public Transformational
{
public:
    Object3D();

    ~Object3D() {

        for(auto o: objects)
            delete o;

    }

    void loadObjectFromFile(const QString &path);
    void addObject(SimpleObject *object );
    SimpleObject *getObject(quint32 index);

    void calculateTBN(QVector<VertexData> &data);

    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions, bool usingTextures = true);

    void Rotate(const QQuaternion &r);

    void Translate(const QVector3D &t);

    void Scale(const float &s);

    void SetGlobalTransform(const QMatrix4x4 &q);


private:
    QVector<SimpleObject *> objects;
    MaterialLibrary materialLibrary;

};
#endif // OBJECT3D_H
