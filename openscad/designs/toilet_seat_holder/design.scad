// --- parameters

$fn=100;
height = 20;
outerWidth = 18;
holdingOuterWidth = 25;
innerScrewWidth = 5.5;
minWidth = 1;
nutWidth = 9;
nutHeight = 6;

// ----
difference() {
    difference() {
        union() {
            cylinder(h=height, d1=outerWidth, d2=innerScrewWidth + minWidth * 2, center=true);
            translate([0, 0, -(height - 5) / 2]) {
                cylinder(h=5, d=holdingOuterWidth, center=true);
            }
            cube([4, outerWidth, height], center=true);
        }
        cylinder(h=height + 2, d=innerScrewWidth, center=true);
    }
    translate([0, 0, -height / 2 + nutHeight / 2 - 0.1]) {
        cylinder(h=nutHeight, d=nutWidth, center=true);
    }
}
