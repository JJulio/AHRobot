// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// PS3 Camera holder (needs a 8mm carbon tube)

module rod_base(angle=20,height=20)
{
	rotate([angle,0,0]) cylinder(r=6,h=height,$fn=32);
}

module rod_holes(angle=20,height=22)
{
	rotate([angle,0,0]) cylinder(r=4.2,h=height,$fn=32);
}

module PS3Cam_base()
{
	cylinder(r=12,h=4.5,$fn=64);
	translate([-6,-29,0]) cube([12,20,4.5]);
	// Support to print the rod holder
	translate([-3,-38.3,0]) cube([6,30,4.5]);
	// Rod holder
	translate([0,-18,2]) rod_base(72);
}

module PS3Cam_holes()
{
	translate([-15,3.4,-1]) cube([30,20,10]);
	translate([0,0,-1]) cylinder(r=9,h=6,$fn=64);
	// Rod holder
	translate([0,-18,2]) rod_holes(72);
	// Cut elements lower than 0
	translate([0,0,-5]) cube([200,200,10],center=true);
}

module base()
{
	translate([-20,0,0]) cube([40,11,3]);
	translate([-8,-20,0]) cube([16,42,3]);
	//translate([-8,-33,0]) cube([16,49,3]);
	//translate([-20,-11-22,0]) cube([40,11,3]);
	// Rod reinforcement
	translate([-5,0,2.5]) cube([10,22,11]);
	
	// Rod holder
	translate([0,24,10]) rod_base(68,24);
}

module holes()
{
	translate([-15,5.5,0]) cylinder(r=1.7,h=10,$fn=8,center=true);
	translate([15,5.5,0]) cylinder(r=1.7,h=10,$fn=8,center=true);
	translate([0,-11,0]) cylinder(r=1.7,h=10,$fn=8,center=true);

	//translate([-15,-27.5,0]) cylinder(r=1.7,h=10,$fn=8,center=true);
	//translate([15,-27.5,0]) cylinder(r=1.7,h=10,$fn=8,center=true);
	
// rod holder
	translate([0,24,10]) rod_holes(68,22);

	// Cut elements lower than 0
	translate([0,0,-5]) cube([200,200,10],center=true);
}


difference()
{
 base();
 holes();
}


translate([36,10,0]) difference()
{
 PS3Cam_base();
 PS3Cam_holes();
}



