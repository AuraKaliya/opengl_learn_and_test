#include "testopenglwidget.h"

#include <QWheelEvent>
//#include <GL/gl.h>
//#include <GL/glu.h>
QMap<QString,InitFunc> TestOpenGLWidget::m_initFuncDictionary={};
bool TestOpenGLWidget::s_initClassFlag=false;
TestOpenGLWidget::TestOpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    if(!s_initClassFlag)
    {
        initClass();
    }


    qDebug()<<"~";

    m_dataDescriptionList<<"Grid"<<"Arrow";//<<"Model";

    for(int i=0;i<m_dataDescriptionList.size();++i)
    {
        //QOpenGLVertexArrayObject * vao=new QOpenGLVertexArrayObject;


       // QOpenGLBuffer *vbo=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);


       // m_VAOList.append(vao);
       // m_VBOList.append(vbo);
         m_VAOList.append(* new unsigned int);
         m_VBOList.append(* new unsigned int);
    }

    QVector<GLfloat> gridPosList;
    for(int i=0;i<10;i++)
    {
        GLfloat f=((float)i)/10.0;
        //x
        gridPosList<<0.0f<<f<<0.0f   <<1.0f<<f<<0.0f;
        gridPosList<<f<<0.0f<<0.0f   <<f<<1.0f<<0.0f;
        // y
        gridPosList << 0.0f << f << 0.0f  << 0.0f << f << -1.0f;
        gridPosList << 0.0f << 0.0f << -f  << 0.0f << 1.0 << -f;
        // z
        gridPosList << f << 0.0f << 0.0f  << f << 0.0f << -1.0f;
        gridPosList << 0.0f << 0.0f << -f  << 1.0f << 0.0f << -f;

    }
    m_dataList.append(gridPosList);


    QVector<GLfloat> arrowPosList;
    arrowPosList<< 0.0f<<0.0f<<0.0f  <<1.0f<<0.0f<<0.0f;
    arrowPosList<< 0.0f<<0.0f<<0.0f  <<0.0f<<1.0f<<0.0f;
    arrowPosList<< 0.0f<<0.0f<<0.0f  <<0.0f<<0.0f<<-1.0f;

    arrowPosList<< 1.0f<<0.0f<<0.1f << 1.0f<<0.0f<<-0.1f << 1.1f<<0.0f<<0.0f;
    arrowPosList<< 1.0f<<0.1f<<0.0f << 1.0f<<-0.1f<<0.0f << 1.1f<<0.0f<<0.0f;

    arrowPosList<< 0.0f<<1.0f<<0.1f << 0.0f<<1.0f<<-0.1f << 0.0f<<1.1f<<0.0f;
    arrowPosList<< 0.1f<<1.0f<<0.0f << -0.1f<<1.0f<<-0.0f << 0.0f<<1.1f<<0.0f;

    arrowPosList<< 0.1f<<0.0f<<-1.0f << -0.1f<<0.0f<<-1.0f << 0.0f<<0.0f<<-1.1f;
    arrowPosList<< 0.0f<<0.1f<<-1.0f << -0.0f<<-0.1f<<-1.0f << 0.0f<<0.0f<<-1.1f;

    m_dataList.append(arrowPosList);





    m_shaderProgram=new QOpenGLShaderProgram;
    m_errorFlag=true;
    m_dataOffset=0;
    m_gridXUniform=0;


    m_cameraPos=QVector3D(0.0f,0.0f,3.0f);
    m_cameraFromt=QVector3D(0.0f,0.0f,-1.0f);
    m_cameraUp=QVector3D(0.0f,1.0f,0.0f);
    m_cameraTarget=QVector3D(0.0f,0.0f,0.0f);
    m_up=QVector3D(0.0f,1.0f,0.0f);
}

TestOpenGLWidget::~TestOpenGLWidget()
{

    glDeleteVertexArrays(m_dataDescriptionList.size(),m_VAOList.data());
    glDeleteBuffers     ( m_dataDescriptionList.size(),m_VBOList.data());

}

void TestOpenGLWidget::initColor(int i)
{
    //makeCurrent();
    float greenValue=0.11*i;
    float redValue=0.11*i;
    float blueValue=0.52;
    m_shaderProgram->setUniformValue("ourColor",redValue,greenValue,blueValue,1.0f);
    //doneCurrent();
    //update();
}

void TestOpenGLWidget::setColor(QString colorName)
{
    float greenValue=0.0;
    float redValue=0.0;
    float blueValue=0.0;
    if(colorName=="Red")
    {
        redValue=1.0;
    }
    else if(colorName=="Blue")
    {
        blueValue=1.0;
    }
    else if(colorName=="Green")
    {
        greenValue=1.0;
    }
    else if(colorName=="Yellow")
    {
        redValue=1;
        greenValue=1;
    }

    m_shaderProgram->setUniformValue("ourColor",redValue,greenValue,blueValue,1.0f);
}

QVector<GLfloat> TestOpenGLWidget::getColum(double r, double h)
{

    int count=36;
    double pi=3.1415926;



}

void TestOpenGLWidget::initializeGL()
{
    //初始化opengl
    initializeOpenGLFunctions();


    //初始化VAO VBO
    int genSize=m_dataDescriptionList.size();
    glGenVertexArrays(  genSize,m_VAOList.data());
    glGenBuffers(       genSize,m_VBOList.data());

    qDebug()<< m_dataList[0].size()*sizeof(m_dataList[0].data());


    //===
//    m_VAOList[0]->create();
//    m_VBOList[0]->create();

//    m_VAOList[0]->bind();
//    m_VBOList[0]->bind();

//    m_VBOList[0]->allocate(m_dataList[0].data(), m_dataList[0].size()*sizeof(m_dataList[0].data()));
    //===
    {


    }
    //====================================ShaderProgram==============================
    //从文件中读入源
    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(    "E:/qtFile/TestOpenGLByMe2/shapes.vert"));
    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString(  "E:/qtFile/TestOpenGLByMe2/shapes.frag"));

    m_errorFlag=m_shaderProgram->link();
    if(!m_errorFlag)
    {
        qDebug()<<"LINKERROR:"<<m_shaderProgram->log();
    }

    m_gridXUniform=m_shaderProgram->uniformLocation("pos");

    int tmpIndex=0;
    for(auto itor:m_dataDescriptionList)
    {
        m_errorFlag=m_initFuncDictionary.find(itor).value()(this,tmpIndex);
        if(!m_errorFlag)
        {
        }
        tmpIndex++;
    }
    for(int i=0;i<m_dataDescriptionList.size();++i)
    {
        //m_VBOList[i]->release();
       // m_VAOList[i]->release();
    }





}

void TestOpenGLWidget::resizeGL(int w, int h)
{
    if ( h == 0 )
    {
        h = 1;
    }

    glViewport( 0, 0, ( GLint )w, ( GLint )h );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluPerspective( 45.0, ( GLdouble )w / ( GLdouble )h, 0.001, 100.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void TestOpenGLWidget::paintGL()
{
    glClearColor(0.2f,0.2f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    //glTranslatef( 0.0f, 0.0f, -15.0f );

//    glRotatef( 45.0f, 1.0f, 0.0f, 0.0f );
//    glRotatef( 45.0f, 0.0f, 1.0f, 0.0f );
//    glPopMatrix();


   // QMatrix4x4 view;

    //trans
   // view.translate(0.4f, 0.0f, 0.0f);

    //x
    //view.rotate(45.0f,1.0f,0.0f,0.0f);
    //y
    //view.rotate(45.0f, 0.0f, 1.0f, 0.0f);

    //scale
    //view.scale(0.5, 0.5,1);

    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("viewMatrix",m_view);

    //test
   // m_VAOList[0]->bind();
    //m_VBOList[0]->bind();
    initColor(6);
    glBindVertexArray(m_VAOList[0]);
    glDrawArrays(GL_LINES,0,360);

    glLineWidth( 1.0f );

    glBindVertexArray(m_VAOList[1]);
    setColor("Red");
    glDrawArrays(GL_LINES,0,2);
    setColor("Blue");
    glDrawArrays(GL_LINES,2,2);
    setColor("Green");
    glDrawArrays(GL_LINES,4,2);
    setColor("Yellow");
    glDrawArrays(GL_TRIANGLES,6,18);
    //glDrawArrays(GL_TRIANGLES,9,3);



    //glDrawArrays(GL_LINES,0,60);

    //m_VBOList[0]->release();
    //m_VAOList[0]->release();
    //test


}

void TestOpenGLWidget::initClass()
{

    m_initFuncDictionary.insert("Grid",TestOpenGLWidget::initGrid);
    m_initFuncDictionary.insert("Arrow",TestOpenGLWidget::initArrow);
    s_initClassFlag=true;
}

/*
 绑定vao vbo ebo对象
 vbo设置缓冲区数据
 ebo 设置点序
 设置绘制模式、填充方式、顺逆时针等
 vao记录
 解绑
*/
bool TestOpenGLWidget::initGrid(TestOpenGLWidget *me, int index)
{
  qDebug()<<"init Grid";
//  TestOpenGLWidget *Me=me;
//  QOpenGLVertexArrayObject* vao=Me->m_VAOList[index];
//  QOpenGLBuffer* vbo=Me->m_VBOList[index];
//  QVector<GLfloat> vertices=Me->m_dataList[index];
//  QOpenGLShaderProgram* program=Me->m_shaderProgram;
//  vao->create();
//  vao->bind();

//  vbo->create();
//  vbo->bind();
//  vbo->allocate(vertices.data(), sizeof(vertices.data()));
//  //qDebug()<<vertices;
//  //qDebug()<<"vboState:"<<vbo->bufferId();

//  program->bind();
//  int posGridX=-1;
//  posGridX=program->attributeLocation("posGridX");
//  qDebug()<<"posGridX:"<<posGridX;
//  program->enableAttributeArray(posGridX);
//  program->setAttributeBuffer(posGridX,GL_FLOAT,1,3,sizeof(vertices.data())*vertices.size());
//  //program->setAttributeBuffer(vbo->bufferId(),GL_FLOAT,0,3,sizeof(vertices.data())*vertices.size());
//  //program->enableAttributeArray(vbo->bufferId());
//  //qDebug()<<"vboState:"<<vbo->bufferId();
//  vbo->release();
//  vao->release();

      TestOpenGLWidget *Me=me;
      QVector<GLfloat> vertices=Me->m_dataList[index];
      QOpenGLShaderProgram* program=Me->m_shaderProgram;
      Me->glBindVertexArray(Me->m_VAOList[index]);
      Me->glBindBuffer(GL_ARRAY_BUFFER,Me->m_VBOList[index]);
      Me->glBufferData( GL_ARRAY_BUFFER ,sizeof(vertices.data())*vertices.size(),vertices.data(),GL_STATIC_DRAW );
      //Me->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(Me->m_dataOffset*sizeof(float)));
      Me->m_dataOffset+=vertices.size();
      Me->glEnableVertexAttribArray(index);

      program->bind();
      int posGrid=-1;
      posGrid=program->attributeLocation("pos");
      qDebug()<<"posGrid:"<<posGrid;
      program->enableAttributeArray(posGrid);
      program->setAttributeBuffer(posGrid,GL_FLOAT,0,3,sizeof(GLfloat)*3);


      Me->glBindBuffer(GL_ARRAY_BUFFER,0);
      Me->glBindVertexArray(0);
  return true;
}

bool TestOpenGLWidget::initArrow(TestOpenGLWidget *me, int index)
{
     qDebug()<<"init Arrow";
//    TestOpenGLWidget *Me=me;
//    Me->glBindVertexArray(Me->m_VAOList[index]);

//    Me->glBindBuffer(GL_ARRAY_BUFFER,Me->m_VBOList[index]);
//    Me->glBufferData(GL_ARRAY_BUFFER,sizeof(m_dataList[index].data()),m_dataList[index].data(),GL_STATIC_DRAW);
//    Me->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(m_dataOffset*sizeof(float)));
//    m_dataOffset+=this->m_dataList[index].size();
//    Me->glEnableVertexAttribArray(index);


//    glBindVertexArray(0);

    TestOpenGLWidget *Me=me;
    QVector<GLfloat> vertices=Me->m_dataList[index];
    QOpenGLShaderProgram* program=Me->m_shaderProgram;

    Me->glBindVertexArray(Me->m_VAOList[index]);
    Me->glBindBuffer(GL_ARRAY_BUFFER,Me->m_VBOList[index]);
    Me->glBufferData( GL_ARRAY_BUFFER ,sizeof(vertices.data())*vertices.size(),vertices.data(),GL_STATIC_DRAW );
    Me->m_dataOffset+=vertices.size();
    Me->glEnableVertexAttribArray(index);

    program->bind();
    int posArrow=-1;
    posArrow=program->attributeLocation("pos");
    qDebug()<<"posArrow:"<<posArrow;
    program->enableAttributeArray(posArrow);
    program->setAttributeBuffer(posArrow,GL_FLOAT,0,3,sizeof(GLfloat)*3);


    Me->glBindBuffer(GL_ARRAY_BUFFER,0);
    Me->glBindVertexArray(0);

    return true;
}

bool TestOpenGLWidget::initModel(TestOpenGLWidget *me, int index)
{
//    qDebug()<<"init Model";
//    TestOpenGLWidget *Me=me;
//    Me->glBindVertexArray(Me->m_VAOList[index]);

//    Me->glBindBuffer(GL_ARRAY_BUFFER,Me->m_VBOList[index]);
//    Me->glBufferData(GL_ARRAY_BUFFER,sizeof(m_dataList[index].data()),m_dataList[index].data(),GL_STATIC_DRAW);
//    Me->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(m_dataOffset*sizeof(float)));
//    m_dataOffset+=this->m_dataList[index].size();
//    Me->glEnableVertexAttribArray(index);


//    glBindVertexArray(0);
    return true;
}

void TestOpenGLWidget::wheelEvent(QWheelEvent *e)
{
    QOpenGLWidget::wheelEvent(e);

    int delta=e->delta();
    float scaleFactor = 1.15;
    if(delta>0)
    {
        m_view.scale(scaleFactor);
    }
    else
    {
        m_view.scale(1.0/scaleFactor);
    }
    update();
}

void TestOpenGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    m_endPoint=e->pos();
    double disX=(m_endPoint-m_startPoint).x();
    double disY=(m_endPoint-m_startPoint).y();

    float transXFactor=disX/width();
    float transYFactor=disY/height();

    float rotatedFactor=120;

    if ( e->buttons() & Qt::LeftButton )
    {
        m_view.translate(transXFactor,-transYFactor,0);
    }
    else if(e->buttons() & Qt::RightButton)
    {
        //x y z
        m_view.rotate(transYFactor*rotatedFactor,1.0f,0.0f,0.0f);
        m_view.rotate(transXFactor*rotatedFactor,0.0f,1.0f,0.0f);
         //m_view.rotate(transXFactor*rotatedFactor,0.0f,0.0f,1.0f);
    }
    update();

    m_startPoint=m_endPoint;
}

void TestOpenGLWidget::mousePressEvent(QMouseEvent *e)
{
    m_startPoint=e->pos();

}

void TestOpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{

}
