#include "ofApp.h"

static bool shouldRemove(shared_ptr<ofxBox2dBaseShape> shape)
{
	return !ofRectangle(0, 0, ofGetWidth(), ofGetHeight()).inside(shape->getPosition());
}

void ofApp::setup()
{
	background.load("starrynight.png");
	background.resize(ofGetWidth(), ofGetHeight());

	contourTracker.setup();
	spotlight = contourTracker.findSpotlight(background);

	sound.load("sounds/music.mp3");

	box2d.init();
	box2d.setGravity(0, 1);
	box2d.setFPS(30);

	shape = loadPoints("shape.dat");
}

vector<ofDefaultVertexType> ofApp::loadPoints(const std::string &file)
{
	vector<ofDefaultVertexType> pts;
	vector<string> ptsStr = ofSplitString(ofBufferFromFile(file).getText(), "\n");

	for (int i = 0; i < ptsStr.size(); i++)
	{
		string point = ptsStr[i];
		if (point.size() > 0) { // ofSplitString has empty string at the end.
			vector<string> p = ofSplitString(point, ",");
			float x = ofToFloat(p[0]);
			float y = ofToFloat(p[1]);
			pts.push_back(ofDefaultVertexType(x, y, 0));
		}
	}
	return pts;
}

void ofApp::update()
{
	ofSoundUpdate();
	if (!sound.isPlaying())
		sound.play();

	updateSoundSpectrum();

	if ((int)ofRandom(0, 5) == 0 && bSmoothed) // Control the number of particles being initialized.
		loadParticles();

	contourTracker.update();
	loadContours();

	// remove shapes offscreen
	ofRemove(particles, shouldRemove);
	
	// Remove some particles to prevent serious lagging.
	if (particles.size() > 100) {
		for (int i = 0; i < 10; i++) {
			particles.erase(particles.begin());
		}
	}

	box2d.update();
}

void ofApp::draw()
{
	ofSetColor(255, 255, 255);
	background.draw(0, 0);

	for (auto particle : particles)
	{
		ofFill();
		ofPoint spot = spotlight[ofRandom(0, spotlight.size())];
		ofColor color = background.getColor(spot.x, spot.y);
		ofSetColor(color);
		particle->draw();
	}

	contourTracker.draw();

	// Drawing the contours in ofApp will generate glitching polygons.
	// So the contours are drawn from ofPath object in contourFinder.
}

void ofApp::mousePressed(int x, int y, int button)
{
	contourTracker.mousePressed(x, y, button);
}

void ofApp::keyPressed(int key)
{
	contourTracker.keyPressed(key);
}

void ofApp::updateSoundSpectrum()
{
	float *val = ofSoundGetSpectrum(nBandsToGet);
	bSmoothed = false;
	for (size_t i = 0; i < nBandsToGet; i++)
	{
		fftSmoothed[i] *= 0.96f;
		if (fftSmoothed[i] < val[i])
		{
			fftSmoothed[i] = val[i];
			bSmoothed = true;
		}
	}
}

void ofApp::loadParticles()
{
	int scale = 40;
	int minRadius = 2;

	for (size_t i = 0; i < nBandsToGet; i++)
	{
		float r = fftSmoothed[i] * scale;
		if (r >= minRadius)
		{
			auto poly = std::make_shared<ofxBox2dPolygon>();
			vector<ofDefaultVertexType> pts;

			// Scale the custom shape.
			for (auto p : shape)
			{
				float x = p.x * r;
				float y = p.y * r;
				pts.push_back(ofDefaultVertexType(x, y, 0));
			}

			// Initialize polygon.
			poly->addVertices(pts);
			poly->setPhysics(r, 0.1*i, 0);
			poly->triangulatePoly();
			poly->create(box2d.getWorld());

			// Filter to prevent colliding each other (reduces lag and improves visual).
			for (b2Fixture *f = poly->body->GetFixtureList(); f; f = f->GetNext())
			{
				b2Filter filter = f->GetFilterData();
				filter.groupIndex = kParticleGroup;
				f->SetFilterData(filter);
			}

			// Set position according to background spotlight.
			// Velocity depends on the index of the bands.
			auto position = spotlight[i % spotlight.size()];
			poly->setPosition(position.x, position.y);
			poly->setVelocity(i * ofRandom(-1, 1), i);
			particles.push_back(poly);
		}
	}
}

void ofApp::loadContours()
{
	contours.clear();
	vector<ofPath> &paths = contourTracker.getPaths();
	for (auto path : paths)
	{
		auto poly = std::make_shared<ofxBox2dPolygon>();

		ofPolyline polyline = path.getOutline()[0]; // This is the correct way to get polyline from path object
		poly->addVertices(polyline.getVertices());
		poly->close();
		poly->simplify(3);
		poly->setPhysics(1000, 0, 0);
		poly->triangulatePoly();
		poly->create(box2d.getWorld());

		contours.push_back(poly);
	}
}