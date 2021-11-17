// --- parameters

$fn=100;
height = 20;
outerWidth = 18;
holdingOuterWidth = 25;
innerScrewWidth = 6;
outerScrewWidth = 8;
minWidth = 1;
nutWidth = 9;
nutHeight = 6;

// ----
difference() {
    difference() {
        union() {
            cylinder(h=height, d1=outerWidth, d2=outerScrewWidth + minWidth * 2, center=true);
            translate([0, 0, -(height - 5) / 2]) {
                cylinder(h=5, d=holdingOuterWidth, center=true);
            }
            cube([4, outerWidth, height], center=true);
        }
        cylinder(h=height + 2, d1=innerScrewWidth, d2=outerScrewWidth,center=true);
    }
    translate([0, 0, -height / 2 + nutHeight / 2 - 0.1]) {
        cylinder(h=nutHeight, d=nutWidth, center=true);
    }
}
