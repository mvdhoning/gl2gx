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

ColorElement _tempcolorelement;
ColorElement _colorelements[MAX_ARRAY];

Mtx _mtxelements[MAX_ARRAY];
int _mtxcurrentelement;

VertexElement _tempnormalelement;
TexCoordElement _temptexcoordelement;

VertexElement _normalelements[MAX_ARRAY];
VertexElement _vertexelements[MAX_ARRAY];
TexCoordElement _texcoordelements[MAX_ARRAY];

u8 depthtestenabled;
GLenum depthfunction;
float _cleardepth;

int _numelements;
int _type;

Mtx view,perspective; // view and perspective matrices
Mtx model, modelview, inversemodel, normalmodel;

/* light */
GXLightObj gxlight[8];
GXColor AmbientColor;

/* textures */
GLint curtexture;
GLenum curtexturetarget;
GXTexObj gxtextures[MAX_ARRAY];

/* end */
