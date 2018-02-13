# Using MaxMSP with AlloSystem in the AlloSphere

This example has 3 files:

- `simulator.maxpat` is a simulator (i.e., the master or server---the thing that controls everything else). It sends OSC messages to all the AlloSphere renderer machines.
- `renderer_allosphere.cpp` is a renderer. It interprets messages from the master and renders a scene using OmniStereo rendering to make things look right in the AlloSphere.
- `noise+sine_60ch.maxpat` is an example of how we test the audio system in the AlloSphere.


