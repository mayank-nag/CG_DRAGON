3D DRAGON TRANSFORMATIONS (C / OpenGL / GLUT / Assimp)
======================================================

This project uses the actual high-poly Tarisland Dragon model you uploaded.
The model files are included in the model/ folder. Its original license.txt
and source attribution are retained.

CONTROLS
--------
Translation:
  A / D  move left / right (X axis)
  W / S  move up / down (Y axis)
  Q / E  move toward / away (Z axis)

Rotation:
  I / K  rotate around X axis
  J / L  rotate around Y axis
  U / O  rotate around Z axis

Scaling:
  +      increase size
  -      decrease size
  R      reset transformations
  Esc    exit

DEPENDENCIES
------------
- C compiler (GCC/MinGW)
- OpenGL and GLU development libraries
- GLUT or FreeGLUT
- Assimp development library (C API headers and library)

LINUX (Debian/Ubuntu)
---------------------
Install packages (package names can vary by distribution):
  sudo apt install build-essential freeglut3-dev libassimp-dev

From this folder, compile:
  gcc dragon_transform.c -o dragon_transform -lglut -lGLU -lGL -lassimp -lm

Run:
  ./dragon_transform model/scene.gltf

WINDOWS
-------
Install MinGW-w64, FreeGLUT, and Assimp, and configure their include/library
paths. Link against the matching GLUT, OpenGL, GLU, Assimp, and math libraries.
Then run with the model path as the first argument, for example:
  dragon_transform.exe model\scene.gltf

NOTES
-----
- The application uses legacy/fixed-function OpenGL to keep the transformations
  visible and exam-friendly.
- Assimp imports and triangulates the model and generates normals. This sample
  uses material diffuse colors and lighting; it does not implement the model's
  original PBR texture/shader pipeline, so its appearance may differ from the
  Sketchfab viewer.
- The dragon model is by Doctor A. and is marked CC BY 4.0 in the supplied
  archive. Keep the attribution/license when redistributing it.
