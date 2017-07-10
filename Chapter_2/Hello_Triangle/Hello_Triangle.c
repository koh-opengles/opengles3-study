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
    //一个数据结构体，里面带一个对象句柄
} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;

   // Create the shader object
    //创建一个着色器
   shader = glCreateShader ( type );

   if ( shader == 0 )
   {
      return 0;
   }

   // Load the shader source
    //为着色器对象句柄指定着色器源代码
   glShaderSource ( shader, 1, &shaderSrc, NULL );

   // Compile the shader
    //编译着色器
   glCompileShader ( shader );

   // Check the compile status
    //查询是否编译状态
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled )
   {
      GLint infoLen = 0;
       //查询错误日志
      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );
          //获取错误日志
         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
          //打印错误日志
         esLogMessage ( "Error compiling shader:\n%s\n", infoLog );
          //释放错误日志
         free ( infoLog );
      }
       //删除多无的着色器对象句柄
      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   char vShaderStr[] =
      "#version 300 es                          \n"
      "layout(location = 0) in vec4 vPosition;  \n"
      "void main()                              \n"
      "{                                        \n"
      "   gl_Position = vPosition;              \n"
      "}                                        \n";

   char fShaderStr[] =
      "#version 300 es                              \n"
      "precision mediump float;                     \n"
      "out vec4 fragColor;                          \n"
      "void main()                                  \n"
      "{                                            \n"
      "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
      "}                                            \n";

   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   // Load the vertex/fragment shaders
    //创建一个顶点着色器
   vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
    //创建一个片段着色器
   fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

   // Create the program object
    //创建一个程序对象
   programObject = glCreateProgram ( );

   if ( programObject == 0 )
   {
      return 0;
   }
    //连接对象和顶点着色器
   glAttachShader ( programObject, vertexShader );
    //连接对象和片段着色器
   glAttachShader ( programObject, fragmentShader );

   // Link the program
    //链接这个对象和他的着色器
   glLinkProgram ( programObject );

   // Check the link status
    //获取对象链接状态
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked )
   {
      GLint infoLen = 0;
       //获取对象错误日志长度
      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );
          //获取对象错误日志
         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
          //打印错误日志
         esLogMessage ( "Error linking program:\n%s\n", infoLog );
          //释放错误日志
         free ( infoLog );
      }
       //删除这个程序对象
      glDeleteProgram ( programObject );
      return FALSE;
   }

   // Store the program object
    //对用户数据的对象进行赋值
   userData->programObject = programObject;
    //把颜色缓冲区设置成白色
   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
    //获取对象数据
   UserData *userData = esContext->userData;
    //创建顶点数据
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f
                         };

   // Set the viewport
    //定义视口大小
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
    //清除颜色缓冲区
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
    //设置程序对象为活动程序
   glUseProgram ( userData->programObject );

   // Load the vertex data
    //加载顶点数据
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
   //启用通用顶点属性数组
    glEnableVertexAttribArray ( 0 );
    //绘制三角形
   glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

void Shutdown ( ESContext *esContext )
{
    //获取对象数据
   UserData *userData = esContext->userData;
    //删除对象数据中的程序对象
   glDeleteProgram ( userData->programObject );
}

int esMain ( ESContext *esContext )
{
    //用户数据空间开辟
   esContext->userData = malloc ( sizeof ( UserData ) );
    //创建一个320*240的窗口，并请求一个RGB缓冲区
   esCreateWindow ( esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }
    //绑定结束句柄
   esRegisterShutdownFunc ( esContext, Shutdown );
    //绑定绘画句柄
   esRegisterDrawFunc ( esContext, Draw );

   return GL_TRUE;
}
