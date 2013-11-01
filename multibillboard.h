#ifndef MULTIBILLBOARD_H
#define MULTIBILLBOARD_H

#include <QQuickItem3D>
#include <QGLAbstractScene>
#include <QElapsedTimer>
#include <armadillo>
#include <QMutex>

using namespace arma;

class PositionReader;

class MultiBillboard : public QQuickItem3D
{
    Q_OBJECT
    Q_PROPERTY(SortMode sortPoints READ sortPoints WRITE setSortPoints NOTIFY sortPointsChanged)
    Q_PROPERTY(bool faceCamera READ faceCamera WRITE setFaceCamera NOTIFY faceCameraChanged)
    Q_PROPERTY(int nVisibleSampleSteps READ nVisibleSampleSteps WRITE setNVisibleSampleSteps NOTIFY nVisibleSampleStepsChanged)
    Q_PROPERTY(int currentSampleStep READ currentSampleStep WRITE setCurrentSampleStep NOTIFY currentSampleStepChanged)
    Q_PROPERTY(bool useAlphaTest READ useAlphaTest WRITE setUseAlphaTest NOTIFY useAlphaTestChanged)
    Q_PROPERTY(double size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int sampleStep READ sampleStep WRITE setSampleStep NOTIFY sampleStepChanged)
    Q_PROPERTY(PositionReader* positionReader READ positionReader WRITE setPositionReader NOTIFY positionReaderChanged)

public:
    explicit MultiBillboard(QQuickItem *parent = 0);
    ~MultiBillboard();

    SortMode sortPoints() const
    {
        return m_sortPoints;
    }

    bool faceCamera() const
    {
        return m_faceCamera;
    }

    int nVisibleSampleSteps() const
    {
        return m_nVisiblePoints;
    }

    int currentSampleStep() const
    {
        return m_currentSampleStep;
    }

    bool useAlphaTest() const
    {
        return m_useAlphaTest;
    }

    double size() const
    {
        return m_size;
    }

    int sampleStep() const
    {
        return m_sampleStep;
    }

    PositionReader* positionReader() const
    {
        return m_positionReader;
    }

protected:
    void drawItem(QGLPainter *painter);
signals:

    void spacingChanged(double arg);

    void frequencyChanged(double arg);

    void sortPointsChanged(SortMode arg);

    void faceCameraChanged(bool arg);

    void nVisibleSampleStepsChanged(int arg);

    void currentSampleStepChanged(int arg);

    void useAlphaTestChanged(bool arg);

    void sizeChanged(double arg);

    void sampleStepChanged(int arg);

    void positionReaderChanged(PositionReader* arg);

public slots:

    void setSortPoints(SortMode arg)
    {
        if (m_sortPoints != arg) {
            m_sortPoints = arg;
            update();
            emit sortPointsChanged(arg);
        }
    }

    void setFaceCamera(bool arg)
    {
        if (m_faceCamera != arg) {
            m_faceCamera = arg;
            update();
            emit faceCameraChanged(arg);
        }
    }

    void setNVisibleSampleSteps(int arg)
    {
        if (m_nVisiblePoints != arg) {
            m_nVisiblePoints = arg;
            update();
            emit nVisibleSampleStepsChanged(arg);
        }
    }

    void setCurrentSampleStep(int arg)
    {
        if (m_currentSampleStep != arg) {
            m_currentSampleStep = arg;
            update();
            emit currentSampleStepChanged(arg);
        }
    }

    void setUseAlphaTest(bool arg)
    {
        if (m_useAlphaTest != arg) {
            m_useAlphaTest = arg;
            update();
            emit useAlphaTestChanged(arg);
        }
    }

    void setSize(double arg)
    {
        if (m_size != arg) {
            m_size = arg;
            update();
            emit sizeChanged(arg);
        }
    }

    void setSampleStep(int arg)
    {
        if(arg < 1) {
            arg = 1;
        }
        if (m_sampleStep != arg) {
            m_sampleStep = arg;
            update();
            emit sampleStepChanged(arg);
        }
    }

    void setPositionReader(PositionReader* arg);

private:
    QGLSceneNode *m_topNode;
    bool m_sceneSet;
    QGLSceneNode* m_geometry;
    QGLAbstractScene *scene;

//    QList<QVector3D> m_points;
    SortMode m_sortPoints;

    bool m_faceCamera;
    bool m_needsRebuildGeometry;
    int m_nVisiblePoints;
    int m_currentSampleStep;
    bool m_useAlphaTest;
    double m_size;
    int m_sampleStep;

    QMutex drawMutex;
    PositionReader* m_positionReader;
};

#endif // MULTIBILLBOARD_H
