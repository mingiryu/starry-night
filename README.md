# starry-night
Particle FFT visualization of music and visual interaction through webcam/OpenCV

# How to
- make, make run
- If that doesn't work, make sure to ./install_codes.sh.
- Audio file directory is bin/data/sounds, update StarryNight.cpp - setup() for other file name.

# Main components
- ContourTracker: 
	1. Color tracking through pixel by pixel comparison.
	2. Simple image manipulations for contourFinder().
	3. absDiff() gray scale image and background image.
	4. Store polylines of the contours in a vector<T>. 
- StarryNight:
	1. Load up the music file using SoundPlayer.
	2. Analyze it using FFT, store it in an array.
	3. Update and draw particles based on results from the FFT.
	4. Store the information in a queue to be accessed next frame.
- ofApp:
	1. Initialize the compoments.
	2. Update() and Draw().
	3. update() handles the interaction between the contours and particles.

# Issues
- Increase in number of contours decreases FPS. Inside() may be ineffienct for handling interaction.
- The size of the particles needs to be adjusted based on the type and genre of the music for good visual.

# Resources
ofDoc:
http://halfdanj.github.io/ofDocGenerator/index.html

ofBook/Image Processing and Computer Vision: https://openframeworks.cc/ofBook/chapters/image_processing_computer_vision.html

# Credit
https://www.bensound.com/ for royal free music.