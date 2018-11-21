#include "ContourTracker.h"

void ContourTracker::setup(int width_, int height_)
{
	width = width_;
	height = height_;
	scale = ofGetHeight() / height;

	vidGrabber.setup(width, height);

	colorImage.allocate(width, height);
	grayImage.allocate(width, height);
	grayBgImage.allocate(width, height);
	grayDiff.allocate(width, height);
	grayImageFloat.allocate(width,height);
	grayBgFloat.allocate(width,height);

	gui.setup();
	gui.setPosition(width * scale, height * 2); // Draw Gui at the bottom corner

	// Color tracking parameter
	targetColor = ofColor(87, 71, 65);
	gui.add(delta.set("Color Delta", 50, 0, 255));

	// ContourFinder Parameter
	gui.add(threshold.set("Contour Threshold", 80, 0, 255));
	gui.add(minArea.set("Minimum Area", 2000, 0, width*height));
	gui.add(maxArea.set("Maximum Area", 20000, 0, width*height));
	gui.add(maxContours.set("Maximum Contours", 10, 0, 100));
	gui.add(bApproximateContour.set("Approximate Contour", true));
	bLearnBackground = true;
}

void ContourTracker::update()
{
	bool bNewFrame = false;
	vidGrabber.update();
	bNewFrame = vidGrabber.isFrameNew();
	if (bNewFrame)
	{
		ofPixels pixels = vidGrabber.getPixels();
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				ofColor color = pixels.getColor(i, j);
				float r = color.r - targetColor.r;
				float g = color.g - targetColor.g;
				float b = color.b - targetColor.b;

				// Euclidean distance in RGB space.
				float distance = sqrt(r*r + g*g + b*b);
				if (distance < delta) {
					pixels.setColor(i, j, ofColor::black);
				} else {
					pixels.setColor(i, j, ofColor::white);
				}
			}
		}
		colorImage.setFromPixels(pixels);
		grayImage = colorImage; // Convert to grayscale for faster processing
		grayImage.mirror(false, true);

		if (bLearnBackground == true)
		{
			grayBgImage = grayImage;
			bLearnBackground = false;
		} 
		else {
			// Convert from ofxGrayscaleImage to ofxFloatImage, so we can perform *= operation.
			grayImageFloat = grayImage;
			grayBgFloat = grayBgImage;

			// Adaptive background
			grayBgFloat *= 0.999;
    		grayImageFloat *= 0.001;
			grayBgFloat += grayImageFloat;

			// Convert back to ofxGrayscaleImage
    		grayBgImage = grayBgFloat;
		}
		grayDiff.absDiff(grayBgImage, grayImage);
		grayDiff.threshold(threshold);

		contourFinder.findContours(grayDiff, minArea, maxArea, maxContours, bApproximateContour);

		// Update outlines
		outlines.clear();
		for (int i = 0; i < contourFinder.nBlobs; i++)
		{
			ofxCvBlob blob = contourFinder.blobs[i];
			for (auto p = blob.pts.begin(); p != blob.pts.end(); ++p) {
				p->x *= scale;
				p->y *= scale;
			}
			outlines.push_back(ofPolyline(blob.pts).getSmoothed(7));
		}
	}
}

void ContourTracker::draw()
{
	// Processed video input
	ofSetHexColor(0xffffff);
	grayDiff.draw(width*scale, 0);
	
	// Contour outline
	ofSetHexColor(0x333333);
	ofDrawRectangle(width*scale, height, width, height);
	contourFinder.draw(width*scale, height);

	// targetColor
	ofSetColor(targetColor);
	ofDrawRectangle(width*scale, height * 2, width, height);

	// Outlines
	for (auto line : outlines) {
		ofSetColor(ofColor::pink);
		line.draw();
	}

	gui.draw();
}

void ContourTracker::mousePressed(int x, int y, int button)
{
	ofPixels pixels = vidGrabber.getPixels();
	targetColor = pixels.getColor(x - width*scale, y);
}

vector<ofPolyline> & ContourTracker::getOutlines()
{
	return outlines;
}

	
