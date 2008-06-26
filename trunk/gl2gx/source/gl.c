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
     
     GX_SetCullMode(GX_CULL_FRONT);

	Mtx mvi;
	Mtx mv;
//	Mtx inversemodelview;

	// load the modelview matrix into matrix memory
	guMtxConcat(view,model,modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	//for normals first calculate normal matrix (thanks shagkur)
    guMtxInverse(modelview,mvi); 
    guMtxTranspose(mvi,modelview); 
    GX_LoadNrmMtxImm(modelview,GX_PNMTX0); //experimtal leave out (hmm works good?)


	//use global ambient light together with current material ambient and add emissive material color
	GXColor constcolor;
	constcolor.r = (gxcurrentmaterialambientcolor.r*gxglobalambientlightcolor.r) * 0xFF;
	constcolor.g = (gxcurrentmaterialambientcolor.g*gxglobalambientlightcolor.g) * 0xFF;
	constcolor.b = (gxcurrentmaterialambientcolor.b*gxglobalambientlightcolor.b) * 0xFF;
	constcolor.a = (gxcurrentmaterialambientcolor.a*gxglobalambientlightcolor.a) * 0xFF;
	GX_SetTevColor(GX_TEVREG0, constcolor);
	
	GXColor emiscolor;
	emiscolor.r = gxcurrentmaterialemissivecolor.r * 0xFF;
	emiscolor.g = gxcurrentmaterialemissivecolor.g * 0xFF;
	emiscolor.b = gxcurrentmaterialemissivecolor.b * 0xFF;
	emiscolor.a = gxcurrentmaterialemissivecolor.a * 0xFF;
	GX_SetTevColor(GX_TEVREG1, emiscolor);

	//first check if a lightdirtyflag is set (thanks ector) so we do not have to set up light every run
	//also usefull on matrices etc.

	//now set each light
	int lightcounter = 0;
	for (lightcounter =0; lightcounter < 8; lightcounter++){

		if(gxlightenabled[lightcounter]){ //when light is enabled

			GXColor gxchanambient;
			gxchanambient.r = ((gxcurrentmaterialambientcolor.r * gxlightambientcolor[lightcounter].r) * 0xFF);// /2;
			gxchanambient.g = ((gxcurrentmaterialambientcolor.g * gxlightambientcolor[lightcounter].g) * 0xFF);// /2;
			gxchanambient.b = ((gxcurrentmaterialambientcolor.b * gxlightambientcolor[lightcounter].b) * 0xFF);// /2;
			gxchanambient.a = ((gxcurrentmaterialambientcolor.a * gxlightambientcolor[lightcounter].a) * 0xFF);// /2;

			//GXColor gxchandiffuse;
			//gxchandiffuse.r = gxcurrentmaterialdiffusecolor.r * gxlightdiffusecolor[lightcounter].r;
			//gxchandiffuse.g = gxcurrentmaterialdiffusecolor.g * gxlightdiffusecolor[lightcounter].g;
			//gxchandiffuse.b = gxcurrentmaterialdiffusecolor.b * gxlightdiffusecolor[lightcounter].b;
			//gxchandiffuse.a = gxcurrentmaterialdiffusecolor.a * gxlightdiffusecolor[lightcounter].a;

			GX_SetChanAmbColor(GX_COLOR0A0, gxchanambient ); 
			//GX_SetChanMatColor(GX_COLOR0A0, gxchandiffuse);
			GXColor mdc;
			mdc.r = (gxcurrentmaterialdiffusecolor.r * 0xFF);// / 2;
			mdc.g = (gxcurrentmaterialdiffusecolor.g * 0xFF);// / 2;
			mdc.b = (gxcurrentmaterialdiffusecolor.b * 0xFF);// / 2;
			mdc.a = (gxcurrentmaterialdiffusecolor.a * 0xFF);// / 2;
			GX_SetChanMatColor(GX_COLOR0A0, mdc ); 

//			GX_InitLightShininess(&gxlight[lightcounter], 2.0);

			//postion
	        guMtxConcat(view,model,mv);
			Vector lpos;
			lpos.x = gxlightpos[lightcounter].x;
			lpos.y = gxlightpos[lightcounter].y;
			lpos.z = gxlightpos[lightcounter].z;
			guVecMultiply(view,&lpos,&lpos);	//update light position by current view matrix
												//light position should be transformed by world-to-view matrix (thanks h0lyRS)
			GX_InitLightPos(&gxlight[lightcounter], lpos.x, lpos.y, lpos.z); //feed corrected coord to light pos
//			GX_InitLightPos(&gxlight[lightcounter], 0.0f, 0.0f, 2.0f); //feed corrected coord to light pos
			
			
			
			//dir attn spot TODO: these should be controleable from opengl
			
			Vector ldir = { 0, 0, -1 };
            //guMtxConcat(view,model,mv);
            guMtxInverse(view,mvi);
            //guMtxTranspose(mvi,mv);
            guVecMultiply(mvi,&ldir,&ldir); //update light position by current view matrix
	        //dir attn spot TODO: these should be controleable from opengl
            GX_InitLightDir(&gxlight[lightcounter], ldir.x, ldir.y, ldir.z); //shine down from y axis? Is this opengl default also?
            //and direction should be transformed by inv-transposed of world-to-view (thanks h0lyRS)
			
//			GX_InitLightDir(&gxlight[lightcounter], 0, 0, -1);	//shine down from y axis? Is this opengl default also?
			//													//and direction should be transformed by inv-transposed of world-to-view (thanks h0lyRS)

			//make this line optional? If on it disturbs diffuse light?
			//GX_InitSpecularDir(&gxlight[lightcounter], 0, -1, 0); //needed to enable specular light

			GX_InitLightDistAttn(&gxlight[lightcounter], 100.0f, 0.5f, GX_DA_GENTLE);
			//GX_InitLightSpot(&gxlight[lightcounter], 30.0f, GX_SP_COS2); //not this is not a spot light
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

void glLightModelfv( GLenum pname, const GLfloat *params ){
	switch(pname)
	{
		case GL_LIGHT_MODEL_AMBIENT: 
			gxglobalambientlightcolor.r = params[0];
			gxglobalambientlightcolor.g = params[1];
			gxglobalambientlightcolor.b = params[2];
			gxglobalambientlightcolor.a = params[3];
			break;
	}
};

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
	GXColor defcolor={0xFF,0xFF,0xFF,0xFF};
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
			gxlightdiffusecolor[lightNum].r = params[0];
			gxlightdiffusecolor[lightNum].g = params[1];
			gxlightdiffusecolor[lightNum].b = params[2];
			gxlightdiffusecolor[lightNum].a = params[3];
			//GX_InitLightColor(&gxlight[lightNum], defcolor);
			GXColor ldc;
			ldc.r = (gxlightdiffusecolor[lightNum].r * 0xFF);// / 2;
			ldc.g = (gxlightdiffusecolor[lightNum].g * 0xFF);// / 2;
			ldc.b = (gxlightdiffusecolor[lightNum].b * 0xFF);// / 2;
			ldc.a = (gxlightdiffusecolor[lightNum].a * 0xFF);// / 2;
			GX_InitLightColor(&gxlight[lightNum], ldc ); //move call to glend or init?;
			
			break;
		case GL_AMBIENT:
			gxlightambientcolor[lightNum].r = params[0];
			gxlightambientcolor[lightNum].g = params[1];
			gxlightambientcolor[lightNum].b = params[2];
			gxlightambientcolor[lightNum].a = params[3];
			break;
		case GL_SPECULAR:
			gxlightspecularcolor[lightNum].r = params[0];
			gxlightspecularcolor[lightNum].g = params[1];
			gxlightspecularcolor[lightNum].b = params[2];
			gxlightspecularcolor[lightNum].a = params[3];
			break;
	}
};

/* material */

void glMaterialf( GLenum face, GLenum pname, GLfloat param ){
	switch(pname)
	{
		case GL_SHININESS: gxcurrentmaterialshininess = param; break;
	}
};

void glMaterialfv( GLenum face, GLenum pname, const GLfloat *params ){
	switch(pname)
	{
		case GL_EMISSION:
			gxcurrentmaterialemissivecolor.r = params[0];
			gxcurrentmaterialemissivecolor.g = params[1];
			gxcurrentmaterialemissivecolor.b = params[2];
			gxcurrentmaterialemissivecolor.a = params[3];
		break;
		case GL_DIFFUSE:
			gxcurrentmaterialdiffusecolor.r = params[0];
			gxcurrentmaterialdiffusecolor.g = params[1];
			gxcurrentmaterialdiffusecolor.b = params[2];
			gxcurrentmaterialdiffusecolor.a = params[3];
		break;
		case GL_AMBIENT:
			gxcurrentmaterialambientcolor.r = params[0];
			gxcurrentmaterialambientcolor.g = params[1];
			gxcurrentmaterialambientcolor.b = params[2];
			gxcurrentmaterialambientcolor.a = params[3];
		break;
		case GL_SPECULAR:
			gxcurrentmaterialspecularcolor.r = params[0];
			gxcurrentmaterialspecularcolor.g = params[1];
			gxcurrentmaterialspecularcolor.b = params[2];
			gxcurrentmaterialspecularcolor.a = params[3];
		break;
	}
};

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
	int countlights =0;

		switch(type)
		{
			case GL_DEPTH_TEST: depthtestenabled = GX_TRUE; break;
			case GL_LIGHTING:

				//getting opengl lights to work on gx
				//
				//xg has only one light color (a diffuse one)
				//opengl lights have the following colors: diffuse, ambient and specular
				//also opengl has a global ambient color independend of a individual light
				//gx has 2 material colors: diffuse (mat) and ambient (can also be considered part of light?) 
				//opengl material have the followning colors: diffuse, ambient, specular and emission
				//so we (may) have problem (or call it a challenge)

				//now how does opengl calculate light with all these colors
				//vertex color	= material emission 
				//				+ global ambient scaled by material ambient
				//				+ ambient, diffuse, specular contributions from light(s), properly attinuated
				//
				//let us take these apart.
				//
				//material emission is like a constant color. So we can just add this in a tev stage. The only problem is how to add a color to a specific stage?)
				//
				//global ambient scaled by material ambient
				//this is global ambient * material ambient so we can feed that result to an tev stage.
				//
				//Now comes the hard part as each color is used in the light calulation. And we only have once color in gx.
				//Maybe we are lucky as each colors term is added to each other and only then used in light calculation
				//So we might get away with just adding the 3 colors upfront and feed that as color to the light source
				//But first let see how these terms are calculated.
				//
				//Ambient Term = light ambient * material ambient					= GXChanAmbColor ?							
				//Diffuse Term = surface * (light diffuse * material diffuse)		light diffues = light color	| material diffuse = GXChanMatColor	(let gx handle this)	
				//Specular Term = normal.shininess * (light specular * material specular)	(let gx handle this, but add lspec*mspec to GXChanMatColor)
				//
				//now we could use 3 light to emulate 1 opengl light but that would not be helpfull
				//so maybe there is an other way also gx material misses color components
				//
				//gx has max to channels
				//each can be setup differently so we can have on chanel for normal diffuse
				//and the other for specular. But we only have on light color so unless the specular color equals light color this it not usefull)
				//maybe some experiments with GXChanMatColor help with that? So light color to none and all color CHANMatColor?
				//
				//also we have multiple tev stages.
				//as we have used 2 channels we have to use 3 stages
				//stage 1 = emissive + global ambient scaled by material as constant color (maybe 2 stages?)
				//stage 2 = ambient + diffuse
				//stage 3 = specular
				//
				//So this might do the trick in theory. Now on to practice...


				//<h0lyRS> did you try setting specular to channel GX_COLOR1A1, and diffuse to GX_COLOR0A0, so you can blend them anyway you want?
				//this could add an extra color?
				//one way is adding the specular light, other way is multiplying

				//expand this to create opengl lighting mode.

				//GX_SetChanCtrl(GX_COLOR0A0, TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);
				//GX_SetTevOp(GX_TEVSTAGE0,GX_MODULATE); //TODO: a call to gl_texture_2d should not undo this?
				//GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

				//making a lightmask (hmm there must be a more efficient way to do this, at least it should be somewhere else)
				//<h0lyRS>	you can simply shift GX_LIGHT0 << lightnum

				
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

				GX_SetNumChans(1); //use/enable one light (the first?)

				//channel 1 (ambient + diffuse)
				GX_SetChanCtrl(GX_COLOR0A0,GX_TRUE,GX_SRC_REG,GX_SRC_REG,gxlightmask,GX_DF_CLAMP,GX_AF_SPOT); //uses (texture)perpixel colors (light works)
				
				//channel 2 (specular)
				//GXColor white = { 0, 0, 255, 255 };
				//GX_SetChanMatColor(GX_COLOR1, white); // use white as test color
				//GX_SetChanCtrl(GX_COLOR1,GX_ENABLE,GX_SRC_REG,GX_SRC_REG,gxlightmask,GX_DF_NONE,GX_AF_SPEC); //uses (texture)perpixel colors (light works)


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


				//GX_COLOR0 = rgb
				//GX_COLOR0A0 = rgba

				//light setup?

				
				
				//GX_SetChanCtrl(GX_COLOR0A0, TRUE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_SIGNED, GX_AF_NONE); //write out what this does
																//uses vertex? then what is GX_SRC_REG

				//GXColor amb = { 128, 64, 64, 255 };
				//GXColor amb = { 100, 100, 100, 255 }; //glmaterialf ? 
				//GXColor mat = { 123, 123, 123, 123 }; //glmaterialf ?
				//GX_SetChanAmbColor(GX_COLOR0A0, amb); //glmaterialf ? or GLOBAL AMBIEN LIGHT glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb); ??
				//GX_SetChanMatColor(GX_COLOR0A0, gxcurrentmaterialdiffusecolor); 
				
				
				
				//How does this relate to light color? e.g. in opengl both light and material have diffuse and ambient component

				// Set up shader (write out what each step means)
				GX_SetNumTevStages(3); //each extra color takes another stage?
				
					
				//GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
				//GX_SetTevOrder(GX_TEVSTAGE0,GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				
    //GX_SetNumTexGens(1);
	//GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	// Set up shader
	//GX_SetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
	//GX_SetNumTevStages(1);
	//GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, GX_FALSE, GX_TEVPREV);
	//GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	//GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, GX_FALSE, GX_TEVPREV);
	//GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
	//GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	//GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				


                

				//stage 1 (global ambient light)
				
				//GX_SetTevClampMode(GX_TEVSTAGE0, GX_TC_EQ);

				//color
				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0); //shagkur method
				//GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0); 
				//GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_ONE, GX_CC_CPREV); //add constant color to color from previous stage
				GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0); //shagkur method
				//GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_ZERO, GX_CA_A0);
				//GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CC_ONE, GX_CA_APREV); //add constant alpha to alpha from previous stage
				GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//tevorder
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0); //?

				//end stage 1

				//stage 2 (global ambient light)
				
				//GX_SetTevClampMode(GX_TEVSTAGE1, GX_TC_EQ);

				//color
				GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1); //shagkur method
				//GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C1, GX_CC_ONE, GX_CC_CPREV); //add constant color to color from previous stage
				GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1); //shagkur method
				//GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_A1, GX_CC_ONE, GX_CA_APREV); //add constant alpha to alpha from previous stage
				GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//tevorder
				GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0); //?

				//end stage 2
				
				//stage 3
				
                /*
				GX_SetTevClampMode(GX_TEVSTAGE2, GX_TC_EQ);

				GX_SetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);

				//color
				GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//tevorder
				if (tex2denabled){                                
					//GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC); 
					//GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA); 

					GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_ONE, GX_CC_TEXC, GX_CC_RASC); //modulate
					GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA); //modulate

					GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0); //texturing
				}
				else { 
					GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO); //vertex color?
					GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_KONST, GX_CA_RASA, GX_CA_ZERO);
					GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0); //no texturing
				};				
				GX_SetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				*/
				
				//color
				GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
				GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CC_RASA); 
				GX_SetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//tevorder
				GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0); //?
				
				// end stage 3
				
				//putt textures in its own additional stage
				// stage 4 (textures)
				
				GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0); //texturing
				GX_SetTevOp(GX_TEVSTAGE3, GX_MODULATE);

                // end stage 4

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
				GX_SetNumTevStages(1);
				if (tex2denabled){
				  GX_SetNumTexGens(1); //multitexturing so set to 1 for now
				  GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
				  GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);                
                }
                else
                {
				 GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
				 GX_SetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
                }
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
