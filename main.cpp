#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <array>
#include <cmath>

using namespace std;

struct Camera {
    double angleBelowHorizon; // угол наклона камеры ниже горизонта
    double angleVertical; // угол раскрытия по вертикали
    double angleHorizontal; // угол раскрытия по горизонтали
    double height; // высота подвеса камеры в метрах
    double rotationAngle; // угол поворота камеры относительно начала координат против часовой стрелки
    double verticalResolution; // вертикальное разрешение
    double horizontalResolution; // горизонтальное разрешение
};


struct Point2D {
    double x;
    double y;
};

Point2D operator+(const Point2D& a, const Point2D& b) {
    return { a.x + b.x, a.y + b.y };
}

Point2D operator-(const Point2D& a, const Point2D& b) {
    return { a.x - b.x, a.y - b.y };
}

Point2D operator*(const Point2D& a, double b) {
    return { a.x * b, a.y * b };
}

double AngleToRadian(double angle) {
    return angle * M_PI / 180;
};

// поворот координат на угол против часовой стрелки
Point2D Rotate(Point2D point, double angle) {
    angle = AngleToRadian(angle);
    return {
        point.x * cos(angle) + point.y * sin(angle),
        -point.x * sin(angle) + point.y * cos(angle)
    };
};


array<Point2D, 4> GetCamerawiseTrapezoidCoordinates(const Camera& camera) {
    // получаем трапецию поля обзоры камеры на плоскости, считая центром координат камеру
    
    double lower_vertical_angle = camera.angleBelowHorizon + camera.angleVertical / 2;
    double distance_to_lower_side = camera.height * tan(AngleToRadian(90 - lower_vertical_angle));
    double upper_vertical_angle = camera.angleBelowHorizon - camera.angleVertical / 2;
    double distance_to_upper_side = camera.height * tan(AngleToRadian(90 - upper_vertical_angle));

    double horizontal_angle = AngleToRadian(camera.angleHorizontal / 2);
    double lower_side_width = 2 * distance_to_lower_side * tan(horizontal_angle);
    double upper_side_width = 2 * distance_to_upper_side * tan(horizontal_angle);

    array<Point2D, 4> res = {
        Point2D{ -lower_side_width / 2, distance_to_lower_side }, // нижний левый угол
        Point2D{ lower_side_width / 2, distance_to_lower_side }, // нижний правый угол
        Point2D{ -upper_side_width / 2, distance_to_upper_side }, // верхний левый угол
        Point2D{ upper_side_width / 2, distance_to_upper_side } // верхний правый угол
    };
    return res;
};


Point2D GetCamerawisePointCoordinates(Point2D image_point, const Camera& camera) {
    // получаем реальные координаты произвольного пикселя, считая центром координат камеру
   
    array<Point2D, 4> trapezoid = GetCamerawiseTrapezoidCoordinates(camera);
    Point2D lower_left = trapezoid.at(0);
    Point2D lower_right = trapezoid.at(1);
    Point2D upper_left = trapezoid.at(2);
    Point2D upper_right = trapezoid.at(3);
    //переворот у
    double relative_y = (camera.verticalResolution - image_point.y) / camera.verticalResolution;
    double relative_x = image_point.x / camera.horizontalResolution;

    Point2D temp_left = (upper_left - lower_left) * relative_y + lower_left;
    Point2D temp_right = (upper_right - lower_right) * relative_y + lower_right;
    Point2D res_point = (temp_right - temp_left) * relative_x + temp_left;

    return res_point;
}

Point2D GetRealPointCoordinates(Point2D image_point, const Camera& camera) {
    auto camerawise_cords = GetCamerawisePointCoordinates(image_point, camera);
    return Rotate(camerawise_cords, -camera.rotationAngle);
}

void PrintCoordinates(Point2D image_point, Point2D real_point) {
    cout <<"Pixel coordinates: ["
        << image_point.x
        << " " << image_point.y
        << "]\n Metric coordinates: ["
        << real_point.x
        << " " << real_point.y << "]\n********* \n";
}

int main()
{
    Point2D pixel_coordinates[]{
        { 1117, 1080 },
        { 1161, 523 },
        { 1015, 303 },
        { 991, 174 },
        { 1161, 523 },
        { 1054, 98 },
        { 1167, 70 },
        { 1189, 32 },
        { 1160, 0 }
    };

    Camera camera{ 37, 40, 60, 3, 10, 1080, 1920 };

    for (size_t i = 0; i < size(pixel_coordinates); ++i) {
        Point2D real_point = GetRealPointCoordinates(pixel_coordinates[i], camera);
        PrintCoordinates(pixel_coordinates[i], real_point);
    }

    return 0;
}