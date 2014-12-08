/**
 * Decorative star containing a LED strip.
 * Print the star container twice (print1 mode)
 * and print the blocker once (print2 mode)
 */

// Display mode
mode = "view"; // [all: Full star preview, view: Inside preview, print1:Half star, print2: Binder]

// Number of branches of the star
branches = 5; // [3:50]
// Internal radius of the star
int_radius = 21; // [1:500]
// External radius of the star
ext_radius = 60; // [1:500]
// Height of the half star
height = 20; // [1:500]
// Width of the star container
width = 2; // [1:10]
// Diameter of the cable space
cable_diam = 5; // [1:20]
// Diameter of the holding wire
wire_diam = 2; // [1:10]

/* [Hidden] */

// Resolution
$fa = 3; // Minimum angle
$fs = 0.5; // Maximum fragment size

/**
 * Draw one star branch
 */
module branch(branches, int_radius, ext_radius, height) {
	origin = [-int_radius/2, 0, 0];
	top = [0, 0, height];
	summit = [ext_radius, 0, 0];
	angle = 360 / branches / 2;
	int1 = [int_radius * cos(angle), int_radius * sin(angle), 0];
	int2 = [int_radius * cos(angle), -int_radius * sin(angle), 0];
	points = [origin, int1, int2, summit, top];
	faces = [[0, 2, 3, 1], [4, 3, 2], [4, 1, 3], [4, 2, 0], [4, 0, 1]];
	polyhedron(points = points, faces = faces);
}

/**
 * Draw a full star
 */
module star(branches, int_radius, ext_radius, height) {
	for (i = [0:branches-1]) {
		rotate([0, 0, i * 360 / branches]) {
			branch(branches, int_radius, ext_radius, height);
		}
	}
}

/**
 * Draw an empty star
 */
module empty_star(branches, int_radius, ext_radius, height, width) {
	difference() {
		star(branches, int_radius, ext_radius, height);
		translate([0, 0, -width*0.825])
			star(branches, int_radius, ext_radius, height);
	}
}

/**
 * Size computation for the LED strip
 */
function strip_height() = 10;
function strip_diam(leds) = (16.7 - 0.2) * leds / PI;
function strip_diam_in(leds) = strip_diam(leds) - 0.4;
function strip_diam_sup(leds) = strip_diam_in(leds);

/**
 * Draw a real-size model of the LED strip
 */
module led_strip(leds) {
	translate([0, 0, -5]) {
		difference() {
			cylinder(h = strip_height(), d = strip_diam(leds));
			cylinder(h = strip_height(), d = strip_diam_in(leds));
		}
	}
	for (i = [0:leds-1]) {
		rotate([0, 0, i * 360 / leds]) {
			translate([strip_diam(leds)/2, -2.5, -2.5])
				cube([2, 5, 5]);
		}
	}
}

/**
 * Star containing a LED strip
 */
module led_star() {
	difference() {
		// Base star
		union() {
			empty_star(branches, int_radius, ext_radius, height, width);
			difference() {
				intersection() {
					star(branches, int_radius, ext_radius, height, width);
					translate([-ext_radius, 0, 0])
						rotate([0, 90, 0])
							cylinder(d = cable_diam+width, h = ext_radius);
				}
				cylinder(h = support_height, d = strip_diam_sup(branches) + 4*width);
			}
		}
		// Cable space
		translate([-ext_radius, 0, 0])
			rotate([0, 90, 0])
				cylinder(d = cable_diam, h = ext_radius);
		// Holder wire space
		translate([0.75*ext_radius, 0, 0])
			cylinder(h = height, d = wire_diam);
	}
	// LED strip support
	support_strip_height = strip_height() / 2 + 0.5;
	support_height = support_strip_height + 2;
	intersection() {
		star(branches, int_radius, ext_radius, height, width);
		difference() {
			union() {
				cylinder(h = support_height, d = strip_diam_sup(branches));
				translate([0, 0, support_strip_height])
					cylinder(h = support_height - support_strip_height, d = strip_diam_sup(branches) + width);
			}
			cylinder(h = height, d = strip_diam_sup(branches)-width);
		}
	}
}

/**
 * Blocker to hold both star halves
 */
module star_blocker(shift) {
	blocker_height = 7;
	translate([0, 0, shift ? -blocker_height : 0]) {
		difference() {
			cylinder(d = strip_diam_sup(branches)-width-0.5, h = blocker_height * 2);
			cylinder(d = strip_diam_sup(branches)-2*width-0.5, h = blocker_height * 2);
		}
	}
}

// Half of the LED container
if (mode == "all" || mode == "view" || mode == "print1")
	led_star();
// Second half of the LED container
if (mode == "all")
	rotate([180, 0, 0])
		led_star();
// Blocker of the container
if (mode == "all" || mode == "view" || mode == "print2")
	color("red")
		star_blocker(mode != "print2");
// Model of the LED strip
if (mode == "all" || mode == "view")
	color("blue")
		led_strip(branches);