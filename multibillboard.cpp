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
    m_currentSampleStep(2),
    m_nSampleSteps(10),
    m_size(1),
    m_sampleStep(1),
    m_useAlphaTest(true)
{
    generateRandomPoints();
}

void MultiBillboard::generateRandomPoints() {
    m_filePositions.reset();
    m_filePositions = arma::zeros(1,20*20*20,3);
    double spacing = 1;
    int nPerDim = 20;
    int counter = 0;
    for(int i = 0; i < nPerDim; i++) {
        for(int j = 0; j < nPerDim; j++) {
            for(int k = 0; k < nPerDim; k++) {
                m_filePositions(0,counter,0) = i;
                m_filePositions(0,counter,1) = j;
                m_filePositions(0,counter,2) = k;
                counter++;
            }
        }
    }
    m_nSampleSteps = 1;
    emit sampleStepChanged(m_nSampleSteps);
    m_needsRebuildGeometry = true;
}

void MultiBillboard::drawItem(QGLPainter *painter) {
#ifndef Q_OS_ANDROID
    if(m_sortPoints != BackToFront && m_useAlphaTest) {
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
        for(int i = startPoint;
            i < m_currentSampleStep && i < m_filePositions.n_rows;
            i+= m_sampleStep) {
            for(int j = 0; j < m_filePositions.n_cols; j++) {
//                const QVector3D &center = m_points.at(i);
                center.setX(m_filePositions(i,j,0));
                center.setY(m_filePositions(i,j,1));
                center.setZ(m_filePositions(i,j,2));
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
}

void MultiBillboard::loadPointsFromFile()
{
    qDebug() << "Loading points from file";
    if(m_fileName == "") {
        m_filePositions.reset();
        return;
    }
    if(!m_filePositions.load(m_fileName.toStdString())) {
        qDebug() << "Could not load file " << m_fileName;
        return;
    }
    qDebug() << "Loaded " << m_filePositions.n_rows << "x" << m_filePositions.n_cols << "x" << m_filePositions.n_slices << " rows x cols x slices";
    //    m_points.clear();
    //    for(uint k = 0; k < filePositions.n_slices; k++) {
    //        for(uint i = 0; i < filePositions.n_rows; i++) {
    //            QVector3D center(filePositions(i,0,k), filePositions(i,1,k), filePositions(i,2,k));
    //            m_points.push_back(center);
    //        }
    //    }
    m_needsRebuildGeometry = true;
    m_nSampleSteps = m_filePositions.n_rows;
    emit nSampleStepsChanged(m_nSampleSteps);
}

MultiBillboard::~MultiBillboard()
{
}
