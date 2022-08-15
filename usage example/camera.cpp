#include "camera.h"

Camera::Camera() : m_scale(1.0)
{
    m_globalTransforms.setToIdentity();
}

void Camera::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions, bool usingTextures)
{
    Q_UNUSED(usingTextures);

    if (functions)
        return;

    program->setUniformValue("u_viewMatrix", ViewMatrix);
}

void Camera::Rotate(const QQuaternion &r)
{
    m_rotate = r * m_rotate;

    updateViewMatrix();
}

void Camera::Translate(const QVector3D &t)
{
    m_translate += t;

    updateViewMatrix();
}

void Camera::Scale(const float &s)
{
    m_scale *= s;

    updateViewMatrix();
}

void Camera::SetGlobalTransform(const QMatrix4x4 &q)
{
    m_globalTransforms = q;

    updateViewMatrix();
}

void Camera::RotateX(const QQuaternion &r)
{
    rotateX = r * rotateX;
    m_rotate = rotateX * rotateY;

    updateViewMatrix();
}

void Camera::RotateY(const QQuaternion &r)
{
    rotateY = r * rotateY;
    m_rotate = rotateX * rotateY;

    updateViewMatrix();
}

const QMatrix4x4 &Camera::getViewMatrix() const
{
    return ViewMatrix;
}

void Camera::updateViewMatrix()
{
    ViewMatrix.setToIdentity();
    ViewMatrix.translate(m_translate);
    ViewMatrix.rotate(m_rotate);
    ViewMatrix.scale(m_scale);
    ViewMatrix = ViewMatrix * m_globalTransforms.inverted();
}
