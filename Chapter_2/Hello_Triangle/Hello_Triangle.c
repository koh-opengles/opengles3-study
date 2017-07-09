// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// Hello_Triangle.c
//
//    This is a simple example that draws a single triangle with
//    a minimal vertex/fragment shader.  The purpose of this
//    example is to demonstrate the basic concepts of
//    OpenGL ES 3.0 rendering.
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

} UserData; //定义一个用户数据，用来存放程序句柄

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc ) //定义一个加载shader的函数，可以返回shader的写法正确与否
{
   GLuint shader; //shader的句柄
   GLint compiled; //存放shader的编译结果

   // Create the shader object
   shader = glCreateShader ( type );//通过传入参数判断创建的shader类型，顶点还是片段

   if ( shader == 0 )//创建失败
   {
      return 0;
   }

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );//把写好的shader代码加载到显卡上

   // Compile the shader
   glCompileShader ( shader );//编译加载到显卡上的shader代码

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );//把编译结果返回到compiled上

   if ( !compiled )//如果编译失败
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );//获取编译失败信息编码

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );//查找错误编码信息对应的描述
         esLogMessage ( "Error compiling shader:\n%s\n", infoLog );

         free ( infoLog );
      }

      glDeleteShader ( shader );//删除失败的shader对象
      return 0;
   }

   return shader;//返回成功的shader

}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;//创建一个对象为es程序上下文中的特定结构体对象
    //顶点shader的代码文本
   char vShaderStr[] =
      "#version 300 es                          \n"
      "layout(location = 0) in vec4 vPosition;  \n"
      "void main()                              \n"
      "{                                        \n"
      "   gl_Position = vPosition;              \n"//传入什么点就传出什么点，不做处理
      "}                                        \n";

    //片段shader的代码文本
   char fShaderStr[] =
      "#version 300 es                              \n"
      "precision mediump float;                     \n"
      "out vec4 fragColor;                          \n"
      "void main()                                  \n"
      "{                                            \n"
      "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"//传入任何颜色，都被改成红色传出
      "}                                            \n";

   GLuint vertexShader;//顶点着色器的对象句柄
   GLuint fragmentShader;//片段着色器的对象句柄
   GLuint programObject;//gl程序对象
   GLint linked;//是否连接的标识

   // Load the vertex/fragment shaders
   vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );//创建一个顶点着色器
   fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );//创建一个片段着色器

   // Create the program object
   programObject = glCreateProgram ( );//创建一个gl程序对象

   if ( programObject == 0 )//创建失败，各种原因
   {
      return 0;
   }

   glAttachShader ( programObject, vertexShader );//把顶点着色器加载到gl程序对象上
   glAttachShader ( programObject, fragmentShader );//把片段着色器加载到gl程序对象上

   // Link the program
   glLinkProgram ( programObject );//连接程序对象

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );//查看连接状态

   if ( !linked )//连接失败
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         esLogMessage ( "Error linking program:\n%s\n", infoLog );

         free ( infoLog );
      }

      glDeleteProgram ( programObject );
      return FALSE;
   }

   // Store the program object
   userData->programObject = programObject;//把程序对象储存到用户数据中

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );//设置指定色刷新颜色缓冲区中的颜色
   return TRUE;//初始化成功
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
    //三角形的三个角的坐标，三维坐标系
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f
                         };

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );//设置视口为窗口的大小，无边界

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );//刷新颜色缓冲区

   // Use the program object
   glUseProgram ( userData->programObject );//使用传入draw函数的es对象创建并初始化gl程序

   // Load the vertex data
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );//传入顶点数据数组
   glEnableVertexAttribArray ( 0 );//开启顶点属性数据

   glDrawArrays ( GL_TRIANGLES, 0, 3 );//画三角形，从数组缓存中的0位开始，画3个顶点数据
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteProgram ( userData->programObject );//删除传入的gl程序
}

//以下是es工具会被调用的主函数
int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterShutdownFunc ( esContext, Shutdown );//设置关闭窗口后调用的函数
   esRegisterDrawFunc ( esContext, Draw );//设置绘画函数,会被一直调用

   return GL_TRUE;
}
