#include "ContourTracker.h"

void ContourTracker::setup()
{
	int width = 320;
	int height = 240;
	vidGrabber.setup(width, height);

	colorImage.allocate(width, height);
	grayImage.allocate(width, height);
	grayBgImage.allocate(width, height);
	grayDiff.allocate(width, height);
	grayImageFloat.allocate(width, height);
	grayBgFloat.allocate(width, height);

	gui.setup();
	gui.setPosition(0, 0);

	// Color tracking parameter
	targetColor = ofColor(87, 71, 65);
	gui.add(delta.set("Color Delta", 50, 0, 255));

	// ContourFinder Parameter
	gui.add(threshold.set("Contour Threshold", 80, 0, 255));
	gui.add(minArea.set("Minimum Area", 2000, 0, width * height));
	gui.add(maxArea.set("Maximum Area", 20000, 0, width * height));
	gui.add(maxContours.set("Maximum Contours", 10, 0, 100));
	gui.add(bApproximateContour.set("Approximate Contour", true));

	// Other parameter
	gui.add(blurCoefficient.set("Blur Coefficient", 5, 0, 100));

	bLearnBackground = true;
	drawGui = false;
	drawVideo = false;
}

void ContourTracker::update()
{
	vidGrabber.update();

	if (vidGrabber.isFrameNew())
	{
		colorImage.setFromPixels(vidGrabber.getPixels());
		colorImage.mirror(false, true);

		applyColorFilter();

		grayImage = colorImage; // Convert to grayscale for faster processing
		grayImage.blur(blurCoefficient);
		grayImage.dilate();

		updateBackground();

		grayDiff.absDiff(grayBgImage, grayImage);
		grayDiff.threshold(threshold);

		contourFinder.findContours(grayDiff, minArea, maxArea, maxContours, bApproximateContour);

		// Update the ofPath lists
		paths.clear();
		for (int i = 0; i < contourFinder.nBlobs; i++)
		{
			ofxCvBlob blob = contourFinder.blobs[i];
			ofPath path;

			bool moveTo = false;
			int scale = ofGetWidth() / vidGrabber.getWidth();
			for (auto p : blob.pts)
			{
				if (!moveTo)
				{
					moveTo = true;
					path.moveTo(p.x * scale, p.y * scale);
				}
				else
				{
					path.lineTo(p.x * scale, p.y * scale);
				}
			}
			path.close();

			paths.push_back(path);
		}
	}
}

void ContourTracker::draw()
{
	for (auto path : paths)
	{
		path.setFilled(true);
		path.setFillColor(targetColor);
		path.draw();
	}

	// Togglable draw of camera and gui
	if (drawVideo)
	{
		ofSetColor(targetColor);
		grayDiff.draw(0, 0);
		contourFinder.draw(0, 0);
	}
	else if (drawGui)
	{
		gui.draw();
	}
}

void ContourTracker::mousePressed(int x, int y, int button)
{
	if (button == 2)
	{ // Right Click
		ofPixels pixels = vidGrabber.getPixels();
		targetColor = pixels.getColor(ofGetWidth() - x, y);
	}
}

void ContourTracker::keyPressed(int key)
{
	if (key == 'v')
	{
		drawVideo = !drawVideo;
		drawGui = false;
	}
	else if (key == 'g')
	{
		drawGui = !drawGui;
		drawVideo = false;
	}
}

void ContourTracker::applyColorFilter()
{
	ofPixels &pixels = colorImage.getPixels();
	for (int i = 0; i < colorImage.getWidth(); i++)
	{
		for (int j = 0; j < colorImage.getHeight(); j++)
		{
			ofColor color = pixels.getColor(i, j);
			float r = color.r - targetColor.r;
			float g = color.g - targetColor.g;
			float b = color.b - targetColor.b;

			// Euclidean distance in RGB space.
			float distance = sqrt(r * r + g * g + b * b);

			if (distance < delta)
			{
				pixels.setColor(i, j, ofColor::white);
			}
			else
			{
				pixels.setColor(i, j, ofColor::black);
			}
		}
	}
}

void ContourTracker::updateBackground()
{
	if (bLearnBackground == true)
	{
		grayBgImage = grayImage;
		bLearnBackground = false;
	}
	else
	{
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
}

vector<ofPath> &ContourTracker::getPaths()
{
	return paths;
}

vector<ofPoint> ContourTracker::findSpotlight(ofImage &background)
{
	vector<ofPoint> spotlight;

	ofxCvColorImage cImage;
	ofxCvGrayscaleImage image;
	cImage.allocate(ofGetWidth(), ofGetHeight());
	image.allocate(ofGetWidth(), ofGetHeight());

	cImage.setFromPixels(background.getPixels());
	image = cImage;

	image.blur(7);
	image.dilate();
	image.threshold(180); // Higher the threshold, finer and smaller the spots.

	ofxCvContourFinder finder;
	finder.findContours(image, 20, 2000, 50, true);
	for (int i = 0; i < finder.nBlobs; i++)
	{
		auto center = finder.blobs[i].centroid;
		if (center.y < ofGetHeight() / 3)
		{
			spotlight.push_back(center);
		}
	}

	cImage.clear();
	image.clear();

	return spotlight;
}