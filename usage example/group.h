#ifndef GROUP_H
#define GROUP_H


#include "transformational.h"

#include <QVector>
#include <QQuaternion>
#include <QVector3D>

#include <QMatrix4x4>


class Group : public Transformational
{
public:
    Group();
    ~Group();

    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions, bool usingTextures = true);

    void Rotate(const QQuaternion &r);

    void Translate(const QVector3D &t);

    void Scale(const float &s);

    void SetGlobalTransform(const QMatrix4x4 &q);

    void addObject(Transformational *obj);

    void delObject(Transformational *obj);
    void delObject(const int &&index);

private:

    QQuaternion m_rotate;
    QVector3D m_translate;
    float m_scale;
    QMatrix4x4 m_globalTransforms;

    QVector<Transformational *> m_objects;

protected:

    QMatrix4x4 getLocalMatrix();
};

#endif // GROUP_H
