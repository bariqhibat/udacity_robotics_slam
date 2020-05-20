#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget service;
    service.request.linear_x = lin_x;
    service.request.angular_z = ang_z;
    client.call(service);
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    //data = color1,color2,color3,color1,color2,color3

    int i = img.data.size()/3;
    int p = i*2;
    int pose;
    bool cond;
    for(i;i+2<p;i+=3){
      cond = false;
      if(img.data[i]==255 && img.data[i+1]==255 && img.data[i+2]==255){
        cond = true;
        pose = (i % (img.width*3))/3; //fast act, greedy player
        break;
      }
    }
    if(!cond){
      drive_robot(0.0,0.0);
    }
    else if(cond && pose < img.width/3){
      drive_robot(0.1,0.5);
    }
    else if (cond && pose < 2*img.width/3){
      drive_robot(0.1,0);
    }
    else{
      drive_robot(0.1,-0.5);
    }
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}

