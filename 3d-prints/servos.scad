$fn=32;
module servo_micro() {
	color("LightBlue", 0.5) {
		cube([23.5,12.6,16.4]);
		translate([-4.65,0,16.3]) difference() {
			cube([32.8,12.6,2]);
			translate([2.65,6.3,-0.1]) cylinder(r=1,h=3,$fn=45);
			translate([32.8-2.65,6.3,-0.1]) cylinder(r=1,h=3,$fn=45);
		}
		translate([0,0,18.2]) cube([23.5,12.6,4.4]);
		translate([6.3,6.3,22.5]) cylinder(r=6.3,h=4.1,$fn=45);
		translate([6.3,6.3,26.5]) cylinder(r=2.25,h=2.8,$fn=45);
	}
}
