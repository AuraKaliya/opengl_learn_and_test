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
    resize(1000,1000);
    m_shaderProgram=new QOpenGLShaderProgram;
    m_modelDir=new QDir("E:/qtFile/OpenGLLearn/model");
    initSetting();
    initObjData();
    check(m_obj);

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

void MyOpenGLWidget::initObject(MyOpenGLObject *obj)
{
    qDebug()<<"===now init "<<obj->name<<"===";
    //====obj property==========
    //====modelMatrix
    obj->model.setToIdentity();
    obj->modelRotateMatrix.setToIdentity();
    obj->modelTranslateMatrix.setToIdentity();
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
}

void MyOpenGLWidget::reInitObject(MyOpenGLObject *obj)
{
    //因posList等改变而需要重新递送数据

    glBindVertexArray(obj->vao);
    glBindBuffer(GL_ARRAY_BUFFER,obj->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,obj->ebo);

    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(obj->posList.data())*obj->posList.size(),obj->posList.data());
    glVertexAttribPointer(0,3,GL_DOUBLE,GL_FALSE,3*sizeof(double),(void*)0);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,sizeof(obj->indexList.data())*obj->indexList.size(),obj->indexList.data());
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
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

double MyOpenGLWidget::posXFromQtToOpenGL(double posX)
{
    return (posX*2)/this->width()-1.0;
}

double MyOpenGLWidget::posYFromQtToOpenGL(double posY)
{
    return ((this->height()-posY)*2)/this->height()+-1.0;
}

void MyOpenGLWidget::initSetting()
{
    m_shift=false;



    //相机设置
    m_camera.rightVector=QVector3D(1,0,0);
    m_camera.frontVector=QVector3D(0,0,1);
    m_camera.upVector=QVector3D(0,1,0);
    m_camera.pos=QVector3D(0,0,-3);
    //光源点
    m_lightPos=QVector3D(-m_camera.pos);
    m_camera.viewMatrix.setToIdentity();
    m_camera.rotationMatrix.setToIdentity();
    m_camera.translateMatrix.setToIdentity();
    m_camera.translateMatrix.translate(m_camera.pos);

    //m_camera.zoom=45.0f;
    //m_camera.projection.setToIdentity();
    //m_camera.projection.perspective(qRadiansToDegrees(m_camera.zoom), float(width()) / float(height()), 0.1f, 100.0f);


    //投影设置
    m_project.setToIdentity();
    m_project.perspective(qRadiansToDegrees(45.0),(float)width()/(float)height(),0.1f,100.0f);

    //m_project.ortho(-1,1,-1,1,0,100);

    qDebug()<<"chek project:"<<m_project;

    //widget设置
    setFocusPolicy(Qt::ClickFocus);
}

void MyOpenGLWidget::initShaderProgram()
{
    //从文件中读入源----初始化shaderProgram
    bool success;

    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(   "E:/qtFile/OpenGLLearn/shaderProgram/shapes.vert"));
    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString( "E:/qtFile/OpenGLLearn/shaderProgram/shapes.frag"));
    success= m_shaderProgram->link();
    if(!success)
    {
        qDebug()<<"ERR:"<<m_shaderProgram->log();
        return;
    }
}

void MyOpenGLWidget::initObjData()
{
    QStringList filter;
    filter<<"*.obj";
    m_modelDir->setNameFilters(filter);
    for(auto it: m_modelDir->entryList(filter,QDir::Files | QDir::Readable,QDir::Name) )
    {
        MyOpenGLObject* obj=new MyOpenGLObject;
        obj->name="Cube";
        //qDebug()<<it;
        obj->normalVectorFlag=false;
        obj->objNormalFlag=false;

        QString path=m_modelDir->path()+"/"+it;
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
        m_obj=obj;
        file.close();
    }
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

    qDebug()<<"===================test======================end";
}

bool MyOpenGLWidget::intersectTriangle(const QVector3D &origPos, const QVector3D &rayDir,
                                       QVector3D v0, QVector3D v1, QVector3D v2,
                                       double *t, double* u, double* v)
{

    Eigen::Vector3d E1((v1-v0).x(),(v1-v0).y(),(v1-v0).z());
    Eigen::Vector3d E2((v2-v0).x(),(v2-v0).y(),(v2-v0).z());

    Eigen::Vector3d Dir(rayDir.x(),rayDir.y(),rayDir.z());


    Eigen::Vector3d P=Dir.cross(E2);

    double det= E1.dot(P);

    Eigen::Vector3d T;

    if(det>0)
    {
        T=Eigen::Vector3d((origPos-v0).x(),(origPos-v0).y(),(origPos-v0).z());

    }
    else
    {
        T=Eigen::Vector3d((v0-origPos).x(),(v0-origPos).y(),(v0-origPos).z());
        det=-det;
    }

    if(det<0.0001)
    {
        return false;
    }
    *u=T.dot(P);
    if(*u <0.0 ||*u >det)
    {
        return false;
    }

    Eigen::Vector3d Q=T.cross(E1);

    *v=Dir.dot(Q);
    if(*v <0.0 || *u+*v >det)
    {
        return false;
    }

    *t =E2.dot(Q);

    double fInvDet=1.0f/det;
    *t *=fInvDet;
    *u *=fInvDet;
    *v *=fInvDet;

    return true;
}

bool MyOpenGLWidget::checkSelectModel(const QVector3D &origPos, const QVector3D &rayDir, MyOpenGLObject *obj)
{
    qDebug()<<"==================MyOpenGLWidget::checkSelectModel======================";
    bool res=false;
    qDebug()<<"origPos:"<<origPos;
    qDebug()<<"rayDir:"<<rayDir;
    for(int i=0;i<obj->indexList.size();i+=3)
    {
        int posIndex1=obj->indexList[i]*3;
        int posIndex2=obj->indexList[i+1]*3;
        int posIndex3=obj->indexList[i+2]*3;
        QVector3D p1(obj->posList[posIndex1],obj->posList[posIndex1+1],obj->posList[posIndex1+2]);
        QVector3D p2(obj->posList[posIndex2],obj->posList[posIndex2+1],obj->posList[posIndex2+2]);
        QVector3D p3(obj->posList[posIndex3],obj->posList[posIndex3+1],obj->posList[posIndex3+2]);
        qDebug()<<"Model org Pos:"<<p1<<p2<<p3;
        obj->model.setToIdentity();
        obj->model=obj->modelRotateMatrix*      obj->model;
        obj->model=obj->modelTranslateMatrix*   obj->model;

        p1=QVector3D(obj->model * QVector4D(p1,1.0));
        p2=QVector3D(obj->model * QVector4D(p2,1.0));
        p3=QVector3D(obj->model * QVector4D(p3,1.0));

        QVector3D v0(p2-p1);
        QVector3D v1(p3-p2);
        QVector3D v2(p1-p3);

        double t;
        double u;
        double v;
        bool flag=intersectTriangle(origPos,rayDir,p1,p2,p3,&t,&u,&v);
        qDebug()<<"Model Pos:"<<p1<<p2<<p3;
        qDebug()<<"Model vector:"<<v0<<v1<<v2;
        qDebug()<<"flag"<<flag;
        if(flag)
        {
            res=true;
            qDebug()<<"index:"<<obj->indexList[i]<<obj->indexList[i+1]<<obj->indexList[i+2];
            break;
        }

    }
    qDebug()<<"==================MyOpenGLWidget::checkSelectModel======================end";
    return res;
}

bool MyOpenGLWidget::rayTriangleIntersect(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, const Eigen::Vector3d &v0, const Eigen::Vector3d &v1, const Eigen::Vector3d &v2)
{
    // 计算三角形的两个边
    Eigen::Vector3d edge1 = v1 - v0;
    Eigen::Vector3d edge2 = v2 - v0;

    // 计算向量P
    Eigen::Vector3d pvec = dir.cross(edge2);

    // 计算行列式
    double det = edge1.dot(pvec);

    // 如果行列式接近0，射线与三角形平行
    if (fabs(det) < 1e-8) return false;

    double invDet = 1 / det;

    // 计算射线和三角形顶点0之间的向量T
    Eigen::Vector3d tvec = orig - v0;

    // 计算u参数并测试边界
    double u = tvec.dot(pvec) * invDet;
    if(u<0||u>1)
    {
        return false;
    }

    // 准备测试v参数
    Eigen::Vector3d qvec = tvec.cross(edge1);

    // 计算v参数并测试边界
    double v = dir.dot(qvec) * invDet;
    if (v < 0 || u + v > 1)
    {
        return false;
    }

    return true;

}

QVector3D MyOpenGLWidget::getRayDirection(double posX, double posY, const QMatrix4x4 &rotationMatrix, const QMatrix4x4 &translateMatrix, const QVector3D &cameraPos)
{
    // 将平面坐标转换为齐次坐标
      QVector3D screenPos(posX, posY, 0);

      // 计算变换矩阵
      QMatrix4x4 transformMatrix = rotationMatrix.transposed() * translateMatrix.transposed();

      // 将屏幕坐标转换为世界坐标
      QVector3D worldPos = transformMatrix * screenPos;

      // 计算射线方向
      QVector3D rayDir = worldPos - cameraPos;

      // 返回射线方向
      return rayDir;
}

QVector3D MyOpenGLWidget::getIntersectionWithZPlane(const QVector3D &origin, const QVector3D &direction, double z)
{
    if (direction.z() == 0) {
           // 如果方向向量的z分量为0，那么射线与z平面平行，没有交点
           return QVector3D();
       }

       // 计算射线与z平面的交点
       double t = (z - origin.z()) / direction.z();
       QVector3D intersection = origin + t * direction;

       return intersection;
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

    // initShader===
    initShaderProgram();
    initObject(m_obj);



    //Widget设置----其实可以放在构造函数中----
    //Widget设置----其实可以放在构造函数中----end
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    if (h == 0) { // 防止被零除
        h = 1; // 将高设为1
    }
    glViewport(0, 0, w, h); //重置当前的视口
}

void MyOpenGLWidget::paintGL()
{

    //=================预处理====================

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_MODE);
    //glEnable(GL_CULL_FACE);

    // 设置深度缓存
   // glClearDepth(1.0f);

    // 告诉系统对透视进行修正
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor(0.2f,0.5f,0.5f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT,GL_FILL);
    // 启用阴影平滑
    glShadeModel(GL_SMOOTH);

    //  更新Camera
    m_camera.viewMatrix.setToIdentity();
    m_camera.viewMatrix=m_camera.rotationMatrix*m_camera.viewMatrix;
    m_camera.viewMatrix=m_camera.translateMatrix*m_camera.viewMatrix;
    m_camera.viewMatrix=m_camera.scaleMatrix*m_camera.viewMatrix;

    //=================预处理====================

    //=================模型渲染部分====================
    m_shaderProgram->bind();
    //处理模型
    MyOpenGLObject* modelObj=m_obj;

    glBindVertexArray(modelObj->vao);

    //更新model
    modelObj->model.setToIdentity();
    modelObj->model=modelObj->modelRotateMatrix*modelObj->model;
    modelObj->model=modelObj->modelTranslateMatrix*modelObj->model;
    // modelObj->model.rotate();


    //uniform  交互数据处理
    //=====  最好应该是用obj内的model的数据，但会引出一个新需求：obj选中列表进行变换处理。


    //没用了
    m_shaderProgram->setUniformValue("view",m_camera.viewMatrix);
    m_shaderProgram->setUniformValue("model",modelObj->model);


//    QMatrix4x4 viewMatrix;
//    viewMatrix.setToIdentity();
//    viewMatrix.translate(0,0,-1.0f);


//    QMatrix4x4 modelMatrix;
//    modelMatrix.setToIdentity();


    QMatrix4x4 mvpMatrix;
    mvpMatrix.setToIdentity();
    mvpMatrix=m_camera.viewMatrix*modelObj->model;


    //qDebug()<<"========================================================";
    //QVector3D testPos(0.5,0.5,0.5);
    //qDebug()<<"Model"<<modelObj->model;
    //qDebug()<<"camera View"<<m_camera.viewMatrix;
    //qDebug()<<"MV Matrix"<<mvpMatrix;
    //qDebug()<<"MVP Matrix"<<m_project*mvpMatrix;
    //qDebug()<<"========================================================";


    //m_shaderProgram->setUniformValue("mvp",mvpMatrix);
    m_shaderProgram->setUniformValue("mvp",m_project*mvpMatrix);

    m_shaderProgram->setUniformValue("lightPos",m_lightPos);
    //m_shaderProgram->setUniformValue("viewPos",m_camera.pos);

    // ===需要用一组变量来存
    m_shaderProgram->setUniformValue("material.ambient",  1.0f, 0.5f, 0.6f);
    m_shaderProgram->setUniformValue("material.diffuse",  1.0f, 0.5f, 0.6f);
    m_shaderProgram->setUniformValue("material.specular", 0.5f, 0.5f, 0.5f);
    m_shaderProgram->setUniformValue("material.shininess", 32.0f);

    m_shaderProgram->setUniformValue("light.ambient",  0.2f, 0.4f, 0.2f);
    m_shaderProgram->setUniformValue("light.diffuse",  0.7f, 0.7f, 0.7f); // 将光照调暗了一些以搭配场景
    m_shaderProgram->setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);
    //uniform  交互数据处理

    //绘制
    if(modelObj->normalVectorFlag)
    {
        //qDebug()<<"check size:"<<modelObj->indexList.size();
        glDrawArrays(GL_TRIANGLES,0,modelObj->indexList.size());
    }
    else
    {
        glDrawElements(GL_TRIANGLES,modelObj->indexList.size(),GL_UNSIGNED_INT,0);
    }

    //=================模型渲染部分====================
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent *e)
{

    if(e->key()==Qt::Key_Shift)
    {
        m_shift=true;
    }

    if(e->key()==Qt::Key_Alt)
    {
        m_alt=true;
    }

    if(e->key()==Qt::Key_R)
    {

        m_camera.translateMatrix.setToIdentity();
        m_camera.rotationMatrix.setToIdentity();
        m_camera.rotationQuat=QQuaternion();
        m_camera.translateMatrix.translate(m_camera.pos);
        m_camera.scaleMatrix.setToIdentity();

        update();
    }
    if(e->key()==Qt::Key_V)
    {

        m_obj->modelTranslateMatrix.setToIdentity();
        m_obj->modelRotateMatrix.setToIdentity();
        m_obj->modelRotation=QQuaternion();
        m_obj->model.setToIdentity();

        update();
    }

    if(e->key()==Qt::Key_Q)
    {
        qDebug()<<"Q pressed!";
    }

    QOpenGLWidget::keyPressEvent(e);
}

void MyOpenGLWidget::keyReleaseEvent(QKeyEvent *e)
{
    m_shift=false;
    //m_alt=false;
    QOpenGLWidget::keyReleaseEvent(e);
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *e)
{
    QOpenGLWidget::wheelEvent(e);

    int delta=e->delta();
    float scaleFactor = 1.05;
    if(delta>0)
    {
        m_camera.scaleMatrix.scale(scaleFactor);
    }
    else if(delta <0)
    {
        m_camera.scaleMatrix.scale(1.0/scaleFactor);
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

    QQuaternion  rotationY=QQuaternion ::fromAxisAndAngle(m_camera.rightVector,angleY);
    QQuaternion  rotationX=QQuaternion ::fromAxisAndAngle(m_camera.upVector,angleX);

    if ( e->buttons() & Qt::LeftButton )
    {
        //modelObj.value()->modelTranslateMatrix.translate(transXFactor,-transYFactor,0);
        QMatrix4x4 moveMatrix;
        moveMatrix.setToIdentity();
        moveMatrix.rotate(m_camera.rotationQuat);

        QVector3D moveVector(transXFactor,-transYFactor,0);
        moveVector=QVector3D(moveMatrix*moveVector);

        m_obj->modelTranslateMatrix.translate(moveVector);

    }
    if(e->buttons() & Qt::RightButton)
    {
        m_obj->modelRotation=rotationY*rotationX*m_obj->modelRotation;
        m_obj->modelRotateMatrix.setToIdentity();
        m_obj->modelRotateMatrix.rotate(m_obj->modelRotation);
    }

    update();
    m_startPoint=m_endPoint;
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *e)
{
    m_startPoint=e->pos();


    if(m_alt)
    {

        double posX =    posXFromQtToOpenGL(m_startPoint.x());
        double posY =    posYFromQtToOpenGL(m_startPoint.y());
        qDebug()<<"posX"<<posX;
        qDebug()<<"posY"<<posY;
//        QMatrix4x4 tRotateMatrix=m_camera.rotationMatrix.transposed();

        QVector4D rayClip(posX,posY,-1.0,1.0);

        QMatrix4x4 inverseProjection = m_project.inverted();
        QVector4D rayEye = inverseProjection * rayClip;
        rayEye = QVector4D(rayEye.x(), rayEye.y(), -1.0, 0.0);

        m_camera.viewMatrix.setToIdentity();
        m_camera.viewMatrix=m_camera.rotationMatrix*m_camera.viewMatrix;
        m_camera.viewMatrix=m_camera.translateMatrix*m_camera.viewMatrix;
        m_camera.viewMatrix=m_camera.scaleMatrix*m_camera.viewMatrix;

        QMatrix4x4 inverseView = m_camera.viewMatrix.inverted();
        QVector4D rayWorld = inverseView * rayEye;

        QMatrix4x4 tTranslateMatrix;
        tTranslateMatrix.setToIdentity();
//        //tTranslateMatrix.translate(-m_camera.translateMatrix(0,3),-m_camera.translateMatrix(1,3),0);


//        QVector3D spacePos(posX,posY,1.0);

//        QMatrix4x4 tView;

//        tView.setToIdentity();
//        //tView=tRotateMatrix*tView;
//        //tView=tTranslateMatrix*tView;


//        QVector3D res=QVector3D(tView*QVector4D(spacePos));
        QVector3D origPos(m_camera.pos.x(),m_camera.pos.y(),-m_camera.pos.z());
        //QVector3D rayDir(res.x()-origPos.x(),res.y()-origPos.y(),res.z()-origPos.z());
       // QVector3D rayDir=getRayDirection(posX,posY,m_camera.rotationMatrix,m_camera.translateMatrix,origPos);
        //QVector3D rayDir=getRayDirection(posX,posY,m_camera.rotationMatrix,tTranslateMatrix,origPos);
        QVector3D rayDir = rayWorld.toVector3D().normalized();
        //rayDir.setZ(0.1);
        qDebug()<<"-0.5:"<<getIntersectionWithZPlane(origPos,rayDir,-0.5);
        qDebug()<<"0:"<<getIntersectionWithZPlane(origPos,rayDir,0);
        qDebug()<<"0.5:"<<getIntersectionWithZPlane(origPos,rayDir,0.5);
        bool selectFlag=false;
        selectFlag=checkSelectModel(origPos,rayDir,m_obj);
        qDebug()<<"select:"<<selectFlag;
    }

    qDebug()<<"mouse pressed!";
    update();
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{

}

void MyOpenGLWidget::paintEvent(QPaintEvent *e)
{
    QOpenGLWidget::paintEvent(e);
}
