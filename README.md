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

* better error handling, make a thing for displaying OpenGL errors
* compile with MSVC++
* swap out freeglut for GLFW (to avoid X11 on macOS, etc)
* compile on macOS
* separate mesh, camera, texture etc things out into their own files, #include them all into puffin.c
* stop using OpenGL types, start using stdint
* Look into frame rate limiting. Does it work right now or does it merely seem like it works? Is there a cleaner way. How can we best measure and display the frame rate?
* Clean up pufMeshRender() and matrix.c. Make the transformation matrix calculations nicer.
* make pufInit(), pufCameraInit(), ... return their type (RATIONALE: mesh = pufMeshInit() rather than pufMeshInit(&mesh) makes it easier to spot where the mesh is first used in a piece of code)
* Hook up some external libraries. At a minimum we want: 
    - joystick (GLFW)
    - some additional image formats (stb_image)
    - capture device (OpenCV)
    - UI (nuklear)
* make pufMeshLoadOBJ, pufMeshShapeQuad(), etc mesh creators and loaders call pufMeshInit() so that You don't have to!
* Add world space transforms. Currently all transforms are object space only. Passing WORLD or OBJECT to a transformation function should determine which one is used. To transform an object in world space, make a transformation matrix from the current object coordinates and transform the input using that.
* document the example better
* compile on Linux