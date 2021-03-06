//
//  main.cpp
//  OpenGLEmpty
//
//  Created by yz on 2020/7/23.
//  Copyright © 2020 yz. All rights reserved.
//

#include <stdio.h>
#include "GLTools.h"
#include <glut/glut.h>
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "math3d.h"

//简单的批次容器。是GLTools的一个简单的容器类
GLBatch triangleBatch;

//着色器管理
GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix; //模型视图矩阵
GLMatrixStack projectionMatrix;//投影矩阵
GLFrustum viewFrustum; //视景体
GLGeometryTransform transformPipeLine;//几何变换管道

//批次类
GLBatch floorBatch;//地板
GLBatch ceilingBatch;//天花板
GLBatch leftWallBatch;//左侧墙体
GLBatch rightWallBatch;//右侧墙体

//深度初始值
GLfloat zView = -65;

//纹理标识符
#define TEXTURE_BRICK 0 //墙面
#define TEXTURE_FLOOR 1 //地板
#define TEXTURE_CEILIN 2 //天花板
#define TEXTURE_COUNT 3 //纹理个数

GLuint textures[TEXTURE_COUNT];//纹理标记数组
//纹理文件名数字
const char *texturesFiles[TEXTURE_COUNT]={"brick.tga","floor.tga","ceiling.tga"};

//为程序做一次性设置
void SetUpRC(){
    //背景色
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //初始化着色器
    shaderManager.InitializeStockShaders();
    
    GLbyte *pBytes;
    GLint iWidth,iHeight,iComponents;
    GLenum eFormat;
    GLint iLoop;
    
    //生成纹理标记
    /**
     分配纹理对象，glGenTextures(<#GLsizei n#>, <#GLuint *textures#>)
     参数1 纹理对象个数
     参数2 纹理对象标识数组
     */
    glGenTextures(TEXTURE_COUNT, textures);
    //循环设置纹理数组的纹理参数
    for (iLoop =0; iLoop<TEXTURE_COUNT ; iLoop ++) {
        //绑定纹理对象
        /*glBindTexture(<#GLenum target#>, <#GLuint texture#>)
         参数1:纹理模式GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_3D
         参数2:需要绑定的纹理对象
         */
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        //加载纹理tga文件
        /*
         gltReadTGABits(<#const char *szFileName#>, <#GLint *iWidth#>, <#GLint *iHeight#>, <#GLint *iComponents#>, <#GLenum *eFormat#>)
         参数1:纹理文件名称
         参数2:文件宽带变量地址
         参数3:文件高度变量地址
         参数4:文件组件变量地址
         参数5:文件格式变量地址
         返回值pBytes:指向图像数据的指针*/
        pBytes = gltReadTGABits(texturesFiles[iLoop], &iWidth, &iHeight, &iComponents, &eFormat);
        
        //加载纹理、设置过滤器、和包装模式
        //GL_TEXTURE_MAG_FILTER（放大过滤器,GL_NEAREST(最邻近过滤)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //GL_TEXTURE_MIN_FILTER(缩小过滤器),GL_NEAREST(最邻近过滤)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_TEXTURE_WRAP_S(s轴环绕),GL_CLAMP_TO_EDGE(环绕模式强制对范围之外的纹理坐标沿着合法的纹理单元的最后一行或一列进行采样)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //GL_TEXTURE_WRAP_T(t轴环绕)，GL_CLAMP_TO_EDGE(环绕模式强制对范围之外的纹理坐标沿着合法的纹理单元的最后一行或一列进行采样)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        //载入纹理
        /*
         glTexImage2D(<#GLenum target#>, <#GLint level#>, <#GLint internalformat#>, <#GLsizei width#>, <#GLsizei height#>, <#GLint border#>, <#GLenum format#>, <#GLenum type#>, <#const GLvoid *pixels#>)
         
         参数1:纹理维度GL_TEXTURE_2D
         参数2:mip 贴图层次
         参数3:纹理单元存储的颜色成分（从读取像素图中获取）
         参数4:加载纹理宽度
         参数5:加载纹理高度
         参数6:加载纹理深度
         参数7:像素数据的数据类型，GL_UNSIGNED_BYTE无符号整型
         参数8:指向纹理图像数据的指针
         */
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        
        //为纹理对象生成一组完整的mipmap，glGenerateMipmap
        /*
         参数1：纹理维度，GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_2D
         */
        glGenerateMipmap(GL_TEXTURE_2D);
        //释放原始纹理数据，不在需要纹理原始数据了
        free(pBytes);
    }
    
    //设置几何图行顶点/纹理坐标(上、下、左、右)
    GLfloat z;
    /*
    GLTools库中的容器类，GBatch，
    void GLBatch::Begin(GLenum primitive,GLuint nVerts,GLuint nTextureUnits = 0);
    参数1：图元枚举值
    参数2：顶点数
    参数3：1组或者2组纹理坐标
    */
    floorBatch.Begin(GL_TRIANGLE_STRIP, 28,1);
    //z表示深度。隧道的深度
    //Z表示深度，隧道的深度
     for(z = 60.0f; z >= 0.0f; z -=10.0f)
     {
         floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
         floorBatch.Vertex3f(-10.0f, -10.0f, z);
         
         floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
         floorBatch.Vertex3f(10.0f, -10.0f, z);
         
         floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
         floorBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
         
         floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
         floorBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
     }
     floorBatch.End();
    
    //参考PPT图6-11
     ceilingBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
     for(z = 60.0f; z >= 0.0f; z -=10.0f)
     {
         ceilingBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
         ceilingBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
         
         ceilingBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
         ceilingBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
         
         ceilingBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
         ceilingBatch.Vertex3f(-10.0f, 10.0f, z);
         
         ceilingBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
         ceilingBatch.Vertex3f(10.0f, 10.0f, z);
     }
     ceilingBatch.End();
     
     //参考PPT图6-12
     leftWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
     for(z = 60.0f; z >= 0.0f; z -=10.0f)
     {
         leftWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
         leftWallBatch.Vertex3f(-10.0f, -10.0f, z);
         
         leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
         leftWallBatch.Vertex3f(-10.0f, 10.0f, z);
         
         leftWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
         leftWallBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
         
         leftWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
         leftWallBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
     }
     leftWallBatch.End();
    
    //参考PPT图6-13
     rightWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
     for(z = 60.0f; z >= 0.0f; z -=10.0f)
     {
         rightWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
         rightWallBatch.Vertex3f(10.0f, -10.0f, z);
         
         rightWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
         rightWallBatch.Vertex3f(10.0f, 10.0f, z);
         
         rightWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
         rightWallBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
         
         rightWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
         rightWallBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
     }
     rightWallBatch.End();
    
}

void ChangeSize(int w,int h){
    if (h==0) {
        h=1;
    }
    glViewport(0, 0, w, h);
    GLfloat fAspect= GLfloat(w)/GLfloat(h);
    //生成透视投影
    viewFrustum.SetPerspective(80.0f, fAspect, 1.0f, 120.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeLine.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
}

//开始渲染
void RenderScene(void){
    glClear(GL_COLOR_BUFFER_BIT);
    //模型视图压栈
    modelViewMatrix.PushMatrix();
    //z轴平移
    modelViewMatrix.Translate(0.0f, 0.0f, zView);
    //纹理替换矩阵着色器
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,transformPipeLine.GetModelViewProjectionMatrix(),0);
    //绑定纹理
    //地板
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FLOOR]);
    floorBatch.Draw();
    
    //天花板
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CEILIN]);
    ceilingBatch.Draw();
    
    //左侧墙
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
    leftWallBatch.Draw();
    
    //右侧墙
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
    rightWallBatch.Draw();
    
    
    modelViewMatrix.PopMatrix();
    glutSwapBuffers();
}
void SpecailKeys(int key,int x,int y){
    if (key == GLUT_KEY_UP) {
        zView +=0.5f;
    }
    if (key == GLUT_KEY_DOWN) {
        zView -=0.5f;
    }
    glutPostRedisplay();
}
//菜单栏选择
void ProcessMenu(int value)
{
    GLint iLoop;
    
    for(iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
    {
        /**绑定纹理 glBindTexture
         参数1：GL_TEXTURE_2D
         参数2：需要绑定的纹理对象
         */
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        
        /**配置纹理参数 glTexParameteri
         参数1：纹理模式
         参数2：纹理参数
         参数3：特定纹理参数
         
         */
        switch(value)
        {
            case 0:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST（最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
                
            case 1:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR（线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
                
            case 2:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_NEAREST（选择最邻近的Mip层，并执行最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                break;
                
            case 3:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                break;
                
            case 4:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（选择最邻近Mip层，并执行线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                break;
                
            case 5:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行线性过滤，又称为三线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                break;
                
            case 6:
            
                //设置各向异性过滤
                GLfloat fLargest;
                //获取各向异性过滤的最大数量
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
                //设置纹理参数(各向异性采样)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
                break;
        
            case 7:
                //设置各向同性过滤，数量为1.0表示(各向同性采样)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
                break;
                
        }
    }
    
    //触发重画
    glutPostRedisplay();
}
int main(int argc,char* argv[])
{
    //设置当前工作目录。针对mac osx
    gltSetWorkingDirectory(argv[0]);
    //c初始化 GLUT库
    glutInit(&argc, argv);
    //初始化双缓冲窗口，其中标志GLUT_DOUBLE（双缓冲窗口）GLUT_RGBA（RGBA颜色模式）GLUT_DEPTH（深度测试）GLUT_STENCIL（模版缓冲区）
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    // 窗口大小
    glutInitWindowSize(800,600);
    glutCreateWindow("Triangle");
    //注册回调函数
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecailKeys);
    glutDisplayFunc(RenderScene);
    
    // 添加菜单入口，改变过滤器
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("GL_NEAREST",0);
    glutAddMenuEntry("GL_LINEAR",1);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST",2);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 3);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 4);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);
    glutAddMenuEntry("Anisotropic Filter", 6);
    glutAddMenuEntry("Anisotropic Off", 7);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    //驱动程序的初始化中没有出现任何问题
    GLenum err = glewInit(); if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    //调用SetUpRC
    SetUpRC();
    glutMainLoop();
    return 0;
}
