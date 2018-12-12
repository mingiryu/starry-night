#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

class ContourTracker
{
  public:
	void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button); // Right click to select color
	void keyPressed(int key);					 // 'v' := video, 'g' := gui

	vector<ofPath> &getPaths();
	vector<ofPoint> findSpotlight(ofImage &background);

  private:
	ofVideoGrabber vidGrabber;
	ofxPanel gui;
	bool drawGui, drawVideo;

	ofxCvColorImage colorImage;
	ofxCvGrayscaleImage grayImage, grayBgImage, grayDiff;
	ofxCvFloatImage grayImageFloat, grayBgFloat; // Used for adaptive background, image multiplication.
	ofParameter<int> blurCoefficient;

	ofxCvContourFinder contourFinder;
	ofParameter<int> threshold, minArea, maxArea, maxContours;
	ofParameter<bool> bApproximateContour;
	bool bLearnBackground;

	ofColor targetColor;	  // User selected color, selected through right mouse click
	ofParameter<float> delta; // Maximum distance between targetColor and pixel color.

	vector<ofPath> paths;

	/**
	 * Filters user selected color using Euclidean distance in RGB space.
	 */
	void applyColorFilter();

	/**
	 * Use adaptive background to set grayBgImage that is used to make grayDiff.
	 */
	void updateBackground();
};
