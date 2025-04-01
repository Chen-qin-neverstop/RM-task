#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

class KalmanFilter {
public:
    KalmanFilter(double q, double r, double initial_estimate, double initial_error)
        : Q(q), R(r), estimate(initial_estimate), error(initial_error) {}

    double update(double measurement) {
        // Prediction step
        error += Q;

        // Update step
        double K = error / (error + R); // Kalman gain
        estimate = estimate + K * (measurement - estimate);
        error = (1 - K) * error;

        return estimate;
    }

private:
    double Q;        // Process noise covariance
    double R;        // Measurement noise covariance
    double estimate; // Current estimate
    double error;    // Current error covariance
};

int main() {
    // 从文件读取数据
    std::vector<double> measurements;
    std::ifstream file("homework_data_1.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        double time, value;
        std::istringstream iss(line);
        if (iss >> time >> value) {
            measurements.push_back(value);
        }
    }

    // 初始化卡尔曼滤波器参数
    double process_noise = 0.1;    // Q: 过程噪声增大以跟踪趋势
    double measurement_noise = 0.5; // R: 测量噪声根据数据波动调整
    double initial_estimate = measurements[0]; // 初始估计为第一个测量值
    double initial_error = 1.0;

    KalmanFilter kf(process_noise, measurement_noise, initial_estimate, initial_error);

    // 处理所有数据点
    std::cout << "Time\tMeasurement\tEstimate" << std::endl;
    for (size_t i = 0; i < measurements.size(); ++i) {
        double estimate = kf.update(measurements[i]);
        std::cout << i*0.001 << "\t" << measurements[i] << "\t" << estimate << std::endl;
    }

    return 0;
}