#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ContourTracker.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void mousePressed(int x, int y, int button);
	void keyPressed(int key);

private:
	/**
	 * Loads the points of the Polygon shape.
	 * Supported format is csv with newline for each point.
	 */
	std::vector<ofDefaultVertexType> loadPoints(const std::string &file);

	/**
	 * Call ofSoundGetSpectrum() to get FFT of the music
	 */
	void updateSoundSpectrum();

	/**
	 * Adds particles into the box2d world.
	 */
	void loadParticles();

	/**
	 * Gets ofPath objects from ContourTracker and convert it to ofxBoxPolygon for box2d world.
	 */
	void loadContours();

	ofImage background; // Image used for finding spotlight and drawing background.

	ContourTracker contourTracker; // Handles video input and object detection.
	vector<ofPoint> spotlight; // Vector of points where particles first appear.

	ofSoundPlayer sound;
	static constexpr size_t nBandsToGet = 256;
	std::array<float, nBandsToGet> fftSmoothed{{0}}; // FFT of sound file
	bool bSmoothed;

	ofxBox2d box2d;
	vector<ofDefaultVertexType> shape; // Shape of the custum polygon
	
	vector<shared_ptr<ofxBox2dPolygon>> particles;
	vector<shared_ptr<ofxBox2dPolygon>> contours;

	const int kParticleGroup = -1;
};
