#include "myopenglwidget.h"

#include <QTime>
#include <QRandomGenerator>
MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    m_shaderProgram=new QOpenGLShaderProgram;
    m_timer=new QTimer;
    m_timer->setInterval(100);

    connect(m_timer,&QTimer::timeout,this,[=](){
        timeoutFunc();
    });

    m_transNum=1;
}

void MyOpenGLWidget::draw(Shap shap)
{
    m_shap=shap;
    update();
}

void MyOpenGLWidget::timeoutFunc()
{
    makeCurrent();
//    int timeValue=QTime::currentTime().second();
//    float greenValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
//    float redValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
//    float blueValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
//    m_shaderProgram->setUniformValue("ourColor",redValue,greenValue,blueValue,1.0f);

    //
        int timeValue=QTime::currentTime().second();
        float greenValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
        float redValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
        float blueValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
        m_colors[0]=greenValue;
        m_colors[1]=redValue;
        m_colors[2]=blueValue;
    //

        //变换处理
        m_matrix.setToIdentity();
        unsigned int time=QTime::currentTime().msec();
        //先旋转再移动
        //m_matrix.translate(m_transNum*0.1,m_transNum*(-0.1),0);
        m_matrix.rotate(time,0,0,1.0f);
        //m_matrix.scale(fabs(sin(time)));
        m_transNum=-m_transNum;
        //

    doneCurrent();
    update();
}

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
                                 "}\n\0";

const char * fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "FragColor= vec4(1.0f, 0.2f, 0.4f, 1.0f);\n"
                                   "}\n\0";
float vertices[]={-0.5f,-0.5f,0.0f,0.5f,-0.5f,0.0f,0.0f,0.5f,0.0f};
float vertices2[]={0.5f,0.5f,0.0f,
                     0.5f,-0.5f,0.0f,
                     -0.5f,0.5f,0.0f,
                     -0.5f,-0.5f,0.0f,
                     0.5f,-0.5f,0.0f,
                     -0.5f,0.5f,0.0f };

float vertices3[]={-0.5f,0.5f,0.0f,
                     0.5f,0.5f,0.0f,
                     -0.5f,-0.5f,0.0f,
                     0.5f,-0.5f,0.0f};
float vertices4[]={-0.5f,0.5f,0.0f,1.0,0.0,0.0,
                     0.5f,0.5f,0.0f,0.0,1.0,0.0,
                     -0.5f,-0.5f,0.0f,0.0,0.0,1.0,
                     0.5f,-0.5f,0.0f,0.5,0.5,0.5};
float vertices5[]={-0.5f,0.5f,0.0f,1.0,0.0,0.0,-0.0,1.0,
                     0.5f,0.5f,0.0f,0.0,1.0,0.0,1.0,1.0,
                     -0.5f,-0.5f,0.0f,0.0,0.0,1.0,1.0,0.0,
                     0.5f,-0.5f,0.0f,0.5,0.5,0.5,-0.0,-0.0};

float vertices6[]={-0.5f,-0.5f,-0.5f,0.0f,0.0f,
                    0.5f,-0.5f,-0.5f,1.0f,0.0f,
                    0.5f,0.5f,-0.5f,1.0f,1.0f,
                    0.5f,0.5f,-0.5f,1.0f,1.0f,
                    -0.5f,0.5f,-0.5f,0.0f,1.0f,
                    -0.5f,-0.5f,-0.5f,0.0f,0.0f,

                    -0.5f,-0.5f,-0.5f,0.0f,0.0f,
                    0.5f,-0.5f,0.5f,1.0f,0.0f,
                    0.5f,0.5f,0.5f,1.0f,1.0f,
                    0.5f,0.5f,0.5f,1.0f,1.0f,
                    -0.5f,0.5f,0.5f,0.0f,1.0f,
                    -0.5f,-0.5f,0.5f,0.0f,0.0f,

                    -0.5f,0.5f,0.5f,1.0f,0.0f,
                    -0.5f,0.5f,-0.5f,1.0f,1.0f,
                    -0.5f,-0.5f,-0.5f,0.0f,1.0f,
                    -0.5f,-0.5f,-0.5f,0.0f,1.0f,
                    -0.5f,-0.5f,0.5f,0.0f,0.0f,
                    -0.5f,0.5f,0.5f,1.0f,0.0f,

                    -0.5f,0.5f,-0.5f,0.0f,1.0f,
                    0.5f,0.5f,-0.5f,1.0f,1.0f,
                    0.5f,0.5f,0.5f,1.0f,0.0f,
                    0.5f,0.5f,0.5f,1.0f,0.0f,
                    -0.5f,0.5f,0.5f,0.0f,0.0f,
                    -0.5f,0.5f,-0.5f,0.0f,1.0f,
                     };



float vertices7[]={  -0.5f,-0.5f,-0.5f,0.0f,0.0f, // 0
                     0.5f,-0.5f,-0.5f,1.0f,0.0f, // 1
                     0.5f, 0.5f,-0.5f,1.0f,1.0f, // 2
                     -0.5f, 0.5f,-0.5f,0.0f,1.0f, // 3
                     -0.5f,-0.5f, 0.5f,0.0f,0.0f, // 4
                     0.5f,-0.5f, 0.5f,1.0f,0.0f, // 5
                     0.5f, 0.5f, 0.5f,1.0f,1.0f, // 6
                     -0.5f, 0.5f, 0.5f,0.0f,1.0f, // 7
};

float vertices8[]={-0.5f,-0.5f,-0.5f,0.0f,0.0f, // 1
    0.5f,-0.5f,-0.5f,1.0f,0.0f, // 2
    0.5f,0.5f,-0.5f,1.0f,1.0f, // 3
    0.5f,0.5f,-0.5f,1.0f,1.0f, // 3
    -0.5f,0.5f,-0.5f,0.0f,1.0f, // 4
    -0.5f,-0.5f,-0.5f,0.0f,0.0f, // 1

    -0.5f,-0.5f,0.5f,0.0f,0.0f, // 5
    0.5f,-0.5f,0.5f,1.0f,0.0f, // 6
    0.5f,0.5f,0.5f,1.0f,1.0f, // 7
    0.5f,0.5f,0.5f,1.0f,1.0f, // 7
    -0.5f,0.5f,0.5f,0.0f,1.0f, // 8
    -0.5f,-0.5f,0.5f,0.0f,0.0f, // 5

    -0.5f,0.5f,0.5f,1.0f,0.0f, // 8
    -0.5f,0.5f,-0.5f,1.0f,1.0f, // 4
    -0.5f,-0.5f,-0.5f,0.0f,1.0f, // 1
    -0.5f,-0.5f,-0.5f,0.0f,1.0f, // 1
    -0.5f,-0.5f,0.5f,0.0f,0.0f, // 5
    -0.5f,0.5f,0.5f,1.0f,0.0f, // 8

    0.5f,0.5f,-0.5f,0.0f,1.0f, // 3
    0.5f,0.5f,0.5f,1.0f,1.0f, // 7
    0.5f,-0.5f,0.5f,1.0f,0.0f, // 6
    0.5f,-0.5f,0.5f,1.0f,0.0f, // 6
    0.5f,-0.5f,-0.5f,0.0f,0.0f, // 2
    0.5f,0.5f,-0.5f,0.0f,1.0f, // 3

    -0.5f,-0.5f,-0.5f,0.0f,0.0f, // 1
    0.5f,-0.5f,-0.5f,1.0f,0.0f, // 2
    0.5f,-0.5f,0.5f,1.0f,1.0f, // 6
    0.5f,-0.5f,0.5f,1.0f,1.0f, // 6
    -0.5f,-0.5f,0.5f,0.0f,1.0f, // 5
    -0.5f,-0.5f,-0.5f,0.0f,0.0f, // 1

    -0.5f,0.5f,-0.5f,0.0f,0.0f, // 4
    0.5f,0.5f,-0.5f,1.0f,0.0f, // 3
    0.5f,0.5f,0.5f,1.0f,1.0f, // 7
    0.5f,0.5f,0.5f,1.0f,1.0f, // 7
    -0.5f,0.5f,0.5f,0.0f,1.0f, // 8
    -0.5f,0.5f,-0.5f,0.0f,0.0f, // 4
};

unsigned int indexs2[]={
    0,1,2,3,
    4,5,6,7,
    0,4,7,3,
    1,5,6,2,
    3,2,6,7,
    0,1,5,4,
};

QVector<QVector3D> cubPos={
    QVector3D(0.0f,0.0f,0.0f),
    QVector3D(2.0f,-3.0f,14.0f),
    QVector3D(-3.5f,3.2f,14.0f),
    QVector3D(-4.1f,-3.0f,-6.0f),
    QVector3D(-2.0f,3.9f,-14.0f)
};



unsigned int indexs[]={
    0,1,3,
    1,2,3
};



unsigned int VBO,VAO,EBO;
unsigned int shaderProgram;


//void keyPressEvent(QKeyEvent* e)
//{

//}


void MyOpenGLWidget::initializeGL()
{

    initializeOpenGLFunctions();

    //创建VAO、VBO对象并赋予ID；

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    //绑定VAO、VBO对象（通过ID绑定）
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    //为当前绑定到target的缓冲区对象创建一个新的数据存储
    //如果data不是Null，则使用data的数据初始化数据存储。
    //glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices8),vertices8,GL_STATIC_DRAW);

    //告知显卡如何使用缓冲区内的属性值（解析）
    //使用属标号为0的性，3个内容，每个内容3个float，偏移量为0，不需要标准化。
    //VAO同时对其进行记录
    //glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);

    //glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    //开启VAO中第一个属性值 顶点坐标xyz
    glEnableVertexAttribArray(0);


    //glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    //开启VAO中第2个属性值  --颜色RGB
    //glEnableVertexAttribArray(1);


    //glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    //开启VAO中第3个属性值  --纹理坐标xy
    //glEnableVertexAttribArray(2);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

#if 0

    //编写顶点着色器
    unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);

    int success;
    char infolog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }


    //编写片段着色器
    unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }

    //进行连接
    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);


    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }



    //已使用完，对着色器进行回收
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#endif

    //使用shaderProgram类
    //可以通过返回值来进行验证
    bool success;

    //从字符串中读入源
    //m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vertexShaderSource);
    //m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fragmentShaderSource);

    //从文件中读入源

    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(":/shaders/shapes.vert"));
    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString(":/shaders/shapes.frag"));
    success= m_shaderProgram->link();
    if(!success)
    {
        qDebug()<<"ERR:"<<m_shaderProgram->log();
    }


    //设置绘制模式--正反面，填充方式
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    //glPolygonMode(GL_FRONT,GL_LINE);

    //glPolygonMode(GL_BACK,GL_FILL);

    //默认顺时针是反面，逆时针是正面   GL_CCW 逆时针   GL_CW 顺时针
    glFrontFace(GL_CCW); //将顺时针设置为“正面” 同时逆时针变为反面

    //创建EBO，这时候VAO会进行记录所有操作！（包括解绑）
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indexs2),indexs2,GL_STATIC_DRAW);


    m_texturArgyle=new QOpenGLTexture(QImage(":/argyle1.png").mirrored());
    m_texturOIP=new QOpenGLTexture(QImage(":/OIP.jpg").mirrored());

    m_shaderProgram->bind();        //先绑定！
    m_shaderProgram->setUniformValue("textureEx",1);
    m_shaderProgram->setUniformValue("textureWall",0);


    m_texturArgyle->bind(0);
    //glActiveTexture(GL_TEXTURE1);
    m_texturOIP->bind(1);
    //纹理填充样式 --x方向-y方向
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_BORDER);

    //边界的边框填充
    //glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,m_colors);

    //纹理像素过滤设置
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);    //缩小
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);     //放大

    //


    m_projection.setToIdentity();
    m_projection.perspective(45,(float)width()/height(),0.1,100);
    m_shaderProgram->setUniformValue("projection",m_projection);


    //设置完毕，解绑
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


    m_timer->start();

}

void MyOpenGLWidget::resizeGL(int w, int h)
{

}

void MyOpenGLWidget::paintGL()
{
    //变换处理
//    QMatrix4x4 matrix;
//    unsigned int time=QTime::currentTime().msec();
//    matrix.rotate(time,0,0,1.0f);

    QMatrix4x4 model;
    QMatrix4x4 view;



    //初始化 单位矩阵
    model.setToIdentity();
    view.setToIdentity();


    unsigned int time=QTime::currentTime().msec();
    //变换
    //m_matrix.translate(m_transNum*0.1,m_transNum*(-0.1),0);

    model.rotate(time,1,0.5,1.0f);
    //model.rotate(-45,1,0,0);
    //相对于相机而言平移的坐标
    view.translate(0.0,0.0,-3);


    //m_matrix.scale(fabs(sin(time)));
    //



    //也可以放到逻辑函数中
    //


    glClearColor(0.2f,0.2f,0.3f,1.0f);
    glEnable(GL_DEPTH_TEST);
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

#if 0
    glUseProgram(shaderProgram);
#endif
    m_shaderProgram->bind();


    //绑定VAO，从VAO中取点进行绘制  从0开始拿6个数据
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES,0,6);

    //使用EBO进行绘制
    //glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);




    switch (m_shap) {
    case Rect:
        //纹理
        //glActiveTexture(GL_TEXTURE0);
        m_texturArgyle->bind(0);
        //glActiveTexture(GL_TEXTURE1);
        m_texturOIP->bind(1);
        //边界的边框填充
        //glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,m_colors);
        //m_shaderProgram->setUniformValue("theMatrix",m_matrix);
       // m_shaderProgram->setUniformValue("model",model);
        m_shaderProgram->setUniformValue("view",view);


        //glDrawElements(GL_TRIANGLES,24,GL_UNSIGNED_INT,0);
        //glDrawArrays(GL_TRIANGLES,0,36);
        //glDrawArrays(GL_TRIANGLES,0,36);
        foreach (auto it, cubPos) {
            model.setToIdentity();
            model.translate(it);
            model.rotate(time,1,0.5,1.0f);
            m_shaderProgram->setUniformValue("model",model);
            glDrawArrays(GL_TRIANGLES,0,36);
        }



        break;
    default:
        break;
    }






}
