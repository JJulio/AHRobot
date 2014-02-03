// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// X axis carriage
// designed for only two bearings (I could test a 3 bearings version)

diameter = 80;
rod_separation=45;
bearing_diameter=15.5;
bearing_length=26;

module horizontal_bearing_holes(bearings=1){
 cutter_lenght = 4+bearings*25;
 one_holder_lenght = 8+25;
 holder_lenght = 8+bearings*25;
 
 // Main bearing cut
 difference(){
  translate(v=[0,0,12]) rotate(a=[90,0,0]) translate(v=[0,0,-cutter_lenght/2]) cylinder(h = cutter_lenght, r=bearing_diameter/2, $fn=64);
  // Bearing retainers
  translate(v=[0,-holder_lenght/2,3]) cube(size = [24,6,34], center = true);
  translate(v=[0,+holder_lenght/2,3]) cube(size = [24,6,34], center = true);
 }
 
 // Ziptie cutouts

 ziptie_cut_ofset = 0;
 for ( i = [0 : bearings-1] ){
  // For easier positioning I move them by half of one 
  // bearing holder then add each bearign lenght and then center again
  translate(v=[0,-holder_lenght/2,0]) translate(v=[0,one_holder_lenght/2+i*25,0]) difference(){
   union(){
    translate(v=[0,2-6,12]) rotate(a=[90,0,0]) translate(v=[0,0,0]) cylinder(h = 4, r=12.5, $fn=50);
    translate(v=[0,2+6,12]) rotate(a=[90,0,0]) translate(v=[0,0,0]) cylinder(h = 4, r=12.5, $fn=50);
   }
   translate(v=[0,10,12]) rotate(a=[90,0,0]) translate(v=[0,0,0]) cylinder(h = 24, r=9.5, $fn=50);
  }
 }

 
}

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

difference()
{
x_carriage_base();
x_carriage_holes();
}




