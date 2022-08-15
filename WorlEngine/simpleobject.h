#ifndef SIMPLEOBJECT_H
#define SIMPLEOBJECT_H

#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "transformational.h"
#include "material.h"

struct VertexData {

    VertexData() {}

    VertexData(QVector3D p, QVector2D t, QVector3D n) : position (p),
        texCoord(t), normal(n) {}

    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;

    QVector3D tangent,
              bitangent;
};

class SimpleObject : public Transformational
{
public:
    SimpleObject();

    SimpleObject(const QVector<VertexData> &data,
                   const QVector<GLuint> &indexes, const QImage &texture);

    SimpleObject(const QVector<VertexData> &data, const QVector<GLuint> &indexes, Material *material);

    ~SimpleObject();

    void Init(const QVector<VertexData> &data,
              const QVector<GLuint> &indexes, const QImage &texture);


    void Init(const QVector<VertexData> &data,
              const QVector<GLuint> &indexes, Material *material);

    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions, bool usingTextures = true);

    void Rotate(const QQuaternion &r);

    void Translate(const QVector3D &t);

    void Scale(const float &s);

    void SetGlobalTransform(const QMatrix4x4 &q);

private:
    QOpenGLBuffer VertexBuffer,
                  IndexBuffer; //чтоб было понятно, как пройтись треугольниками по объявленному выше массиву с данными

    QOpenGLTexture *m_diffuseMap,
                    *m_normalMap;

    //составляющие модельно-видовой матрицы:
    QQuaternion m_rotate;
    QVector3D m_translate;
    float m_scale;
    QMatrix4x4 m_globalTransforms;

    Material *material;
};

#endif // SIMPLEOBJECT_H
