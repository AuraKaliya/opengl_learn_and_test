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

	//当用户改变窗口大小时，视口也应该被调整，所以注册一个回调函数，在每次窗口大小被调整时被调用
	//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	//进行注册   -窗口变化监听函数
	glfwSetFramebufferSizeCallback(w,framebuffer_size_callback);


	////		3  进行渲染循环
	while (!glfwWindowShouldClose(w))
	{
		//// 2-1 输入控制
		processInput(w);


		//// 2-2 渲染
		glClearColor(0.2f,0.3f,0.3f,1.0f);		//状态设置函数
		glClear(GL_COLOR_BUFFER_BIT);		//状态使用函数



		glfwSwapBuffers(w);		//交换颜色缓冲--双缓冲机制
		glfwPollEvents();		//检查事件并执行回调
	}

	glfwTerminate();		//释放资源
	return 0;
}