# Audio_Tracer_2D

A 2D prototype of impulse response bakeing, using raytracing to precalculate impulse responses at any position, given an audio source position and boundry geometry.

## Usage

Simulation bounds and an audio source position is specified in the Tracer constructor, after which boundry objects can be added with Tracer::AddObject.  This Simulation uses the Win32 API to display the magnitude response at each position for a given frequency.  The up and down arrow keys can be used to sweep the displayed frequency.

![](https://github.com/Joshua2880/Audio_Tracer_2D/blob/master/media/Render.gif)

## Current issues

Ray leakage occurs occasionaly at boundries, this is possible due to floating point error.

Memory requirements render the concept impractical for any impulse response of significant length and space of significant size.
