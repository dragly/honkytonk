#include "positionreader.h"

#include <QDebug>
#include <cmath>

using namespace std;

PositionReader::PositionReader(QQuickItem *parent) :
    QQuickItem(parent),
    m_nSampleSteps(0),
    m_voxelData(0),
    m_voxelEdgeMin(0),
    m_voxelEdgeMax(0)
{
    generateRandomPoints();
}

PositionReader::~PositionReader()
{

}

void PositionReader::generateRandomPoints() {
    m_filePositions.reset();
    m_filePositions = arma::zeros(1,20*20*20,3);
    double spacing = 1;
    int nPerDim = 20;
    int counter = 0;
    for(int i = 0; i < nPerDim; i++) {
        for(int j = 0; j < nPerDim; j++) {
            for(int k = 0; k < nPerDim; k++) {
                m_filePositions(0,counter,0) = -nPerDim / 2 + i;
                m_filePositions(0,counter,1) = -nPerDim / 2 + j;
                m_filePositions(0,counter,2) = -nPerDim / 2 + k;
                counter++;
            }
        }
    }
    m_nSampleSteps = 1;
    emit nSampleStepsChanged(m_nSampleSteps);
    emit dataChanged();
}

int PositionReader::voxelDataWidth()
{
    return m_densityVoxels.n_rows;
}

int PositionReader::voxelDataHeight()
{
    return m_densityVoxels.n_cols;
}

int PositionReader::voxelDataDepth()
{
    return m_densityVoxels.n_slices;
}

const cube &PositionReader::positions() const
{
    return m_filePositions;
}


void PositionReader::loadPointsFromFile()
{
    qDebug() << "Loading points from file";
    if(m_fileName == "") {
        return;
    }
    cube filePositions;
    if(!filePositions.load(m_fileName.toStdString(), arma::raw_binary)) {
        qDebug() << "Could not load file " << m_fileName;
        return;
    }
//    filePositions.reshape(filePositions.n_rows / 6, 2, 3);

    cube realFilePositions = zeros(filePositions.n_rows / 6, 2, 3);
    for(int i = 0; i < filePositions.n_rows / 6; i++) {
        for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 3; k++) {
                realFilePositions(i,j,k) = filePositions(i * 2 * 3 + j * 3 + k, 0, 0);
            }
        }
    }
    filePositions.reset();

    m_filePositions = realFilePositions;
    realFilePositions.reset();
    qDebug() << "Loaded " << m_filePositions.n_rows << "x" << m_filePositions.n_cols << "x" << m_filePositions.n_slices << " rows x cols x slices";
    //    m_points.clear();
    //    for(uint k = 0; k < filePositions.n_slices; k++) {
    //        for(uint i = 0; i < filePositions.n_rows; i++) {
    //            QVector3D center(filePositions(i,0,k), filePositions(i,1,k), filePositions(i,2,k));
    //            m_points.push_back(center);
    //        }
    //    }

    m_densityVoxels = zeros(100,100,100);
    double minValue = m_filePositions.min() - 1e-6;
    double maxValue = m_filePositions.max() + 1e-6;
    double mostMaxValue = max(-minValue, maxValue);
    minValue = -mostMaxValue;
    maxValue = mostMaxValue;

    for(uint i = 0; i < m_filePositions.n_rows; i++) {
        for(uint j = 0; j < m_filePositions.n_cols; j++) {
            int xIndex = m_densityVoxels.n_rows * (m_filePositions(i,j,0) - minValue) / (maxValue - minValue);
            int yIndex = m_densityVoxels.n_cols * (m_filePositions(i,j,1) - minValue) / (maxValue - minValue);
            int zIndex = m_densityVoxels.n_slices * (m_filePositions(i,j,2) - minValue) / (maxValue - minValue);
            m_densityVoxels(xIndex, yIndex, zIndex) += 1;
        }
    }
    m_densityVoxels /= m_densityVoxels.max();

    m_nSampleSteps = m_filePositions.n_rows;
    setupVoxelData();
    m_voxelEdgeMin = minValue;
    m_voxelEdgeMax = maxValue;
    emit voxelEdgeMinChanged(m_voxelEdgeMin);
    emit voxelEdgeMaxChanged(m_voxelEdgeMax);
    emit nSampleStepsChanged(m_nSampleSteps);
    emit dataChanged();
}

void PositionReader::setupVoxelData() {
    uint nElements = m_densityVoxels.n_rows * m_densityVoxels.n_cols * m_densityVoxels.n_slices;
    if(m_voxelData) {
        delete[] m_voxelData;
    }
    m_voxelData = new GLushort[nElements];
    for(int i = 0; i < m_densityVoxels.n_rows; i++) {
        for(int j = 0; j < m_densityVoxels.n_cols; j++) {
            for(int k = 0; k < m_densityVoxels.n_slices; k++) {
                int index = i
                        + j * m_densityVoxels.n_rows
                        + k * m_densityVoxels.n_cols * m_densityVoxels.n_rows;
                m_voxelData[index] = m_densityVoxels(i,j,k) * 65534;
            }
        }
    }
}

GLushort *PositionReader::voxelData() const
{
    return m_voxelData;
}


void PositionReader::setFileName(QString arg)
{
    if (m_fileName != arg) {
        m_fileName = arg;
        loadPointsFromFile();
        emit fileNameChanged(arg);
    }
}
