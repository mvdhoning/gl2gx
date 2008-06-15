#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include <ogcsys.h>
#include <gccore.h>

#include "glint.h"
#include "GL/gl.h"
#include "GL/glu.h"

/* glu */

void gluLookAt( GLdouble eyeX,
				GLdouble eyeY,
				GLdouble eyeZ,
				GLdouble centerX,
				GLdouble centerY,
				GLdouble centerZ,
				GLdouble upX,
				GLdouble upY,
				GLdouble upZ ) {

	Vector cam = {eyeX, eyeY, eyeZ},
			up = {upX, upY, upZ},
		  look = {centerX, centerY, centerZ};

	guLookAt(view, &cam, &up, &look);
}

void gluPerspective( GLdouble	fovy,
			       GLdouble	aspect,
			       GLdouble	zNear,
				   GLdouble	zFar ) {
	guPerspective(perspective, fovy, aspect, zNear, zFar);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
}
