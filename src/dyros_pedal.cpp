#include "../include/dyros_pedal/dyros_pedal.h"
// sudo apt install libusb-dev
#include <usb.h>

DyrosPedal::DyrosPedal()
{
    std::cout << "check"  << std::endl;
}

void pedalCallback(const sensor_msgs::Joy::ConstPtr& pedal)
{
    walk_cmd_msg_.step_length_x = pedal->axes[0];
    walk_cmd_msg_.z = pedal->axes[1];
    walk_cmd_msg_.theta = pedal->axes[2];
}

int main(int argc, char** argv)
{
    struct usb_bus *bus;
    struct usb_device *dev;
    usb_init();

    std::cout << "Pedal Started"<<std::endl;
    DyrosPedal dyrospedal;

    ros::init(argc, argv, "dyros_pedal");
    ros::NodeHandle nh_;
    walking_cmd_pub_ = nh_.advertise<dyros_pedal::WalkingCommand>("/tocabi/pedalcommand", 100);
    pedal_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, pedalCallback);

    while(ros::ok())
    {
        usb_check = 0;

        for(int n = 0 ; n<6 ;n++)
        {
            printf("\x1b[A\33[2K\r");
        }

        if( i%100 == 0){
            usb_find_busses();
            i = 1;
        }
        if( i%10 == 0){
            usb_find_devices();
        }

        for (bus = usb_busses; bus; bus = bus->next)
            for (dev = bus->devices; dev; dev = dev->next){
                if (dev->descriptor.idVendor == 0x044f && dev->descriptor.idProduct == 0xb679)
                {
                    usb_check ++;
                }
            }
        
        std::cout <<" --------------------- \n" ;
        if (usb_check == 1) {
            std::cout <<  " usb Connected \n" ;
        }
        else {
            std::cout << " usb Disconnected \n";
            walk_cmd_msg_.step_length_x = -1.0;
            walk_cmd_msg_.z = -1.0;
            walk_cmd_msg_.theta = 0.0;
        }

        walking_cmd_pub_.publish(walk_cmd_msg_);

        std::cout << "step_length : " << walk_cmd_msg_.step_length_x << std::endl;
        std::cout << "trigger(z) : " << walk_cmd_msg_.z << std::endl;
        std::cout << "theta : " << walk_cmd_msg_.theta << std::endl;
        std::cout << " --------------------- \n" ;
        
        i++;

        ros::spinOnce();
    }
    //ros::spin();

    return 0;
}
