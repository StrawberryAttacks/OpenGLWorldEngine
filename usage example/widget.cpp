#include "widget.h"

#include <QtMath>
#include <QOpenGLFunctions>

#include <QOpenGLContext>

#define WIDTH 100.0f

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_camera = new Camera;
    m_camera->Translate(QVector3D(0.0f, 0.0f, -5.0f));
}

Widget::~Widget()
{
    delete m_camera;

    for (qsizetype i = 0; i < objects.size(); ++i)
        delete objects[i];

    for (qsizetype i = 0; i < groups.size(); ++i)
        delete groups[i];
}

void Widget::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); //не рисовать задние грани

    initShaders();

    float x = -0.5f, y = -3.5f, z = -8.0;

    QImage FloorTexture(":/ice_texture.jpg"),
            FloorNormalMap(":/ice1_n.jpg"),

            PlayerFloorTexture(":/water.jpg");

    groups.append(new Group);

    short tmp = 8;

    for (short Z = 0; Z < tmp; ++Z) {

        for (short X = 0; X < tmp; ++X) {

            if (((X == 0 || X == 1) && (Z == 0)) //первые два квадратика в начале карты
               || ((X == tmp - 1 || X == tmp - 2) && (Z == tmp - 1))) //два квадратика в конце

                initCube(3.0f, 3.3f, 3.0f, &PlayerFloorTexture);

            else
                initCube(3.0f, 3.0f, 3.0f, &FloorTexture, &FloorNormalMap);

            objects.last()->Translate(QVector3D(x, y, z));

            transformObjects.append(objects.last());

            selectObjects.append(objects.last());

            groups.last()->addObject(objects.last());

            x += 3.5f;
        }

        z += 3.5f;

        x = -0.5f;
    }

    groups.first()->Translate(QVector3D(-tmp - 3.0, -3.5f, -tmp * 4.0f));

    m_skybox = new SkyBox(WIDTH, QImage(":/skybox0.png"));
}

void Widget::resizeGL(int w, int h)
{
    float aspect = w / (float)h;

    ProjectionMatrix.setToIdentity();
    ProjectionMatrix.perspective(45, aspect, /*0.1f*/ 0.01f, /*10.0f*/ 1000.0f); //3 параметр: передняя плоскость отсечения, 4 - задняя
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SkyBoxShaderProgram.bind();
    SkyBoxShaderProgram.setUniformValue("u_projectionMatrix", ProjectionMatrix);

    m_camera->draw(&SkyBoxShaderProgram);

    m_skybox->draw(&SkyBoxShaderProgram, context()->functions());

    SkyBoxShaderProgram.release();

    CharacterShaderProgram.bind();

    CharacterShaderProgram.setUniformValue("u_projectionMatrix", ProjectionMatrix);
    CharacterShaderProgram.setUniformValue("u_lightPosition", QVector4D(0.0, 0.0, 0.0, 1.0));
    CharacterShaderProgram.setUniformValue("u_lightPower", 1.0f);

    m_camera->draw(&CharacterShaderProgram);

//    FloorShaderProgram.bind();
//    FloorShaderProgram.setUniformValue("u_projectionMatrix", ProjectionMatrix);

//    m_camera->draw(&FloorShaderProgram);

    for (int i = 0; i < transformObjects.size(); ++i)
           transformObjects[i]->draw(&CharacterShaderProgram, context()->functions());

//    FloorShaderProgram.release();

    CharacterShaderProgram.release();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        MousePosition = QVector2D(event->position()); //координаты указателя относительно левого верхнего угла данного окна

    else if (event->button() == Qt::RightButton) {

        qDebug() << SelectObject(event->x(), event->y(), selectObjects);
    }

    event->accept();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
//    if (event->button() != Qt::LeftButton)
//        return;

    QVector2D diff = QVector2D(event->position()) - MousePosition;

    MousePosition = QVector2D(event->position()); //localPos()

    float angX = diff.y() / 2.0f,
          angY = diff.x() / 2.0f;

    m_camera->RotateX(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, angX));
    m_camera->RotateY(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, angY));

    update();
}

void Widget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        m_camera->Translate(QVector3D(0.0f, 0.0f, 0.25f));

    else if (event->angleDelta().y() < 0)
        m_camera->Translate(QVector3D(0.0f, 0.0f, -0.25f));

    update();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {

    case Qt::Key_Left:
        groups.first()->Translate(QVector3D(0.5f, 0.0f, 0.0f));
        break;

    case Qt::Key_Right:
        groups.first()->Translate(QVector3D(-0.5f, 0.0f, 0.0f));
        break;

    case Qt::Key_Up:
        groups.first()->Translate(QVector3D(0.0f, 0.0f, 0.5f));
        break;

    case Qt::Key_Down:
        groups.first()->Translate(QVector3D(0.0f, 0.0f, -0.5f));
    }

    update();
}

void Widget::initShaders()
{
   if (!CharacterShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vcharacter.vsh"))
       exit(-230);

   if (!CharacterShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fcharacter.fsh"))
       exit(-229);

   if (!CharacterShaderProgram.link())
       exit(-228);


   if (!SkyBoxShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vskybox.vsh"))
       exit(-2330);

   if (!SkyBoxShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fskybox.fsh"))
       exit(-2229);

   if (!SkyBoxShaderProgram.link())
       exit(-2228);



   if (!SelectShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vselect.vsh"))
       exit(-2013);

   if (!SelectShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fselect.fsh"))
       exit(-2019);

   if (!SelectShaderProgram.link())
       exit(-2018);


//   if (!FloorShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vfloor.vsh"))
//       exit(-34);

//   if (!FloorShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/ffloor.fsh"))
//       exit(-35);

//   if (!FloorShaderProgram.link())
//       exit(-38);
}

void Widget::initCube(float width, float height, float depth, QImage *diffuseMap, QImage *normalMap)
{
    float w_by2 = width / 2.0f;

    float h_by2 = height / 2.0f;

    float d_by2 = depth / 2.0f;

    QVector<VertexData> vertexes;

    vertexes.append(VertexData(QVector3D(-w_by2, h_by2, d_by2), QVector2D(0.0, 1.0), QVector3D(0.0, 0.0, 1.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, -h_by2, d_by2), QVector2D(0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertexes.append(VertexData(QVector3D(w_by2, h_by2, d_by2), QVector2D(1.0, 1.0), QVector3D(0.0, 0.0, 1.0)));
    vertexes.append(VertexData(QVector3D(w_by2, -h_by2, d_by2), QVector2D(1.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

    vertexes.append(VertexData(QVector3D(w_by2, h_by2, d_by2), QVector2D(0.0, 1.0), QVector3D(1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(w_by2, -h_by2, d_by2), QVector2D(0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(w_by2, h_by2, -d_by2), QVector2D(1.0, 1.0), QVector3D(1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(w_by2, -h_by2, -d_by2), QVector2D(1.0, 0.0), QVector3D(1.0, 0.0, 0.0)));

    vertexes.append(VertexData(QVector3D(w_by2, h_by2, d_by2), QVector2D(0.0, 1.0), QVector3D(0.0, 1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(w_by2, h_by2, -d_by2), QVector2D(0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, h_by2, d_by2), QVector2D(1.0, 1.0), QVector3D(0.0, 1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, h_by2, -d_by2), QVector2D(1.0, 0.0), QVector3D(0.0, 1.0, 0.0)));

    vertexes.append(VertexData(QVector3D(w_by2, h_by2, -d_by2), QVector2D(0.0, 1.0), QVector3D(0.0, 0.0, -1.0)));
    vertexes.append(VertexData(QVector3D(w_by2, -h_by2, -d_by2), QVector2D(0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, h_by2, -d_by2), QVector2D(1.0, 1.0), QVector3D(0.0, 0.0, -1.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, -h_by2, -d_by2), QVector2D(1.0, 0.0), QVector3D(0.0, 0.0, -1.0)));

    vertexes.append(VertexData(QVector3D(-w_by2, h_by2, d_by2), QVector2D(0.0, 1.0), QVector3D(-1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, h_by2, -d_by2), QVector2D(0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, -h_by2, d_by2), QVector2D(1.0, 1.0), QVector3D(-1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, -h_by2, -d_by2), QVector2D(1.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));

    vertexes.append(VertexData(QVector3D(-w_by2, -h_by2, d_by2), QVector2D(0.0, 1.0), QVector3D(0.0, -1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-w_by2, -h_by2, -d_by2), QVector2D(0.0, 0.0), QVector3D(0.0, -1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(w_by2, -h_by2, d_by2), QVector2D(1.0, 1.0), QVector3D(0.0, -1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(w_by2, -h_by2, -d_by2), QVector2D(1.0, 0.0), QVector3D(0.0, -1.0, 0.0)));

    QVector<GLuint> indexes;

    for (short i = 0; i < 24; i += 4) {

        indexes.append(i + 0);
        indexes.append(i + 1);
        indexes.append(i + 2);
        indexes.append(i + 2);
        indexes.append(i + 1);
        indexes.append(i + 3);
    }

    Material* mtl = new Material;


    if (diffuseMap)
        mtl->setDiffuseMap(*diffuseMap);
    else
        mtl->setDiffuseMap(":/water.jpg");

    if (normalMap)
        mtl->setNormalMap(*normalMap);
    else
        mtl->setNormalMap(":/ice1_n.jpg");

    mtl->setShinnes(96.0);
    mtl->setDiffuseColor(QVector3D(1.0, 1.0, 1.0));
    mtl->setSpecularColor(QVector3D(1.0, 1.0, 1.0));
    mtl->setAmbienceColor(QVector3D(1.0, 1.0, 1.0));


    Object3D *newObj = new Object3D;

    newObj->calculateTBN(vertexes);

    newObj->addObject(new SimpleObject(vertexes, indexes, mtl));

    objects.append(newObj);
}

int Widget::SelectObject(int x, int y, QVector<Transformational *> &objs)
{
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); //для корректной работы оси z: дальние объекты не должны перекрывать ближние

    SelectShaderProgram.bind();
    SelectShaderProgram.setUniformValue("u_projectionMatrix", ProjectionMatrix);;
    m_camera->draw(&SelectShaderProgram);

    for (qsizetype i = 0; i < objs.size(); ++i) {

        SelectShaderProgram.setUniformValue("u_code", float(i + 1)); //i + 1 чтоб не совпадал с цветом фона (чёрный)
        objs[i]->draw(&SelectShaderProgram, context()->functions(), false);
    }

    SelectShaderProgram.release();

    GLint viewport[4]; //x, y, w, h
    glGetIntegerv(GL_VIEWPORT, viewport);

    unsigned char res[4]; //4 компоненты RGBA, каждый по байту, [0-255], поэтому такой тип данных

    //1, 1 - ширина и высота пикселя который нужно считать под указателем мыши
    glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);

    glDisable(GL_DEPTH_TEST);

    return res[0]; //красная компонента
}
