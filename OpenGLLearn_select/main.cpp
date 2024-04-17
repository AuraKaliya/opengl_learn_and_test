#include "mainwindow.h"

#include <QApplication>
//#include <lesson02/myglwidget.h>
#include <myopenglwidget.h>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();


    MyOpenGLWidget w;
    w.show();

    return a.exec();
}
