// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// Y axis parts (motot mount & end part)


motor_center_x = 24.5;
motor_center_y = -22;
separation = 14;            // laterial separation to center of rod
base_height = 2.5;

y_end_part_pulley_separation = 22;
y_end_height = 16;
y_end_length = 22;


module y_motor_part_base()
{	
	translate([0,-46,0]) cube([49,46,base_height]); // Motor mount
	
	// Rod holder part
	translate([0,0,0]) cube([motor_center_x*2,22,base_height]);

	// Reinforce
	translate([2.5,-20,0]) cube([2,42,base_height*1.5]);
	translate([15,-5,0]) cube([2,27,base_height*1.5]);
	translate([32,-5,0]) cube([2,27,base_height*1.5]);
	translate([44.5,-20,0]) cube([2,42,base_height*1.5]);

	// Rod holder part
	rod_holder_base();

}

module y_motor_part_holes()
{
	// Center motor hole
	translate([motor_center_x,motor_center_y,2]) cylinder(r=13,h=5,center=true,$fn=16);
	// Motor holes
	translate([motor_center_x-15.5,motor_center_y-15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x+15.5,motor_center_y-15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x-15.5,motor_center_y+15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x+15.5,motor_center_y+15.5,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
 
	
   // Mount holes
	translate([motor_center_x-15.5,10,2]) cylinder(r=1.8,h=5,center=true,$fn=8);
	translate([motor_center_x+15.5,10,2]) cylinder(r=1.8,h=5,center=true,$fn=8);

	// motor mount cuts
	translate([motor_center_x*2+15,-motor_center_x*4+3,-1]) rotate([0,0,45]) cube([50,50,4]); // Motor mount
	translate([-15,-motor_center_x*4+3,-1]) rotate([0,0,45]) cube([50,50,4]); // Motor mount

	// Rod holder holes	
	rod_holder_holes();	

}

module rod_holder_base()
{
	translate([motor_center_x-9,0,0]) cube([18,18,6+separation+0.5]);
}
module rod_holder_holes()
{
	translate([motor_center_x,9,separation]) rotate([90,0,0]) cylinder(r=4.1,h=20,$fn=24,center=true);  // 8mm rod


// Rod bar aperture
	translate([motor_center_x-0.5,-1,60]) rotate([0,90,0]) cube([50,50,1]); 

/*
// Cube bevel
	translate([motor_center_x-9,16,base_height+0.1]) rotate([0,0,45]) cube([24,24,10+separation-base_height]);
	translate([motor_center_x,25,base_height+0.1]) rotate([0,0,-45]) cube([24,24,10+separation-base_height]);
	translate([motor_center_x-26,-15,base_height+0.1]) rotate([0,0,-45]) cube([24,24,10+separation-base_height]);
	translate([motor_center_x+15,-26,base_height+0.1]) rotate([0,0,45]) cube([24,24,10+separation-base_height]);
*/
}


// *** Y_END ***
module y_end_part_base()
{
	cube([separation+8+6,12,y_end_height]);
	translate([0,12,0]) cube([6,y_end_part_pulley_separation,y_end_height]);
	translate([separation+8,12,0]) cube([6,y_end_part_pulley_separation,y_end_height]);

	//translate([0,0,12]) cube([2,25,10]);
	//translate([0,0,-10]) cube([2,25,10]);

	// Bearing
	//translate([6.25,12+end_part_pulley_separation,6]) rotate([0,90,0]) cylinder(r=5,h=0.5,$fn=16,center=true);
	//translate([13.75,12+end_part_pulley_separation,6]) rotate([0,90,0]) cylinder(r=5,h=0.5,$fn=16,center=true);

}

module y_end_part_holes()
{
   // CUTS
	translate([separation-0.5,-1,y_end_height/2-5]) cube([1,20,10]);
	translate([separation-5,-1,y_end_height/2]) cube([10,20,1]);

	translate([separation,0,y_end_height/2]) rotate([90,0,0]) cylinder(r=4.05,h=25,$fn=16,center=true);
	// Bearing hole 8mm (608)
	translate([separation,12+y_end_part_pulley_separation,y_end_height/2]) rotate([0,90,0]) cylinder(r=4,h=separation+8+4,$fn=32,center=true);
	
	//translate([0,12+y_end_part_pulley_separation,0]) cube([20,10,50]);

	translate([0,16,y_end_height/2]) rotate([0,90,0]) cylinder(r=1.6,h=50,$fn=12,center=true);
	translate([0,25,y_end_height/2]) rotate([0,90,0]) cylinder(r=1.6,h=50,$fn=12,center=true);

	translate([y_end_length,16,y_end_height/2]) rotate([0,90,0]) cylinder(r=3.9,h=35,$fn=16,center=true);
	translate([y_end_length,25,y_end_height/2]) rotate([0,90,0]) cylinder(r=3.9,h=35,$fn=16,center=true);
}


translate([0,0,0]) rotate ([0,0,0]) difference()
{
y_motor_part_base();
y_motor_part_holes();
}


translate([-30,-20,0]) difference()
{
y_end_part_base();
y_end_part_holes();
}




