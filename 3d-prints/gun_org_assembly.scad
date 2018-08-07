
piston_offset = 0; // 60;

// main body
color("red")
rotate([90, 0, 0])
import("./gun-orig-stl/weapon_tower_v2.stl");

// clip / mag
color("green")
translate([28.5, -25, 55])
rotate([90, 0, 0])
import("./gun-orig-stl/Magazin.stl");

// piston
color("yellow")
translate([20 + piston_offset, -25.5, 31.5])
rotate([90, 0, 0])
import("./gun-orig-stl/Weapon-Gear.stl");

// top cover
color("blue")
translate([44, -25, 100])
rotate([-90, 0, 0])
import("./gun-orig-stl/Magazin-Deckel_v1.stl");

color("cyan")
translate([10, -32, 18])
rotate([0, 90, 0])
import("./gun-orig-stl/Servo_Gear_v4.stl");
