#include "simpleobject.h"

SimpleObject::SimpleObject() :
    IndexBuffer(QOpenGLBuffer::IndexBuffer),  m_diffuseMap(0), m_scale(1.0)
{

}

SimpleObject::SimpleObject(const QVector<VertexData> &data, const QVector<GLuint> &indexes, const QImage &texture)
    : IndexBuffer(QOpenGLBuffer::IndexBuffer),  m_diffuseMap(0), m_scale(1.0)
{
    Init(data, indexes, texture);
}

SimpleObject::SimpleObject(const QVector<VertexData> &data, const QVector<GLuint> &indexes, Material *material)
    : IndexBuffer(QOpenGLBuffer::IndexBuffer), m_diffuseMap(0), m_scale(1.0)
{
    Init(data, indexes, material);
}

SimpleObject::~SimpleObject()
{
    if (VertexBuffer.isCreated())
        VertexBuffer.destroy();

    if (IndexBuffer.isCreated())
        IndexBuffer.destroy();
}

void SimpleObject::Init(const QVector<VertexData> &data, const QVector<GLuint> &indexes, const QImage &texture)
{
    if (VertexBuffer.isCreated())
        VertexBuffer.destroy();

    if (IndexBuffer.isCreated())
        IndexBuffer.destroy();

    if (m_diffuseMap != 0)
        if (m_diffuseMap->isCreated()) {
            delete m_diffuseMap;
            m_diffuseMap = 0;
        }

    VertexBuffer.create();
    VertexBuffer.bind();
    VertexBuffer.allocate(data.constData(), data.size() * sizeof(VertexData));
    VertexBuffer.release();

    IndexBuffer.create();
    IndexBuffer.bind();
    IndexBuffer.allocate(indexes.constData(), indexes.size() * sizeof(GLuint));
    IndexBuffer.release();

    m_diffuseMap = new QOpenGLTexture(texture.mirrored());

    m_diffuseMap->setMinificationFilter(QOpenGLTexture::Nearest);

    m_diffuseMap->setMinificationFilter(QOpenGLTexture::Linear);

    m_diffuseMap->setWrapMode(QOpenGLTexture::Repeat);
}

void SimpleObject::Init(const QVector<VertexData> &data, const QVector<GLuint> &indexes, Material *material)
{
    if (VertexBuffer.isCreated())
        VertexBuffer.destroy();

    if (IndexBuffer.isCreated())
        IndexBuffer.destroy();

    if (m_diffuseMap != 0)
        if (m_diffuseMap->isCreated()) {
            delete m_diffuseMap;
            m_diffuseMap = 0;
        }

    VertexBuffer.create();
    VertexBuffer.bind();
    VertexBuffer.allocate(data.constData(), data.size() * sizeof(VertexData));
    VertexBuffer.release();

    IndexBuffer.create();
    IndexBuffer.bind();
    IndexBuffer.allocate(indexes.constData(), indexes.size() * sizeof(GLuint));
    IndexBuffer.release();

    this->material = material;


    if (material->isDiffuseMapSet()) {

        m_diffuseMap = new QOpenGLTexture(material->getDiffuseMap().mirrored());

        m_diffuseMap->setMinificationFilter(QOpenGLTexture::Nearest);

        m_diffuseMap->setMinificationFilter(QOpenGLTexture::Linear);

        m_diffuseMap->setWrapMode(QOpenGLTexture::Repeat);

    }

    if (material->isNormalMapSet()) {

        m_normalMap = new QOpenGLTexture(material->getNormalMap().mirrored());

        m_normalMap->setMinificationFilter(QOpenGLTexture::Nearest);

        m_normalMap->setMinificationFilter(QOpenGLTexture::Linear);

        m_normalMap->setWrapMode(QOpenGLTexture::Repeat);

    }
}


void SimpleObject::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions, bool usingTextures)
{
    if (!VertexBuffer.isCreated() || !IndexBuffer.isCreated())
        return;

    QMatrix4x4 ModelMatrix;
    ModelMatrix.setToIdentity();

    //порядок следующий преобразований важен!! Реальные преобразования матрицы (внутри программы) делаются снизу вверх.
    ModelMatrix.translate(m_translate);
    ModelMatrix.rotate(m_rotate);
    ModelMatrix.scale(m_scale);
    ModelMatrix = m_globalTransforms * ModelMatrix;


    if (usingTextures) {

        if (material->isDiffuseMapSet()) {

            m_diffuseMap->bind(0);
            program->setUniformValue("u_diffuseMap", 0);
        }

        if (material->isNormalMapSet()) {

            m_normalMap->bind(1); //номер слота, на который биндим
            program->setUniformValue("u_normalMap", 1);
        }
    }

    program->setUniformValue("u_modelMatrix", ModelMatrix);

    program->setUniformValue("u_materialPropery.diffuseColor", material->getDiffuseColor());
    program->setUniformValue("u_materialPropery.ambienceColor", material->AmbienceColor());
    program->setUniformValue("u_materialPropery.specularColor", material->SpecularColor());
    program->setUniformValue("u_materialPropery.shinnes", material->getShinnes());

    program->setUniformValue("u_isUsingDiffuseMap", material->isDiffuseMapSet());
    program->setUniformValue("u_isUsingNormalMap", material->isNormalMapSet());

    VertexBuffer.bind();
    IndexBuffer.bind();

    int VertLoc = program->attributeLocation("a_position");
    program->enableAttributeArray(VertLoc);
    program->setAttributeBuffer(VertLoc, GL_FLOAT, 0, 3, sizeof(VertexData));

    int texLoc = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texLoc);
    program->setAttributeBuffer(texLoc, GL_FLOAT, sizeof(QVector3D), 2, sizeof(VertexData));


    int offset = sizeof(QVector3D) + sizeof(QVector2D);

    int normalLoc = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLoc);
    program->setAttributeBuffer(normalLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int tangentLoc = program->attributeLocation("a_tangent");
    program->enableAttributeArray(tangentLoc);
    program->setAttributeBuffer(tangentLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int BiTangentLoc = program->attributeLocation("a_bitangent");
    program->enableAttributeArray(BiTangentLoc);
    program->setAttributeBuffer(BiTangentLoc, GL_FLOAT, offset, 3, sizeof(VertexData));


    functions->glDrawElements(GL_TRIANGLES, IndexBuffer.size(), GL_UNSIGNED_INT, 0);

    VertexBuffer.release();
    IndexBuffer.release();

    if (usingTextures) {

        if (material->isDiffuseMapSet())
            m_diffuseMap->release();

        if (material->isNormalMapSet())
            m_normalMap->release();
    }
}

void SimpleObject::Rotate(const QQuaternion &r)
{
    m_rotate = r * m_rotate;
}

void SimpleObject::Translate(const QVector3D &t)
{
    m_translate += t;
}

void SimpleObject::Scale(const float &s)
{
    m_scale *= s;
}

void SimpleObject::SetGlobalTransform(const QMatrix4x4 &q)
{
    m_globalTransforms = q; //должен рассчитываться родителем и передаваться в готовом виде
}
