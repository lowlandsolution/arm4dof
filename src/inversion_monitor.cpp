#include <ros/ros.h>
#include <tf/transform_listener.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "inversion_monitor");

    ros::NodeHandle nh;
    ros::NodeHandle pn("~");
    tf::TransformListener listener;
    ros::Rate rate(10.0);

    std::string base_frame = pn.param("base_frame", std::string("base_link"));
    std::string object_frame = pn.param("object_frame", std::string(""));
    if (object_frame.size() == 0) {
        ROS_ERROR_STREAM("object_frame parameter is required!");
        return -1;
    }

    while (nh.ok()) {

        tf::StampedTransform transform;

        try {

            listener.lookupTransform(
                base_frame,
                object_frame,
                ros::Time(0),
                transform);

            tf::Quaternion q = transform.getRotation();
 
            // Extract the direction vector of the rotated frame
            // This calculation was stolen from NewDeal's answer
            // for the following question:
            // https://www.gamedev.net/forums/topic/
            //     56471-extracting-direction-vectors-from-quaternion/
            tf::Vector3 d(
                2 * ((q.x() * q.z()) - (q.w() * q.y())),
                2 * ((q.y() * q.z()) + (q.w() * q.x())),
                1 - (2 * ((q.x() * q.x()) + (q.y() * q.y())))); 

            // Object is "upside down" relative to the base frame if
            // the Z component of the transform's direction vector
            // is negative.
            if (d.getZ() < 0) {
                ROS_INFO_STREAM("Object is inverted!");
            }

        } catch(tf::TransformException ex) {
            //ROS_ERROR_STREAM("Transform Exception: " << ex.what());
        }

        rate.sleep();

    }

    return 0;
}

