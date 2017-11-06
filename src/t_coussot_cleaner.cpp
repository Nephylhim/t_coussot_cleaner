#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include <sstream>
#include "rosgraph_msgs/Log.h"

ros::Publisher velocity_publisher;
ros::Subscriber pose_subscriber;
ros::Subscriber rosout_subscriber;
turtlesim::Pose turtlesim_pose;

using namespace std;

const double PI = 3.14159265359;
bool boom = false;

void move(double speed, double distance, bool isForward);
void test_move();
void rotate(double angular_speed, double angle, bool clockwise);
void test_rotate();
double degrees2radians(double angle);
void setDesiredOrientation(double angle);
void poseCallback(const turtlesim::Pose::ConstPtr & pose_message);
void rosoutCallback(const rosgraph_msgs::Log & log_msg);
void test_setDesiredOrientation();
void shitty_ia();
void change_direction();

int main(int argc, char **argv){
    
    // Initiate new ROS node named "talker"
    ros::init(argc, argv, "t_coussot_cleaner");
    ros::NodeHandle n;

    velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    pose_subscriber = n.subscribe("/turle/pose", 10, poseCallback);
    rosout_subscriber = n.subscribe("/rosout", 10, rosoutCallback);

//    test_rotate();
//    test_move();
//    test_setDesiredOrientation();
    
    shitty_ia();

    ros::spin();

    return EXIT_SUCCESS;
}

void shitty_ia(){
    while(1){
        move(8, 70, 1);
        if(boom) boom = false;
        move(5, 4, 0);
        change_direction();
        if(boom) boom = false;
        ros::spinOnce();
    }
}

void change_direction(){
    double rangle = rand() % 380 - 180;
    rotate(50, degrees2radians(rangle), !(rangle < 0));
}

// Make the robot moove w/ a certain veolocity for a certain distance forward or backward
void move(double speed, double distance, bool isForward){
    geometry_msgs::Twist vel_msg;

    // make it move on x axis
    vel_msg.linear.x = isForward ? abs(speed) : -abs(speed);
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;

    // no angular velocity for this method
    vel_msg.angular.x=0;
    vel_msg.angular.y=0;
    vel_msg.angular.z=0;

    // loop to move
    double t0 = ros::Time::now().toSec();
    double current_distance = 0;
    ros::Rate loop_rate(1000);
    do {
        velocity_publisher.publish(vel_msg);
        double t1  = ros::Time::now().toSec();
        current_distance = speed * (t1 - t0);
        ros::spinOnce();
        loop_rate.sleep();
    } while(current_distance < distance && !boom);
    vel_msg.linear.x = 0;
    velocity_publisher.publish(vel_msg);
}

void test_move(){
    double speed; double distance; bool isForward;
    cout<<"speed ? ";
    cin>>speed;
    cout<<"distance ? ";
    cin>>distance;
    cout<<"forward ? (bool) ";
    cin>>isForward;
    move(speed, distance, isForward);
}

void rotate(double angular_speed, double angle, bool clockwise){
    geometry_msgs::Twist vel_msg;

    vel_msg.linear.x = 0;
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;

    // no angular velocity for this method
    vel_msg.angular.x=0;
    vel_msg.angular.y=0;
    vel_msg.angular.z= abs(angular_speed) * (clockwise ? -1 : 1);

    double current_angle = 0.0;
    double t0 = ros::Time::now().toSec();
    ros::Rate loop_rate(10);
    do{
        cout<<"z = "<<vel_msg.angular.z;
        velocity_publisher.publish(vel_msg);
        double t1 = ros::Time::now().toSec();
        current_angle = angular_speed * (t1-t0);
        ros::spinOnce();
        loop_rate.sleep();
    }while(current_angle<angle);
    vel_msg.angular.z= 0;
    velocity_publisher.publish(vel_msg);
    ros::spinOnce();
}

void test_rotate(){
    double angular_speed; double angle; bool clockwise;
    cout<<"angular speed ? ";
    cin>>angular_speed;
    cout<<"angle ? ";
    cin>>angle;
    cout<<"clockwise ? (bool) ";
    cin>>clockwise;
    rotate(degrees2radians(angular_speed), degrees2radians(angle), clockwise);
}

double degrees2radians(double angle){
    return angle * PI / 180.0;
}

void setDesiredOrientation(double desired_angle){
    double relative_angle = desired_angle - turtlesim_pose.theta;
    bool clockwise = ((relative_angle<0) ? true : false);
    //cout<<desired_angle<<", "<<turtlesim_pose.theta<<", "<<relative_angle;
    rotate(30, abs(relative_angle), clockwise);
}

void test_setDesiredOrientation(){
    setDesiredOrientation(degrees2radians(120));
    ros::Rate loop_rate(0.5);
    loop_rate.sleep();
    setDesiredOrientation(degrees2radians(-60));
    loop_rate.sleep();
    setDesiredOrientation(degrees2radians(0));
}

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message){
    turtlesim_pose.x=pose_message->x;
    turtlesim_pose.y=pose_message->y;
    turtlesim_pose.theta=pose_message->theta;
}

void rosoutCallback(const rosgraph_msgs::Log & log_msg){
    if( log_msg.level == log_msg.WARN && log_msg.name == "/turtlesim" ){
        boom = true;
    }
}
