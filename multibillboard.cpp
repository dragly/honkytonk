#include "multibillboard.h"

#include <QGLBuilder>
#include <QQuickEffect>
#include <qmath.h>

#include <armadillo>

//using namespace arma;

MultiBillboard::MultiBillboard(QQuickItem *parent) :
    QQuickItem3D(parent),
    m_sortPoints(DefaultSorting),
    m_faceCamera(true),
    m_needsRebuildGeometry(true),
    m_nVisiblePoints(1e4),
    m_currentTimeStep(2)
{
    generateRandomPoints();
}

void MultiBillboard::generateRandomPoints() {
    m_points.clear();
    double spacing = 1;
    int nPerDim = 20;
    double frequency = 0.1;
    for(int i = 0; i < nPerDim; i++) {
        for(int j = 0; j < nPerDim; j++) {
            for(int k = 0; k < nPerDim; k++) {
                QVector3D center(-nPerDim/2 + i * spacing,-nPerDim/2 + j,-nPerDim/2 + k);
                QVector3D center2 = center;
                center.setX(center.x() + cos(2 * frequency * center2.z()));
                center.setZ(center.z() + sin(2 * frequency * center2.y()));
                m_points.push_back(center);
            }
        }
    }
    m_needsRebuildGeometry = true;
}

void MultiBillboard::drawItem(QGLPainter *painter) {
#ifndef Q_OS_ANDROID
    if(m_sortPoints != BackToFront) {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1);
    }
#endif
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

    if(m_sortPoints == BackToFront) {
        QMultiMap<double, QVector3D> sortedPoints;
        int startPoint = std::max(0, m_currentTimeStep - m_nVisiblePoints);
        for(int i = startPoint;
            i < m_currentTimeStep && i < m_points.length();
            i++) {
            const QVector3D &center = m_points.at(i);
            const QVector4D &depthVector = painter->modelViewMatrix() * center;
            double depth = depthVector.z();
            sortedPoints.insert(depth, center);
        }
//        m_points.clear();
        int iReal = startPoint;
        QMapIterator<double, QVector3D> i(sortedPoints);
        while(i.hasNext()) {
            m_points.removeAt(iReal);
            m_points.insert(iReal-1, i.next().value());
            iReal++;
        }
        sortedPoints.clear();
    }

    if(!m_geometry || m_faceCamera || m_needsRebuildGeometry) {
        QVector3D a;
        QVector3D b;
        QVector3D c;
        QVector3D d;
        QVector2D ta(0,0);
        QVector2D tb(0,1);
        QVector2D tc(1,1);
        QVector2D td(1,0);
        int startPoint = std::max(0, m_currentTimeStep - m_nVisiblePoints);
        for(int i = startPoint;
            i < m_currentTimeStep && i < m_points.length();
            i++) {
            const QVector3D &center = m_points.at(i);
            if(painter->isCullable(center)) {
                continue;
            }
            double size = 0.2 * (i - startPoint) / m_nVisiblePoints;
            a = center - right * size * 0.5 - up * size * 0.5;
            b = center + right * size * 0.5 - up * size * 0.5;
            c = center + right * size * 0.5 + up * size * 0.5;
            d = center - right * size * 0.5 + up * size * 0.5;
            quad.appendVertex(a,b,c,d);
            quad.appendTexCoord(ta, tb, tc, td);
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
    if(m_sortPoints != BackToFront) {
        glDisable(GL_ALPHA_TEST);
    }
#endif
}

void MultiBillboard::loadPointsFromFile()
{
    arma::mat filePositions;
    qDebug() << "Loading points from file";
    if(m_fileName == "") {
        m_points.clear();
        return;
    }
    if(!filePositions.load(m_fileName.toStdString())) {
        qDebug() << "Could not load file " << m_fileName;
        return;
    }
    qDebug() << "Loaded " << filePositions.n_rows << "x" << filePositions.n_cols << " rows x cols";
    m_points.clear();
    for(uint i = 0; i < filePositions.n_rows; i++) {
        QVector3D center(filePositions(i,0), filePositions(i,1), filePositions(i,2));
        m_points.push_back(center);
    }
    m_needsRebuildGeometry = true;
}

MultiBillboard::~MultiBillboard()
{
}
