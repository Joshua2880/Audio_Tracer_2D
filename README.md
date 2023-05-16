# Audio_Tracer_2D

A 2D prototype of impulse response bakeing, using raytracing to precalculate impulse responses at any position, given an audio source position and boundry geometry.

## Currenty issues

Ray leakage occurs occasionaly at boundries, this is possible due to floating point error.

Memory requirements render the concept impractical for any impulse response of significant length and space of significant size.
