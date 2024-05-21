#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/compressed_image.hpp"
#include "opencv2/opencv.hpp"
#include <memory>
#include <functional>
#include <iostream>
using std::placeholders::_1;

class videocap
{
    public:
        videocap();
        ~videocap();
        void frameinsert(cv::Mat input);

    private:
        cv::VideoWriter writer;
        cv::Mat frame;
};

videocap::videocap()
{
    writer.open("./video.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size(640, 360), true);
    if (!writer.isOpened()){
        std::cout << "ERR!!!!!!!!!!!!!!!!";
        writer.release();
    }
}

videocap::~videocap()
{
    writer.release();
}
void videocap::frameinsert(cv::Mat input)
{
    writer << input;
}

void mysub_callback(rclcpp::Node::SharedPtr node, const sensor_msgs::msg::CompressedImage::SharedPtr msg)
{
    static videocap vc;
    static cv::Mat frame;
    frame = cv::imdecode(cv::Mat(msg->data), cv::IMREAD_COLOR);
    cv::imshow("wsl", frame);
    vc.frameinsert(frame);
    cv::waitKey(1);
    RCLCPP_INFO(node->get_logger(), "Received Image : %s,%d,%d", msg->format.c_str(),frame.rows,frame.cols);
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("camsub_wsl");
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10)).best_effort();
    std::function<void(const sensor_msgs::msg::CompressedImage::SharedPtr msg)> fn;
    fn = std::bind(mysub_callback, node, _1);
    auto mysub = node->create_subscription<sensor_msgs::msg::CompressedImage>("image/compressed",qos_profile,fn);
    //cv::namedWindow("wsl");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

