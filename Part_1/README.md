# cs311
Assingments for CS311: Computer Graphics

We implement a 3D graphics engine in C. We begin with a minimal hardware interface that lets us make a window and set individual pixels to specified colors. Atop this primitive foundation we implement triangle rasterization, texture mapping, scene graphs, projections, lighting, etc.

020mainRasterizing.c 	: Rasterize a triangle
030mainInterpolating.c 	: Color interpolation
040mainTexturing.c 		: Texture mapping, nearest-neighbor and quadratic filtering
050mainAbstracted.c 	: Abstraction of information, uniform, texel, and attribute
070mainMesh.c 			: Meshs can be rendered
080mainPosable.c 		: Transformation of vertices
090mainScene.c 			: Scene Graphs
100main3D.c 			: 3D Scenes
110mainDepth.c 			: Depth Tests
120mainCamera.c 		: Add camera to scene
130mainProjection.c 	: Perspective and Orthographic Projections
140mainClipping.c 		: Near plane and side plane clipping
160mainDiffuse.c 		: Diffused Lighting
170mainSpecular.c 		: Specular Lighting
180mainAmbient.c 		: Ambient Lighting
190mainFog.c 			: Fog implemented