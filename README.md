# starry-night
Visualization of music with native FFT and ofBox2d. Primitive interaction through webcam/OpenCV.

![starry-night.png](https://github.com/mingir2/starry-night/blob/master/starry_night.png)

[![starry-night-vimeo](https://github.com/mingir2/starry-night/blob/master/starry_night_vimeo.png)](https://vimeo.com/305878055 "Click to Watch!")

# How to
- make, make run
- If that doesn't work, make sure to ./install_codes.sh.
- Press 'v' to view webcam input, 'g' to view gui.
- You can right click on the webcam input to select specific color for the color tracking.
- The default parameters should work most of the time, but adjust them based on your preference.
- I recommend using different mp3 file of your choice. The directory is bin/data/sounds

# Main components
- ContourTracker: 
	1. Color tracking (pixel by pixel, euclidean distance in RGB space).
	2. Blur(), dilate(), and other image manipulations for contourFinder().
	3. Adaptive backgound by mutiplying float images.
	4. ofPath/ofPolyline of the contours to be used in box2d. 
- ofApp:
	1. Load up the music file using SoundPlayer.
	2. Set up background and box2d world.
	3. Analyze the music using FFT, store it in an array.
	4. Update and initialize box2d particles based on results from the FFT.

# Issues
- Increase in number of contours and particles decreases FPS. Currently, maximum number of particles is set to 100.
- ofBox2d warnings populate the CLI and contour representation of box2d world is visually gitchy.

# Resources
ofDoc:
http://halfdanj.github.io/ofDocGenerator/index.html

ofBook/Image Processing and Computer Vision: https://openframeworks.cc/ofBook/chapters/image_processing_computer_vision.html

# Credit
https://www.bensound.com/ for royalty free music.