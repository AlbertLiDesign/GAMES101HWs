/*
给定一个点 P=(2,1), 将该点绕原点先逆时针旋转 45◦，再平移 (1,2), 计算出
变换后点的坐标（要求用齐次坐标进行计算）。
*/

#include <iostream>
#include<cmath>
#include <Eigen/Dense>

#define _USE_MATH_DEFINES
using namespace Eigen;

int main()
{
    // define a 2d point (2,1)
    Vector3f vec = Vector3f(2, 1, 1);
    std::cout << "The point is: \n" << vec << std::endl;

    // build a rotation matrix
    Matrix3f R45(3, 3);
    float theta = 45.0f / 180.0f * M_PI;
    R45 << 
        cos(theta), -sin(theta), 0,
        sin(theta), cos(theta), 0,
        0.0f, 0.0f, 1.0f;
    std::cout << "Rotation Matrix: \n" << R45 << std::endl;

    // build a translation matrix
    Matrix3f T12(3, 3);
    T12 << 
        1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 2.0f,
        0.0f, 0.0f, 1.0f;
    std::cout << "Translation Matrix: \n" << T12 << std::endl;

    // compute the result
    Vector3f result = T12 * R45 * vec;
    std::cout << "The result is: \n" << result << std::endl;

    return 0;
}