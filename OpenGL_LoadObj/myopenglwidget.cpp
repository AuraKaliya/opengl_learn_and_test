#include "myopenglwidget.h"

#include <QTime>
#include <QRandomGenerator>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPen>
#include <QPaintEvent>
#include <QStringList>
#include <QDir>
#include <QStringList>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    m_shaderProgram=new QOpenGLShaderProgram;
    m_directionShaderProgram=new QOpenGLShaderProgram;
    m_lightShaderProgram=new QOpenGLShaderProgram;
    m_modelDir=new QDir("E:/qtFile/TestOpenGLByMe_3/model");


    initSetting();
    initObjectDictionary();

    m_textShow=false;




    test();


}

void MyOpenGLWidget::read()
{
    //step1 : 初始化modelList；
    m_modelList.clear();

    QStringList filter;
    filter<<"*.obj";
    m_modelDir->setNameFilters(filter);
    m_modelList=m_modelDir->entryList(filter,QDir::Files | QDir::Readable,QDir::Name);
    //qDebug()<<m_modelList;
    for(auto it:m_modelList)
    {
        MyOpenGLObject* obj=new MyOpenGLObject;
        m_objDictionary.insert(it,obj);
        //m_modelDictionary.insert("Model",obj);
        //m_modelList.append("Model");
        obj->name=it;
        obj->normalVectorFlag=false;
        obj->objNormalFlag=false;

        m_posList.clear();
        m_indexList.clear();

        QString path=m_modelDir->path()+"/"+it;
        //qDebug()<<path;



        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开文件：" << path;
            continue;
        }

        QTextStream in(&file);

        //目前 ： 处理 v vn  f
        while (!in.atEnd())
        {
            QString line = in.readLine();
            line = line.trimmed();
            QStringList lineParts = line.split(" ", Qt::SkipEmptyParts);
            if (lineParts.size()>0)
            {
                if( lineParts[0] == "v")
                {
                    // 'v' 开头的行包含顶点数据
                    double x = lineParts[1].toDouble();
                    double y = lineParts[2].toDouble();
                    double z = lineParts[3].toDouble();

                    obj->posList<<x<<y<<z;
                }
                else if(lineParts[0]=="vn")
                {
                    // 'vn' 开头的包含法线数据
                    obj->normalVectorFlag=true;
                    obj->objNormalFlag=true;

                    double x = lineParts[1].toDouble();
                    double y = lineParts[2].toDouble();
                    double z = lineParts[3].toDouble();
                    obj->normalVectorList<<x<<y<<z;
                }
                else if(lineParts[0]=="f" )
                {
                    //  ‘f’ 开头的包含索引数据     顶点/纹理/法线
                    if(obj->objNormalFlag)
                    {
                         for(int i=2;i<lineParts.size()-1;++i)
                         {
                             int p1=lineParts[1].split("/")[0].toInt()-1;
                             int p2=lineParts[i].split("/")[0].toInt()-1;
                             int p3=lineParts[i+1].split("/")[0].toInt()-1;
                             obj->indexList<<p1<<p2<<p3;
                             int p4=lineParts[1].split("/")[2].toInt()-1;
                             int p5=lineParts[i].split("/")[2].toInt()-1;
                             int p6=lineParts[i+1].split("/")[2].toInt()-1;
                             obj->normalIndexList<<p4<<p5<<p6;
                         }
                    }
                    else
                    {
                        for(int i=2;i<lineParts.size()-1;++i)
                        {
                            int p1=lineParts[1].toInt()-1;
                            int p2=lineParts[i].toInt()-1;
                            int p3=lineParts[i+1].toInt()-1;
                            obj->indexList<<p1<<p2<<p3;
                        }

                    }
                }
            }
        }

        //计算法向量 ---在顶点数据和索引数据加载完成后再计算,三个点形成的面对应的法向量应该相同
        //处理完没得到法线向量时进入---手动算
        if(!obj->normalVectorFlag)
        {
            obj->normalVectorList.clear();
            for(int i=0;i<obj->indexList.size();i+=3)
            {
                Eigen::Vector3d normalVector=getNormalVector(obj,i,i+1,i+2);
                obj->normalVectorList<<normalVector(0)<<normalVector(1)<<normalVector(2);
            }
            obj->normalVectorFlag=true;
        }

        file.close();
    }
}

void MyOpenGLWidget::check(MyOpenGLObject *obj)
{
    qDebug()<<"===================";
    qDebug()<<"==========posList============";
    for(int i=0;i<obj->posList.size();++i)
    {
        qDebug()<<i<<" :"<<obj->posList[i];
    }
    qDebug()<<"==========indexList============";
    for(int i=0;i<obj->indexList.size();++i)
    {
        qDebug()<<i<<" :"<<obj->indexList[i];
    }

    if(obj->normalVectorFlag)
    {
        qDebug()<<"==========normalVectorList============";
        for(int i=0;i<obj->normalVectorList.size();++i)
        {
            qDebug()<<i<<" :"<<obj->normalVectorList[i];
        }
    }

    if(obj->objNormalFlag)
    {
        for(int i=0;i<obj->normalIndexList.size();++i)
        {
            qDebug()<<i<<" :"<<obj->normalIndexList[i];
        }
    }
    qDebug()<<"===================";
}

void MyOpenGLWidget::check(MySpaceText *text)
{
    qDebug()<<"=========Check Text=========="<<text->text;
    qDebug()<<"id:"<<text->id;
    qDebug()<<"Point:"<<text->spacePoint;
    qDebug()<<"=========Check Text==========";
}

void MyOpenGLWidget::initObject(MyOpenGLObject *obj)
{
    qDebug()<<"===now init "<<obj->name<<"===";
    //check(obj);
    //====obj property==========
    //====modelMatrix
    obj->model.setToIdentity();
    obj->modelRotateMatrix.setToIdentity();
    obj->modelTranslateMatrix.setToIdentity();
    //====vector
    //obj->modelNormalVector=QVector3D(0,1,0);
    //obj->modelRightVector=QVector3D(1,0,0);
    //====obj property==========END

    //====opengl==========
    glGenVertexArrays(1,&(obj->vao));
    glGenBuffers(1,&(obj->vbo));
    glGenBuffers(1,&(obj->ebo));

    glBindVertexArray(obj->vao);
    glBindBuffer(GL_ARRAY_BUFFER,obj->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,obj->ebo);


    if(obj->normalVectorFlag)
    {
        obj->dataList.clear();
        obj->dataList.reserve(obj->indexList.size()*6);

        if(obj->objNormalFlag)
        {
            for(int i=0;i<obj->indexList.size();++i)
            {
            int posIndex=obj->indexList[i]*3;
            int vectorIndex=obj->normalIndexList[i]*3;
            obj->dataList<<obj->posList[posIndex]<<obj->posList[posIndex+1]<<obj->posList[posIndex+2];
            obj->dataList<<obj->normalVectorList[vectorIndex]<<obj->normalVectorList[vectorIndex+1]<<obj->normalVectorList[vectorIndex+2];
            }
        }
        else
        {
            for(int i=0;i<obj->indexList.size();++i)
            {
                int posIndex=obj->indexList[i]*3;
                int vectorIndex=(i/3)*3;
                obj->dataList<<obj->posList[posIndex]<<obj->posList[posIndex+1]<<obj->posList[posIndex+2];
                obj->dataList<<obj->normalVectorList[vectorIndex]<<obj->normalVectorList[vectorIndex+1]<<obj->normalVectorList[vectorIndex+2];
            }
        }

        glBufferData(GL_ARRAY_BUFFER,sizeof(obj->dataList.data())*obj->dataList.size(),obj->dataList.data(),GL_STATIC_DRAW);

        glVertexAttribPointer(0,3,GL_DOUBLE,GL_FALSE,6*sizeof(double),(void*)0);
        glVertexAttribPointer(1,3,GL_DOUBLE,GL_FALSE,6*sizeof(double),(void*)(3*sizeof(double)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

    }else
    {
        glBufferData(GL_ARRAY_BUFFER,sizeof(obj->posList.data())*obj->posList.size(),obj->posList.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_DOUBLE,GL_FALSE,3*sizeof(double),(void*)0);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(obj->indexList.data())*obj->indexList.size(),obj->indexList.data(),GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);
    //====opengl==========END
    //=====text==========
    //=====text==========END
}

double MyOpenGLWidget::posXFromOpenGLToQt(double posX)
{
    return ((posX+1.0)/2) * this->width();
}

double MyOpenGLWidget::posYFromOpenGLToQt(double posY)
{
    //因为Qt左手坐标系 opengl右手坐标系，原点也不同，所以在这里需要进行一次统一
    return this->height()-((posY+1.0)/2) * this->height();
}

void MyOpenGLWidget::initTextByObject(MyOpenGLObject *obj)
{
    //=====text==========
    for(int i=0;i<obj->posList.size();i+=3)
    {
        MySpaceText* text=new MySpaceText();
        text->id=m_textIdCount;
        text->spacePoint=QVector3D(obj->posList[i],obj->posList[i+1],obj->posList[i+2]);
        text->sizeScale=0.1;
        text->baseSize=QSize(30,30);
        text->fontSize=12;
        text->rotationMatrix.setToIdentity();
        text->translateMatrix.setToIdentity();
        text->text=QString::number(text->id);
        obj->textList.append(text);
        m_textDictionary.insert(m_textIdCount,text);
        m_textIdCount++;
        //check(text);
    }
    //=====text==========END
}

void MyOpenGLWidget::initSetting()
{
    //check();
    //text 全局id   --点索引
    m_textIdCount=0;
    //视角（相机）
    m_upVector=QVector3D(0,1,0);
    m_rightVector =QVector3D(1,0,0);
    m_frontVector=QVector3D(0,0,1);

    //坐标系的位置
    m_directionPosView.setToIdentity();
    m_directionPosView.translate(-0.8f,-0.8f,0);

    m_sun.setToIdentity();

    //m_projection.setToIdentity();
    //m_projection.ortho(-1,1,-1,1,0,-1);

    m_shift=false;

    //光源点
    m_lightPos=QVector3D(0,2,0);
    m_viewPos=QVector3D(0,1,0);

    m_view.setToIdentity();
    m_model.setToIdentity();
    m_viewTranslateMatrix.setToIdentity();
    m_viewRotateMatrix.setToIdentity();
    m_viewScaleMatrix.setToIdentity();




    m_xColor=QVector3D(1.0,0.0,0.0);
    m_yColor=QVector3D(0.0,1.0,0.0);
    m_zColor=QVector3D(0.0,0.0,1.0);

    setFocusPolicy(Qt::ClickFocus);

}

void MyOpenGLWidget::initObjectDictionary()
{
    initDirectionObject();
    initLightObject();
    read();
}

void MyOpenGLWidget::initDirectionObject()
{
    //Direction================
    MyOpenGLObject* directionObj=new MyOpenGLObject;
    m_objDictionary.insert("Direction",directionObj);

    directionObj->name="Direction";
    directionObj->normalVectorFlag=false;
    directionObj->objNormalFlag=false;
    //====point
    // o
    directionObj->posList<<0.0 <<0.0 << 0.0;
    // x
    directionObj->posList<<0.1 <<0.0 << 0.0;
    // y
    directionObj->posList<<0.0 <<0.1 << 0.0;
    // z
    directionObj->posList<<0.0 <<0.0 << 0.1;

    //====index
    directionObj->indexList<<0<<1;
    directionObj->indexList<<0<<2;
    directionObj->indexList<<0<<3;
    //================
}

void MyOpenGLWidget::initLightObject()
{
    //Light================
    MyOpenGLObject* lightObj=new MyOpenGLObject;
    m_objDictionary.insert("LightPoint",lightObj);
    lightObj->name="LightPoint";
    lightObj->normalVectorFlag=false;
    lightObj->objNormalFlag=false;

    lightObj->posList<<m_lightPos.x()<<m_lightPos.y()<<m_lightPos.z()
                   ;
    lightObj->indexList<<0
                     ;
    //================
}

Eigen::Vector3d MyOpenGLWidget::getNormalVector(MyOpenGLObject *obj, int point1, int point2, int point3)
{
    Eigen::Vector3d res(0,0,0);
    int start1=obj->indexList[point1]*3;
    int start2=obj->indexList[point2]*3;
    int start3=obj->indexList[point3]*3;

    Eigen::Vector3d tmpVector1(obj->posList[start1]-obj->posList[start2],obj->posList[start1+1]-obj->posList[start2+1],obj->posList[start1+2]-obj->posList[start2+2]);
    Eigen::Vector3d tmpVector2(obj->posList[start2]-obj->posList[start3],obj->posList[start2+1]-obj->posList[start3+1],obj->posList[start2+2]-obj->posList[start3+2]);

    res=tmpVector1.cross(tmpVector2);
    return res;
}

void MyOpenGLWidget::testNormalVector(QVector3D point1, QVector3D point2, QVector3D point3)
{

    Eigen::Vector3d tmpVector1(point1.x()-point2.x(),point1.y()-point2.y(),point1.z()-point2.z());
    Eigen::Vector3d tmpVector2(point2.x()-point3.x(),point2.y()-point3.y(),point2.z()-point3.z());
    qDebug()<<"Point";
    qDebug()<<point1;
    qDebug()<<point2;
    qDebug()<<point3;
    Eigen::Vector3d res=tmpVector1.cross(tmpVector2);
    qDebug()<<"NormalVector:"<<res[0]<<res[1]<<res[2];
}

void MyOpenGLWidget::test()
{
    qDebug()<<"===================test======================";
    QDir dir("E:/qtFile/TestOpenGLByMe_3/model");

    qDebug()<<dir.path();


    QStringList filter;
    filter<<"*.obj";
    dir.setNameFilters(filter);
    auto res=dir.entryList(filter,QDir::Files | QDir::Readable,QDir::Name);

    qDebug()<<res;

    qDebug()<<"===================test======================end";
}

void MyOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    //OPENGL -----设置-----全局设置

    //绘制策略     -
    //glPolygonMode(GL_FRONT,GL_FILL);
    //将顺时针设置为“正面” 同时逆时针变为反面
    //glFrontFace(GL_CCW);

    //OPENGL -----设置--------end


    //从文件中读入源----初始化shaderProgram
    bool success;

    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(   "E:/qtFile/TestOpenGLByMe_3/shapes.vert"));
    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString( "E:/qtFile/TestOpenGLByMe_3/shapes.frag"));
    success= m_shaderProgram->link();
    if(!success)
    {
        qDebug()<<"ERR:"<<m_shaderProgram->log();
    }
    m_directionShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString( "E:/qtFile/TestOpenGLByMe_3/direction.frag"));
    m_directionShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(   "E:/qtFile/TestOpenGLByMe_3/direction.vert"));
    success= m_directionShaderProgram->link();
    if(!success)
    {
        qDebug()<<"ERR direct:"<<m_directionShaderProgram->log();
    }
    m_lightShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString( "E:/qtFile/TestOpenGLByMe_3/light.frag"));
    m_lightShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(   "E:/qtFile/TestOpenGLByMe_3/light.vert"));
    success= m_lightShaderProgram->link();
    if(!success)
    {
        qDebug()<<"ERR direct:"<<m_lightShaderProgram->log();
    }

    for(auto it=m_objDictionary.begin();it!=m_objDictionary.end();++it)
    {
        initObject(it.value());
    }

    //初始化model的顶点对应文字
    auto modelObject=m_objDictionary.find("Model");
    if(modelObject!=m_objDictionary.end())
    {
        initTextByObject(modelObject.value());
    }

    m_nowObject=m_objDictionary.find("Direction").value();;

    //Widget设置----其实可以放在构造函数中----
    //Widget设置----其实可以放在构造函数中----end
}

void MyOpenGLWidget::resizeGL(int w, int h)
{

}

void MyOpenGLWidget::paintGL()
{

    //=================预处理====================
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_MODE);
    //glEnable(GL_CULL_FACE);

    glClearColor(0.2f,0.5f,0.5f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT,GL_FILL);
    //glFrontFace(GL_CW);

//    //更新view
    m_view.setToIdentity();
    m_view=m_viewRotateMatrix*m_view;
    m_view=m_viewTranslateMatrix*m_view;
    m_view=m_viewScaleMatrix*m_view;
    //=================预处理====================

    //=================模型渲染部分====================
    m_shaderProgram->bind();

    for(auto it: m_modelList)
    {
        MyOpenGLObject* modelObj=m_objDictionary.find(it).value();
        glBindVertexArray(modelObj->vao);

        //更新model
        modelObj->model.setToIdentity();
        modelObj->model=modelObj->modelRotateMatrix*modelObj->model;
        modelObj->model=modelObj->modelTranslateMatrix*modelObj->model;


        //=====  最好应该是用obj内的model的数据，但会引出一个新需求：obj选中列表进行变换处理。
        m_shaderProgram->setUniformValue("model",modelObj->model);
        m_shaderProgram->setUniformValue("view",m_view);
        m_shaderProgram->setUniformValue("scale",m_viewScaleMatrix);

        m_shaderProgram->setUniformValue("lightPos",m_lightPos);
        m_shaderProgram->setUniformValue("viewPos",m_viewPos);
        //m_shaderProgram->setUniformValue("projection",m_projection);

        // ===需要用一组变量来存
        m_shaderProgram->setUniformValue("material.ambient",  1.0f, 0.5f, 0.6f);
        m_shaderProgram->setUniformValue("material.diffuse",  1.0f, 0.5f, 0.6f);
        m_shaderProgram->setUniformValue("material.specular", 0.5f, 0.5f, 0.5f);
        m_shaderProgram->setUniformValue("material.shininess", 32.0f);

        m_shaderProgram->setUniformValue("light.ambient",  0.2f, 0.4f, 0.2f);
        m_shaderProgram->setUniformValue("light.diffuse",  0.7f, 0.7f, 0.7f); // 将光照调暗了一些以搭配场景
        m_shaderProgram->setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);



        if(modelObj->normalVectorFlag)
        {
            //qDebug()<<"check size:"<<modelObj->indexList.size();
            glDrawArrays(GL_TRIANGLES,0,modelObj->indexList.size());
        }
        else
        {
            glDrawElements(GL_TRIANGLES,modelObj->indexList.size(),GL_UNSIGNED_INT,0);
        }

    }

    //=================模型渲染部分====================


    //=================坐标轴渲染部分====================
    m_directionShaderProgram->bind();
    MyOpenGLObject* directionObj=m_objDictionary.find("Direction").value();
    glBindVertexArray(directionObj->vao);
    //更新model
    directionObj->model.setToIdentity();
    directionObj->model=directionObj->modelRotateMatrix*directionObj->model;
    directionObj->model=directionObj->modelTranslateMatrix*directionObj->model;


    m_directionShaderProgram->setUniformValue("model",directionObj->model);
    //m_directionShaderProgram->setUniformValue("model",m_view);

    //m_shaderProgram->setUniformValue("model",m_model);
    m_directionShaderProgram->setUniformValue("view",m_directionPosView);

    //x-----
    m_directionShaderProgram->setUniformValue("ourColor",m_xColor);
    glDrawElements(GL_LINES,2,GL_UNSIGNED_INT,0);

    //y-----
    m_directionShaderProgram->setUniformValue("ourColor",m_yColor);
    glDrawElements(GL_LINES,2,GL_UNSIGNED_INT,(void*)(2*sizeof(GL_UNSIGNED_INT)));

    //z-----
    m_directionShaderProgram->setUniformValue("ourColor",m_zColor);
    glDrawElements(GL_LINES,2,GL_UNSIGNED_INT,(void*)(4*sizeof(GL_UNSIGNED_INT)));
    //=================坐标轴渲染部分====================

    //===================LIGHT部分=====================
    m_lightShaderProgram->bind();
    MyOpenGLObject* lightObj=m_objDictionary.find("LightPoint").value();
    glBindVertexArray(lightObj->vao);
    m_lightShaderProgram->setUniformValue("model",lightObj->model);
    m_lightShaderProgram->setUniformValue("view",m_view);
    m_lightShaderProgram->setUniformValue("lightPos",m_lightPos);
    m_lightShaderProgram->setUniformValue("ourColor",m_xColor);
    glPointSize( 16.0f );

    glDrawElements(GL_POINTS,1,GL_UNSIGNED_INT,0);

    //===================LIGHT部分=====================

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_MODE);
    glBindVertexArray(0);

    //======================Text部分===================

    if(m_textShow)
    {
        QPainter painter(this);
        QPen pen(Qt::red);
        painter.setPen(pen);

        for(auto modelName:m_modelList)
        {
            MyOpenGLObject* modelObj=m_objDictionary.find(modelName).value();
            for(MySpaceText* it:modelObj->textList)
            {
                //
                QVector3D pos=QVector3D(  m_view*modelObj->model*QVector4D(it->spacePoint.x(),
                                                                             it->spacePoint.y(),
                                                                             it->spacePoint.z(),
                                                                             1.0                ));

                double x=posXFromOpenGLToQt(pos.x());
                double y=posYFromOpenGLToQt(pos.y());
                //    随远近缩放字体        (pos.z()+1+it->sizeScale)*
                int fontSize=it->fontSize;
                painter.setFont(QFont("Arial",fontSize));
                painter.drawText(QPoint(x-fontSize/2,y-fontSize/2),it->text);
            }


        }


        painter.end();
    }
    //======================Text部分===================
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_MODE);

}

void MyOpenGLWidget::keyPressEvent(QKeyEvent *e)
{

    if(e->key()==Qt::Key_Shift)
    {
        m_shift=true;
    }
    if(e->key()==Qt::Key_R)
    {
        for(auto it :m_objDictionary)
        {
            it->modelTranslateMatrix.setToIdentity();
            it->modelRotateMatrix.setToIdentity();
            it->modelRotation=QQuaternion();
            it->model.setToIdentity();
        }
        m_viewTranslateMatrix.setToIdentity();
        m_viewRotateMatrix.setToIdentity();
        m_viewRotation=QQuaternion();
        m_view.setToIdentity();

        update();
    }
    if(e->key()==Qt::Key_Q)
    {
        m_textShow=!m_textShow;
    }

    QOpenGLWidget::keyPressEvent(e);
}

void MyOpenGLWidget::keyReleaseEvent(QKeyEvent *e)
{
    m_shift=false;

    QOpenGLWidget::keyReleaseEvent(e);
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *e)
{
    QOpenGLWidget::wheelEvent(e);

    int delta=e->delta();
    float scaleFactor = 1.05;
    if(delta>0)
    {
        m_viewScaleMatrix.scale(scaleFactor);
    }
    else if(delta <0)
    {
        m_viewScaleMatrix.scale(1.0/scaleFactor);
    }
    update();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *e)
{

    m_endPoint=e->pos();

    double disX=(m_endPoint-m_startPoint).x();
    double disY=(m_endPoint-m_startPoint).y();

    float transXFactor=disX/width();
    float transYFactor=disY/height();

    float rotatedFactor=60;
    float angleX=transXFactor*rotatedFactor;
    float angleY=transYFactor*rotatedFactor;

    QQuaternion  rotationY=QQuaternion ::fromAxisAndAngle(m_rightVector,angleY);
    QQuaternion  rotationX=QQuaternion ::fromAxisAndAngle(m_upVector,angleX);

    //m_shift
    if(m_shift)
    {
        if ( e->buttons() & Qt::LeftButton )
        {
            m_viewTranslateMatrix.translate(transXFactor,-transYFactor,0);
        }

        if(e->buttons() & Qt::RightButton)
        {
            m_viewRotation=rotationY*rotationX*m_viewRotation;
            m_viewRotateMatrix.setToIdentity();
            m_viewRotateMatrix.rotate(m_viewRotation);
        }
    }
    else
    {
        if ( e->buttons() & Qt::LeftButton )
        {
            auto modelObj=m_objDictionary.find("Model");
            if(modelObj!=m_objDictionary.end())
            {
                modelObj.value()->modelTranslateMatrix.translate(transXFactor,-transYFactor,0);
            }
        }
        if(e->buttons() & Qt::RightButton)
        {
            for(auto it:m_objDictionary)
            {
                it->modelRotation=rotationY*rotationX*it->modelRotation;

                it->modelRotateMatrix.setToIdentity();
                it->modelRotateMatrix.rotate(it->modelRotation);
            }
        }
    }
    update();
    m_startPoint=m_endPoint;
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *e)
{
    m_startPoint=e->pos();
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{

}

void MyOpenGLWidget::paintEvent(QPaintEvent *e)
{
    QOpenGLWidget::paintEvent(e);
}