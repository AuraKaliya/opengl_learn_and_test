#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

//glad应该放在glfw之前包含


//step1   启动
/*
   1.使用glfwWindowHint函数来配置GLFW，第一个参数为选项名称，第二个参数接受一个整型，用于设置这个选项的值。
   2.使用glfwCreateWindow函数创建一个窗口对象并返回，之后通知GLFW将窗口的上下文设置为当前线程的主上下文。
   3.进行循环渲染，使用glfwWindowShouldClose在每次渲染帧开始检查GLFW是否被要求退出；glfwPollEvents检查是否触发其他事件（键盘输入、鼠标移动），进行更新状态和回调函数的调用；glfwSwapBuffers交换颜色缓冲；
 */

//step2  
/*
	1.	输入控制。   --写成一个函数，使代码保持整洁
	使用glfwGetKey检测按键按下事件。

	2.	渲染 。 在新的渲染迭代开始时先清空颜色缓冲，然后再进行渲染。

*/

//step3
/*
	绘制三角形
	1.定义顶点组
	2.定义VBO顶点缓冲并进行绑定。 glGenBuffers   glBindBuffer
	3.数据传递   glBufferData  --第四个参数希望显卡如何管理给定的数据  GL_STATIC_DRAW | GL_DYNAMIC_DRAW | GL_STREAM_DRAW  
*/

//step4
/*
	定义着色器--通常定义顶点着色器和片段着色器

	顶点着色器：对输入的顶点数据进行整合，形成 点
	片段着色器：计算像素颜色输出

	1. 创建顶点着色器对象，编写顶点着色器代码
	2. 编译顶点着色器,进行检查
	3. 创建片段着色器对象，编写片段着色器代码
	4. 编译片段着色器，进行检查
*/


//step5
/*
	合并、连接着色器，形成一个着色器程序对象。

	1.定义着色器程序对象
	2.进行连接
	3.使用
	4.删除着色器
*/


//step6
/*
	在openGL中绘制物体
	1. 复制顶点数组到缓冲中  VBO中	glBindBUffer  glBufferData
	2. 设置顶点属性指针 （处理方式设定） glVertexAttribPointer	glEnableVertexAttribArray
	3. 使用着色器程序  glUseProgram
	4. 绘制物体  someOpenGLFunctionThatDrawsOurTriangle
*/

//step7
/*
	使用VAO管理配置属性，使用VBO管理顶点数据
	1. 创建VAO 进行绑定
	2. 配置VBO
	3. 配置完成 解绑
	4. 需要绘制时，进行绑定，绘制。
*/

//setp8
/*
	使用EBO管理顶点集，达到去重绘制的目的
	vertices 顶点集   indices  索引列表
	1.创建索引缓冲对象EBO 进行绑定 传递数据
	2.使用glDrawElements替换glDrawArrays 表示从EBO中取索引绘制

	EBO和VBO的绑定策略都存储在VAO中
*/

//step9
/*
	glPolygonMode用于配置 如何绘制 图元
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)第一个参数表示我们打算将其应用到所有的三角形的正面和背面，第二个参数告诉我们用线来绘制。
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)将其设置回默认模式
*/


//step10
/*
	练习1：添加更多顶点到数据中，使用glDrawArrays，尝试绘制两个彼此相连的三角形
	练习2：创建相同的两个三角形，但对它们的数据使用不同的VAO和VBO
	练习3：创建两个着色器程序，第二个程序使用一个不同的片段着色器，输出黄色；再次绘制这两个三角形，让其中一个输出为黄色。
*/

//step11
/*
	GLSL 
	1. uniform   在fragment中定义一个uniform变量 ，在外部找到它的索引并进行变更。

*/



void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		std::cout << "sec is pressed" << std::endl;
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0,width,height);
}


//定义顶点组
//float vertices[] = {
//	-0.5f, -0.5f, 0.0f,
//	 0.5f, -0.5f, 0.0f,
//	 0.0f,  0.5f, 0.0f,
//};

//
////定义顶点集
//float vertices[] = {
//	0.5f, 0.5f, 0.0f,   // 右上角
//	0.5f, -0.5f, 0.0f,  // 右下角
//	-0.5f, -0.5f, 0.0f, // 左下角
//	-0.5f, 0.5f, 0.0f   // 左上角
//};

//练习-绘制两个相连的三角形-顶点集
//float vertices[] = {
//
//	0.0f,-0.5f,0.0f,
//	-0.5f,0.0f,0.0f,
//	0.0f,0.5f,0.0f,
//
//	
//	0.0f,-0.5f,0.0f,
//	0.0f,0.5f,0.0f,
//	0.5f,0.0f,0.0f
//};

//练习-用不同VAO VBO绘制三角形
float vertices[] = {

	0.0f,-0.5f,0.0f,
	-0.5f,0.0f,0.0f,
	0.0f,0.5f,0.0f,

	
	0.0f,-0.5f,0.0f,
	0.0f,0.5f,0.0f,
	0.5f,0.0f,0.0f
};






//定义索引表
unsigned int indices[] = {
	0, 1, 3, // 第一个三角形
	1, 2, 3  // 第二个三角形
};


//顶点着色器代码
const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in  vec3 aPos;\n"
"out vec4 vertexColor1;\n"
"out vec4 vertexColor2;\n"
"void main()\n"
"{\n"
" gl_Position=vec4(aPos.x,aPos.y,aPos.z,1.0f);\n"
" gl_PointSize=10.0f;\n"
"vertexColor1=vec4(1.0f,0.5f,0.2f,1.0f);\n"
"vertexColor2=vec4(0.0f,0.5f,0.2f,1.0f);\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 myColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f,0.5f,0.2f,1.0f);\n"
"}\0";


const char* fragmentShaderSource1 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor1;\n"
"uniform vec4 myColor1;\n"
"void main()\n"
"{\n"
"FragColor = vec4(vertexColor1.r,myColor1.g,vertexColor1.b,vertexColor1.a);\n"
"}\0";

const char* fragmentShaderSource2 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor2;\n"
"uniform vec4 myColor2;\n"
"void main()\n"
"{\n"
"FragColor = myColor2;\n"
"}\0";








int main()
{
	////		1
	//初始化
	glfwInit();

	//设置版本--主要版本3 次要版本3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);

	//设置模式-核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);


	////		2  创建窗口对象
	GLFWwindow* w = glfwCreateWindow(800,600,"LearnOpenGL",NULL,NULL);
	if (w == NULL)
	{
		std::cout << "FAILED TO CREATE GLFW WINDOW" << std::endl;
		glfwTerminate();		//结束线程
		return -1;
	}
	glfwMakeContextCurrent(w);		//将该窗口的上下文设置为当前线程的上下文

	//GLAD是用来管理OpenGL的函数指针的，再调用OpenGL的函数之前需要初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize GLAD" << std::endl;
		return -1;
	}

	//设置渲染窗口的维度(Dimension)   --设置视口
	glViewport(0,0,800,600);      //--左下角位置    宽高      --将-1~1的值映射到0~800 0~600之内

	//启用参数
	glEnable(GL_PROGRAM_POINT_SIZE);

	//当用户改变窗口大小时，视口也应该被调整，所以注册一个回调函数，在每次窗口大小被调整时被调用
	//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	//进行注册   -窗口变化监听函数
	glfwSetFramebufferSizeCallback(w,framebuffer_size_callback);



	//练习-用不同VAO VBO绘制三角形
	unsigned int VAOS[2],VBOS[2];
	glGenVertexArrays(2, VAOS);
	glGenBuffers(2, VBOS);
	//绑定第一个VAO
	glBindVertexArray(VAOS[0]);
	//绑定缓冲
	glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
	//填充数据	--传递顶点	GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//绑定第二个VAO
	glBindVertexArray(VAOS[1]);
	//绑定缓冲
	glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]);
	//填充数据	--传递顶点	GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(9*sizeof(float)));
	glEnableVertexAttribArray(0);









	//生成VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	//绑定配置
	glBindVertexArray(VAO);

	//生成VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	//绑定缓冲
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//填充数据	--传递顶点	GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);





	//生成EBO     --填充之后渲染的函数（渲染策略）也会改变
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	//绑定元素缓冲   --传递索引   GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

	


	

	//设置顶点属性指针  --配置
	//glVertexAttribPointer 第一个参数对应location  第二个参数一组属性多少个点  最后一个参数起始偏移量
	//glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	//练习-绘制两个相连的三角形-配置属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
	glEnableVertexAttribArray(0);



	//配置设置完成，解绑VAO
	glBindVertexArray(0);








	//创建顶点着色器对象
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);


	//编译顶点着色器   
	//glShaderSource 第二个参数指定的是传入的字符串的数量
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//检查编译是否成功
	int success;

	char infoLog[512];
	glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
		std::cout << "error : v shader failed to compilation;\n" << infoLog << std::endl;
	}


	//练习3
	unsigned int fragmentShader1, fragmentShader2;
	fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
	glCompileShader(fragmentShader1);
	glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
		std::cout << "error : f shader failed to compilation;\n" << infoLog << std::endl;
	}

	glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "error : f shader failed to compilation;\n" << infoLog << std::endl;
	}

	//练习3
	unsigned int shaderProgram1,shaderProgram2;
	shaderProgram1 = glCreateProgram();
	shaderProgram2 = glCreateProgram();

	//进行连接
	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShader1);
	glLinkProgram(shaderProgram1);
	//检查
	glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
		std::cout << "error : failed link;\n" << infoLog << std::endl;
	}

	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);
	//检查
	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "error : failed link;\n" << infoLog << std::endl;
	}






	//创建片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//编译片段着色器
	glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
	glCompileShader(fragmentShader);

	//检查编译是否成功
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "error : f shader failed to compilation;\n" << infoLog << std::endl;
	}


	//定义着色器程序对象
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	//进行连接
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	glLinkProgram(shaderProgram);

	//检查
	glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
		std::cout << "error : failed link;\n" << infoLog << std::endl;
	}




	//使用着色器程序
	glUseProgram(shaderProgram);





	//删除着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//练习3
	glDeleteShader(fragmentShader1);
	glDeleteShader(fragmentShader2);

	//test  打印能声明的顶点属性个数
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;


	//uniform
	float timeValue = glfwGetTime();
	
	float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	std::cout << greenValue << std::endl;
	int vertexColorLocation1 = glGetUniformLocation(shaderProgram1, "myColor1");
	glUseProgram(shaderProgram1);
	glUniform4f(vertexColorLocation1, 0.0f, greenValue, 0.0f, 1.0f);

	int vertexColorLocation2 = glGetUniformLocation(shaderProgram2, "myColor2");
	glUseProgram(shaderProgram2);
	glUniform4f(vertexColorLocation2, 0.0f, greenValue, 0.0f, 1.0f);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	////		3  进行渲染循环
	while (!glfwWindowShouldClose(w))
	{
	
		//前处理
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		glUseProgram(shaderProgram1);
		glUniform4f(vertexColorLocation1, 0.0f, greenValue, 0.0f, 1.0f);
		glUseProgram(shaderProgram2);
		glUniform4f(vertexColorLocation2, 0.0f, greenValue, 0.0f, 1.0f);


		//// 2-1 输入控制
		processInput(w);


		//// 2-2 渲染
		glClearColor(0.2f,0.3f,0.3f,1.0f);		//状态设置函数
		glClear(GL_COLOR_BUFFER_BIT);		//状态使用函数

		//使用VAO VBO的形式
		glUseProgram(shaderProgram);
		//绑定
		glBindVertexArray(VAO);

		


		//--------------------绘制

		//	从VBO中取点绘制
		
		//glDrawArrays(GL_TRIANGLES,0,3);	//三角形
		//glDrawArrays(GL_LINE_LOOP,0,3);		//线段
		//glDrawArrays(GL_POINTS, 0, 3);		//点
		//glDrawArrays(GL_LINE_LOOP, 0, 3);

		//练习-绘制两个相连的三角形-从VBO中绘制
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		
		//练习-使用不同VAO VBO绘制
	/*	glBindVertexArray(VAOS[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(VAOS[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);*/

		//练习3
		glUseProgram(shaderProgram1);
		glBindVertexArray(VAOS[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		glUseProgram(shaderProgram2);
		glBindVertexArray(VAOS[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);




		//	从EBO中取索引绘制

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);


		//--------------------绘制


		//绘制完成，解绑
		glBindVertexArray(0);

		glfwSwapBuffers(w);		//交换颜色缓冲--双缓冲机制
		glfwPollEvents();		//检查事件并执行回调
	}


	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(2, VAOS);
	glDeleteBuffers(2, VBOS);

	glDeleteProgram(shaderProgram);





	glfwTerminate();		//释放资源
	return 0;
}