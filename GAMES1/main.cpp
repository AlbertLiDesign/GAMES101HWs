#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    float theta = rotation_angle / 180.0f * M_PI;

    model <<
        cos(theta), -sin(theta), 0.0f, 0.0f,
        sin(theta), cos(theta), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
    float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    float fovY = eye_fov / 180.0f * M_PI;
    float t = tan(fovY / 2) * abs(zNear);
    float r = aspect_ratio * t;
    float l = -r;
    float b = -t;
    float n = zNear;
    float f = zFar;

    // build a matrix of orthographic projection
    Eigen::Matrix4f orthoA(4, 4);
    orthoA <<
        2.0f / (r - l), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (t - b), 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f / (n - f), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    Eigen::Matrix4f orthoB(4, 4);
    orthoB <<
        1.0f, 0.0f, 0.0f, -(r + l) / 2.0f,
        0.0f, 1.0f, 0.0f, -(t + b) / 2.0f,
        0.0f, 0.0f, 1.0f, -(n + f) / 2.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    Eigen::Matrix4f ortho = orthoA * orthoB;

    // build a matrix from perspective to orthographic
    Matrix4f pto(4, 4);
    pto <<
        n, 0.0f, 0.0f, 0.0f,
        0.0f, n, 0.0f, 0.0f,
        0.0f, 0.0f, n + f, -(n * f),
        0.0f, 0.0f, 1.0f, 0.0f;

    // compute the projection matrix
    projection = ortho * pto;

    return projection;
}

// build a rotation matrix about any axis through orgin
Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float angle)
{
    Eigen::Matrix3f N(3, 3);

    float theta = angle / 180.0f * M_PI;

    N <<
        0.0f, -axis.z(), axis.y(),
        axis.z(), 0.0f, -axis.x(),
        -axis.y(), axis.x(), 0.0f;

    Eigen::Matrix3f R = cos(theta) * Eigen::Matrix3f::Identity(3, 3) + (1 - cos(theta)) * axis * axis.transpose() + sin(theta) * N;
    Eigen::Matrix4f RN(4, 4);
    RN <<
        R(0, 0), R(0, 1), R(0, 2), 0.0f,
        R(1, 0), R(1, 1), R(1, 2), 0.0f,
        R(2, 0), R(2, 1), R(2, 2), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return RN;
}

int main(int argc, const char** argv)
{
    float angle = 45;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = { 0, 0, 5 };

    std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

    std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };
    Eigen::Vector3f axis(-1, 0, 0);

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        //r.set_model(get_model_matrix(angle));
        r.set_model(get_rotation(axis, angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        //r.set_model(get_model_matrix(angle));
        r.set_model(get_rotation(axis, angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
