#include "first_challenge_kimizuka/first_challenge_kimizuka.h"

FirstChallenge::FirstChallenge():private_nh_("~")
{
    private_nh_.param("hz", hz_, {10});
    odom_sub_ = nh_.subscribe("/roomba/odometry", 1, &FirstChallenge::odometry_callback, this);
    laser_sub_ = nh_.subscribe("/scan", 1, &FirstChallenge::laser_callback, this);
    cmd_vel_pub_ = nh_.advertise<roomba_500driver_meiji::RoombaCtrl>("/roomba/control", 1);
}

void FirstChallenge::odometry_callback(const nav_msgs::Odometry::ConstPtr& msg)
{
    odometry_ = *msg;
}

void FirstChallenge::laser_callback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
    laser_ = *msg;
}

void FirstChallenge::run(float v, float o)
{
    cmd_vel_.mode = 11;
    cmd_vel_.cntl.linear.x = v;
    cmd_vel_.cntl.angular.z = o;

    cmd_vel_pub_.publish(cmd_vel_);
}

//float FirstChallenge::scan()
//{
//    len_ = laser_.ranges[laser_.ranges.size() / 2];
//    return len_;
//}

void FirstChallenge::process()
{
    ros::Rate loop_rate(hz_);

    while(ros::ok())
    {
//        printf("here1\n");
        len_ = odometry_.pose.pose.position.x;
        rot_ = tf2::getYaw(odometry_.pose.pose.orientation);

        std::cout<<tf2::getYaw(odometry_.pose.pose.orientation)<<std::endl;

        std::cout<<rotcount_<<std::endl;

        std::cout<<lenscan_<<std::endl;


        if(len_ >= 1.0){
            if(rotcount_ <= 30){
                run(0.0, 0.1);
            }

            if(rot_ < 0){
                rotcount_++;
            }

            if(rotcount_ >= 30){
                if(rot_ >= -0.1){
                    run(0.05, 0.0);
                    lenscan_ = laser_.ranges[laser_.ranges.size() /2];

                    if(lenscan_ <= 0.5){
                        run(0.0, 0.0);
                    }
                }
            }
        }else{
            run(0.2, 0.0);
        }

//        if(laser_.ranges.size() != 0){
//            std::cout<<laser_.ranges.size() / 2<<std::endl;
//            std::cout<<laser_.ranges[laser_.ranges.size() / 2]<<std::endl;
//            len_ = laser_.ranges[laser_.ranges.size() / 2];
//          }

        ros::spinOnce();
        loop_rate.sleep();
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "first_challenge_kimizuka");
    FirstChallenge first_challenge;
    first_challenge.process();
    return 0;
}
