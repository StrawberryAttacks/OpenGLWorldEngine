#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>

#include "group.h"
#include "camera.h"
#include "skybox.h"
#include "object3d.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <QFile>

class Widget : public QOpenGLWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:

    void initializeGL(); //вызывается один раз, при создании приложения
    void resizeGL(int w, int h); //вызывается при масштабировании окна
    void paintGL(); //можно вызвать принудительно методом update()

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void initShaders();

    void initCube(float width, float height, float depth, QImage *diffuseMap = 0, QImage *normalMap = 0);

    void loadObj(const QString &&path);

    int SelectObject(int x, int y, QVector<Transformational *> &objs);

private:

    QVector3D screenCoordsToWorldCoords(const QVector2D &mousePos);

    QMatrix4x4 ProjectionMatrix,
               ProjectionLightMatrix/*,
               LightMatrix,
               ShadowLightMatrix*/;

    QOpenGLShaderProgram CharacterShaderProgram,
                           SkyBoxShaderProgram,
                            SelectShaderProgram,
                            FloorShaderProgram;

    QVector2D MousePosition;

    QVector<Object3D *> objects;

    QVector<Transformational *> transformObjects,
                                selectObjects,
                                floorObjects;

    QVector<Group *> groups;

    Camera* m_camera;

    SkyBox* m_skybox;

};
#endif // WIDGET_H
