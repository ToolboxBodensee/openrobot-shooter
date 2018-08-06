use <servos.scad>
use <pd-gears/pd-gears.scad>
$fn=20;
pi=3.1415926535897932384626433832795;

mm_per_tooth=3;
gear_width=2.5;
length=60;
servo_shaft_r=2.2;
ball_r=2.5;
clearence=0.2;
simple_gear=1;

display_servo           = 0;
display_gear            = 0;
display_servo_mount     = 0;
display_bolt            = 1;

if(display_gear) {
    translate([6.3,6.3,26.0/*+0.5*/])
    color([0.8,0.8,0.8,0.9]) {
            servo_gear();
    }
}

if(display_bolt) {
    translate([2*abs(0.5-$t)*(length-17.1),0,0])
    translate([7.8-length*1.2,length/pi-mm_per_tooth/20-2.4,27+gear_width+clearence])
    color([0.8,0.8,0.8,0.9]) {
        bolt_cylinder();
    }
}
if(display_servo_mount) {
    servo_mount();
}

if(display_servo) {
    %servo_micro();
}
// gear
module servo_gear_() {
        union() {
        gear(
            $fn=10,
            mm_per_tooth= mm_per_tooth,
            number_of_teeth = length/mm_per_tooth,
            thickness       = gear_width+1,
            hole_diameter   = servo_shaft_r*2,
            twist           = 0,
            teeth_to_hide   = length/mm_per_tooth*0.25
        );
        }
}

module servo_gear()
{
    difference() {
        union() {
            cylinder(d=length/pi-mm_per_tooth/2,h=gear_width+2.5);
            offset_rot=10;

            if ($t<=0.5) {
                rot=offset_rot+$t*360;
                rotate([0,0,rot])
                    servo_gear_();
            } else {
                rot=offset_rot+180;
                rotate([0,0,rot])
                    servo_gear_();
            }
        }
        translate([0,0,-1]) {
            cylinder(h=gear_width*6,d=1.5);
            if (simple_gear) {
                cylinder(h=10,d=4.5);
            } else {
                gear(
                    $fn=1,
                    mm_per_tooth= 1.1,
                    //to big 1.3
                    //to small 0.9
                    number_of_teeth = 15,
                    thickness       = 2+gear_width,
                    hole_diameter   = 0,
                    twist           = 0,
                    teeth_to_hide   = 0
                );
            }
        }
    }
}

// spring system
module spring_holder()
{
    translate([0,0,-15]) {
        cylinder(d=3,h=30);
    }
    translate([0,0,-15]) {
        cylinder(d=5,h=3);
    }
    translate([0,0,-15+30-3]) {
        cylinder(d=5,h=3);
    }
    translate([0,0,-10]) {
        cylinder(d=5,h=20);
    }
}

// push
module bolt_cylinder(clearence=0,l=0, display_spring=false) {
    // linear gear
    rotate([180,0,0]) {
        rack (
            mm_per_tooth= mm_per_tooth,
            number_of_teeth = l/mm_per_tooth+length/mm_per_tooth*1.4,
            thickness       = gear_width+1+2*clearence,
            height          = 10+clearence
        );
    }
    // pusher
    translate([0,6,-1.75-clearence])
    rotate([0,90,0]) {
        cylinder(r=ball_r+clearence,h=l+length*1.4+2);
    }
    // sprin_plate
    translate([-2.5,-2,-6-1.75]) {
        cube([5,13,12]);
    }
    // spin holder
    translate([0,6,-1.75]) {
        spring_holder();
    }
    // spring
    if(display_spring) {
        translate([3,6,7.75])
        rotate([0,90,0]) {
                %cylinder(r=4,h=30);
        }
    }
}



// case
case_thickness=3;

module servo_mount() {
    difference() {
        union() {
            // servo holder
            translate([-7.5,0,12.2])
                cube([7.2,13,4]);
            translate([-5.5+29.3,0,12.2])
                cube([5.25,13,4]);
            translate([-7.5,-10,-5])
                cube([36.55,10,30]);
            translate([-7.5,-5+18,-5])
                cube([36.55,6,30]);

            // angle plate
            translate([-18,-10,-6]) {
                cube([20,29,3]);
                rotate([0,5,0])
                    cube([20,29,3]);
                rotate([0,10,0])
                    cube([20,29,3]);
            }

            // connection to barrel
            translate([22,-5+18,12.2])
                cube([10,14.5,12.5]);

            translate([length*1.2-27,0,0])
            difference() {
                union() {
                    // barrel cube
                    translate([10.2-length*1.2,5+length/pi-mm_per_tooth/20-5.4,20+gear_width]) {
                        cube([length*1.2+20,12,10]);
                    }
                    translate([7.8-length*1.2+25,length/pi-mm_per_tooth/20-5.4,27+gear_width+clearence]) {
                        // spring holder on barel
                        translate([12,10,-1.75]) {
                            spring_holder();
                        }
                        // ball filler extension
                        translate([18.5,5.5,-1.75]) {
                            cube([7,7,10]);
                        }
                    }

                }

                union() {
                    // barrel guide hole
                    translate([7.8-length*1.2,length/pi-mm_per_tooth/20-2.4,27+gear_width+clearence])
                        bolt_cylinder(clearence=clearence,l=20+5);

                    // barrel fill hole
                    translate([19.8-length*1.2+35,9+length/pi-mm_per_tooth/20-5.4,20+gear_width+4]) {
                        cylinder(r=ball_r+clearence, h=20);
                    }
                }
            }

        }
        translate([-2,6.4,12.2])
            cylinder(d=2,h=4);
        translate([-3.75+29.3,6.4,12.2])
            cylinder(d=2,h=4);
    }
}



// main body

/* piston_offset = -5; // 40; */

/* rotate([0,0,180]) { */
/* translate([0,100,0]) { */
/* // main body */
/* color("red") */
/* rotate([90, 0, 0]) */
/* import("./gun-orig-stl/weapon_tower_v2.stl"); */

/* // clip / mag */
/* color("green") */
/* translate([28.5, -25, 55]) */
/* rotate([90, 0, 0]) */
/* import("./gun-orig-stl/Magazin.stl"); */

/* // piston */
/* color("yellow") */
/* translate([20 + piston_offset, -25.5, 31.5]) */
/* rotate([90, 0, 0]) */
/* import("./gun-orig-stl/Weapon-Gear.stl"); */

/* // top cover */
/* color("blue") */
/* translate([44, -25, 100]) */
/* rotate([-90, 0, 0]) */
/* import("./gun-orig-stl/Magazin-Deckel_v1.stl"); */

/* color("cyan") */
/* translate([10, -32, 18]) */
/* rotate([0, 90, 0]) */
/* import("./gun-orig-stl/Servo_Gear_v4.stl"); */
/* } */
/* } */

//openrobot-shooter/gun-assembly.scad);
