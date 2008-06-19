#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include <ogcsys.h>
#include <gccore.h>

#include "glint.h"
#include "GL/gl.h"

GXColor _clearcolor = {0, 0, 0, 0xff};

//textures need to be 4x4 tiles


/* matrix */

void glLoadIdentity(void) {
	guMtxIdentity(model);
}

void glTranslatef(float x,float y,float z) {
	Mtx temp;

	guMtxIdentity(temp);
	guMtxTrans(temp, x, y, z);	
	guMtxConcat(model,temp,model);
}

void  glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	Mtx temp;
	Vector axis;

	axis.x = x;
	axis.y = y;
	axis.z = z;
	guMtxIdentity(temp);
	guMtxRotAxisDeg(temp, &axis, angle);
	guMtxConcat(model,temp,model);

}

void  glScalef (GLfloat x, GLfloat y, GLfloat z){
	Mtx temp;

	guMtxIdentity(temp);
	guMtxScale(temp, x, y, z);
	guMtxConcat(model,temp,model);
}

void  glPopMatrix (void){
	_mtxcurrentelement--;
	guMtxCopy(_mtxelements[_mtxcurrentelement], model);
}

void  glPushMatrix (void){
	guMtxCopy(model, _mtxelements[_mtxcurrentelement]);
	_mtxcurrentelement++;
}

/* glVertex */

void glVertex3f(float x,float y,float z) {
	//store the vertex and keep index
	_vertexelements[_numelements].x = x;
	_vertexelements[_numelements].y = y;
	_vertexelements[_numelements].z = z;

	_colorelements[_numelements].r = _tempcolorelement.r;
	_colorelements[_numelements].g = _tempcolorelement.g;
	_colorelements[_numelements].b = _tempcolorelement.b;
	_colorelements[_numelements].a = _tempcolorelement.a;

	_normalelements[_numelements].x = _tempnormalelement.x;
	_normalelements[_numelements].y = _tempnormalelement.y;
	_normalelements[_numelements].z = _tempnormalelement.z;

	_texcoordelements[_numelements].s = _temptexcoordelement.s;
	_texcoordelements[_numelements].t = _temptexcoordelement.t;

	_numelements +=  1;	
}

void glNormal3f(float x, float y, float z){
	_tempnormalelement.x = x;
	_tempnormalelement.y = y;
	_tempnormalelement.z = z;
}

void glColor3f(float r,float g,float b) {
	//store the vertex and keep index
	_tempcolorelement.r = r;
	_tempcolorelement.g = g;
	_tempcolorelement.b = b;
	_tempcolorelement.a = 0.0f;
}

void glTexCoord2f( GLfloat s, GLfloat t ){
	_temptexcoordelement.s = s;
	_temptexcoordelement.t = t;
};

/* functions */
void glBegin(GLenum type) {
	_numelements =0;
	//store the type
	switch(type)
	{
		case GL_TRIANGLES: _type = GX_TRIANGLES; break;
		case GL_QUADS: _type = GX_QUADS; break;
		case GL_LINES: _type = GL_LINES; break;
	};
}

void glEnd(void) {

	Mtx mvi;

	// load the modelview matrix into matrix memory
	guMtxConcat(view,model,modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	//for normals first calculate normal matrix (thanks shagkur)
	guMtxInverse(modelview,mvi);
	guMtxTranspose(mvi,modelview);
	GX_LoadNrmMtxImm(modelview,GX_PNMTX0);

	//first check if a lightdirtyflag is set (thanks ector) so we do not have to set up light every run
	//also usefull on matrices etc.

	//now set each light
	int lightcounter = 0;
	for (lightcounter =0; lightcounter < 8; lightcounter++){

		if(gxlightenabled[lightcounter]){ //when light is enabled

			//postion
			Vector lpos;
			lpos.x = gxlightpos[lightcounter].x;
			lpos.y = gxlightpos[lightcounter].y;
			lpos.z = gxlightpos[lightcounter].z;
			guVecMultiply(view,&lpos,&lpos);	//update light position by current view matrix
												//light position should be transformed by world-to-view matrix (thanks h0lyRS)
			GX_InitLightPos(&gxlight[lightcounter], lpos.x, lpos.y, lpos.z); //feed corrected coord to light pos
			
			//dir attn spot TODO: these should be controleable from opengl
			GX_InitLightDir(&gxlight[lightcounter], 0, -1, 0);	//shine down from y axis? Is this opengl default also?
																//and direction should be transformed by inv-transposed of world-to-view (thanks h0lyRS)
			GX_InitLightDistAttn(&gxlight[lightcounter], 20.0f, 0.5f, GX_DA_MEDIUM);
			GX_InitLightSpot(&gxlight[lightcounter], 0.0f, GX_SP_OFF); //not this is not a spot light

			//Load the light up
			switch (lightcounter){
				case 0: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT0); break;
				case 1: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT1); break;
				case 2: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT2); break;
				case 3: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT3); break;
				case 4: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT4); break;
				case 5: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT5); break;
				case 6: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT6); break;
				case 7: GX_LoadLightObj(&gxlight[lightcounter], GX_LIGHT7); break;
			}
		}
	}

	//set the curtexture if tex2denabled
	if (tex2denabled){
	GX_LoadTexObj(&gxtextures[curtexture], GX_TEXMAP0); //TODO: make GX_TEXMAP0 dynamic for multitexturing
	};

	//now we can draw the gx way
	GX_Begin(_type, GX_VTXFMT0, _numelements);
	int i =0;
	for( i=0; i<_numelements; i++)
	{
		GX_Position3f32( _vertexelements[i].x, _vertexelements[i].y, _vertexelements[i].z);	
		GX_Normal3f32(_normalelements[i].x, _normalelements[i].y, _normalelements[i].z);

		//when using GL_FLAT only one color is allowed!!! //GL_SMOOTH allows for an color to be specified at each vertex
		GX_Color3f32( _colorelements[i].r, _colorelements[i].g, _colorelements[i].b); //glmaterialfv call instead when glcolormaterial call is used

		GX_TexCoord2f32(_texcoordelements[i].s,_texcoordelements[i].t);
		
	}	
	GX_End();

	//clean up just to be sure
	i =0;
	for( i=0; i<_numelements; i++)
	{
		_vertexelements[i].x = 0.0F;
		_vertexelements[i].y = 0.0F;
		_vertexelements[i].z = 0.0F;

		_normalelements[i].x = 0.0F;
		_normalelements[i].y = 0.0F;
		_normalelements[i].z = 0.0F;

		_colorelements[i].r = 0.0F;
		_colorelements[i].g = 0.0F;
		_colorelements[i].b = 0.0F;
		_colorelements[i].a = 0.0F;

		_texcoordelements[i].s = 0.0F;
		_texcoordelements[i].t = 0.0F;

	}
	_numelements =0;
}

/* light */

void glLightfv( GLenum light, GLenum pname, const GLfloat *params ){
	int lightNum = 0;
	switch(light)
	{
		case GL_LIGHT0: lightNum = 0; break;
		case GL_LIGHT1: lightNum = 1; break;
		case GL_LIGHT2: lightNum = 2; break;
		case GL_LIGHT3: lightNum = 3; break;
		case GL_LIGHT4: lightNum = 4; break;
		case GL_LIGHT5: lightNum = 5; break;
		case GL_LIGHT6: lightNum = 6; break;
		case GL_LIGHT7: lightNum = 7; break;						
	}

	Vector lightPos={0.0F,0.0F,0.0F}; //TODO: make it opengl default light position
	GXColor lightCol = {255, 255, 255, 0xAA}; //TODO: make it opengl default light color
	switch(pname)
	{
		case GL_POSITION: 
			lightPos.x = params[0];
			lightPos.y = params[1];
			lightPos.z = params[2];
			guVecMultiply(model,&lightPos,&lightPos); //update light position by current model matrix
			gxlightpos[lightNum].x = lightPos.x;
			gxlightpos[lightNum].y = lightPos.y;
			gxlightpos[lightNum].z = lightPos.z;
			gxcurlight = lightNum;
			break;
		case GL_DIFFUSE:
			lightCol.r = params[0] * 0xff;
			lightCol.g = params[1] * 0xff;
			lightCol.b = params[2] * 0xff; 
			lightCol.a = params[3] * 0xff; 
			GX_InitLightColor(&gxlight[lightNum],lightCol);
			break;
		case GL_AMBIENT:
			AmbientColor.r = params[0] * 0xff;
			AmbientColor.g = params[1] * 0xff;
			AmbientColor.b = params[2] * 0xff;
			AmbientColor.a = params[3] * 0xff;
	}
};

/* material */

//GX_SetChanMatColor(GX_COLOR0A0,MaterialColor); //glMaterialfv(GLFRONT, GL_DIFUSE, materialcolor);

/* texture */

void glGenTextures( GLsizei n, GLuint *textures ){
	textures = malloc(sizeof(GLuint)*n);
	int i=0;
	for(i=0;i<n;i++)
	textures[i] = i;
};

void glBindTexture( GLenum target, GLuint texture ){
	curtexture = texture;
	curtexturetarget = target;
};

void glTexImage2D( GLenum target, GLint level,
                                    GLint internalFormat,
                                    GLsizei width, GLsizei height,
                                    GLint border, GLenum format, GLenum type,
									const GLvoid *pixels ){
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, crate0tex.width, crate0tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, crate0tex.tex_data);
	GX_InitTexObj(&gxtextures[curtexture], (void*)pixels, width,height,6,1,1,GX_FALSE); //6 = colfmt make it dynamic

};

/* clear */

void  glClear (GLbitfield mask) {
	//implement logic dealing with mask.
	GX_SetCopyClear(_clearcolor, _cleardepth); // this clears both buffers

	//to clear only one of buffers then something more clever has to be done (thanks to samson)
	//Disable colour-write, enable zwrite, disable z-test, write a screen-aligned quad at whatever depth you want. 
}

void glClearColor(	GLclampf red,
					GLclampf green,
					GLclampf blue,
					GLclampf alpha ) {
	_clearcolor.r = red * 0xff; //color needs to be converted to byte
	_clearcolor.g = green * 0xff;
	_clearcolor.b = blue * 0xff;
	_clearcolor.a = alpha * 0xff;
}

void  glClearDepth (GLclampd depth){
	_cleardepth = depth * 0x00ffffff;
}

/* misc */

void glFlush(void) {
	GX_DrawDone();
}

void glEnable(GLenum type){

	u8 gxlightmask = 0x00000000;
	int lightcounter = 0;

		switch(type)
		{
			case GL_DEPTH_TEST: depthtestenabled = GX_TRUE; break;
			case GL_LIGHTING:

				//<h0lyRS> did you try setting specular to channel GX_COLOR1A1, and diffuse to GX_COLOR0A0, so you can blend them anyway you want?
				//this could add an extra color?
				//one way is adding the specular light, other way is multiplying

				//expand this to create opengl lighting mode.

				//GX_SetChanCtrl(GX_COLOR0A0, TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);
				//GX_SetTevOp(GX_TEVSTAGE0,GX_MODULATE); //TODO: a call to gl_texture_2d should not undo this?
				//GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

				//making a lightmask (hmm there must be a more efficient way to do this, at least it should be somewhere else)
				//<h0lyRS>	you can simply shift GX_LIGHT0 << lightnum
				int countlights =0;
				for (lightcounter =0; lightcounter < 8; lightcounter++){
					if(gxlightenabled[lightcounter]){ //when light is enabled
						gxlightmask |= (GX_LIGHT0 << lightcounter);
						countlights++;
					}
				};
/*
				for (lightcounter =0; lightcounter < 8; lightcounter++){
					if(gxlightenabled[lightcounter]){ //when light is enabled
						switch (lightcounter){
							case 0: gxlightmask = gxlightmask | GX_LIGHT0; break;
							case 1: gxlightmask = gxlightmask | GX_LIGHT1; break;
							case 2: gxlightmask = gxlightmask | GX_LIGHT2; break;
							case 3: gxlightmask = gxlightmask | GX_LIGHT3; break;
							case 4: gxlightmask = gxlightmask | GX_LIGHT4; break;
							case 5: gxlightmask = gxlightmask | GX_LIGHT5; break;
							case 6: gxlightmask = gxlightmask | GX_LIGHT6; break;
							case 7: gxlightmask = gxlightmask | GX_LIGHT7; break;
						}
					}
				}
*/

				
				GX_SetChanCtrl(GX_COLOR0A0,GX_ENABLE,GX_SRC_VTX,GX_SRC_REG,gxlightmask,GX_DF_CLAMP,GX_AF_NONE); //uses (texture)perpixel colors (light works)
				
				//GX_SetChanCtrl(s32 channel,u8 enable,u8 ambsrc,u8 matsrc,u8 litmask,u8 diff_fn,u8 attn_fn)
				//sets how an channel works
			
//	<RedShade>	GX_SetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_REG,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE); = settings for that channel

//		s32 channel = color that light will result in
//		u8 enable = second is (ON/OFF)
//		u8 ambsrc = ambient color is on vertex (GX_SRC_VTX), or uses global GX_SetChanAmbColor(GX_COLOR0A0, ambient); (GX_SRC_REG)
//		u8 matsrc = material color is based on vertex (GX_SRC_VTX), or global with GX_SetChanMatColor(GX_COLOR0A0, material); (GX_SRC_REG)
//		u8 litmask = Defines what gx lights are used. light null = 00000000, light_1 = 00000001
//		u8 diff_fn = turning on/off difuse light (material?)
//		u8 attn_fn =  type of lighting (spetral/spotlight)

//				GX_SetChanCtrl(GX_COLOR0A0,GX_ENABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT0,GX_DF_CLAMP,GX_AF_NONE); //vertex lighting (light does not work?)

				//light setup?

				GX_SetNumChans(1); //use/enable one light (the first?)
				
				//GX_SetChanCtrl(GX_COLOR0A0, TRUE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_SIGNED, GX_AF_NONE); //write out what this does
																//uses vertex? then what is GX_SRC_REG

				//GXColor amb = { 128, 64, 64, 255 };
				GXColor amb = { 128, 128, 128, 128 }; //glmaterialf ? 
				GXColor mat = { 123, 123, 123, 123 }; //glmaterialf ?
				GX_SetChanAmbColor(GX_COLOR0A0, amb); //glmaterialf ? or GLOBAL AMBIEN LIGHT glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb); ??
				GX_SetChanMatColor(GX_COLOR0A0, mat); //glmaterialf ? if to low no light effect visible to high object to bright be default
				//How does this relate to light color? e.g. in opengl both light and material have diffuse and ambient component

				// Set up shader (write out what each step means)
				GX_SetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
				GX_SetNumTevStages(1); //each extra color takes another stage?
				
				//color
				GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
				
				//tevorder
				if (tex2denabled){
					GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_RASC); //modulate
					GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_TEXA, GX_CA_RASA);
					GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0); //texturing
				}
				else {
					GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC); //vertex color?
					GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
					GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0); //no texturing
				};				
				GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);



				break;
			case GL_TEXTURE_2D:
				tex2denabled = true;
				GX_SetNumTexGens(1); //multitexturing so set to 1 for now
				GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				break;
			case GL_LIGHT0: gxlightenabled[0]=true; break;
			case GL_LIGHT1: gxlightenabled[1]=true; break;
			case GL_LIGHT2: gxlightenabled[2]=true; break;
			case GL_LIGHT3: gxlightenabled[3]=true; break;
			case GL_LIGHT4: gxlightenabled[4]=true; break;
			case GL_LIGHT5: gxlightenabled[5]=true; break;
			case GL_LIGHT6: gxlightenabled[6]=true; break;
			case GL_LIGHT7: gxlightenabled[7]=true; break;
		};
}

void glDisable(GLenum type){
		switch(type)
		{
			case GL_DEPTH_TEST: depthtestenabled = GX_FALSE; break;
			case GL_LIGHTING:
				GX_SetNumChans(1); //keep this at one all time?
				GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
				GX_SetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE); //how to disable ligting?
				break;
			case GL_TEXTURE_2D:
				tex2denabled = false;
				GX_SetNumTexGens(0); //texturing is of so no textures
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);			
				GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
				break;
			case GL_LIGHT0: gxlightenabled[0]=false; break;
			case GL_LIGHT1: gxlightenabled[1]=false; break;
			case GL_LIGHT2: gxlightenabled[2]=false; break;
			case GL_LIGHT3: gxlightenabled[3]=false; break;
			case GL_LIGHT4: gxlightenabled[4]=false; break;
			case GL_LIGHT5: gxlightenabled[5]=false; break;
			case GL_LIGHT6: gxlightenabled[6]=false; break;
			case GL_LIGHT7: gxlightenabled[7]=false; break;
		};
}

/* Depth Buffer */

void glDepthFunc(GLenum type){
		switch(type)
		{
		case GL_NEVER: depthfunction = GX_NEVER; break;
		case GL_LESS: depthfunction = GX_LESS; break;
		case GL_EQUAL: depthfunction = GX_EQUAL; break;
		case GL_LEQUAL: depthfunction = GX_LEQUAL; break;
		case GL_GREATER: depthfunction = GX_GREATER; break;
		case GL_NOTEQUAL: depthfunction = GX_NEQUAL; break;
		case GL_GEQUAL: depthfunction = GX_GEQUAL; break;
		case GL_ALWAYS: depthfunction = GX_ALWAYS; break;

		};
}

/* end */
