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
    m_rayShaderProgram=new QOpenGLShaderProgram;
    m_modelDir=new QDir("E:/qtFile/TestOpenGLByMe_3/model");


    initSetting();
    initObjectDictionary();

    m_textShow=false;
    m_testFlag=false;
    m_rayFlag=false;


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
    for(auto it:m_modelList)
    {
        MyOpenGLObject* obj=new MyOpenGLObject;
        m_objDictionary.insert(it,obj);
        obj->name=it;
        //qDebug()<<it;
        obj->normalVectorFlag=false;
        obj->objNormalFlag=false;
        m_posList.clear();
        m_indexList.clear();
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

void MyOpenGLWidget::reInitObject(MyOpenGLObject *obj)
{
    //因posList等改变而需要重新递送数据


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

        //glBufferData(GL_ARRAY_BUFFER,sizeof(obj->dataList.data())*obj->dataList.size(),obj->dataList.data(),GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(obj->dataList.data())*obj->dataList.size(),obj->dataList.data());

        glVertexAttribPointer(0,3,GL_DOUBLE,GL_FALSE,6*sizeof(double),(void*)0);
        glVertexAttribPointer(1,3,GL_DOUBLE,GL_FALSE,6*sizeof(double),(void*)(3*sizeof(double)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

    }else
    {
        qDebug()<<" update!";
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(obj->posList.data())*obj->posList.size(),obj->posList.data());
        glVertexAttribPointer(0,3,GL_DOUBLE,GL_FALSE,3*sizeof(double),(void*)0);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(obj->indexList.data())*obj->indexList.size(),obj->indexList.data(),GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,sizeof(obj->indexList.data())*obj->indexList.size(),obj->indexList.data());
        glEnableVertexAttribArray(0);
    }
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

    m_xColor=QVector3D(1.0,0.0,0.0);
    m_yColor=QVector3D(0.0,1.0,0.0);
    m_zColor=QVector3D(0.0,0.0,1.0);

    m_sun.setToIdentity();

    //m_projection.setToIdentity();
    //m_projection.ortho(-1,1,-1,1,0,-1);

    m_shift=false;

    //光源点
    m_lightPos=QVector3D(0,2,0);
    //m_viewPos=QVector3D(0,0,1);

    m_camera.rightVector=QVector3D(1,0,0);
    m_camera.frontVector=QVector3D(0,0,1);
    m_camera.upVector=QVector3D(0,1,0);
    m_camera.pos=QVector3D(0,0,1);

    m_camera.viewMatrix.setToIdentity();
    m_camera.rotationMatrix.setToIdentity();
    m_camera.translateMatrix.setToIdentity();
    m_camera.translateMatrix.translate(m_camera.pos);


    //m_camera.zoom=45.0f;
    //m_camera.projection.setToIdentity();
    //m_camera.projection.perspective(qRadiansToDegrees(m_camera.zoom), float(width()) / float(height()), 0.1f, 100.0f);


    setFocusPolicy(Qt::ClickFocus);
}

void MyOpenGLWidget::initObjectDictionary()
{
    initDirectionObject();
    initLightObject();
    read();
    initRayObject();
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

void MyOpenGLWidget::initRayObject()
{
    //ray================
    MyOpenGLObject* rayObj=new MyOpenGLObject;
    m_objDictionary.insert("Ray",rayObj);
    rayObj->name="Ray";
    rayObj->normalVectorFlag=false;
    rayObj->objNormalFlag=false;

    rayObj->posList<<-m_camera.pos.x()<<-m_camera.pos.y()<<-m_camera.pos.z()
                  << 1<<0<<0
                     ;
    rayObj->indexList<<0<<1
                     ;
    //================
}

void MyOpenGLWidget::updateRay()
{

    //===================================屏幕坐标转opengl空间坐标
    double posX =    posXFromQtToOpenGL(m_startPoint.x());
    double posY =    posYFromQtToOpenGL(m_startPoint.y());
    //qDebug()<<"pos:"<<posX<<posY;

    //=====================================变换矩阵求逆向

   // qDebug()<<"==========old Matrix=========";
    //qDebug()<<"View Rotate    :"<<m_viewRotateMatrix;
   // qDebug()<<"View Translate :"<<m_viewTranslateMatrix;

    QMatrix4x4 tRotateMatrix=m_camera.rotationMatrix.transposed();

    QMatrix4x4 tTranslateMatrix;
    tTranslateMatrix.setToIdentity();
    tTranslateMatrix.translate(-m_camera.translateMatrix(0,3),-m_camera.translateMatrix(1,3),0);

    //qDebug()<<"==========now Matrix=========";
    //qDebug()<<"View Rotate    :"<<tRotateMatrix;
    //qDebug()<<"View Translate :"<<tTranslateMatrix;

    //QMatrix4x4

    QVector3D spacePos(posX,posY,1.0);

    QMatrix4x4 tView;
    tView.setToIdentity();
    //tView=tRotateMatrix*tView;
    //tView=tTranslateMatrix*tView;

    QVector3D res=QVector3D(tView*QVector4D(spacePos));
    //qDebug()<<"Res: "<<res;
    MyOpenGLObject * obj=m_objDictionary.find("Ray").value();
    obj->posList.clear();
    obj->posList<<m_camera.pos.x()<<m_camera.pos.y()<<-m_camera.pos.z()
               <<res.x()<<res.y()<<res.z()
                 ;
    reInitObject(obj);
    QVector3D origPos(m_camera.pos.x(),m_camera.pos.y(),-m_camera.pos.z());
    QVector3D rayDir(res.x()-origPos.x(),res.y()-origPos.y(),res.z()-origPos.z());
    MyOpenGLObject* modelObj=m_objDictionary.find("cube2.obj").value();


    bool selectFlag=false;
    selectFlag=checkSelectModel(origPos,rayDir,modelObj);

    qDebug()<<"select:"<<selectFlag;


    m_rayFlag=true;

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
    QVector3D origPos(0,0,0);


    //true
    QVector3D rayDir(1,0,0);
    //false
    QVector3D rayDir2(0,1,0);
    //true
    QVector3D rayDir3(1,0.1,0);


    QVector3D p1(1,1,0);
    QVector3D p2(1,-1,-1);
    QVector3D p3(1,-1,1);



    qDebug()<<"p1"<<p1;
    qDebug()<<"p2"<<p2;
    qDebug()<<"p3"<<p3;
    qDebug()<<"=========================================";

    QVector3D v0(p2-p1);
    QVector3D v1(p3-p2);
    QVector3D v2(p1-p3);
    qDebug()<<"v1"<<v0;
    qDebug()<<"v2"<<v1;
    qDebug()<<"v3"<<v2;
    qDebug()<<"=========================================";

    double t;
    double u;
    double v;


    bool res=intersectTriangle(origPos,rayDir,v0,v1,v2,&t,&u,&v);
        qDebug()<<"Res: "<<res;
    res=intersectTriangle(origPos,rayDir2,v0,v1,v2,&t,&u,&v);
        qDebug()<<"Res: "<<res;
    res=intersectTriangle(origPos,rayDir3,v0,v1,v2,&t,&u,&v);
        qDebug()<<"Res: "<<res;



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
    m_rayShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString( "E:/qtFile/TestOpenGLByMe_3/test.frag"));
    m_rayShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(   "E:/qtFile/TestOpenGLByMe_3/test.vert"));
    success= m_rayShaderProgram->link();
    if(!success)
    {
        qDebug()<<"ERR direct:"<<m_rayShaderProgram->log();
    }




    for(auto it=m_objDictionary.begin();it!=m_objDictionary.end();++it)
    {
        initObject(it.value());
    }

    //初始化model的顶点对应文字
    for(auto it:m_modelList)
    {
        auto modelObject=m_objDictionary.find(it);
        if(modelObject!=m_objDictionary.end())
        {
            initTextByObject(modelObject.value());
        }
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

    // 设置深度缓存
    glClearDepth(1.0f);

    // 告诉系统对透视进行修正
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor(0.2f,0.5f,0.5f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT,GL_FILL);
    // 启用阴影平滑
    glShadeModel(GL_SMOOTH);

    //glFrontFace(GL_CW);

//    //更新view
//    m_view.setToIdentity();
//    //m_view.translate(m_viewPos);
//    m_view=m_viewRotateMatrix*m_view;
//    m_view=m_viewTranslateMatrix*m_view;
//    m_view=m_viewScaleMatrix*m_view;

    //  更新Camera
    m_camera.viewMatrix.setToIdentity();
    m_camera.viewMatrix=m_camera.rotationMatrix*m_camera.viewMatrix;
    m_camera.viewMatrix=m_camera.translateMatrix*m_camera.viewMatrix;
    m_camera.viewMatrix=m_camera.scaleMatrix*m_camera.viewMatrix;

   // m_camera.projection.setToIdentity();
   // m_camera.projection.perspective(qRadiansToDegrees(m_camera.zoom), float(width()) / float(height()), 0.1f, 100.0f);
    //m_camera.projection.ortho(-1,1,-1,1,0,-1);
    //=================预处理====================

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
   // m_lightShaderProgram->setUniformValue("view",m_view);
    m_lightShaderProgram->setUniformValue("view",m_camera.viewMatrix);
    m_lightShaderProgram->setUniformValue("lightPos",m_lightPos);
    m_lightShaderProgram->setUniformValue("ourColor",m_xColor);
    glPointSize( 16.0f );

    glDrawElements(GL_POINTS,1,GL_UNSIGNED_INT,0);

    //===================LIGHT部分=====================

    //=================模型渲染部分====================
    m_shaderProgram->bind();

    //m_camera.projection.setToIdentity();
    //m_camera.projection.ortho(-1,1,-1,1,-1,1);

    //m_shaderProgram->setUniformValue("projection",m_camera.projection);
    //m_camera.viewMatrix=m_camera.projection*m_camera.viewMatrix;

    m_lightShaderProgram->setUniformValue("view",m_camera.viewMatrix);
    m_shaderProgram->setUniformValue("lightPos",m_lightPos);
    m_shaderProgram->setUniformValue("viewPos",m_camera.pos);
    for(auto it: m_modelList)
    {
        MyOpenGLObject* modelObj=m_objDictionary.find(it).value();
        glBindVertexArray(modelObj->vao);

        //更新model
        modelObj->model.setToIdentity();
        modelObj->model=modelObj->modelRotateMatrix*modelObj->model;
        modelObj->model=modelObj->modelTranslateMatrix*modelObj->model;
       // modelObj->model.rotate();


        //=====  最好应该是用obj内的model的数据，但会引出一个新需求：obj选中列表进行变换处理。
        m_shaderProgram->setUniformValue("model",modelObj->model);
        //m_shaderProgram->setUniformValue("view",m_view);
        //m_shaderProgram->setUniformValue("scale",m_viewScaleMatrix);
        //m_shaderProgram->setUniformValue("viewPos",m_viewPos);





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
                    QVector3D pos=QVector3D(  m_camera.viewMatrix*modelObj->model*QVector4D(it->spacePoint.x(),
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

    //=================模型渲染部分====================


    //=================test部分====================
    //==========ray=========
    if(m_rayFlag)
    {
        m_rayShaderProgram->bind();
        MyOpenGLObject* rayObj=m_objDictionary.find("Ray").value();
        glBindVertexArray(rayObj->vao);
        m_rayShaderProgram->setUniformValue("view",m_camera.viewMatrix);
        m_rayShaderProgram->setUniformValue("ourColor",m_yColor);

        //glPointSize( 16.0f );
        //glDrawElements(GL_LINES,2,GL_UNSIGNED_INT,0);
        glPointSize( 16.0f );
        glDrawArrays(GL_POINTS,0,1);
        glDrawArrays(GL_LINES,0,2);
    }

    //=================test部分====================




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
        for(auto it :m_objDictionary)
        {
            it->modelTranslateMatrix.setToIdentity();
            it->modelRotateMatrix.setToIdentity();
            it->modelRotation=QQuaternion();
            it->model.setToIdentity();
        }
//        m_viewTranslateMatrix.setToIdentity();
//        m_viewTranslateMatrix.translate(-m_viewPos);
//        m_viewRotateMatrix.setToIdentity();
//        m_viewRotation=QQuaternion();
//        m_viewScaleMatrix.setToIdentity();
        m_camera.translateMatrix.setToIdentity();
        m_camera.rotationMatrix.setToIdentity();
        m_camera.rotationQuat=QQuaternion();
        m_camera.translateMatrix.translate(m_camera.pos);
        m_camera.scaleMatrix.setToIdentity();

        //m_view.setToIdentity();

        update();
    }
    if(e->key()==Qt::Key_V)
    {
        for(auto it :m_objDictionary)
        {
            it->modelTranslateMatrix.setToIdentity();
            it->modelRotateMatrix.setToIdentity();
            it->modelRotation=QQuaternion();
            it->model.setToIdentity();
        }
        //m_view.setToIdentity();

        update();
    }


    if(e->key()==Qt::Key_Q)
    {
        m_textShow=!m_textShow;
       //qDebug()<<"now show:"<<m_textShow;
        repaint();
    }

    QOpenGLWidget::keyPressEvent(e);
}

void MyOpenGLWidget::keyReleaseEvent(QKeyEvent *e)
{
    m_shift=false;
    m_alt=false;
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

    QQuaternion  rotationY=QQuaternion ::fromAxisAndAngle(m_rightVector,angleY);
    QQuaternion  rotationX=QQuaternion ::fromAxisAndAngle(m_upVector,angleX);

    //m_shift
    if(m_shift)
    {
        if ( e->buttons() & Qt::LeftButton )
        {
//            m_viewTranslateMatrix.translate(transXFactor,-transYFactor,0);

            m_camera.translateMatrix.translate(transXFactor,-transYFactor,0);

            //00000  视角固定版本=======
//            for(auto it:m_modelList)
//            {
//                auto modelObj=m_objDictionary.find(it);
//                if(modelObj!=m_objDictionary.end())
//                {
//                    modelObj.value()->modelTranslateMatrix.translate(transXFactor,0,-transYFactor);
//                }

//            }
            //00000  视角固定版本=======

        }

        if(e->buttons() & Qt::RightButton)
        {
//            m_viewRotation=rotationY*rotationX*m_viewRotation;
//            m_viewRotateMatrix.setToIdentity();
//            m_viewRotateMatrix.rotate(m_viewRotation);

            m_camera.rotationQuat=rotationY*rotationX*m_camera.rotationQuat;
            m_camera.rotationMatrix.setToIdentity();
            m_camera.rotationMatrix.rotate(m_camera.rotationQuat);
        }
    }
    else
    {
        if ( e->buttons() & Qt::LeftButton )
        {
            for(auto it:m_modelList)
            {
                auto modelObj=m_objDictionary.find(it);
                if(modelObj!=m_objDictionary.end())
                {
                    //modelObj.value()->modelTranslateMatrix.translate(transXFactor,-transYFactor,0);

                    QMatrix4x4 moveMatrix;
                    moveMatrix.setToIdentity();
                    moveMatrix.rotate(m_camera.rotationQuat);

                    QVector3D moveVector(transXFactor,-transYFactor,0);
                    moveVector=QVector3D(moveMatrix*moveVector);

                    modelObj.value()->modelTranslateMatrix.translate(moveVector);

                }
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
    if(m_alt)
    {
        updateRay();
    }

    update();
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{

}

void MyOpenGLWidget::paintEvent(QPaintEvent *e)
{
    QOpenGLWidget::paintEvent(e);
}
