#include "ros/ros.h"
#include "sensor_msgs/Range.h"

#include "LRM30_serial.h"

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "image");

  LRM30_serial lrm30(9600, "/dev/ttyACM0");
  lrm30.connect();

  lrm30.laserON();

  std::cout << "continuous shot value: " << lrm30.continuousshot() << std::endl; 

  ros::NodeHandle n;
  ros::Rate loop_rate(10);

  ros::Publisher lrm30_pub = n.advertise<sensor_msgs::Range>("lrm30_data", 1000);

  while(ros::ok()){

    float measure  = lrm30.getMeasure();
    sensor_msgs::Range lrm30_msg;

    lrm30_msg.range = measure;

    lrm30_pub.publish(lrm30_msg);
    ros::spinOnce();
    loop_rate.sleep();
  }

  lrm30.laserOFF();


  return 0;

}
