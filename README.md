![Puffin Logo](logo.png)
# Puffin: an OpenGL helper library written in C

I started Puffin in 2010 as a project to teach myself "modern" OpenGL and how to approximate object-oriented design in plain C. 
My interest tapered off around 2012, but in late 2016 I picked it up again and slowly started dusting it off and correcting some of the bugs and design mishaps with fresh eyes and deeper undestanding. 

I based my attempt on GLUT and GLEW to get the OpenGL part going, but otherwise wanted to avoid external dependencies, so I rolled my own text file, OBJ and BMP loaders so I can get at some external data.

The design of Puffin is a reflection of my experience as a 3D artist and the level of abstraction at which I'm used to thinking. Beyond the learning experience I hope and expect that it'll find use as a rapid prototyping platform for small programs and experiments, at least for myself. Partly to reflect that, I ripped off the setup() and draw() functions from Processing.org, although I'm definitely aiming for something more compact.


## Puffin currently knows how to:
* Create a window and a camera. Multiple cameras and maybe even multiple windows should work, but haven't been tested.
* Loading 3D meshes in OBJ format, RGB or RGBA (8 bits per channel) textures in BMP format and shader code in plain text.
* Move, rotate and scale meshes and move and rotate cameras.
* Set up and bind shaders (vertex- and fragment shaders) and textures and use them to draw/render meshes into a camera view.
* Set up off-screen frame buffers and use them as textures.
* Responding to keyboard input in the manner of GLUT.

Rudimentary support that hasn't been tested in a long time exists for setting and getting pixel values from textures.


## Required libraries:
* GLEW
* FreeGLUT


## Compiles and has been tested to work on:
* Microsoft Windows 7 with NVIDIA drivers using MinGW


## TODO:

* delete lots of code that's just commented out
* document the example better
* compile on Mac and eventually Linux
* document where to find DLLs, how to set up the build environment, etc
* LOOKINTO: Frame rate limiting. Does it work right now or does it merely seem like it works? Is there a cleaner way. How can we best measure and display the frame rate?
* make sure it's really easy to set up a full screen quad with one or more textures and do stuff to them on both the shader and the GPU (think: an up to date, multiplatform replacement for TinyPTC)
* add separate local and global rotations. Do we need to maintain some sort of local gimbal for this?
* do normal matrix inverse and transpose on the CPU instead of in the vertex shader
* test and optimize interaction with some suitable image and 3D mesh formats. libpng, alembic?
* move texture functions into their own file
* make pufInit(), pufCameraInit(), ... return their type (RATIONALE: mesh = pufMeshInit() rather than pufMeshInit(&mesh) makes it easier to spot where the mesh is first used in a piece of code)
* make pufMeshLoadOBJ, pufMeshShapeQuad(), etc mesh creators and loaders call pufMeshInit() so that You don't have to!
* CONSIDER/LOOKINTO: make the same transform functions work with both PUFmesh and PUFcamera (make them take a void pointer and fiddle the correct place in the structs based on that)
* figure out what to do with PUFcolor. Possibly remove.
* figure out why linking against freeglut.dll with MinGW gives errors. Build your own freeglut and GLEW with MinGW.
* CONSIDER: conform to C90 (replace variable length arrays with malloc() bits, etc) to satisfy Visual Studio
* LOOKINTO: should GLUT's Idle Function be used for something?
