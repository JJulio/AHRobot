// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// X axis carriage belt holder

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
 translate([0,0,12]) cube([8,25,15],center=true);
 // Cut clearing space for the belt
 //translate([-38,5,8]) cube([40,13,15]);
 // Belt slit
 translate([0,0,12]) cube([80,1.25,15],center=true);
 // Smooth entrance
 translate([0,0,24]) rotate([45,0,0]) cube([80,15,15],center=true);
 // Smooth the block on the upper side
 //translate([-66,27,15]) rotate([30,0,0]) cube([80,15,15]);
 // Teeth cuts (GT2)
 for ( i = [-16 : 16] ){
  translate([-i*2,-1,12]) cube([1.15,1.5,15],center=true);
 }
}

module belt()
{
	difference()
	{
	union()
		{
		translate ([0,0,1]) cube([10,45,2],center=true);
		translate([0,23,1]) cylinder(r=5,h=2,center=true,$fn=16);
		translate([0,-23,1]) cylinder(r=5,h=2,center=true,$fn=16);
		}
	union()
		{
		// holes for belt
		translate([0,23,0]) cylinder(r=1.65,h=20,center=true,$fn=8);
		translate([0,-23,0]) cylinder(r=1.65,h=20,center=true,$fn=8);	
		}
	}	
	difference()
	{
	translate ([0,0,18]) cube([10,38,36],center=true);
	union()
		{
		translate([0,0,20]) rotate([0,0,90]) x_carriage_beltcut();
		// holes
		translate([0,-8,10]) rotate([0,90,0]) cylinder(r=6,h=100,center=true,$fn=6);
		translate([0,8,10]) rotate([0,90,0]) cylinder(r=6,h=100,center=true,$fn=6);
		}
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
/*
difference()
{
x_carriage_pusher_base();
x_carriage_pusher_holes();
}
*/

belt();




