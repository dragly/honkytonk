#include "multibillboard.h"

#include <QGLBuilder>
#include <QQuickEffect>
#include <qmath.h>
#include <positionreader.h>

#include <armadillo>

//using namespace arma;

MultiBillboard::MultiBillboard(QQuickItem *parent) :
    QQuickItem3D(parent),
    m_sortPoints(DefaultSorting),
    m_faceCamera(true),
    m_needsRebuildGeometry(true),
    m_nVisiblePoints(1e4),
    m_currentSampleStep(2),
    m_size(1),
    m_sampleStep(1),
    m_useAlphaTest(true),
    m_positionReader(0)
{
}

void MultiBillboard::drawItem(QGLPainter *painter) {
    if(!drawMutex.tryLock()) {
        qWarning() << "MultiBillboard::drawItem(): Called before finishing previous draw call.";
        return;
    }
    if(!m_positionReader) {
        qWarning() << "No position reader set. Cannot draw...";
        return;
    }
#ifndef Q_OS_ANDROID
    if(m_sortPoints != BackToFront && m_useAlphaTest) {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1);
    }
#endif

    QGLVertexBundle vertexBundle;
    QGLIndexBuffer indexBuffer;

    int numberOfPoints = m_nVisiblePoints * m_positionReader->positions().n_cols;

    vertices.clear();
//    normals.clear();
    texCoords.clear();
    indexes.clear();

    vertices.reserve(4*numberOfPoints);
//    normals.reserve(4*m_points.length());
    texCoords.reserve(4*numberOfPoints);
    indexes.reserve(6*numberOfPoints);

//    if(m_sortPoints == BackToFront) {
//        QMultiMap<double, QVector3D> sortedPoints;
//        for(int i = 0; i < m_points.length(); i++) {
//            const QVector3D &center = m_points.at(i);
//            const QVector4D &depthVector = painter->modelViewMatrix() * center;
//            double depth = depthVector.z();
//            sortedPoints.insert(depth, center);
//        }
//        m_points.clear();
//        QMapIterator<double, QVector3D> i(sortedPoints);
//        while(i.hasNext()) {
//            m_points.push_back(i.next().value());
//        }
//        sortedPoints.clear();
//    }

    const QMatrix4x4 &modelViewMatrix = painter->modelViewMatrix();
    QVector3D right;
    right.setX(modelViewMatrix(0,0));
    right.setY(modelViewMatrix(0,1));
    right.setZ(modelViewMatrix(0,2));
    QVector3D up;
    up.setX(modelViewMatrix(1,0));
    up.setY(modelViewMatrix(1,1));
    up.setZ(modelViewMatrix(1,2));

    QVector3D a;
    QVector3D b;
    QVector3D c;
    QVector3D d;
    QVector3D aOffset =  - right * 0.5 - up * 0.5;
    QVector3D bOffset =  right * 0.5 - up * 0.5;
    QVector3D cOffset =  right * 0.5 + up * 0.5;
    QVector3D dOffset =  - right * 0.5 + up * 0.5;
    QVector2D ta(0,0);
    QVector2D tb(0,1);
    QVector2D tc(1,1);
    QVector2D td(1,0);
    QVector3D center;
    QVector3D normal(QVector3D::crossProduct(right, up));
    int count = 0;
    int startPoint = std::max(0, m_currentSampleStep - m_nVisiblePoints * m_sampleStep);
    const cube& positions = m_positionReader->positions();
    for(int i = startPoint;
        i < m_currentSampleStep && i < positions.n_rows;
        i+= m_sampleStep) {
        for(int j = 0; j < positions.n_cols; j++) {
//                const QVector3D &center = m_points.at(i);
            center.setX(positions(i,j,0));
            center.setY(positions(i,j,1));
            center.setZ(positions(i,j,2));
//            const QVector3D &center = m_points.at(i);
    //        if(painter->isCullable(center)) {
    //            continue;
    //        }
            double size = m_size * (i - (m_currentSampleStep - m_nVisiblePoints * m_sampleStep)) / (m_nVisiblePoints * m_sampleStep);
            a = center + size * aOffset;
            b = center + size * bOffset;
            c = center + size * cOffset;
            d = center + size * dOffset;
            vertices.append(a, b, c, d);
            texCoords.append(ta, tb, tc, td);
    //        normals.append(normal, normal, normal, normal);
            indexes.append(count*4 + 0, count*4 + 1, count*4 + 2);
            indexes.append(count*4 + 2, count*4 + 3, count*4 + 0);
            count++;
        }
    }
    vertexBundle.addAttribute(QGL::Position, vertices);
    vertexBundle.addAttribute(QGL::TextureCoord0, texCoords);
//    vertexBundle.addAttribute(QGL::Normal, normals);
    indexBuffer.setIndexes(indexes);

    painter->clearAttributes();
    // Set up normal attributes to use only one element
    painter->glDisableVertexAttribArray(GLuint(QGL::Normal));
    painter->glVertexAttrib3f(GLuint(QGL::Normal), normal.x(), normal.y(), normal.z());

    // Set the rest of the vertex bundle (basically only positions)
    painter->setVertexBundle(vertexBundle);
    painter->draw(QGL::DrawingMode(QGL::Triangles), indexBuffer, 0, indexBuffer.indexCount());
#ifndef Q_OS_ANDROID
    if(m_sortPoints != BackToFront && m_useAlphaTest) {
        glDisable(GL_ALPHA_TEST);
    }
#endif
    drawMutex.unlock();
    glDisable(GL_DEPTH_TEST);
}

void MultiBillboard::setPositionReader(PositionReader *arg)
{
    if (m_positionReader != arg) {
        if(m_positionReader) {
            disconnect(m_positionReader, SIGNAL(dataChanged()), this, SLOT(update()));
        }
        m_positionReader = arg;
        connect(m_positionReader, SIGNAL(dataChanged()), this, SLOT(update()));
        m_needsRebuildGeometry = true;
        update();
        emit positionReaderChanged(arg);
    }
}

MultiBillboard::~MultiBillboard()
{
}
