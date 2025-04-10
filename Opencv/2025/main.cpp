#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

// 全局变量用于滑动条
int h_min = 49, h_max = 116;
int s_min = 61, s_max = 198;
int v_min = 111, v_max = 255;
const string window_name = "HSV Adjustment";

// 装甲板尺寸（适当放大尺寸以提高容错性）
const float ARMOR_WIDTH = 150.0f;      // 原135.0f，放大到150mm
const float LIGHT_BAR_LENGTH = 60.0f;  // 原55.0f，放大到60mm

// 相机参数
const Mat CAMERA_MATRIX = (Mat_<double>(3, 3) <<
    2065.0580175762857, 0.0, 658.9098266395495,
    0.0, 2086.886458338243, 531.5333174739342,
    0.0, 0.0, 1.0);

const Mat DIST_COEFFS = (Mat_<double>(5, 1) << 
    -0.051836613762195866, 0.29341513924119095, 
    0.001501183796729562, 0.0009386915104617738, 0.0);

// 滑动条回调函数
void onTrackbar(int, void*) {
    // 空实现，仅用于触发更新
}

Mat preprocessImage(const Mat &frame) {
    Mat hsv, mask_red, mask_red1, mask_red2, result;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    
    // 使用滑动条调整的阈值
    inRange(hsv, Scalar(h_min, s_min, v_min), Scalar(h_max, s_max, v_max), mask_red1);
    inRange(hsv, Scalar(170, s_min, v_min), Scalar(180, s_max, v_max), mask_red2);
    mask_red = mask_red1 | mask_red2;

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(mask_red, result, MORPH_CLOSE, kernel);
    return result;
}

// 查找灯条（放宽长宽比限制）
vector<RotatedRect> findLightBars(const Mat &binary_img) {
    vector<vector<Point>> contours;
    vector<RotatedRect> light_bars;
    findContours(binary_img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (const auto &cnt : contours) {
        float area = contourArea(cnt);
        if (area < 40.0f) continue;  // 提高最小面积阈值

        RotatedRect rect = minAreaRect(cnt);
        float length = max(rect.size.width, rect.size.height);
        float width = min(rect.size.width, rect.size.height);

        // 放宽长宽比限制（原4.0f）
        if (length/width > 3.5f && length > 25.0f) {
            light_bars.push_back(rect);
        }
    }
    return light_bars;
}

// 匹配装甲板对（放宽几何约束）
vector<pair<Point2f, Point2f>> matchArmorPairs(const vector<RotatedRect> &light_bars) {
    vector<pair<Point2f, Point2f>> armor_pairs;

    for (size_t i = 0; i < light_bars.size(); i++) {
        for (size_t j = i + 1; j < light_bars.size(); j++) {
            const RotatedRect &rect1 = light_bars[i];
            const RotatedRect &rect2 = light_bars[j];

            // 放宽角度差约束（原15度）
            float angle_diff = abs(rect1.angle - rect2.angle);
            if (angle_diff > 20.0f && angle_diff < 160.0f) continue;

            // 放宽距离约束（使用放大后的ARMOR_WIDTH）
            float distance = norm(rect1.center - rect2.center);
            if (distance < ARMOR_WIDTH * 0.5 || distance > ARMOR_WIDTH * 1.5) continue;

            // 放宽高度差约束（原1/3灯带长度）
            if (abs(rect1.center.y - rect2.center.y) > LIGHT_BAR_LENGTH/2.5f) continue;

            armor_pairs.emplace_back(rect1.center, rect2.center);
        }
    }
    return armor_pairs;
}

// 获取装甲板角点（使用放大后的尺寸）
vector<Point2f> getArmorCorners(const pair<Point2f, Point2f> &pair) {
    Point2f p1 = pair.first;
    Point2f p2 = pair.second;

    // 计算垂直方向
    Point2f dir = p2 - p1;
    Point2f perpendicular(-dir.y, dir.x);
    perpendicular = perpendicular / norm(perpendicular) * (LIGHT_BAR_LENGTH/2.0f);

    return {
        p1 + perpendicular,  // 左下
        p2 + perpendicular,  // 右下
        p2 - perpendicular,  // 右上
        p1 - perpendicular   // 左上
    };
}

// PnP解算（使用放大后的3D尺寸）
void solveArmorPose(const vector<Point2f> &image_points, Mat &rvec, Mat &tvec) {
    vector<Point3f> object_points = {
        Point3f(-ARMOR_WIDTH/2, -LIGHT_BAR_LENGTH/2, 0),
        Point3f(ARMOR_WIDTH/2, -LIGHT_BAR_LENGTH/2, 0),
        Point3f(ARMOR_WIDTH/2, LIGHT_BAR_LENGTH/2, 0),
        Point3f(-ARMOR_WIDTH/2, LIGHT_BAR_LENGTH/2, 0)
    };
    solvePnP(object_points, image_points, CAMERA_MATRIX, DIST_COEFFS, rvec, tvec);
}

int main() {
    // 创建调整窗口
    namedWindow(window_name, WINDOW_NORMAL);
    resizeWindow(window_name, 600, 300);
    
    // 创建滑动条
    createTrackbar("H Min", window_name, &h_min, 180, onTrackbar);
    createTrackbar("H Max", window_name, &h_max, 180, onTrackbar);
    createTrackbar("S Min", window_name, &s_min, 255, onTrackbar);
    createTrackbar("S Max", window_name, &s_max, 255, onTrackbar);
    createTrackbar("V Min", window_name, &v_min, 255, onTrackbar);
    createTrackbar("V Max", window_name, &v_max, 255, onTrackbar);

    Mat frame = imread("armor_test.jpg");
    if (frame.empty()) {
        cerr << "Error: Could not load image!" << endl;
        return -1;
    }

    while (true) {
        Mat undistorted_frame;
        undistort(frame, undistorted_frame, CAMERA_MATRIX, DIST_COEFFS);

        // 实时处理
        Mat binary_img = preprocessImage(undistorted_frame);
        
        // 显示处理结果
        imshow("Binary Preview", binary_img);
        
        // 完整处理流程
        vector<RotatedRect> light_bars = findLightBars(binary_img);
        vector<pair<Point2f, Point2f>> armor_pairs = matchArmorPairs(light_bars);
        
        Mat result_img = undistorted_frame.clone();
        for (const auto &pair : armor_pairs) {
            vector<Point2f> corners = getArmorCorners(pair);
            Mat rvec, tvec;
            solveArmorPose(corners, rvec, tvec);
            
            // 绘制装甲板
            for (int i = 0; i < 4; i++) {
                line(result_img, corners[i], corners[(i+1)%4], Scalar(0,255,0), 2);
            }
            
            // 绘制坐标系
            vector<Point3f> axis = {Point3f(0,0,0), Point3f(50,0,0), Point3f(0,50,0), Point3f(0,0,50)};
            vector<Point2f> projected_axis;
            projectPoints(axis, rvec, tvec, CAMERA_MATRIX, DIST_COEFFS, projected_axis);
            arrowedLine(result_img, projected_axis[0], projected_axis[1], Scalar(0,0,255), 2); // X
            arrowedLine(result_img, projected_axis[0], projected_axis[2], Scalar(0,255,0), 2); // Y
            arrowedLine(result_img, projected_axis[0], projected_axis[3], Scalar(255,0,0), 2); // Z
            
            // 输出结果
            cout << "===== Detection Result =====" << endl;
            cout << "Armor Size: " << ARMOR_WIDTH << "x" << LIGHT_BAR_LENGTH << " mm" << endl;
            cout << "旋转矩阵--Rotation Vector:\n" << rvec << endl;
            cout << "平移矩阵--Translation Vector (mm):\n" << tvec << endl;
            cout << "Distance: " << norm(tvec) << " mm" << endl;
        }
        imshow("Detection Result", result_img);

        // 退出条件
        int key = waitKey(30);
        if (key == 27) break; // ESC退出
        if (key == 's') { // 保存当前阈值
            cout << "Current HSV Threshold:\n";
            cout << "H: [" << h_min << ", " << h_max << "]\n";
            cout << "S: [" << s_min << ", " << s_max << "]\n";
            cout << "V: [" << v_min << ", " << v_max << "]" << endl;
        }
    }

    destroyAllWindows();
    return 0;
}