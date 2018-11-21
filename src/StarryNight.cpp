#include "StarryNight.h"

void StarryNight::setup(int width_, int height_)
{
    width = width_;
	height = height_;
	scale = ofGetHeight() / height;

    sound.load("sounds/bensound.mp3");
    ofEnableAlphaBlending();
    ofEnableSmoothing();
}

void StarryNight::update()
{
    ofSoundUpdate();

    if (!sound.isPlaying()) {
        sound.play();
    }

    // Audio processing with FFT.
    float * val = ofSoundGetSpectrum(nBandsToGet);
    for (size_t i = 0; i < nBandsToGet; i++) {
        fftSmoothed[i] *= 0.96f;
        if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
    }

    // Initial new stars based on FTT and store its information in the queue.
    for (size_t i = 0; i < nBandsToGet; i++) {
        // Randomization of starting position.
        int x = ofRandom(width*scale);
        int y = ofRandom(ofGetHeight());

        // Randomization of starting axis.
        if (i % 2 == 0) {
            int a = i*width*scale/nBandsToGet; // a  ~ x.
            stars.push(new ofVec4f(a, y, i, fftSmoothed[i]));
        } else {
            int a = i*ofGetHeight()/nBandsToGet; // a ~ y.
            stars.push(new ofVec4f(x, a, i, fftSmoothed[i]));
        }
    }
}

void StarryNight::draw()
{
    ofBackground(ofColor::midnightBlue);

    // Draw all the stars 
    int size = stars.size();
    int count = 0;
    while (count < size - 1) {
        if (!stars.empty()) {
            ofVec4f* star = stars.front();

            // Move the star by its velocity (Make them fall) and decrease its size by 0.01.
            // star->z can be equal to 0. Adding 1 insures movement.
            star->x += (star->z + 1);
            star->y += (star->z + 1);
            star->w *= 0.99;
            
            ofSetColor(ofColor::lemonChiffon);
            ofDrawCircle(star->x, star->y, star->w * 10); // TODO: Implement drawStar() function.

            stars.pop();

            // Delete star, if it is out of bound or too small.
            if (star->w < 0.001 
				|| (star->x > width*scale && star->y > ofGetHeight())
				|| (star->x < 0 && star->y < 0)) {

                delete star;
                --size; // Must decrement size after deletion.
            } else {
                stars.push(star);
            }
        }
        ++count;
    }
}

queue<ofVec4f*> StarryNight::getStars() 
{
    return stars;
}
