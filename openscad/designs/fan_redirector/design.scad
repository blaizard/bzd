// --- parameters

$fn=100;
height = 80;
width = 110;
thinkness = 2;
widthCables = 30;

// ----
difference() {
    // main plate
    cube([width-10, height, thinkness]);
    // for the cables
    translate([15, -0.1, -5]) {
        cube([widthCables, 5, 10]);
    }
}
// small curve
rotate([0,50,0]) {
    translate([-10, 0, 0]) {
        cube([10, height, thinkness]);
        translate([-5, 4, 1]) {
            rotate([0, 90, 0]) {
                cylinder(h=10, d=2);
            }
        }
        translate([-5, height-4, 1]) {
            rotate([0, 90, 0]) {
                cylinder(h=10, d=2);
            }
        }
    }
}
// bottom
translate([-6, height - thinkness, 0]) {
    cube([width-20, thinkness, 8]);
}