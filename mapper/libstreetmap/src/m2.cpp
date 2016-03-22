#include "m2.h"
#include <cfloat>
#include <utility>
#include "minPriorityQueue.h"

using XY = std::pair<double,double>;
float fullArea;
std::vector<unsigned> path;

// Draws the map whose at map_path; this should be a .bin file.
void draw_map(std::string map_path) {
	load_map(map_path);
	std::cout << "About to start graphics.\n";
	/**************** initialize display **********************/
	
	/*****************testing area ***************************/
	/*std::string inter1 = getIntersectionName(85529);
	std::string inter2 = getIntersectionName(66103);
	std::cout<< "source is: " << inter1 << std::endl;
	std::cout<< "destination is: " << inter2 << std::endl;
	path = find_path_between_intersections(85529, 66103);
	std::cout << "expected travel time is: " << "21.23313198747807" << std::endl;
	std::cout << "actual travel time is: " << compute_path_travel_time(path) << std::endl;
	*/
	//if (path_is_legal(1648, 23162, path))
	//	std::cout << "path is legal" << std::endl;
	//else
	//	std::cout << "path is not legal" << std::endl;
	//std::cout << "path size is: " << path.size() << std::endl;
	
	
	XY minXY = LatLon2XY(LatLon(getLatMin(),getLonMin()));
	XY maxXY = LatLon2XY(LatLon(getLatMax(),getLonMax()));

	const t_bound_box initial_coords = t_bound_box(minXY.first,minXY.second,maxXY.first,maxXY.second); 
	fullArea = initial_coords.area();

	// Set the name of the window (in UTF-8), with white background.
	init_graphics("Some Example Graphics", t_color(224,224,224)); // you could pass a t_color RGB triplet instead
	//set initial coordinates
	set_visible_world(initial_coords);

	event_loop(NULL,NULL,NULL,drawscreen);

	close_graphics();
	std::cout << "Graphics closed down..\n";
	close_map();

}

void drawscreen (void) {

	/* The redrawing routine for still pictures.  The graphics package calls  
	 * this routine to do redrawing after the user changes the window 
	 * in any way.                                                    
	*/
 
	set_draw_mode (DRAW_NORMAL);  // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();  /* Should precede drawing for all drawscreens */

	setfontsize (10);
	setlinestyle (SOLID);
	
	{
		//first draw some water features
		drawFeatures();
		//draw all the streets
		drawStreets();
		setcolor(255,51,51);
		drawPath(path);
	}
}

void drawStreets (void) {
	//let's try drawing some streets
	//find out are we zoomed in or not 0/1/2
	unsigned int zoom = 0;
	t_bound_box currView = get_visible_world();
	float currArea = currView.area();
	//setting zoomedIn as 1/8 of the fullArea
	if (fullArea/currArea >= 150) zoom = 2;
	else  if (fullArea/currArea >= 6) zoom = 1;

	//first loop through all the street ids, then draw segment by segment
	for (unsigned int streetID =0; streetID < getNumberOfStreets(); streetID++) {
		std::string streetName = getStreetName(streetID);
		double maxSpeedLim = getStreetMaxSpeedLim(streetName);
		
		if (zoom==2) {
			if (maxSpeedLim > 80) setcolor(255,153,51);
			else if (maxSpeedLim > 40 && maxSpeedLim <=80) setcolor(255,255,102);
			else setcolor(WHITE);
			drawSingleStreet(streetName);
		} 
		else if (zoom==1) {
			if (maxSpeedLim > 80) {
				setcolor(255,153,51);
				drawSingleStreet(streetName);}
			else if (maxSpeedLim > 40 && maxSpeedLim <=80) {
				setcolor(255,255,102);
				drawSingleStreet(streetName);}
		}
		else if (zoom==0 && maxSpeedLim > 80) {
			setcolor(255,153,51);
			drawSingleStreet(streetName);
		}
	}
}
//draw a single street
void drawSingleStreet(std::string streetName) {
	
	for (auto seg : find_street_street_segments(streetName)) {
		//find line width according to speed limit
		double currSpeedLimit = getStreetSegmentSpeedLimit(seg);
		setlinewidth((int)currSpeedLimit/10);
		//first get the street segment's curve point count
		unsigned num_curvePt = getStreetSegmentCurvePointCount(seg);
		//find the street segments ends
		StreetSegmentEnds SE = getStreetSegmentEnds(seg);
		XY from = LatLon2XY(getIntersectionPosition(SE.from));
		XY to = LatLon2XY(getIntersectionPosition(SE.to));
		if (num_curvePt > 0) {
			XY prevPos = from;
			for(unsigned nCP = 0; nCP < num_curvePt; nCP++) {
				XY cpPos = LatLon2XY(getStreetSegmentCurvePoint(seg, nCP));
				drawline(prevPos.first, prevPos.second, cpPos.first, cpPos.second);
				prevPos = cpPos;
			}
			drawline(prevPos.first, prevPos.second, to.first, to.second);
		}
		else {
			drawline(from.first, from.second, to.first, to.second);
		}
	}
}

//draw a path of street segments
void drawPath(std::vector<unsigned> path) {
	
	for (auto seg : path) {
		//color one way street differently
		if (!getStreetSegmentOneWay(seg))
			setcolor(255,51,51);
		else
			setcolor(51,51,255);

		//find line width according to speed limit
		double currSpeedLimit = getStreetSegmentSpeedLimit(seg);
		setlinewidth((int)currSpeedLimit/10);
		//first get the street segment's curve point count
		unsigned num_curvePt = getStreetSegmentCurvePointCount(seg);
		//find the street segments ends
		StreetSegmentEnds SE = getStreetSegmentEnds(seg);
		XY from = LatLon2XY(getIntersectionPosition(SE.from));
		XY to = LatLon2XY(getIntersectionPosition(SE.to));
		if (num_curvePt > 0) {
			XY prevPos = from;
			for(unsigned nCP = 0; nCP < num_curvePt; nCP++) {
				XY cpPos = LatLon2XY(getStreetSegmentCurvePoint(seg, nCP));
				drawline(prevPos.first, prevPos.second, cpPos.first, cpPos.second);
				prevPos = cpPos;
			}
			drawline(prevPos.first, prevPos.second, to.first, to.second);
		}
		else {
			drawline(from.first, from.second, to.first, to.second);
		}
	}
}


//drawing function for drawing features
void drawFeatures(void) {
	unsigned int featuresCount = getFeatureCount();
	
	for (unsigned int feature=0; feature<featuresCount; feature++) {	
		
		std::string featureAttr = getFeatureAttribute(feature ,"water");		
		std::string featureAttr2 = getFeatureAttribute(feature,"waterway");
		std::string featureAttr3 = getFeatureAttribute(feature,"land");
		std::string featureAttr4 = getFeatureAttribute(feature,"natural");
		unsigned int featurePC = getFeaturePointCount(feature);

		if (!featureAttr.empty()) {
			//setting line width for open feature
			setlinewidth(2);
			//setting color for water
			setcolor(51,153,255);
			if (testFeatureClosed(feature, featurePC))
				drawClosedFeature(feature, featurePC);
			else
				drawOpenFeature(feature, featurePC);	
		}
		else if(!featureAttr2.empty()) {
			//setting line width for open feature
			setlinewidth(2);
			//setting color for water
			setcolor(51,153,255);
			if (testFeatureClosed(feature, featurePC))
				drawClosedFeature(feature, featurePC);
			else
				drawOpenFeature(feature, featurePC);	
		}
		else if(!featureAttr3.empty()) {
			//setting line width for open feature
			setlinewidth(2);
			setcolor(LIGHTGREY);
			if (testFeatureClosed(feature, featurePC))
				drawClosedFeature(feature, featurePC);
			else
				drawOpenFeature(feature, featurePC);	
		}
		else if(!featureAttr4.empty()) {
			setlinewidth(2);
			if(!featureAttr4.compare("sand") || !featureAttr4.compare("beach"))
				setcolor(255,255,204);
			if(!featureAttr4.compare("grassland") || !featureAttr4.compare("tree_row") || !featureAttr4.compare("wood"))
				setcolor(102,204,0);
			if(!featureAttr4.compare("water"))
				setcolor(51,153,255);
			if (testFeatureClosed(feature, featurePC))
				drawClosedFeature(feature, featurePC);
			else
				drawOpenFeature(feature, featurePC);	
		}
	}	
}

void drawClosedFeature(unsigned featureID, unsigned featurePC) {
	// t_point featurePoints[featurePC];
	t_point* featurePoints = new t_point[featurePC];

	for (unsigned int pc=0; pc < featurePC; pc++) {
		XY pt = LatLon2XY(getFeaturePoint(featureID,pc));
		featurePoints[pc] = t_point((float)pt.first,(float)pt.second);
	}
	fillpoly(&featurePoints[0],featurePC);

	delete []featurePoints;
}

void drawOpenFeature(unsigned featureID, unsigned featurePC) {
	
	XY prevPos = LatLon2XY(getFeaturePoint(featureID,0));
	for (unsigned int pc=1; pc < featurePC; pc++) {
		XY currPos = LatLon2XY(getFeaturePoint(featureID,pc));
		drawline(prevPos.first,prevPos.second,currPos.first,currPos.second);
		prevPos = currPos;
	}
}

bool testFeatureClosed(unsigned featureID, unsigned featurePC) {
	XY firstPosition =  LatLon2XY(getFeaturePoint(featureID,0));
	XY secondPosition =  LatLon2XY(getFeaturePoint(featureID,featurePC-1));
	if (firstPosition == secondPosition)
		return true;
	else
		return false;
}

//helper functions for positioning on screen
std::pair<double,double> LatLon2XY (LatLon point) {
	//convert the LatLon points into x,y coordinates
	//double latAvg = DEG_TO_RAD*43.652663;
	double latMax = getLatMax();
	double latMin = getLatMin();

	double latAvg = DEG_TO_RAD*(latMax + latMin)/2;
	double x = EARTH_RADIUS_IN_METERS*DEG_TO_RAD*point.lon*cos(latAvg)/1000;
	double y = EARTH_RADIUS_IN_METERS*DEG_TO_RAD*point.lat/1000;
	XY xy({x,y});	

	
	//calculate the distance using Pythagoras and return
	return xy; 	
}
