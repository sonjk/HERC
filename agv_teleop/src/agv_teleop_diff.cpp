

#include "agv_teleop/agv_teleop_diff.h"


AgvTeleop::AgvTeleop()
{
    init_param();

    twist_pub = nh.advertise<geometry_msgs::Twist>("teleop_cmd_vel", 10);

    lift_req_pub = nh.advertise<std_msgs::String>("lift_cmd", 10);

    nav_ctrl_pub = nh.advertise<agv_msgs::NavigationControl>("nav_ctrl", 10);

    init();
    loop_cnt = 0;
}

int kfd = 0;
struct termios cooked, raw;
char cmd_char;
void quit(int sig)
{
    (void)sig;
    tcsetattr(kfd, TCSANOW, &cooked);
    ros::shutdown();
    exit(0);
}

void AgvTeleop::init_param()
{
    if (!nh.getParam("manual_drive", manual_drive_))
    {
        manual_drive_ = 0;
    }

    if(!nh.getParam("teleop_angular_z_max", angular_z_max_))
    {
        angular_z_max_ = 0.5;
    }

    if(!nh.getParam("teleop_linear_x_max", linear_x_max_))
    {
        linear_x_max_ = 0.5;
    }

    printf("manual drive %d \n", (int)(manual_drive_));
}

void AgvTeleop::init()
{
    linear_x = 0;
    linear_y = 0;
    angular_z = 0;

    clicked_cnt = 0;
    
    current_nav_ctrl = -1;
}

void AgvTeleop::timer_loop()
{
	loop_cnt++;
	printf("loop_cnt: %d", loop_cnt);
}

void AgvTeleop::keyLoop()
{
    char c[3];
    bool dirty = false;

    std_msgs::String lift_req_msg;

    tcgetattr(kfd, &cooked);
    memcpy(&raw, &cooked, sizeof(struct termios));
    raw.c_lflag &=~ (ICANON | ECHO);

    // Setting a new line, then end of line
    raw.c_cc[VEOL] =1;
    raw.c_cc[VEOF] =2;
    tcsetattr(kfd, TCSANOW, &raw);


    puts("AGV Diff keyboard tele-op");
    puts("----------------------------");
    puts("          w i     Vx++" );
    puts("Wz++  a   s   d   Wz--" );
    puts("          x ,     Vx--" );
    puts("Vy++  j   k   l   Vy--"   );
    puts("----------------------------");
    puts("  5   lift down             ");
    puts("  7   lift up               ");
    puts("  0   go to home            ");
    puts("  +   charge_req            ");
    puts("  -   discharge_req         ");
    puts("  p   pillar demo req       ");
    puts("  P   pillar demo cancel    ");
    puts("----------------------------");
    linear_x = 0;
    linear_y = 0;
    angular_z = 0;



    for (;;)
    {
        // gt the next event from the keyboard
        if(read(kfd, &c, 3)<0)
        {
            perror("read():");
            exit(-1);
        }

        printf("%s : %d, %d, %d\n", c, c[0], c[1], c[2]);

        clicked_cnt++;

        if(clicked_cnt> 50)
        {
            clicked_cnt = 0;

            puts("AGV Mecanum keyboard tele-op");
            puts("----------------------------");
            puts("          w i     Vx++" );
            puts("Wz++  a   s   d   Wz--" );
            puts("          x ,     Vx--" );
            puts("Vy++  j   k   l   Vy--"   );
            puts("----------------------------");
            puts("  5   lift down             ");
            puts("  7   lift up               ");
            puts("  0   go to home            ");
            puts("  +   charge_req            ");
            puts("  -   discharge_req         ");
            puts("  p   pillar demo req       ");
            puts("  P   pillar demo cancel    ");
            puts("----------------------------");
        }

        switch(c[0])
        {
            case KEYCODE_w:
            case KEYCODE_i:
                linear_x = linear_x + 0.02;
                dirty = true;
                break;

            case KEYCODE_d:
                angular_z = angular_z - 0.02;
                dirty = true;
                break;

            case KEYCODE_s:
                linear_x = 0;
                linear_y = 0;
                angular_z = 0;
                dirty = true;
                break;

            case KEYCODE_a:
                angular_z = angular_z + 0.02;
                dirty = true;
                break;

            case KEYCODE_x:
            case KEYCODE_comma:
                linear_x = linear_x - 0.02;
                dirty = true;
                break;

            case KEYCODE_j:
              //  linear_y = linear_y + 0.02;
              //  dirty = true;
                break;


            case KEYCODE_l:
             //   linear_y = linear_y - 0.02;
             //   dirty = true;
                break;

            case KEYCODE_k:
             //   linear_x = 0;
             //   linear_y = 0;
             //   angular_z = 0;
             //   dirty = true;
                break;
            
            case KEYCODE_0:
                nav_ctrl_msg.control = 1;
                nav_ctrl_msg.goal_name ="start";
                current_nav_ctrl = 0;
                nav_ctrl_pub.publish(nav_ctrl_msg);
                break;

            case KEYCODE_1:
                nav_ctrl_msg.control = 1;
                nav_ctrl_msg.goal_name ="point_1";
                current_nav_ctrl = 1;
                nav_ctrl_pub.publish(nav_ctrl_msg);
                break;

            case KEYCODE_2:
                nav_ctrl_msg.control = 1;
                nav_ctrl_msg.goal_name ="point_2";
                current_nav_ctrl = 2;
                nav_ctrl_pub.publish(nav_ctrl_msg);
                break;
            
            case KEYCODE_5:
                lift_req_msg.data = "lift_down_cmd";
                lift_req_pub.publish(lift_req_msg);
                break;

            case KEYCODE_7:
                lift_req_msg.data = "lift_up_cmd";
                lift_req_pub.publish(lift_req_msg);
                break;

            case KEYCODE_6:
                lift_req_msg.data = "lift_stop";
                lift_req_pub.publish(lift_req_msg);
                break;

            case KEYCODE_plus:

                break;

            case KEYCODE_minus:
            	switch(current_nav_ctrl)
            	{
            		case 0:
						nav_ctrl_msg.control = 0;
                		nav_ctrl_msg.goal_name ="start";
                		current_nav_ctrl = -1;
                		nav_ctrl_pub.publish(nav_ctrl_msg);
                		break;

            		case 1:
						nav_ctrl_msg.control = 0;
                		nav_ctrl_msg.goal_name ="point_1";
                		current_nav_ctrl = -1;
                		nav_ctrl_pub.publish(nav_ctrl_msg);
                		break;
            		
            		case 2:
						nav_ctrl_msg.control = 0;
                		nav_ctrl_msg.goal_name ="point_2";
                		current_nav_ctrl = -1;
                		nav_ctrl_pub.publish(nav_ctrl_msg);
                		break;
                		
					default :
						break;
				}
                break;

            case KEYCODE_p:
                pillar_req_msg.data = 1;
                pillar_req_pub.publish(pillar_req_msg);
                break;

            case KEYCODE_P:
                pillar_req_msg.data = 2;
                pillar_req_pub.publish(pillar_req_msg);
                break;

            default:
                break;
        }

        if(linear_x > linear_x_max_)
        {
            linear_x = linear_x_max_;
        }

        if(linear_x < -linear_x_max_)
        {
            linear_x = -linear_x_max_;
        }

        if(linear_y > linear_y_max_)
        {
            linear_y = linear_y_max_;
        }

        if(linear_y < -linear_y_max_)
        {
            linear_y = -linear_y_max_;
        }

        if(angular_z > angular_z_max_)
        {
            angular_z = angular_z_max_;
        }

        if(angular_z < -angular_z_max_)
        {
            angular_z = -angular_z_max_;
        }

        geometry_msgs::Twist twist;
        twist.angular.z = angular_z;
        twist.linear.x = linear_x;
        twist.linear.y = linear_y;

        if(dirty == true)
        {
            twist_pub.publish(twist);
            dirty = false;
            printf(" V_x: %f \n V_y: %f \n angular_z: %f \n", twist.linear.x, twist.linear.y, twist.angular.z);
        }

    }

    return;
}


std_msgs::String cmd_msg;

unsigned int loop_cnt;
unsigned int loop_cnt2;
int main(int argc, char **argv)
{
    // Init
    ros::init(argc, argv,"agv_teleop_diff_node");

    AgvTeleop agv_teleop;

    ros::Rate loop_rate(10);

    signal(SIGINT, quit);

    agv_teleop.keyLoop();
    while(ros::ok())
    {
		agv_teleop.timer_loop();
		ros::spin();
		loop_rate.sleep();
    }

    return(0);

}
