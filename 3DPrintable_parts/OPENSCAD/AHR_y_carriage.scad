// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// Y axis carriage and tube end part
// This part is based on ideas of the prusa i3 carriage

bearing_diameter=15.5;

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

module x_carriage_base(){
 
 // Bearings
 translate([29,-10.5,0]) rotate([0,0,90]) cube([24,58,10]);
 translate([29,4.6,0]) rotate([0,0,90]) cube([8.5,58,15]);
 // Base plate
 translate([-30,-10.5,0]) cube([60,70,6]);
 // Belt block
 translate([-30,12,0]) cube([60,10,17]);
 // Rods block
 translate([-30,45,0]) cube([60,15,17]);
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

module x_carriage_holes(){
 // Small bearing holder holes cutter
 translate([0,12,12]) rotate([0,-90,90]) horizontal_bearing_holes(2);
 // Main rod hole
 translate([0,0,12]) rotate([0,90,0]) cylinder(r=5.8,h=120,$fn=64,center=true);
 // Rods holes
 translate([-45/2,52,0])  cylinder(r=4.1,h=50,$fn=16,center=true);
 translate([45/2,52,0])  cylinder(r=4.1,h=50,$fn=16,center=true); 
 // Pulley hole
 translate([0,52,10.5]) rotate([90,0,0]) cylinder(r=4.08,h=20,$fn=16,center=true);
 // Motor holes
 translate([-25/2,52,0])  cylinder(r=1.7,h=50,$fn=16,center=true);
 translate([25/2,52,0])  cylinder(r=1.7,h=50,$fn=16,center=true);
 translate([-25/2,40,0])  cylinder(r=1.7,h=50,$fn=16,center=true);
 translate([25/2,40,0])  cylinder(r=1.7,h=50,$fn=16,center=true);
 translate([-25/2,52,10]) rotate([90,0,0]) cylinder(r=1.7,h=20,$fn=16,center=true);
 translate([25/2,52,10]) rotate([90,0,0]) cylinder(r=1.7,h=20,$fn=16,center=true);

 translate([0,52,0])  cylinder(r=1.65,h=50,$fn=16,center=true);
}

module x_carriage_fancy()
{
	translate([-50,-23,-16]) rotate([30,0,0]) cube([100,20,20]);
	translate([-50,-20,-1]) rotate([45,0,0]) cube([100,20,20]);
	translate([-50,59,0]) rotate([-30,0,0]) cube([100,20,20]);
	translate([-50,58,17.5]) rotate([-45,0,0]) cube([100,20,20]);

	translate([33,52,20]) rotate([0,-30,0]) cube([10,20,10],center=true);
	translate([-33,52,20]) rotate([0,30,0]) cube([10,20,10],center=true);

	translate([-32,-11,10]) rotate([90,0,45]) cube([10,40,10],center=true);
	translate([32,-11,10]) rotate([90,0,45]) cube([10,40,10],center=true);
	translate([-32,62.5,10]) rotate([90,0,45]) cube([10,40,10],center=true);
	translate([32,62.5,10]) rotate([90,0,45]) cube([10,40,10],center=true);
}

module y_carriage_rod_ends_base()
{
 // Rods block
 translate([0,0,0.75]) cube([45,14,1.5],center=true);
 translate([22,0,0.75]) cylinder(r=7,h=1.5,$fn=16,center=true);
 translate([-22,0,0.75]) cylinder(r=7,h=1.5,$fn=16,center=true);	
}

module y_carriage_rod_ends_holes()
{
 // M3 holes
 translate([-25/2,0,0])  cylinder(r=1.75,h=50,$fn=16,center=true);
 translate([25/2,0,0])  cylinder(r=1.75,h=50,$fn=16,center=true);

 translate([-25/2,0,0.75])  cylinder(r=3.2,h=10,$fn=16);
 translate([25/2,0,0.75])  cylinder(r=3.2,h=10,$fn=16);


 // center hole
 cylinder(r=4.5,h=10,$fn=6,center=true);

 
}

// Final part
module x_carriage(){
 translate ([0,0,0]) difference(){
  x_carriage_base();
  x_carriage_beltcut();
  x_carriage_holes();
  x_carriage_fancy();
 }
}

x_carriage();

// tube ends
translate ([40,20,0])rotate([0,0,90]) difference()
{
	y_carriage_rod_ends_base();
	y_carriage_rod_ends_holes();
}
