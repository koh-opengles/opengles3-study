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
   shader = glCreateShader ( type ); //hlp:创建shader对象

   if ( shader == 0 )
   {
      return 0;
   }

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL ); //hlp:加载shader源码

   // Compile the shader
   glCompileShader ( shader ); //hlp:编译shader

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled ); //hlp:检查shader编译错误

   if ( !compiled )
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );//hlp:获取具体的shader编译错误信息长度

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog ); //hlp:获取具体的shader编译错误信息
         esLogMessage ( "Error compiling shader:\n%s\n", infoLog ); //hlp:输出错误信息

         free ( infoLog );
      }

      glDeleteShader ( shader ); //hlp:释放shader
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
   vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr ); //hlp:加载定点着色器
   fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr ); //hlp:加载片段着色器

   // Create the program object
   programObject = glCreateProgram ( ); //hlp:创建gl程序对象，注意这里区分下 着色器对象和程序对象。一般都是一个程序对象，链接两个着色器对象

   if ( programObject == 0 )
   {
      return 0;
   }

   glAttachShader ( programObject, vertexShader ); //hlp：链接顶点着色器对象到程序对象
   glAttachShader ( programObject, fragmentShader );//hlp:链接片段着色器对象到程序对象

   // Link the program
   glLinkProgram ( programObject ); //hlp:最终链接程序对象

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked ); //hlp:获取链接状态

   if ( !linked )
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen ); //hlp:如果有错误，那么获取错误长度

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );//hlp:如果有错误，那么获取具体的链接错误
         esLogMessage ( "Error linking program:\n%s\n", infoLog );//hlp:输出错误信息

         free ( infoLog );
      }

      glDeleteProgram ( programObject ); //删除gl程序对象
      return FALSE;
   }

   // Store the program object
   userData->programObject = programObject;

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f
                         };

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height ); //hlp:设置视口

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );  //hlp:清空颜色缓存

   // Use the program object
   glUseProgram ( userData->programObject ); //hlp:使用gl程序对象

   // Load the vertex data
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices ); //hlp:将顶点数据加载到顶点属性0
   glEnableVertexAttribArray ( 0 ); //hlp:启用顶点属性数组

   glDrawArrays ( GL_TRIANGLES, 0, 3 ); //hlp:开始绘制三角形，这样的话，用的顶点数据就是之前设置的。
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteProgram ( userData->programObject ); //删除gl程序对象
}

int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB );//hlp:创建gl窗口

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterShutdownFunc ( esContext, Shutdown ); //hlp:注册关闭时的回调函数
   esRegisterDrawFunc ( esContext, Draw );//hlp:注册gl绘制时的函数

   return GL_TRUE;
}
