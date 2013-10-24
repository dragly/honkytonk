#ifndef MULTIBILLBOARD_H
#define MULTIBILLBOARD_H

#include <QQuickItem3D>
#include <QGLAbstractScene>
#include <QElapsedTimer>

class MultiBillboard : public QQuickItem3D
{
    Q_OBJECT
    Q_PROPERTY(SortMode sortPoints READ sortPoints WRITE setSortPoints NOTIFY sortPointsChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(bool faceCamera READ faceCamera WRITE setFaceCamera NOTIFY faceCameraChanged)
    Q_PROPERTY(int nVisiblePoints READ nVisiblePoints WRITE setNVisiblePoints NOTIFY nVisiblePointsChanged)
    Q_PROPERTY(int currentTimeStep READ currentTimeStep WRITE setCurrentTimeStep NOTIFY currentTimeStepChanged)
    Q_PROPERTY(bool useAlphaTest READ useAlphaTest WRITE setUseAlphaTest NOTIFY useAlphaTestChanged)

public:
    explicit MultiBillboard(QQuickItem *parent = 0);
    ~MultiBillboard();

    SortMode sortPoints() const
    {
        return m_sortPoints;
    }

    void generateRandomPoints();
    QString fileName() const
    {
        return m_fileName;
    }

    bool faceCamera() const
    {
        return m_faceCamera;
    }

    int nVisiblePoints() const
    {
        return m_nVisiblePoints;
    }

    int currentTimeStep() const
    {
        return m_currentTimeStep;
    }

    bool useAlphaTest() const
    {
        return m_useAlphaTest;
    }

protected:
    void drawItem(QGLPainter *painter);
signals:

    void spacingChanged(double arg);

    void frequencyChanged(double arg);

    void sortPointsChanged(SortMode arg);

    void fileNameChanged(QString arg);

    void faceCameraChanged(bool arg);

    void nVisiblePointsChanged(int arg);

    void currentTimeStepChanged(int arg);

    void useAlphaTestChanged(bool arg);

public slots:

    void setSortPoints(SortMode arg)
    {
        if (m_sortPoints != arg) {
            m_sortPoints = arg;
            update();
            emit sortPointsChanged(arg);
        }
    }

    void setFileName(QString arg)
    {
        if (m_fileName != arg) {
            m_fileName = arg;
            loadPointsFromFile();
            update();
            emit fileNameChanged(arg);
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

    void setNVisiblePoints(int arg)
    {
        if (m_nVisiblePoints != arg) {
            m_nVisiblePoints = arg;
            update();
            emit nVisiblePointsChanged(arg);
        }
    }

    void setCurrentTimeStep(int arg)
    {
        if (m_currentTimeStep != arg) {
            m_currentTimeStep = arg;
            update();
            emit currentTimeStepChanged(arg);
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

private:
    QGLSceneNode *m_topNode;
    bool m_sceneSet;
    QGLSceneNode* m_geometry;
    QGLAbstractScene *scene;

    QList<QVector3D> m_points;
    SortMode m_sortPoints;
    QString m_fileName;

    void loadPointsFromFile();
    bool m_faceCamera;
    bool m_needsRebuildGeometry;
    int m_nVisiblePoints;
    int m_currentTimeStep;
    bool m_useAlphaTest;
};

#endif // MULTIBILLBOARD_H
