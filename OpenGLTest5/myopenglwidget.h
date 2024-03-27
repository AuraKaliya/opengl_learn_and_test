#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QOpenGLTexture>

class MyOpenGLWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    enum Shap{None,Rect,Circle,Triangle};   //用策略模式优化
    explicit MyOpenGLWidget(QWidget *parent = nullptr);
    void draw(Shap shap);
public slots:
    void timeoutFunc();
protected:
    void initializeGL()override;
    void resizeGL(int w, int h)override;
    void paintGL()override;
    //void keyPressEvent(QKeyEvent* e)override;
signals:
private:
    Shap m_shap=None;
    QOpenGLShaderProgram* m_shaderProgram;
    QTimer * m_timer;
    QOpenGLTexture * m_texturArgyle;
    QOpenGLTexture * m_texturOIP;
    float m_colors[4]={0.0f,0.0f,0.0f,1.0f};
    QMatrix4x4 m_matrix;
    int m_transNum;
    QMatrix4x4 m_projection;
};

#endif // MYOPENGLWIDGET_H
