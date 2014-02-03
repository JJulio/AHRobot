// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// X axis motor mount


motor_center_x = 22.5;
motor_center_y = -22.5;
base_height = 2.5;


module x_motor_mount_base()
{	
	translate([0,-46,0]) cube([45,46,base_height]); // Motor mount
	
	// mount part
	translate([0,0,0]) cube([motor_center_x*2,20,base_height]);

	// Reinforce
	//translate([2.5,-20,0]) cube([2,42,base_height*1.5]);
	//translate([16,-6,0]) cube([2,28,base_height*1.5]);
	//translate([32,-6,0]) cube([2,28,base_height*1.5]);
	//translate([45.5,-20,0]) cube([2,42,base_height*1.5]);

}

module x_motor_mount_holes()
{
	// Center motor hole
	translate([motor_center_x,motor_center_y,2]) cylinder(r=13,h=5,center=true,$fn=16);
	// Motor holes
	translate([motor_center_x-15.5,motor_center_y-15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x+15.5,motor_center_y-15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x-15.5,motor_center_y+15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x+15.5,motor_center_y+15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
 
	
   // Mount holes
	translate([motor_center_x-12.5,12.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x+12.5,12.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);

	// motor mount cuts
	translate([motor_center_x*2+25,-motor_center_x*4+3,-1]) rotate([0,0,45]) cube([50,50,4]); // Motor mount
	translate([-25,-motor_center_x*4+3,-1]) rotate([0,0,45]) cube([50,50,4]); // Motor mount

}





translate([0,0,0]) rotate ([0,0,0]) difference()
{
x_motor_mount_base();
x_motor_mount_holes();
}



