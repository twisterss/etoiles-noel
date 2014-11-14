/**
 * Decorative star containing a LED strip.
 * Print the star container twice (print1 mode)
 * and print the blocker once (print2 mode)
 */

// Display mode (all, view, print1, print2)
mode = "view";

// Number of branches of the start
branches = 5;
// Internal radius of the star
int_radius = 20;
// External radius of the star
ext_radius = 60;
// Height of the half star
height = 19;
// Width of the star container
width = 1.7;
// Diameter of the cable space
cable_diam = 8;

// Resolution
$fa = 3; // Minimum angle (lower = higher max resolution, def 12)
$fs = 0.5; // Max fragment size (lower = higher min resolution, def 2)

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
		translate([0, 0, -width])
			star(branches, int_radius, ext_radius, height);
	}
}

/**
 * Size computation for the LED strip
 */
function strip_height() = 10;
function strip_diam(leds) = (16.7 - 1) * leds / PI;
function strip_diam_in(leds) = strip_diam(leds) - 1;

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
		empty_star(branches, int_radius, ext_radius, height, width);
		// Cable space
		translate([-ext_radius, 0, 0])
			rotate([0, 90, 0])
				cylinder(d = cable_diam, h = ext_radius);
	}
	// LED strip support
	intersection() {
		star(branches, int_radius, ext_radius, height, width);
		difference() {
			union() {
				cylinder(h = height, d = strip_diam_in(branches));
				translate([0, 0, strip_height() / 2 + 0.5])
					cylinder(h = height, d = strip_diam_in(branches) + 1.5);
			}
			cylinder(h = height, d = strip_diam_in(branches)-width);
		}
	}
}

/**
 * Blocker to hold both star halves
 */
module star_blocker(shift) {
	blocker_height = 6;
	translate([0, 0, shift ? -blocker_height : 0]) {
		difference() {
			cylinder(d = strip_diam_in(branches)-width, h = blocker_height * 2);
			cylinder(d = strip_diam_in(branches)-2*width, h = blocker_height * 2);
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