#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QOpenGLTexture>
#include <Eigen/Dense>
#include <qmath.h>
#include <QQuaternion>
#include <QDir>
//==========================================

using MySpaceText = struct MySpaceText;
using MyOpenGLObject = struct MyOpenGLObject;
using Camera = struct Camera;

struct Camera
{
QVector3D frontVector;
QVector3D upVector;
QVector3D rightVector;
QVector3D pos;

QMatrix4x4 viewMatrix;

QMatrix4x4 rotationMatrix;
QMatrix4x4 translateMatrix;
QMatrix4x4 scaleMatrix;

QQuaternion rotationQuat;
float zoom;
};

struct MySpaceText
{
    int id;
    QVector3D spacePoint;
    QVector3D viewPoint;
    QSize baseSize;
    int fontSize;
    double sizeScale;
    QMatrix4x4 rotationMatrix;
    QMatrix4x4 translateMatrix;
    QQuaternion rotationQuat;
    QString text;
};

struct MyOpenGLObject
{
    bool normalVectorFlag;
    bool objNormalFlag;
    bool initFlag;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    QMatrix4x4 model;
    QMatrix4x4 modelRotateMatrix;
    QMatrix4x4 modelTranslateMatrix;
    QQuaternion modelRotation;

    QString name;
    QVector<double> posList;
    QVector<double> normalVectorList;
    QVector<int > indexList;
    QVector<int > normalIndexList;
    QVector<double >dataList;
};

//==========================================
class MyOpenGLWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit MyOpenGLWidget(QWidget *parent = nullptr);
    void check(MyOpenGLObject*obj);
    void initObject( MyOpenGLObject* obj );
    void reInitObject( MyOpenGLObject* obj );
    double posXFromOpenGLToQt(double posX);
    double posYFromOpenGLToQt(double posY);

    double posXFromQtToOpenGL(double posX);
    double posYFromQtToOpenGL(double posY);

    void initSetting();
    void initShaderProgram();
    void initObjData();
    Eigen::Vector3d getNormalVector(MyOpenGLObject*obj,int point1,int point2,int point3);



    bool intersectTriangle(const QVector3D & origPos, const QVector3D & rayDir, QVector3D v0, QVector3D v1, QVector3D v2, double * t, double *u, double *v);
    bool checkSelectModel(const QVector3D & origPos, const QVector3D & rayDir,MyOpenGLObject*obj);
    bool rayTriangleIntersect(
        const Eigen::Vector3d &orig, const Eigen::Vector3d &dir,
        const Eigen::Vector3d &v0, const Eigen::Vector3d &v1, const Eigen::Vector3d &v2);
    QVector3D getRayDirection(double posX, double posY, const QMatrix4x4& rotationMatrix, const QMatrix4x4& translateMatrix, const QVector3D& cameraPos);
    QVector3D getIntersectionWithZPlane(const QVector3D& origin, const QVector3D& direction, double z);
    void testNormalVector(QVector3D point1,QVector3D point2,QVector3D point3);
    void test();
protected:
    void initializeGL()override;
    void resizeGL(int w, int h)override;
    void paintGL()override;
    void keyPressEvent(QKeyEvent* e)override;
    void keyReleaseEvent(QKeyEvent* e)override;

    void wheelEvent( QWheelEvent *e ) override;
    void mouseMoveEvent( QMouseEvent *e ) override;
    void mousePressEvent( QMouseEvent *e ) override;
    void mouseReleaseEvent( QMouseEvent *e ) override;
    void paintEvent(QPaintEvent *e)override;

signals:
private:
    QDir *m_modelDir;
    QOpenGLShaderProgram* m_shaderProgram;

    MyOpenGLObject* m_obj;
    Camera m_camera;
    QMatrix4x4 m_project;

    QVector3D m_lightPos;

    QPoint m_startPoint;
    QPoint m_endPoint;

    bool m_shift;
    bool m_alt;
};

#endif // MYOPENGLWIDGET_H
