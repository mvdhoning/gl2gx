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

void glTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
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

void glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
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

void glNormal3f( GLfloat x, GLfloat y, GLfloat z ){
	_tempnormalelement.x = x;
	_tempnormalelement.y = y;
	_tempnormalelement.z = z;
}

void glColor3f( GLfloat r, GLfloat g, GLfloat b ) {
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
        case GL_TRIANGLE_STRIP: _type = GX_TRIANGLESTRIP; break;        
		case GL_TRIANGLES: _type = GX_TRIANGLES; break;
		case GL_QUADS: _type = GX_QUADS; break;
		case GL_LINES: _type = GL_LINES; break;
	};
}


void UploadVertex(int index){
        int pos = index; 
/*            
        if (_type == GX_TRIANGLESTRIP) 
        {              
        if ( temp == 3)
        {
             reverse = 1;
        }; 
        
        
        
        if (reverse==1)
        {
           switch(temp)
           {
              case 3: pos = index + 1; break;
              case 4: pos = index + 1; break;
              case 5: pos = index - 2; break;
           }
        }
        }
*/          
                       
		GX_Position3f32( _vertexelements[pos].x, _vertexelements[pos].y, _vertexelements[pos].z);	
		
        GX_Normal3f32(_normalelements[pos].x, _normalelements[pos].y, _normalelements[pos].z);

		//when using GL_FLAT only one color is allowed!!! //GL_SMOOTH allows for an color to be specified at each vertex
		GX_Color3f32( _colorelements[pos].r, _colorelements[pos].g, _colorelements[pos].b); //glmaterialfv call instead when glcolormaterial call is used

		GX_TexCoord2f32(_texcoordelements[pos].s,_texcoordelements[pos].t);
/*		
		temp += 1;
		if (temp >= 6) {temp = 0; reverse = 0; };
*/
		
};

 void GX_TestInitSpecularDir(GXLightObj *lit_obj,f32 nx,f32 ny,f32 nz)
 {
     //f32 px, py, pz;
     f32 hx, hy, hz, mag;
 
     // Compute half-angle vector
     hx  = -nx;
     hy  = -ny;
     hz  = (-nz + 1.0f);
     mag = 1.0f / sqrtf((hx * hx) + (hy * hy) + (hz * hz));
     hx *= mag;
     hy *= mag;
     hz *= mag;
 
     //px  = -nx * BIG_NUMBER;
     //py  = -ny * BIG_NUMBER;
     //pz  = -nz * BIG_NUMBER;
 
 	//((f32*)lit_obj)[10] = px;
 	//((f32*)lit_obj)[11] = py;
 	//((f32*)lit_obj)[12] = pz;
 	((f32*)lit_obj)[13] = hx;
 	((f32*)lit_obj)[14] = hy;
 	((f32*)lit_obj)[15] = hz;
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
	GXColor gxchanambient;
	gxchanambient.r = gxcurrentmaterialambientcolor.r;
	gxchanambient.g = gxcurrentmaterialambientcolor.g;
	gxchanambient.b = gxcurrentmaterialambientcolor.b;
	gxchanambient.a = gxcurrentmaterialambientcolor.a;
	
	GXColor gxchanspecular;
	gxchanspecular.r = gxcurrentmaterialspecularcolor.r;
	gxchanspecular.g = gxcurrentmaterialspecularcolor.g;
	gxchanspecular.b = gxcurrentmaterialspecularcolor.b;
	gxchanspecular.a = gxcurrentmaterialspecularcolor.a;	
	
	int lightcounter = 0;
	for (lightcounter =0; lightcounter < 8; lightcounter++){

		if(gxlightenabled[lightcounter]){ //when light is enabled

            //somewhere here an error happens?

            //Setup mat/light ambient color 
			gxchanambient.r = ((gxchanambient.r * gxlightambientcolor[lightcounter].r) * 0xFF);
			gxchanambient.g = ((gxchanambient.g * gxlightambientcolor[lightcounter].g) * 0xFF);
			gxchanambient.b = ((gxchanambient.b * gxlightambientcolor[lightcounter].b) * 0xFF);
			gxchanambient.a = ((gxchanambient.a * gxlightambientcolor[lightcounter].a) * 0xFF);
			GX_SetChanAmbColor(GX_COLOR0A0, gxchanambient ); 
			
			//Setup diffuse material color
			GXColor mdc;
			mdc.r = (gxcurrentmaterialdiffusecolor.r * 0xFF);
			mdc.g = (gxcurrentmaterialdiffusecolor.g * 0xFF);
			mdc.b = (gxcurrentmaterialdiffusecolor.b * 0xFF);
			mdc.a = (gxcurrentmaterialdiffusecolor.a * 0xFF);
			GX_SetChanMatColor(GX_COLOR0A0, mdc ); 
			
			//Setup specular material color
			gxchanspecular.r = (gxchanspecular.r * gxlightspecularcolor[lightcounter].r)* 0xFF;
			gxchanspecular.g = (gxchanspecular.g * gxlightspecularcolor[lightcounter].g)* 0xFF;
			gxchanspecular.b = (gxchanspecular.b * gxlightspecularcolor[lightcounter].b)* 0xFF;
			gxchanspecular.a = (gxchanspecular.a * gxlightspecularcolor[lightcounter].a)* 0xFF;
			GX_SetChanMatColor(GX_COLOR1A1, gxchanspecular); // use red as test color

            //Setup light diffuse color
            GXColor ldc;
			ldc.r = gxlightdiffusecolor[lightcounter].r * 0xFF;
			ldc.g = gxlightdiffusecolor[lightcounter].g * 0xFF;
			ldc.b = gxlightdiffusecolor[lightcounter].b * 0xFF;
			ldc.a = gxlightdiffusecolor[lightcounter].a * 0xFF;
			GX_InitLightColor(&gxlight[lightcounter], ldc ); //move call to glend or init?;

			//Setup light postion
			
			//check on w component when 1. light is positional
			//                     when 0. light is directional at infinite pos
			
			Vector lpos;
			Vector wpos;
            lpos.x = gxlightpos[lightcounter].x;
            lpos.y = gxlightpos[lightcounter].y;
            lpos.z = gxlightpos[lightcounter].z;
               
               
            if (gxlightpos[lightcounter].w == 0){
                guVecNormalize(&lpos);
                lpos.x *= BIG_NUMBER;
                lpos.y *= BIG_NUMBER;
                lpos.z *= BIG_NUMBER;
            }
            
			guVecMultiply(view,&lpos,&wpos);	   //light position should be transformed by world-to-view matrix (thanks h0lyRS)
			GX_InitLightPosv(&gxlight[lightcounter], &wpos); //feed corrected coord to light pos
			
		


            //Setup light direction (when w is 1 dan dir = 0,0,0
            Vector ldir;
            if (gxlightpos[lightcounter].w==0){ 
               //lpos.x = gxlightpos[lightcounter].x;
			   //lpos.y = gxlightpos[lightcounter].y;
               //lpos.z = gxlightpos[lightcounter].z;
                                                
               ldir.x = gxlightpos[lightcounter].x;
               ldir.y = gxlightpos[lightcounter].y;
               ldir.z = gxlightpos[lightcounter].z;
            }
            else
            {
                if (gxspotcutoff[lightcounter] != 180){ //if we have a spot light direction is needed
                   ldir.x = gxspotdirection[lightcounter].x;
                   ldir.y = gxspotdirection[lightcounter].y;
                   ldir.z = gxspotdirection[lightcounter].z;
                }
                else { 
                     ldir.x = 0;
                     ldir.y = 0;
                     ldir.z = -1;
               }
            }
            
            guVecNormalize(&ldir);
            ldir.x *= BIG_NUMBER;
            ldir.y *= BIG_NUMBER;
            ldir.z *= BIG_NUMBER;
            
            guMtxInverse(view,mvi);
            guMtxTranspose(mvi,view);
            
            guVecMultiply(view,&ldir,&ldir); //and direction should be transformed by inv-transposed of world-to-view (thanks h0lyRS)
            
            GX_InitLightDir(&gxlight[lightcounter], ldir.x, ldir.y, ldir.z); //feed corrected coord to light dir
           
            
			if (gxspotcutoff[lightcounter] != 180){
               //Setup specular light (only for spotlight when GL_SPOT_CUTOFF <> 180)
			   //make this line optional? If on it disturbs diffuse light?
               Vector sdir;
               sdir.x = gxspotdirection[lightcounter].x;
               sdir.y = gxspotdirection[lightcounter].y;
               sdir.z = gxspotdirection[lightcounter].z;
               guVecNormalize(&sdir);
                     
               sdir.x *= BIG_NUMBER;
               sdir.y *= BIG_NUMBER;
               sdir.z *= BIG_NUMBER;       
                              
			   guVecMultiply(view,&sdir,&sdir);
			   
               GX_TestInitSpecularDir(&gxlight[lightcounter], sdir.x, sdir.y, sdir.z); //needed to enable specular light
               
            };
            
            GX_InitLightShininess(&gxlight[lightcounter], gxcurrentmaterialshininess ); // /180?

            //Setup distance attinuation (opengl vs gx differences?)
			//GX_InitLightDistAttn(&gxlight[lightcounter], 100.0f, gxspotexponent[lightcounter], GX_DA_GENTLE); //gxspotexponent was 0.5f
                                                     //ref_dist, bright, dist func  
            //k0 = 1.0;                   
            //k1 = 0.5f*(1.0f-ref_brite)/(ref_brite*ref_dist);
 			//k2 = 0.5f*(1.0f-ref_brite)/(ref_brite*ref_dist*ref_dist); or 0.0f;                  
                    
                    
                     
            //Attenuation factor = 1 / (kc + kl*d + kq*d2) 
            //kc = constant attenuation factor (default = 1.0) 
            //kl = linear attenuation factor (default = 0.0) 
            //kq = quadratic attenuation factor (default = 0.0) 
         
            float distance = BIG_NUMBER; //either distance of light or falloff factor
            float factor = 1 / (gxconstantattanuation[lightcounter] + gxlinearattanuation[lightcounter]*distance + gxquadraticattanuation[lightcounter]*distance*distance);                   
                                                     
            //GX_InitLightAttnK(&gxlight[lightcounter], 1.0f , factor ,0.0f);
            GX_InitLightDistAttn(&gxlight[lightcounter], -39.0F,40.0F, GX_DA_GENTLE); //gxspotexponent[lightcounter]
            //                                           factor / strenght
//1.0 is //    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10.0f); ??
                                                     
                                                           
            //Setup light type (normal/spotlight)
            GX_InitLightSpot(&gxlight[lightcounter], gxspotcutoff[lightcounter], GX_SP_COS2); //not this is not a spot light
                                                //cut_off, spot func
            //GX_InitLightSpot(&gxlight[lightcounter], 0.0f, GX_SP_OFF); //not this is not a spot light


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

	//now we can draw the gx way (experiment try render in reverse ivm normals pointing the wrong way)
	
	int countelements = _numelements*2;
	if (gxcullfaceanabled==true){
       countelements = _numelements;
    }
	
	GX_Begin(_type, GX_VTXFMT0, countelements); //dependend on culling setting
	int i =0;
                                //default
//order dependend on glFrontFace(GL_CCW); 
//or GL_CW //	for( i=0; i<_numelements; i++)

//GX_TRIANGLESTRIP   GL_TRIANGLE_STRIP
//0 1 2			     0 1 2
//1 3 2			     2 1 3
//2 3 4			     2 3 4
//better think of a clever swapping routine
//maybe then no need to invert normal for trianglestrip anymore

//also GX_TRIANLES need to be drawn in reverse?
//but GX_QUAD does not

//so GX = CW by default while opengl is CCW by default?

//bushing say cannot i be possibel that opengl reorders vertexes

//u32 reverse = 0;
//int pos = 0;
//int temp = 0;

//GL_POLYGON: http://www.gamedev.net/reference/articles/article425.asp

bool cw = true;
bool ccw = true;

if(gxcullfaceanabled==true){
   cw = false;
   ccw = false;                            
   switch(gxwinding){
      case GL_CW: cw = true; break;
      case GL_CCW: ccw = true; break;
   }                         
}

    if (cw==true){ 
       //CW     
       for( i=_numelements-1; i>=0; i--)
       {
            UploadVertex(i);    	
       }
    }
    
    if (ccw==true){
       //CCW
       for( i=0; i<_numelements; i++)
       {
            UploadVertex(i);    	
       }
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

void glLightf( GLenum light, GLenum pname, GLfloat param ){
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
	};
	
	switch(pname)
	{
		case GL_SPOT_CUTOFF: gxspotcutoff[lightNum] = param; break;
		case GL_SPOT_EXPONENT: gxspotexponent[lightNum] = param; break;
		case GL_CONSTANT_ATTENUATION: gxconstantattanuation[lightNum] = param; break;
		case GL_LINEAR_ATTENUATION: gxlinearattanuation[lightNum] = param; break;
		case GL_QUADRATIC_ATTENUATION: gxquadraticattanuation[lightNum] = param; break;
    };
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
	};

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
			gxlightpos[lightNum].w = params[3]; //leave W param untouched
			gxcurlight = lightNum;
			break;
		case GL_DIFFUSE:
			gxlightdiffusecolor[lightNum].r = params[0];
			gxlightdiffusecolor[lightNum].g = params[1];
			gxlightdiffusecolor[lightNum].b = params[2];
			gxlightdiffusecolor[lightNum].a = params[3];
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
		case GL_SPOT_DIRECTION:
             gxspotdirection[lightNum].x = params[0];
             gxspotdirection[lightNum].y = params[1];
             gxspotdirection[lightNum].z = params[2];
             gxspotdirection[lightNum].w = params[3];
             break;
	};
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


				//Setup lightmask with enabled lights (thanks h0lyRS)
				for (lightcounter =0; lightcounter < 8; lightcounter++){
					if(gxlightenabled[lightcounter]){ //when light is enabled
						gxlightmask |= (GX_LIGHT0 << lightcounter);
						countlights++;
					}
				};

                //Setup light system/channels
				GX_SetNumChans(2); //dependend on if there is a specular color/effect needed

				//channel 1 (ambient + diffuse)
				GX_SetChanCtrl(GX_COLOR0A0,GX_TRUE,GX_SRC_REG,GX_SRC_REG,gxlightmask,GX_DF_CLAMP,GX_AF_SPOT);
				
				//channel 2 (specular)
				GX_SetChanCtrl(GX_COLOR1A1,GX_ENABLE,GX_SRC_REG,GX_SRC_REG,gxlightmask,GX_DF_CLAMP,GX_AF_SPEC);
				
				
				//Setup TEV-shader
				
				//Setup the number of tev stages needed
				int numtevstages = 0;
				if (tex2denabled){ 
                  numtevstages = 5;
                } else {
                  numtevstages = 4;
                }
				GX_SetNumTevStages(numtevstages); //each extra color takes another stage?
				
				
				//stage 1 (global ambient light)
				
				//color
				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0); //shagkur method
				GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0); //shagkur method
				GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//tevorder
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0); //only use color

				//end stage 1


				//stage 2 (global ambient light)
				
				//color
				GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1); //shagkur method
				GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1); //shagkur method
				GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//tevorder
				GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0); //only use color

				//end stage 2
				
				
				//stage 3 (ambient and diffuse light from material and lights)

				//color
				GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
				GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//alpha
				GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CC_RASA); 
				GX_SetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				
				//tevorder
				GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0); //only use color
				
				// end stage 3
				
				//stage 4 (specular light)
				

		// color - blend
		GX_SetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		GX_SetTevColorIn(GX_TEVSTAGE3, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
//		GX_SetTevColorIn(GX_TEVSTAGE3, GX_CC_CPREV, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO); //shagkur method
		
		// alpha - nop
		GX_SetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		GX_SetTevAlphaIn(GX_TEVSTAGE3, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA); //shagkur method
//        GX_SetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO); //shagkur method

		GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR1A1);
			
				// end stage 4
				
				if (tex2denabled){ 
                                   
                    // stage 5 (textures)
				
				    GX_SetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0); //use texture
				    GX_SetTevOp(GX_TEVSTAGE4, GX_MODULATE); //blend with previous stage

                    // end stage 5

                }   

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
			case GL_CULL_FACE: gxcullfaceanabled=true; break;
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
			case GL_CULL_FACE: gxcullfaceanabled=false; break;
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

/* Culling / Winding */

void glFrontFace( GLenum mode ){
		gxwinding = mode;
};

/* end */
