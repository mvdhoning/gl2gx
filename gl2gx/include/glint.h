#include "GL/gl.h"

//TODO: rewrite to support dynamic arrays
#define MAX_ARRAY 1000

typedef struct 
{
	float x;
	float y;
	float z;
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
} ColorElement;

VertexElement _tempnormalelement;
TexCoordElement _temptexcoordelement;
ColorElement _tempcolorelement;

//TODO: integrate in normal, vertex, texcoord and color in struct and place that in dynamic arrray
VertexElement _normalelements[MAX_ARRAY]; //TODO: dynamic
VertexElement _vertexelements[MAX_ARRAY]; //TODO: dynamic
TexCoordElement _texcoordelements[MAX_ARRAY]; //TODO: dynamic
ColorElement _colorelements[MAX_ARRAY]; //TODO: dynamic

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
//GXColor AmbientColor; //? is actually material?

//light specs
GXColor gxlightambientcolor[8];
GXColor gxlightdiffusecolor[8];
GXColor gxlightspecularcolor[8];

//lightmodel specs
GXColor gxglobalambientlightcolor;

//material specs
GXColor gxcurrentmaterialemissivecolor;
GXColor gxcurrentmaterialambientcolor;
GXColor gxcurrentmaterialdiffusecolor;
GXColor gxcurrentmaterialspecularcolor;

/* textures */
bool tex2denabled;
GLint curtexture;
GLenum curtexturetarget;
GXTexObj gxtextures[MAX_ARRAY]; //TODO: dynamic

/* end */
