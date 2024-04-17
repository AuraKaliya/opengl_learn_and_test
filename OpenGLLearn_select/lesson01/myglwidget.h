#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

//#include <QGLWidget>
#include <QOpenGLWidget>
#include <QKeyEvent>

class MyGLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit MyGLWidget(QWidget *parent = 0);

	~MyGLWidget();

protected:

	virtual void paintGL();
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void keyPressEvent(QKeyEvent *event);

private:

	bool mFullScreen;
};

#endif // MYGLWIDGET_H
