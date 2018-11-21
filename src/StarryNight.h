#pragma once

#include "ofMain.h"

class StarryNight{
	public:
		void setup(int width_, int height_);
		void update();
		void draw();

		queue<ofVec4f*> getStars();
		
		int width, height, scale;

		ofSoundPlayer sound;
		static constexpr size_t nBandsToGet = 128;
		std::array<float, nBandsToGet> fftSmoothed{{0}};

		/**
		 * ofVec4f(x,y,z,w):
		 * x,y := position of the star. 
		 *   z := velocity (nth band of FFT). 
		 *   w := size of star (strength of the band)
		 * 
		 * Uses pointer of ofVec4f to make changes on the stars over time.
		 * Queue<T> allows removal during iteration unlike vector<T>.
		 */
		queue<ofVec4f*> stars;
};