#include "m1.h"
#include <iostream>
#include <cmath>
#include <limits>

using namespace std;

// pre-allocate data structs for fast access
unordered_map<string,unsigned int> interNameInterID_map;
//first element is Street ID, second is max Speed limit
unordered_map<string,pair<unsigned int,double>> streetNameStreetID_map;
unordered_map<string,vector<unsigned int>> streetNameSegID_map;
unordered_map<string,unordered_set<unsigned int>> streetNameInterID_map;
vector<pair<LatLon,string>> POI_vec;
//keep track of the latMin and latMax of the city
double latMin = 360;
double latMax = -360;
//keep track of the lonMin and lonMax of the city
double lonMin = 360;
double lonMax = -360;

//keep track of how many unknown streets
unsigned int numUnknownStreet = 0;

const unsigned kMaxInter = std::numeric_limits<unsigned>::max();

//load the map
bool load_map(std::string map_name) {

    bool load_success = loadStreetDatabaseBIN(map_name);

	unsigned long long numInt = getNumberOfIntersections();
	for (unsigned int inter_ID=0; inter_ID<numInt; inter_ID++)
	{
   		//populate the inter name/id pair map
		string intName = getIntersectionName(inter_ID);
		interNameInterID_map.insert({intName,inter_ID});
	
		//populate the streetNameInterID map
		for (auto segID : find_intersection_street_segments(inter_ID))
		{
			unsigned int stID = getStreetSegmentStreetID(segID);
			string stName = getStreetName(stID);
			if (streetNameInterID_map.find(stName) == streetNameInterID_map.end())
				streetNameInterID_map[stName] = {inter_ID};
			else
				streetNameInterID_map[stName].insert(inter_ID);
		}
		//keep track of min and max lat/lon of the city
		LatLon tmp_LatLon = getIntersectionPosition(inter_ID);
		if (tmp_LatLon.lat > latMax) latMax = tmp_LatLon.lat;
		if (tmp_LatLon.lat < latMin) latMin = tmp_LatLon.lat; 
		
		if (tmp_LatLon.lon > lonMax) lonMax = tmp_LatLon.lon;
		if (tmp_LatLon.lon < lonMin) lonMin = tmp_LatLon.lon; 
	}

	//populate the street name/id pair map
	unsigned long long numSt = getNumberOfStreets();
	for (unsigned int ii=0; ii<numSt; ii++)
	{
		string stName = getStreetName(ii);
		pair<unsigned int,double> myPair = {ii,0};
		streetNameStreetID_map.insert({stName,myPair});
	}
	//populate the street name seg id map
	unsigned long long numSegs = getNumberOfStreetSegments();
	for (unsigned int ii=0; ii<numSegs; ii++)
	{
		//street id
		unsigned int stID = getStreetSegmentStreetID(ii);
		//street name
		string stName = getStreetName(stID);
		//update the street's max speed limit
		double speedLim = getStreetSegmentSpeedLimit(ii);
		auto iter = streetNameStreetID_map.find(stName);
		//only set max speed limit for not unknown streets, unknown streets capped at 20 max speed limit
		if (iter != streetNameStreetID_map.end()) {
			if (!stName.compare("(unknown)"))
				(iter->second).second = 20;
			else if (speedLim > (iter->second).second)
				(iter->second).second = speedLim; 
		}

		if (streetNameSegID_map.find(stName) == streetNameSegID_map.end())
			streetNameSegID_map[stName] = {ii};
		else
			//many (unknown) streets, will push all segments into one key-vector
			streetNameSegID_map[stName].push_back(ii);	
	}

	//populate the POI vector
	unsigned long long numPOI = getNumberOfPointsOfInterest();
	for (unsigned int idx=0; idx<numPOI; idx++) {
		pair<LatLon,string> POI({getPointOfInterestPosition(idx),getPointOfInterestName(idx)});
		POI_vec.push_back(POI);
	}

    return load_success;
}

//close the map
void close_map() {
    closeStreetDatabase();
    
    // destroy any data structures you created in load_map
    // ...
}

//implement the rest of the functions provided in m1.h

unsigned find_intersection_id_from_name(std::string intersection_name) {
	unordered_map<string,unsigned int>::iterator iter1 = interNameInterID_map.find(intersection_name);
	
	if (iter1 != interNameInterID_map.end())
		return iter1->second;
	else
		return getNumberOfIntersections();
}

unsigned find_street_id_from_name(std::string street_name){
	auto iter1 = streetNameStreetID_map.find(street_name);

	if (iter1 != streetNameStreetID_map.end())
		return (iter1->second).first;
	else
		return getNumberOfStreets();
}

std::vector<unsigned> find_intersection_street_segments(std::string intersection_name) {
		//first find the intersection id from name
		unsigned interID = find_intersection_id_from_name(intersection_name);
		unsigned numISSegs = getIntersectionStreetSegmentCount(interID);

		vector<unsigned> vec;

		for (unsigned int ii=0; ii<numISSegs; ii++) 
		{
			unsigned int sSegID = getIntersectionStreetSegment(interID,ii);
			vec.push_back(sSegID);
		}

		return vec;
}

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {
	
	    unsigned numISSegs = getIntersectionStreetSegmentCount(intersection_id);

	    vector<unsigned> vec;

		for (unsigned int ii=0; ii<numISSegs; ii++) 
		{
			unsigned int sSegID = getIntersectionStreetSegment(intersection_id,ii);
			vec.push_back(sSegID);
		}

		return vec;
}

std::vector<std::string> find_intersection_street_names(std::string intersection_name) {
	//first find the street segments ids of this intersection
	vector<unsigned> vec = find_intersection_street_segments(intersection_name);
	set<string> segIDset;	

	for (unsigned int ii=0; ii<vec.size(); ii++) {
		unsigned streetID = getStreetSegmentStreetID(vec[ii]);
		std::string streetName = getStreetName(streetID);
		segIDset.insert(streetName);
	}
	
	vector<string> streetNameVec(segIDset.begin(), segIDset.end());
	return streetNameVec;
}	

std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {
	//first find the street segments ids of this intersection
	vector<unsigned> vec = find_intersection_street_segments(intersection_id);
	set<string> segIDset;	

	for (unsigned int ii=0; ii<vec.size(); ii++) {
		unsigned streetID = getStreetSegmentStreetID(vec[ii]);
		std::string streetName = getStreetName(streetID);
		segIDset.insert(streetName);
	}
	
	vector<string> streetNameVec(segIDset.begin(), segIDset.end());
	return streetNameVec;
}	

bool are_directly_connected(std::string intersection_name1, std::string intersection_name2) {

	vector<unsigned> streetSegs1 = find_intersection_street_segments(intersection_name1);
	//copy the street segments into an unordered_set for fast lookup
	unordered_set<unsigned> streetSegSet;
	for (unsigned int ii=0; ii<streetSegs1.size(); ii++) {
		streetSegSet.insert(streetSegs1[ii]);
	}
	
	vector<unsigned> streetSegs2 = find_intersection_street_segments(intersection_name2);
	for (unsigned int ii=0; ii<streetSegs2.size(); ii++) {
		unordered_set<unsigned>::const_iterator iter1 = streetSegSet.find(streetSegs2[ii]);
		if (iter1 != streetSegSet.end())
		{
			unsigned common_seg = *iter1;
			if (!getStreetSegmentOneWay(common_seg)){
				return true;
			}
			else {
				StreetSegmentEnds SE = getStreetSegmentEnds(common_seg);
				unsigned id1 = find_intersection_id_from_name(intersection_name1);
				unsigned id2 = find_intersection_id_from_name(intersection_name2);
				if (SE.from == id1 && SE.to == id2)
					return true;
			}
		}		
	}
	
	return false; 
}

unsigned common_intersection_intersection_segment(unsigned intersection_1, unsigned intersection_2) {

	vector<unsigned> streetSegs1 = find_intersection_street_segments(intersection_1);	
	//copy the street segments into an unordered_set for fast lookup
	unordered_set<unsigned> streetSegSet;
	for (unsigned int ii=0; ii<streetSegs1.size(); ii++) {
		streetSegSet.insert(streetSegs1[ii]);
	}
	
	vector<unsigned> streetSegs2 = find_intersection_street_segments(intersection_2);
	for (unsigned int ii=0; ii<streetSegs2.size(); ii++) {
		//watch out for one way street segments, skip those
		StreetSegmentEnds SE = getStreetSegmentEnds(streetSegs2[ii]);
		if (getStreetSegmentOneWay(streetSegs2[ii]) && SE.to == intersection_1 && SE.from == intersection_2) continue;	

		//see if we found the right common segment
		unordered_set<unsigned>::const_iterator iter1 = streetSegSet.find(streetSegs2[ii]);
		if (iter1 != streetSegSet.end()) return *iter1;
	}
	return kMaxInter; 
}

//find all intersections connected by one street segment from given intersection (hint: check for 1-way streets too)
std::vector<unsigned> find_adjacent_intersections(std::string intersection_name) {

	vector<unsigned> retVec;
	vector<unsigned> streetSegs = find_intersection_street_segments(intersection_name);
	unsigned inter_id = find_intersection_id_from_name(intersection_name);

	for (unsigned int ii=0; ii<streetSegs.size(); ii++)
	{
		StreetSegmentEnds SE = getStreetSegmentEnds(streetSegs[ii]);
		if (getStreetSegmentOneWay(streetSegs[ii])) 
		{
			if (SE.from == inter_id)
				retVec.push_back(SE.to);
		}
		else {
			 if (SE.from == inter_id)
				 retVec.push_back(SE.to);
			 else
				 retVec.push_back(SE.from); 
		}
	}

	return retVec;
}

//for a given street, return all the street segments
std::vector<unsigned> find_street_street_segments(std::string street_name) {
	return streetNameSegID_map[street_name];
}

//for a given street, find all the intersections
std::vector<unsigned> find_all_street_intersections(std::string street_name) {
	
	//copy the set into a vector
	unordered_set<unsigned int> interIDset = streetNameInterID_map[street_name];	
	vector<unsigned> interIDvec(interIDset.begin(), interIDset.end());
	return interIDvec;
}

//find distance between two coordinates
double find_distance_between_two_points(LatLon point1, LatLon point2) {
	//convert the LatLon points into x,y coordinates
	double latAvg = DEG_TO_RAD*(point1.lat + point2.lat)/2;	
	pair<double,double> xy1({DEG_TO_RAD*point1.lon*cos(latAvg),DEG_TO_RAD*point1.lat});	
	pair<double,double> xy2({DEG_TO_RAD*point2.lon*cos(latAvg),DEG_TO_RAD*point2.lat});

	//calculate the distance using Pythagoras and return
	return EARTH_RADIUS_IN_METERS*sqrt(pow((get<1>(xy1)-get<1>(xy2)),2) + pow((get<0>(xy1)-get<0>(xy2)),2)); 	
}

//find the length of a given street segments
double find_street_segment_length(unsigned street_segment_id) {
	double length = 0;
	//first get the street segment's curve point count
	unsigned num_curvePt = getStreetSegmentCurvePointCount(street_segment_id);
	//find the street segments ends
	StreetSegmentEnds SE = getStreetSegmentEnds(street_segment_id);
	LatLon from = getIntersectionPosition(SE.from);
	LatLon to = getIntersectionPosition(SE.to);

	if (num_curvePt > 0) {
		LatLon prevPos = from;
		for(unsigned nCP = 0; nCP < num_curvePt; nCP++) {
			LatLon cpPos = getStreetSegmentCurvePoint(street_segment_id, nCP);
			length += find_distance_between_two_points(prevPos, cpPos);
			prevPos = cpPos;
		}
		length += find_distance_between_two_points(prevPos, to);
	}
	else {
		length = find_distance_between_two_points(from, to);
	}

	return length;
}

//find the length of a whole street
double find_street_length(std::string street_name) {
	double length = 0;

	vector<unsigned> segments = find_street_street_segments(street_name);
			
	for (unsigned seg=0; seg<segments.size(); seg++) {
		length += find_street_segment_length(segments[seg]);
	}

	return length;
}

//find the travel time to drive a street segment (time(minutes) = distance(Km)/speed_limit(Km/h)*60)
double find_segment_travel_time(unsigned street_segment_id) {
	//covert distance to km
	double distance = find_street_segment_length(street_segment_id)/1000;
	double speedLim = getStreetSegmentSpeedLimit(street_segment_id);
	return distance/speedLim*60;
}

//find the nearest point of interest (by name) to a given position
std::string find_closest_point_of_interest(LatLon my_position) {
	
	double minDist = find_distance_between_two_points(my_position, get<0>(*(POI_vec.begin())));
	string minPOI = get<1>(*(POI_vec.begin())); 

	for (auto iter=POI_vec.begin()+1; iter != POI_vec.end(); iter++)
	{
		double dist = find_distance_between_two_points(my_position, get<0>(*iter));
		if (dist < minDist) {
			minDist = dist;
			minPOI = get<1>(*iter);
		}
	}

	return minPOI;
}

double getLatMin () {
	return latMin;
}
double getLatMax () {
	return latMax;
}
double getLonMin () {
	return lonMin;
}
double getLonMax () {
	return lonMax;
}

double getStreetMaxSpeedLim (std::string street_name) {
	auto iter = streetNameStreetID_map.find(street_name);
	if (iter != streetNameStreetID_map.end())
		return (iter->second).second;
	else
		return 0;
}

bool path_is_legal(unsigned start_intersection, unsigned end_intersection, const std::vector<unsigned>& path) {
    //'start_intersection' is the intersection id of the starting intersection
    //'end_intersection' is the intersection id of the ending intersection
    //'path' is a vector street segment id's

    if (path.size() < 1) {
        return false; //If it is a valid path it must contain at-least one segment

    } else {
        //General case
        //To verify the path by walking along each segment checking:
        //  * That we are going a legal direction (e.g. not against one-ways)
        //  * That each segment is connected to the previous intersection
        //  * That the final segment is connected to the end_intersection
        //We start our intersection at the start_intersection
		std::cout<< "size of path is: " << path.size() << std::endl;

        unsigned curr_intersection = start_intersection;
        for (int i = 0; i < (int) path.size(); i++) {
            StreetSegmentEnds seg_endpoints = getStreetSegmentEnds(path[i]);

			std::cout << "We've reached at the " << i << "th segment with ID: " << path[i] << std::endl;

            //Are we moving forward or back along the segment?
            bool seg_traverse_forward;
            if (seg_endpoints.to == curr_intersection) {
                //Moving backwards
				seg_traverse_forward = false;

            } else if (seg_endpoints.from == curr_intersection) {
                //Moving forwards
                seg_traverse_forward = true;

            } else {
                //This segment isn't connected to the current intersection
                std::cout << "segment isn't connected to current intersection" << std::endl;
				return false;
            }

            //Are we going the wrong way along the segment?
            if (!seg_traverse_forward && getStreetSegmentOneWay(path[i])) {
                std::cout <<"we are going wrong way along the segment" <<std::endl;
				return false;
            }

            //Advance to the next intersection
            curr_intersection = (seg_traverse_forward) ? seg_endpoints.to : seg_endpoints.from;

        }

        //We should be at the end intersection
        if (curr_intersection != end_intersection) {
            std::cout << "not at end of intersection" << std::endl;
			return false;
        }

    }

    //Everything validated
    return true;
}

