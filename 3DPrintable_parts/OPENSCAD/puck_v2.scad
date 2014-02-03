// AIR HOCKEY ROBOT PROJECT (AHROBOT)
// 
// by Jose Julio

// PUCK


// PUCK v2 (lighter version)
difference()
{
cylinder(r=30,h=5,center=false,$fn=100);
translate([0,0,-1]) cylinder(r=27.25,h=3,center=false,$fn=64);
translate([0,0,2.8]) cylinder(r=27.25,h=10,center=false,$fn=64);
}


/*
// PUSHER
difference()
{
cylinder(r=40,h=18,center=false,$fn=100);
translate([0,0,10]) cylinder(r=35,h=20,center=false,$fn=64);
}
cylinder(r=20,h=40,center=false,$fn=50);
translate([0,0,40]) sphere(r=20,$fn=50);
*/

