#pragma once
#include <string>
#include "graphics.h"
#include "easygl_constants.h"
#include "m1.h"
#include "m3.h"

// Draws the map whose at map_path; this should be a .bin file.
void draw_map(std::string map_path);
void drawscreen (void); 
void drawStreets (void); 
void drawSingleStreet(std::string streetName);
void drawPath(std::vector<unsigned> path);
void drawFeatures(void);
void drawClosedFeature(unsigned featureID, unsigned featurePC);
void drawOpenFeature(unsigned featureID, unsigned featurePC);
bool testFeatureClosed(unsigned featureID, unsigned featurePC);
//helper functions for positioning
std::pair<double,double> LatLon2XY (LatLon point);
