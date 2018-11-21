#include "ofApp.h"

void ofApp::setup()
{
	int width = 320;
	int height = 240;
	contourTracker.setup(width, height);
	starryNight.setup(width, height);
}

void ofApp::update()
{
	contourTracker.update();
	starryNight.update();
	updateofInside();
}

void ofApp::draw()
{
	starryNight.draw();
	contourTracker.draw();
}

void ofApp::mousePressed(int x, int y, int button)
{
	contourTracker.mousePressed(x, y, button);
}

void ofApp::updateofInside()
{
	vector<ofPolyline> & outlines = contourTracker.getOutlines();
	queue<ofVec4f*> stars = starryNight.getStars();

	while (!stars.empty()) {
        ofVec4f* star = stars.front();

		for (auto line : outlines) {
			if (line.inside(star->x, star->y)) {
				 // Reflect the star by changing its z component (velocity).
				 // Some of the stars will pass through because of (star->z + 1).
				star->z = -star->z;
			}
		}
		stars.pop(); // Safe to do this since it is copy of pointers.
    }
}
