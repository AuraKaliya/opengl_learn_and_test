#ifndef TESTOPENGLWIDGET_H
#define TESTOPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QVector>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <QTimer>
#include <QMap>

#include <GL/gl.h>
#include <GL/glu.h>

class TestOpenGLWidget;
using InitFunc = bool(*)(TestOpenGLWidget*,int);
class TestOpenGLWidget : public QOpenGLWidget,QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    explicit TestOpenGLWidget(QWidget *parent = nullptr);
    ~TestOpenGLWidget();
    void initColor(int i);
    void setColor(QString colorName);
    QVector<GLfloat> getColum(double r,double h);
protected:

    void initializeGL()override;
    void resizeGL(int w, int h)override;
    void paintGL()override;


protected:
    unsigned int m_mainVAO;
    QVector<unsigned int> m_VAOList;
    QVector<QString > m_dataDescriptionList;
    QVector<unsigned int> m_VBOList;


    //QVector<QOpenGLVertexArrayObject*> m_VAOList;
    //QVector<QOpenGLBuffer*>  m_VBOList;


    static QMap<QString,InitFunc> m_initFuncDictionary;
    static void initClass();
    static bool s_initClassFlag;

    static bool initGrid(TestOpenGLWidget*me, int index);
    static bool initArrow(TestOpenGLWidget*me, int index);
    static bool initModel(TestOpenGLWidget*me, int index);


    QVector< QVector<GLfloat> > m_dataList;
    int m_dataOffset;
    QOpenGLShaderProgram* m_shaderProgram;
    bool m_errorFlag;
    QMatrix4x4 m_view;

    QVector3D m_cameraPos;
    QVector3D m_cameraFromt;
    QVector3D m_cameraUp;
    QVector3D m_cameraRight;
    QVector3D m_cameraTarget;
    QVector3D m_cameraDirection;

    QVector3D m_up;



    int m_gridXUniform;

    void wheelEvent( QWheelEvent *e ) override;
    void mouseMoveEvent( QMouseEvent *e ) override;
    void mousePressEvent( QMouseEvent *e ) override;
    void mouseReleaseEvent( QMouseEvent *e ) override;


    QPoint m_startPoint;
    QPoint m_endPoint;

signals:

};

#endif // TESTOPENGLWIDGET_H
