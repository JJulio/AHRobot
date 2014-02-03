// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// X axis carriage base

diameter = 80;
rod_separation=45;
bearing_diameter=15.5;
bearing_length=26;


module border()
{
	difference()
	{
	cylinder (r=diameter/2, h=4.5,center=true,$fn=80);
	cylinder (r=diameter/2-4.5, h=6,center=true,$fn=80);
	}
}

module x_carriage_beltcut(){
 // Cut in the middle for belt
 translate([-5.5+1,16.5,8]) cube([8,25,15]);
 // Cut clearing space for the belt
 //translate([-38,5,8]) cube([40,13,15]);
 // Belt slit
 translate([-32,16,8]) cube([80,1.25,15]);
 // Smooth entrance
 translate([-30,16.5,15.3]) rotate([45,0,0]) cube([80,15,15]);
 // Smooth the block on the upper side
 //translate([-66,27,15]) rotate([30,0,0]) cube([80,15,15]);
 // Teeth cuts (GT2)
 for ( i = [0 : 32] ){
  translate([30-i*2,14.5,8]) cube([1.15,2,15]);
 }
}

module belt()
{
	difference()
	{
	translate ([9,0,8]) cube([12,38,16],center=true);
	translate([25,0,-2]) rotate([0,0,90]) x_carriage_beltcut();
	}
}

module x_carriage_base()
{	
	cylinder (r=diameter/2, h=2,center=true,$fn=80);
	translate([0,0,2]) border();

	// bearing bases
	translate([rod_separation/2,0,5]) cube([20,bearing_length,10],center=true);
	translate([-rod_separation/2,0,5]) cube([20,bearing_length,10],center=true);

	//belt();
}

module x_carriage_holes()
{
	// bearing holes
	translate([rod_separation/2,0,-2]) horizontal_bearing_holes();
	translate([-rod_separation/2,0,-2]) horizontal_bearing_holes();

	// holes for belt
	translate([7.5,23,0]) cylinder(r=1.65,h=20,center=true,$fn=8);
	translate([7.5,-23,0]) cylinder(r=1.65,h=20,center=true,$fn=8);
	translate([-7.5,23,0]) cylinder(r=1.65,h=20,center=true,$fn=8);
	translate([-7.5,-23,0]) cylinder(r=1.65,h=20,center=true,$fn=8);
	

	// M3 holes
	translate([0,-30,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([15,-28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-15,-28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([0,30,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([15,28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-15,28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);

	translate([rod_separation/2,-20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([rod_separation/2,20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-rod_separation/2,-20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-rod_separation/2,20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
}


module border_pusher()
{
	difference()
	{
	cylinder (r=diameter/2, h=12.5,$fn=80);
	cylinder (r=diameter/2-5, h=15,$fn=80);
	}
}

module x_carriage_pusher_base()
{	
	cylinder (r=diameter/2, h=2,$fn=80);
	translate([0,0,0]) border_pusher();

	translate([30,0,5.5]) cube([10,5,11],center=true);
	rotate([0,0,25])translate([30,0,5.5]) cube([10,5,11],center=true);
	rotate([0,0,-25])translate([30,0,5.5]) cube([10,5,11],center=true);
	
}


module x_carriage_pusher_holes()
{
	cylinder (r=diameter/2-14, h=50,center=true,$fn=80);	

	// M3 holes
	translate([0,-30,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([15,-28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-15,-28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([0,30,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([15,28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-15,28,0]) cylinder(r=1.7,h=20,center=true,$fn=8);

	translate([rod_separation/2,-20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([rod_separation/2,20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-rod_separation/2,-20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);
	translate([-rod_separation/2,20,0]) cylinder(r=1.7,h=20,center=true,$fn=8);

	// holes for belt
	translate([7.5,23,0]) cylinder(r=4,h=20,center=true,$fn=8);
	translate([7.5,-23,0]) cylinder(r=4,h=20,center=true,$fn=8);
	translate([-7.5,23,0]) cylinder(r=4,h=20,center=true,$fn=8);
	translate([-7.5,-23,0]) cylinder(r=4,h=20,center=true,$fn=8);
}


/*
difference()
{
x_carriage_base();
x_carriage_holes();
}
*/

difference()
{
x_carriage_pusher_base();
x_carriage_pusher_holes();
}




