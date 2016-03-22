#pragma once //protects against multiple inclusions of this header file

#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include "StreetsDatabaseAPI.h"

//use these defines whenever you need earth radius 
//or conversion from degrees to radians
#define EARTH_RADIUS_IN_METERS 6372797.560856
#define DEG_TO_RAD 0.017453292519943295769236907684886

//getter functions for getting city latMin and latMax
double getLatMin();
double getLatMax();
double getLonMin();
double getLonMax();
double getStreetMaxSpeedLim (std::string street_name);

//function to load bin or osm map
bool load_map(std::string map_name);

//close the loaded map
void close_map();

//function to return intersection id for an intersection name
unsigned find_intersection_id_from_name(std::string intersection_name);

//function to return street id for an intersection name
unsigned find_street_id_from_name(std::string street_name);

//function to return the street segments for a given intersection 
std::vector<unsigned> find_intersection_street_segments(std::string intersection_name);
std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id);

//function to return street names at an intersection
std::vector<std::string> find_intersection_street_names(std::string intersection_name);
std::vector<std::string> find_intersection_street_names(unsigned intersection_id);

//can you get from intersection1 to intersection2 using a single street segment (hint: check for 1-way streets too)
bool are_directly_connected(std::string intersection_name1, std::string intersection_name2);

//find all intersections connected by one street segment from given intersection (hint: check for 1-way streets too)
std::vector<unsigned> find_adjacent_intersections(std::string intersection_name);

//for a given street, return all the street segments
std::vector<unsigned> find_street_street_segments(std::string street_name);

//for a given street, find all the intersections
std::vector<unsigned> find_all_street_intersections(std::string street_name);

//find distance between two coordinates
double find_distance_between_two_points(LatLon point1, LatLon point2);

//find the length of a given street segments
double find_street_segment_length(unsigned street_segment_id);

//find the length of a whole street
double find_street_length(std::string street_name);

//find the travel time to drive a street segment (time(minutes) = distance(Km)/speed_limit(Km/h)*60)
double find_segment_travel_time(unsigned street_segment_id);

//find the nearest point of interest (by name) to a given position
std::string find_closest_point_of_interest(LatLon my_position);

//find the common segment between intersection
unsigned common_intersection_intersection_segment(unsigned intersection_1, unsigned intersection_2); 

//see if the path is legal
bool path_is_legal(unsigned start_intersection, unsigned end_intersection, const std::vector<unsigned>& path); 

