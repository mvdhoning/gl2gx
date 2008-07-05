#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include <ogcsys.h>
#include <gccore.h>

#include "glint.h"
#include "GL/gl.h"
#include "GL/glut.h"

#define DEFAULT_FIFO_SIZE	(256*1024)
 
static void *frameBuffer[2] = { NULL, NULL};
GXRModeObj *rmode;

u32	fb = 0; 	// initial framebuffer index

/* glut */

void glutInit(int *argcp, char **argv) {

	// init the vi.
	VIDEO_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);
	
	//initialize gl default values
	glColor3f(1.0,1.0,1.0);
	glTexCoord2f(0.0f, 0.0f);
	glNormal3f(0.0f,0.0f,0.0f);
	
	int i = 0;
	for (i=0;i<8;i++){
        gxlightenabled[i] = false;
        gxlightambientcolor[i].r=0;
        gxlightambientcolor[i].g=0;
        gxlightambientcolor[i].b=0;
        gxlightambientcolor[i].a=1;
        gxlightdiffusecolor[i].r=0;
        gxlightdiffusecolor[i].g=0;
        gxlightdiffusecolor[i].b=0;
        gxlightdiffusecolor[i].a=0;
        gxlightspecularcolor[i].r=0;
        gxlightspecularcolor[i].g=0;
        gxlightspecularcolor[i].b=0;
        gxlightspecularcolor[i].a=0;
        
        gxspotcutoff[i] = 180;
        gxspotexponent[i] = 0;
        gxspotdirection[i].x = 0.0;
        gxspotdirection[i].y = 0.0;
        gxspotdirection[i].z = -1.0;
        gxspotdirection[i].w = 1.0;       
    }
    
    gxlightdiffusecolor[0].r=1;
    gxlightdiffusecolor[0].g=1;
    gxlightdiffusecolor[0].b=1;
    gxlightdiffusecolor[0].a=1;
        
    gxlightspecularcolor[0].r=1;
    gxlightspecularcolor[0].g=1;
    gxlightspecularcolor[0].b=1;
    gxlightspecularcolor[0].a=1;
    
gxglobalambientlightcolor.r = 0.2;
gxglobalambientlightcolor.g = 0.2;
gxglobalambientlightcolor.b = 0.2;
gxglobalambientlightcolor.a = 1.0;

//material specs
gxcurrentmaterialemissivecolor.r = 0.0;
gxcurrentmaterialemissivecolor.g = 0.0;
gxcurrentmaterialemissivecolor.b = 0.0;
gxcurrentmaterialemissivecolor.a = 1.0;

gxcurrentmaterialambientcolor.r = 0.2;
gxcurrentmaterialambientcolor.g = 0.2;
gxcurrentmaterialambientcolor.b = 0.2;
gxcurrentmaterialambientcolor.a = 1.0;

gxcurrentmaterialdiffusecolor.r = 0.8;
gxcurrentmaterialdiffusecolor.g = 0.8;
gxcurrentmaterialdiffusecolor.b = 0.8;
gxcurrentmaterialdiffusecolor.a = 1.0;

gxcurrentmaterialspecularcolor.r = 0.0;
gxcurrentmaterialspecularcolor.g = 0.0;
gxcurrentmaterialspecularcolor.b = 0.0;
gxcurrentmaterialspecularcolor.a = 1.0;

gxcurrentmaterialshininess = 0.0;


    //texture is disabled by default
    tex2denabled = false;
    
    //setup culling
    gxcullfaceanabled=false; //disable
    gxwinding = GL_CCW;       //counterclockwise

	//VIDEO_ClearFrameBuffer(GXRModeObj *rmode,void *fb,u32 color);?


}

void glutInitDisplayMode(unsigned int mode) {
// allocate 2 framebuffers for double buffering
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	f32 yscale;

	u32 xfbHeight;

	// setup the fifo and then init the flipper
	void *gp_fifo = NULL;
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);
 
	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
  
	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
 
	GX_SetCullMode(GX_CULL_ALL);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
 

	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
 	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
 
	// setup the vertex attribute table
	// describes the data
	// args: vat location 0-7, type of data, data format, size, scale
	// so for ex. in the first call we are sending position data with
	// 3 values X,Y,Z of size F32. scale sets the number of fractional
	// bits for non float data.
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);		//vertex
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);		//normals
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);	//color
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); //texture
	
	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use 
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetNumChans(1); //	<RedShade>	GX_SetNumChans(1); = turn on 1 channel of lighting

	GX_SetNumTexGens(1); //multitexturing so set to 1 for now
	GX_InvalidateTexAll(); //now be carefull as this clears all texture memory could get unpredicted results on second call

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);			
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

}

int glutCreateWindow(const char *title) {
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	gluLookAt(0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 1.0F, 0.0F); //setup opengl compat coord system
	return 0;
}

void glutSwapBuffers(void) {
		fb ^= 1;		// flip framebuffer				//swapbuffers(DC);
		GX_SetZMode(depthtestenabled, depthfunction, depthtestenabled);		//
		GX_SetColorUpdate(GX_TRUE);						//
		GX_CopyDisp(frameBuffer[fb],GX_TRUE);			//
														//
		VIDEO_SetNextFramebuffer(frameBuffer[fb]);		//
														//
		VIDEO_Flush();									//
														// 
		VIDEO_WaitVSync();								//	
}

void glutInitWindowSize(int width, int height) {
		GX_SetViewport(0,0,width,height,0,1);
}

int glutGet(GLenum type) {
	int retval = 0;
	switch(type)
	{
	case GLUT_SCREEN_WIDTH: retval = rmode->viWidth; break;
	case GLUT_SCREEN_HEIGHT: retval = rmode->viHeight; break;
	}
	return retval;
}

/* shapes */
static void
drawBox(GLfloat size, GLenum type)
{
  static GLfloat n[6][3] =
  {
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0}
  };
  static GLint faces[6][4] =
  {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };
  GLfloat v[8][3];
  GLint i;

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

  for (i = 5; i >= 0; i--) {
    glBegin(type);
    glNormal3f(n[i][0],n[i][1],n[i][2]);
    glVertex3f(v[faces[i][0]][0],v[faces[i][0]][1],v[faces[i][0]][2]);
    glVertex3f(v[faces[i][1]][0],v[faces[i][1]][1],v[faces[i][1]][2]);
    glVertex3f(v[faces[i][2]][0],v[faces[i][2]][1],v[faces[i][2]][2]);
    glVertex3f(v[faces[i][3]][0],v[faces[i][3]][1],v[faces[i][3]][2]);
    glEnd();
  }
}

void glutWireCube(GLdouble size)
{
  drawBox(size, GL_LINE_LOOP);
}

void glutSolidCube(GLdouble size)
{
  drawBox(size, GL_QUADS);
}

void (*displayfunc)(void);

void glutDisplayFunc(void (*func)(void)){
	displayfunc = func;
}

void (*idlefunc)(void);

void glutIdleFunc(void (*func)(void)){
	idlefunc = func;
}

void glutMainLoop(void){
     
     displayfunc();
	 while(1) {
              idlefunc();
	 }
}
