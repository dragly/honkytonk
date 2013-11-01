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
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_ALWAYS);
//    glDepthMask(GL_TRUE);
//    glEnable(GL_FOG);
//    glFogi(GL_FOG_COORDINATE_SOURCE, GL_FOG_COORDINATE);
    //    if(m_sortPoints == BackToFront) {
    //        QMultiMap<double, QVector3D> sortedPoints;
    //        int startPoint = std::max(0, m_currentSampleStep - m_nVisiblePoints);
    //        for(int i = startPoint;
    //            i < m_currentSampleStep && i < m_points.length();
    //            i++) {
    //            const QVector3D &center = m_points.at(i);
    //            const QVector4D &depthVector = painter->modelViewMatrix() * center;
    //            double depth = depthVector.z();
    //            sortedPoints.insert(depth, center);
    //        }
    //        //        m_points.clear();
    //        int iReal = startPoint;
    //        QMapIterator<double, QVector3D> i(sortedPoints);
    //        while(i.hasNext()) {
    //            m_points.removeAt(iReal);
    //            m_points.insert(iReal-1, i.next().value());
    //            iReal++;
    //        }
    //        sortedPoints.clear();
    //    }

    if(!m_geometry || m_faceCamera || m_needsRebuildGeometry) {
        // Build the mesh
        QGLBuilder builder;
        builder.newSection(QGL::NoSmoothing);
        const QMatrix4x4 &modelViewMatrix = painter->modelViewMatrix();
        QVector3D right;
        right.setX(modelViewMatrix(0,0));
        right.setY(modelViewMatrix(0,1));
        right.setZ(modelViewMatrix(0,2));
        QVector3D up;
        up.setX(modelViewMatrix(1,0));
        up.setY(modelViewMatrix(1,1));
        up.setZ(modelViewMatrix(1,2));
        QGeometryData quad;
        QVector3D a;
        QVector3D b;
        QVector3D c;
        QVector3D d;
        QVector2D ta(0,0);
        QVector2D tb(0,1);
        QVector2D tc(1,1);
        QVector2D td(1,0);
        QVector3D center;
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
                if(painter->isCullable(center)) {
                    continue;
                }
                double size = m_size * (i - (m_currentSampleStep - m_nVisiblePoints * m_sampleStep)) / (m_nVisiblePoints * m_sampleStep);
                a = center - right * size * 0.5 - up * size * 0.5;
                b = center + right * size * 0.5 - up * size * 0.5;
                c = center + right * size * 0.5 + up * size * 0.5;
                d = center - right * size * 0.5 + up * size * 0.5;
                quad.appendVertex(a,b,c,d);
                quad.appendTexCoord(ta, tb, tc, td);
            }
        }
        //    }
        builder.addQuads(quad);
        QGLSceneNode* geometry = builder.finalizedSceneNode();
        if(m_geometry) {
            delete m_geometry;
        }
        m_geometry = geometry;
        m_needsRebuildGeometry = false;
    }
    m_geometry->draw(painter);
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
