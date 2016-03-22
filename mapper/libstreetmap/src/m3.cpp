#include "m3.h"
#include "minPriorityQueue.h"
#include <limits>
#include <iostream>

const unsigned kMaxInter = std::numeric_limits<unsigned>::max();
//adjacency list for graph algorithms
enum COLOR {WHITE_YUAN,GREY_YUAN,BLACK_YUAN};
struct Intersection { 
	unsigned reachingSeg; //segment from parent to self
	enum COLOR color;
	double timeUsed;
	unsigned parent;
};
//data struct for graph algorithms
std::vector<std::vector<unsigned>> mapAdjList(getNumberOfIntersections());
std::vector<Intersection> nodeList;
const double kMaxTime = std::numeric_limits<double>::max();
bool adjListSet = false;

//declare the traceBack function
std::vector<unsigned> traceBack(unsigned intersect_id_start, unsigned intersect_id_end);

void initAdjList(void) {
	unsigned long long numInt = getNumberOfIntersections();
	for (unsigned int inter_ID=0; inter_ID<numInt; inter_ID++)
	{
		//populate the mapAdjList
		std::string intName = getIntersectionName(inter_ID);
		mapAdjList.push_back(find_adjacent_intersections(intName));
	}
}

void initNodeList(void) {
	nodeList.assign(getNumberOfIntersections(),{kMaxInter, WHITE_YUAN, kMaxTime, kMaxInter});
}

std::vector<unsigned> find_path_between_intersections(unsigned intersect_id_start, unsigned intersect_id_end) {
//	return bfs(intersect_id_start, intersect_id_end);
	return dijkstra(intersect_id_start, intersect_id_end);
}

std::vector<unsigned> dijkstra(unsigned intersect_id_start, unsigned intersect_id_end) {
	//setting the adjacency list
	if (!adjListSet) {
		initAdjList();
		adjListSet = true;
	}
	//initialize graph
	initNodeList();
	//initialize a min priority queue at intersect_id_start
	minPriorityQueue Q(getNumberOfIntersections());
	Q.decreaseKey(intersect_id_start, 0);

	//relax each node starting from source
	while (Q.getQSize() > 0) {
		KEY_ID minNode = Q.extractMin();
		unsigned u = minNode.second;
		double uKey = minNode.first;
		
		//end case
		if (u == intersect_id_end) break;
		
		std::vector<unsigned> adjacentInter = mapAdjList[u];
		for (auto adjID : adjacentInter) {
			if (Q.IDisInQ(adjID)) {
				double localTraveltime;
				//get street id from u's parent to u and u to adjID 
				if (u != intersect_id_start) {
					unsigned streetID1 = getStreetSegmentStreetID(nodeList[u].reachingSeg);
					unsigned common_seg = common_intersection_intersection_segment(u, adjID);
					unsigned streetID2 = getStreetSegmentStreetID(common_seg);	
					if (streetID1 == streetID2) localTraveltime = find_segment_travel_time(common_seg);
					else localTraveltime = 0.25 + find_segment_travel_time(common_seg);
				
					if (Q.peek(adjID) > uKey + localTraveltime) {
						Q.decreaseKey(adjID, uKey + localTraveltime);
						nodeList[adjID].parent = u;
						//segment from intersection u to intersection adjID
						nodeList[adjID].reachingSeg = common_seg;
					}
				}
				else {  //the case where we just started from source
					nodeList[adjID].parent = intersect_id_start;
					nodeList[adjID].reachingSeg = common_intersection_intersection_segment(intersect_id_start, adjID);
					Q.decreaseKey(adjID,find_segment_travel_time(nodeList[adjID].reachingSeg));
				}
			}
		}
	}
	return traceBack(intersect_id_start, intersect_id_end);
}


std::vector<unsigned> bfs(unsigned intersect_id_start, unsigned intersect_id_end) {
	
	//setting the adjacency list
	if (!adjListSet) {
		initAdjList();
		adjListSet = true;
	}
	//initialize graph
	initNodeList();

	std::list<unsigned> visitingQ;
	visitingQ.push_back(intersect_id_start);
	//time used to get to source is 0
	nodeList[intersect_id_start].timeUsed = 0;
	//color the source node black
	nodeList[intersect_id_start].color = BLACK_YUAN;

	while(visitingQ.size()>0) {
		unsigned u = visitingQ.front();
		visitingQ.pop_front();

		//if all node in visitingQ has higher timeUsed than dest, return
	  	if (u == intersect_id_end) {
			bool stop = true;
			for (auto visitNode : visitingQ) 
				if (nodeList[visitNode].timeUsed < nodeList[u].timeUsed)
					stop = false;
			if (stop) return traceBack(intersect_id_start, intersect_id_end);
		}

		std::vector<unsigned> adjacentNode = mapAdjList[u];
		for (auto adjID : adjacentNode) {
			if (nodeList[adjID].color == WHITE_YUAN){
				nodeList[adjID].color = GREY_YUAN;
				visitingQ.push_back(adjID);
			}
			double localTraveltime;
			//get street id from u's parent to u and u to adjID 
			if (u != intersect_id_start) {
				unsigned streetID1 = getStreetSegmentStreetID(nodeList[u].reachingSeg);
				unsigned common_seg = common_intersection_intersection_segment(u, adjID);
				unsigned streetID2 = getStreetSegmentStreetID(common_seg);	
				if (streetID1 == streetID2) localTraveltime = find_segment_travel_time(common_seg);
				else localTraveltime = 0.25 + find_segment_travel_time(common_seg);
			
				if (nodeList[adjID].timeUsed > nodeList[u].timeUsed + localTraveltime) {
					nodeList[adjID].timeUsed = 	nodeList[u].timeUsed + localTraveltime;
					nodeList[adjID].parent = u;
					//segment from intersection u to intersection adjID
					nodeList[adjID].reachingSeg = common_seg;
				}
			}
			else {  //the case where we just started from source
				nodeList[adjID].parent = intersect_id_start;
				nodeList[adjID].reachingSeg = common_intersection_intersection_segment(intersect_id_start, adjID);
				nodeList[adjID].timeUsed = find_segment_travel_time(nodeList[adjID].reachingSeg);
			}
		}
	}
	//trace back and return the path
	return traceBack(intersect_id_start, intersect_id_end);
}

std::vector<unsigned> traceBack(unsigned intersect_id_start, unsigned intersect_id_end) {
	std::vector<unsigned> path;
	Intersection current = nodeList[intersect_id_end];
	//std::cout << "destination node's parent is: " << current.parent << std::endl;
	while(current.parent != intersect_id_start && current.parent != kMaxInter) {
		auto iter = path.begin();
		path.insert(iter,current.reachingSeg);
		current = nodeList[current.parent];
	}

	auto iter = path.begin();
	path.insert(iter,current.reachingSeg);

	return path;
}

double compute_path_travel_time(const std::vector<unsigned>& path) {

	double travelTime = 0;

	if (path.size() != 0) {
	
		travelTime += find_segment_travel_time(path[0]);
		unsigned prevStreetID = getStreetSegmentStreetID(path[0]);	
				
		for (auto seg = path.begin() + 1; seg != path.end(); seg++) {
			unsigned currStreetID = getStreetSegmentStreetID(*seg);
			if (currStreetID == prevStreetID) travelTime += find_segment_travel_time(*seg);
			else travelTime += 0.25 + find_segment_travel_time(*seg);
			prevStreetID = currStreetID;
		}

	}

	return travelTime;
}

std::vector<unsigned> find_path_to_point_of_interest (unsigned intersect_id_start, std::string point_of_interest_name);
