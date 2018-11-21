#pragma once

#include "ofMain.h"
#include "ContourTracker.h"
#include "StarryNight.h"

class ofApp : public ofBaseApp
{

  public:
	void setup();
	void update();
	void draw();

	void mousePressed(int x, int y, int button);

	/**
	 * Uses polylines from contourTracker and stars from the starryNight
	 * to allow simple interaction between object and particles.
	 * 
	 * As of now, this is the most practical method of particle interaction
	 * while keeping boundaries bewteen each classes.
	 */
	void updateofInside();

	ContourTracker contourTracker;
	StarryNight starryNight;
};
