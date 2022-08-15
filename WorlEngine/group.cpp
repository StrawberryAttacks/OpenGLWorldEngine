#include "group.h"

Group::Group() : m_scale(1.0)
{

}

Group::~Group()
{

}

void Group::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions, bool usingTextures)
{
    for (qsizetype i = 0; i < m_objects.size(); ++i)
        m_objects[i]->draw(program, functions, usingTextures);

}

void Group::Rotate(const QQuaternion &r)
{
    m_rotate = r * m_rotate;

    for (qsizetype i = 0; i < m_objects.size(); ++i)
        m_objects[i]->SetGlobalTransform(getLocalMatrix());

}

void Group::Translate(const QVector3D &t)
{
    m_translate += t;

    for (qsizetype i = 0; i < m_objects.size(); ++i)
        m_objects[i]->SetGlobalTransform(getLocalMatrix());
}

void Group::Scale(const float &s)
{
    m_scale *= s;

    for (qsizetype i = 0; i < m_objects.size(); ++i)
        m_objects[i]->SetGlobalTransform(getLocalMatrix());
}

void Group::SetGlobalTransform(const QMatrix4x4 &q)
{
    m_globalTransforms = q;

    for (qsizetype i = 0; i < m_objects.size(); ++i)
        m_objects[i]->SetGlobalTransform(getLocalMatrix());
}

void Group::addObject(Transformational *obj)
{
    m_objects.append(obj);

    m_objects[m_objects.size() - 1]->SetGlobalTransform(getLocalMatrix());
}

void Group::delObject(Transformational *obj)
{
    m_objects.removeAll(obj); //удаляет все объекты. removeOne(obj) - удаляет первый попавшийся такой объект (обход с начала вектора)
}

void Group::delObject(const int &&index)
{
    m_objects.remove(index);
}

QMatrix4x4 Group::getLocalMatrix()
{
    QMatrix4x4 localMatrix;
    localMatrix.setToIdentity();
    localMatrix.translate(m_translate);
    localMatrix.rotate(m_rotate);
    localMatrix.scale(m_scale);
    localMatrix = m_globalTransforms * localMatrix;

    return localMatrix;
}
