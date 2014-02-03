// PART: http://www.thingiverse.com/thing:174353
// AntonioNav
// LM8UU for Print in PLA

$fn=80;

radio_ext=15/2;
radio_int=7.95/2;	//Para varilla M8
altura=23.8;
grosor=0.9;
num_dientes=9;


module varilla_lisa_M8(){
	cylinder(r=4,h=70);
}

module diente(){
	d_ancho=1;
	d_largo=radio_ext-d_ancho-0.2-radio_int;

	translate([0,-radio_int,0])
		hull(){
			translate([0,-d_ancho/2,0])
				cylinder(r=d_ancho/2,h=altura);
			translate([0,-d_largo,0])
				cylinder(r=d_ancho+0.2,h=altura);
		}

}

module lm8uu(){

	difference(){
		cylinder(r=radio_ext,h=altura);
		translate([0,0,-1])
			cylinder(r=radio_ext-grosor,h=altura+2);
	}
	for (i=[0:(num_dientes-1)]){
		rotate(i*360/num_dientes,[0,0,1])
			diente();
	}
}

//color("blue") varilla_lisa_M8();
lm8uu();





