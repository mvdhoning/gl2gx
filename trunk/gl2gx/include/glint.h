#include "GL/gl.h"

//TODO: rewrite to support dynamic arrays
#define MAX_ARRAY 1000

#define LARGE_NUMBER 9.9999998e+017
//1048576
//1.0e-18F
//9.9999998e+017

typedef struct 
{
	float x;
	float y;
	float z;
	float w;
} VertexElement;

typedef struct 
{
	float s;
	float t;
} TexCoordElement;

typedef struct 
{
	float r;
	float g;
	float b;
	float a;
} GXColorf;

VertexElement _tempnormalelement;
TexCoordElement _temptexcoordelement;
GXColorf _tempcolorelement;

//TODO: integrate in normal, vertex, texcoord and color in struct and place that in dynamic arrray
VertexElement _normalelements[MAX_ARRAY]; //TODO: dynamic
VertexElement _vertexelements[MAX_ARRAY]; //TODO: dynamic
TexCoordElement _texcoordelements[MAX_ARRAY]; //TODO: dynamic
GXColorf _colorelements[MAX_ARRAY]; //TODO: dynamic

int _numelements;
int _type;

/* Depth Buffer */

u8 depthtestenabled;
GLenum depthfunction;
float _cleardepth;

/* Matrixes */

Mtx _mtxelements[32]; //max stack depth is 32 in opengl
int _mtxcurrentelement;
//TODO: clean up unneeded and naming
Mtx view,perspective; // view and perspective matrices
Mtx model, modelview, inversemodel, normalmodel;

/* light */
GXLightObj gxlight[8]; //max 8 lights in opengl
VertexElement gxlightpos[8]; //light position for each light
VertexElement gxlightdir[8]; //light direction for each light
bool gxlightenabled[8]; //is specific light enabled
int gxcurlight; //test for light position

//light specs
GXColorf gxlightambientcolor[8];
GXColorf gxlightdiffusecolor[8];
GXColorf gxlightspecularcolor[8];

//spot light specs
VertexElement gxspotdirection[8];
float gxspotexponent[8];
float gxspotcutoff[8];

float gxconstantattanuation[8];
float gxlinearattanuation[8];
float gxquadraticattanuation[8];

//lightmodel specs
GXColorf gxglobalambientlightcolor;

//material specs
GXColorf gxcurrentmaterialemissivecolor;
GXColorf gxcurrentmaterialambientcolor;
GXColorf gxcurrentmaterialdiffusecolor;
GXColorf gxcurrentmaterialspecularcolor;
float gxcurrentmaterialshininess;

/* culling */
bool gxcullfaceanabled;
GLenum gxwinding;

/* textures */
bool tex2denabled;
GLint curtexture;
GLenum curtexturetarget;
GXTexObj gxtextures[MAX_ARRAY]; //TODO: dynamic

/* end */
