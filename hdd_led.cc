// This script flashes the ThinkPad light when there is hard drive activity.
// Run this command to compile it:
//   g++ -Wall -Wextra -O2 -o hdd_led hdd_led.cc
// Then run as root.
//
// Script by Dan Stahlke, dan@stahlke.org.  BSD license.  No warranty.

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

// This is the interface to the LED.
static const char *led_fn = "/proc/acpi/ibm/led";
// The file to watch.  Whenever this changes, the LED flashes.  You could for example tie the
// LED to network activity or something by changing this.
static const char *stats_fn = "/proc/diskstats";
// This defines the check and flash intervals, in microseconds.
static const int check_interval = 200000; // 5 Hz
static const int flash_off_interval = 40000; // flash at 20 Hz, 20% duty cycle
static const int flash_on_interval  = 10000;

void set_led(int new_status) {
	int led_id = 0;
	std::ofstream fh(led_fn, std::ios::out);
	if(fh) {
		fh << led_id << " " << (new_status ? "on" : "off") << std::endl;
	} else {
		std::cerr << "Could not open " << led_fn << ", aborting." << std::endl;
		exit(1);
	}
}

// Easiest way to read a file, from
// http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
// The faster methods don't work, probably because of bad interaction of seek with /proc files.
std::string get_file_contents(const char *fn) {
	std::ifstream in(fn, std::ios::in | std::ios::binary);
	if(in) {
		return std::string(
			std::istreambuf_iterator<char>(in),
			std::istreambuf_iterator<char>()
		);
	}
	std::cerr << "Error reading " << fn << ", aborting." << std::endl;
	exit(1);
}

int main() {
	std::string last_stat = get_file_contents(stats_fn);
	for(;;) {
		while(get_file_contents(stats_fn) != last_stat) {
			last_stat = get_file_contents(stats_fn);
			set_led(0);
			usleep(flash_off_interval);
			set_led(1);
			usleep(flash_on_interval);
		}
		usleep(check_interval);
	}
}