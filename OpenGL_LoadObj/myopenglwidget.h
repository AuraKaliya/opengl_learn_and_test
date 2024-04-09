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

QQuaternion rotationQuat;

QMatrix4x4 projection;
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
    //弃用
    //QVector3D modelNormalVector;
    //QVector3D modelRightVector;
    // ////


    QQuaternion modelRotation;
    QString name;

    QVector<double> posList;
    QVector<double> normalVectorList;
    QVector<int> indexList;
    QVector<int > normalIndexList;
    QVector<double >dataList;
    QVector<MySpaceText*> textList;
};

//==========================================
class MyOpenGLWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit MyOpenGLWidget(QWidget *parent = nullptr);
    void read();
    //void check();
    void check(MyOpenGLObject*obj);
    void check(MySpaceText* text);

    void initObject( MyOpenGLObject* obj );
    void reInitObject( MyOpenGLObject* obj );
    double posXFromOpenGLToQt(double posX);
    double posYFromOpenGLToQt(double posY);

    double posXFromQtToOpenGL(double posX);
    double posYFromQtToOpenGL(double posY);

    void initTextByObject(MyOpenGLObject* obj);
    void initSetting();
    void initObjectDictionary();
    void initDirectionObject();
    void initLightObject();
    void initRayObject();

    void updateRay();

    Eigen::Vector3d getNormalVector(MyOpenGLObject*obj,int point1,int point2,int point3);
    void testNormalVector(QVector3D point1,QVector3D point2,QVector3D point3);


    void test();
    bool intersectTriangle(const QVector3D & origPos, const QVector3D & rayDir, QVector3D v0, QVector3D v1, QVector3D v2, double * t, double *u, double *v);
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
    QStringList m_modelList;

    QOpenGLShaderProgram* m_shaderProgram;
    QOpenGLShaderProgram* m_directionShaderProgram;
    QOpenGLShaderProgram* m_lightShaderProgram;
    QOpenGLShaderProgram* m_rayShaderProgram;

    QMap <int,MySpaceText*> m_textDictionary;
    int m_textIdCount;

    QMap<QString,MyOpenGLObject*> m_objDictionary;
    QMap<QString,MyOpenGLObject*> m_modelDictionary;

    MyOpenGLObject* m_nowObject;


    Camera m_camera;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;
    QMatrix4x4 m_directionPosView;
    QMatrix4x4 m_sun;
    QVector3D  m_lightPos;
    QVector3D  m_viewPos;

    QQuaternion m_viewRotation;
    QMatrix4x4 m_viewTranslateMatrix;
    QMatrix4x4 m_viewRotateMatrix;
    QMatrix4x4 m_viewScaleMatrix;

    QVector3D m_xColor;
    QVector3D m_yColor;
    QVector3D m_zColor;


    QVector<double> m_posList;
    QVector<int> m_indexList;

    QPoint m_startPoint;
    QPoint m_endPoint;

    QVector3D m_upVector;
    QVector3D m_frontVector;
    QVector3D m_rightVector;

    bool m_shift;
    bool m_alt;
    bool m_textShow;
    bool m_rayFlag;


    bool m_testFlag;
};

#endif // MYOPENGLWIDGET_H
