/*
填写一个旋转矩阵和一个透视投影矩阵。给定三维下三个点
v0(2.0, 0.0, -2.0), v1(0.0, 2.0, -2.0), v2( 2.0, 0.0, 2.0), 你需要将这三个点的坐
标变换为屏幕坐标并在屏幕上绘制出对应的线框三角形 (在代码框架中，我们已
经提供了 draw_triangle 函数，所以你只需要去构建变换矩阵即可)。简而言之，
我们需要进行模型、视图、投影、视口等变换来将三角形显示在屏幕上。在提供
的代码框架中，我们留下了模型变换和投影变换的部分给你去完成。

get_model_matrix(float rotation_angle): 逐个元素地构建模型变换矩阵并返回该矩阵。
在此函数中，你只需要实现三维中绕 z 轴旋转的变换矩阵，而不用处理平移与缩放。

get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar): 使用给定的参数逐个元素地构建透视投影矩阵并返回该矩阵。

[Optional] main(): 自行补充你所需的其他操作。
*/

#include <iostream>
#include<cmath>
#include <Eigen/Dense>

#define _USE_MATH_DEFINES
using namespace Eigen;

Matrix4f get_model_matrix(float rotation_angle);
Matrix4f get_rotation(Vector3f axis, float angle);      // build a rotation matrix about any axis through orgin
Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

int main()
{
    // define the 3d points
    Vector4f v0 = Vector4f(2.0f,0.0f,-2.0f,1.0f);
    Vector4f v1 = Vector4f(0.0f, 2.0f, -2.0f, 1.0f);
    Vector4f v2 = Vector4f(-2.0f, 0.0f, -2.0f, 1.0f);

    Matrix4f RZ=get_model_matrix(45);
    std::cout << "The Rotation Matrix: \n" << RZ << std::endl;
}

Matrix4f get_model_matrix(float rotation_angle)
{
    // build a matrix rotating about the z-axis  
    Matrix4f RZ(4, 4);

    float theta = rotation_angle / 180.0f * M_PI;

    RZ <<
        cos(theta), -sin(theta), 0.0f, 0.0f,
        sin(theta), cos(theta), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return RZ;
}

// build a rotation matrix about any axis through orgin
Matrix4f get_rotation(Vector3f axis, float angle)
{
    Matrix3f N(3, 3);

    float theta = angle / 180.0f * M_PI;

    N <<
        0.0f, -axis.z(), axis.y(),
        axis.z(), 0.0f, -axis.x(),
        -axis.y(), axis.x(), 0.0f;

    Matrix3f R = cos(theta) * Matrix3f::Identity(3,3) + (1 - cos(theta)) * axis * axis.transpose() + sin(theta) * N;
    Matrix4f RN(4, 4);
    RN <<
        R(0, 0), R(0, 1), R(0, 2), 0.0f,
        R(1, 0), R(1, 1), R(1, 2), 0.0f,
        R(2, 0), R(2, 1), R(2, 2), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return RN;
}

Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    float fovY = eye_fov / 180.0f * M_PI;
    float t = tan(fovY / 2) * abs(zNear);
    float r = aspect_ratio * t;
    float l = -r;
    float b = -t;
    float n = zNear;
    float f = zFar;

    // build a matrix of orthographic projection
    Matrix4f orthoA(4, 4);
    orthoA <<
        2.0f / (r-l), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (t-b), 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f / (n - f), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    Matrix4f orthoB(4, 4);
    orthoB <<
        1.0f, 0.0f, 0.0f, -(r + l) / 2.0f,
        0.0f, 1.0f, 0.0f, -(t + b) / 2.0f,
        0.0f, 0.0f, 1.0f, -(n + f) / 2.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    Matrix4f ortho = orthoA * orthoB;

    // build a matrix from perspective to orthographic
    Matrix4f pto(4, 4);
    pto <<
        n, 0.0f, 0.0f, 0.0f,
        0.0f, n, 0.0f, 0.0f,
        0.0f, 0.0f, n + f, -(n * f),
        0.0f, 0.0f, 1.0f, 0.0f;

    // compute the projection matrix
    Matrix4f proj = ortho * pto;

    return proj;
}