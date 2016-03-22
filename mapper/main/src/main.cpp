#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "minPriorityQueue.h"
#include <iostream>

int main() {
	
	std::string input_file = "~/Dropbox/mapper_proj/toronto.osm.bin";

    draw_map(input_file);

    return 0;
}
