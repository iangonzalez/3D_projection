#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "graph_final.h"

/*
 The routines defined here are C-callable (only).
 All arguments are VALUES rather than addresses.
 Routines defined:
 
 void ploton(int height,int width)	Open a window of specified size (in pixels)
 void scalx(float xmin,float xmax)	Scale window's x axis
 void scaly(float ymin,float ymax)	Scale window's y axis
 void set(float x,float y)		Place the pen at location x,y
 void dvect(float x,float y)		Draw a line to location x,y, leave the pen at the new point
 void cross(float x,float y)		Draw a cross at location x,y
 void clear()				Erase everything in the window
 void delay(int hunsecs)			Delay for the specified number of hundredths of seconds
 void reset()				Close the window

 void draw_poly(vertex* polygon, int num, int fc, float intensity)    
    Draw a polygon 
    polygon is an array containing num vertices that make up the polygon
    num is between 3 and 10
    fc specifies the color with 0-3 for grey, blue, red, green; fc = -1 draws polygon without fill
    intensity is between 0 - 1
 
 int getmouse(float *x, float *y, int *notdone)	
    Sets the x-y coordinates of a mouse click. 
    Scaling is as per scalx and scaly calls.
    notdone is 1 until there is a click or drag in the lower left corner of the window
    Return value is 0 if the right button is pressed, 1 otherwise.
 
 int get_axis(int *axis, float *dir)
	this function responds to mouse movement and provides information on how the object should be rotated
	axis returns 1 or 2 (rotation about x or y axis)
	dir returns 1 or -1, indicating the direction of rotation
	return value of notdone = 1 except for click or drag in the lower left corner of the window

 
 */

#define  BORDER_WIDTH 3

/* Global parameters for X related stuff */

Display *theDisplay;    /* which display */
Window theWindow;
GC      theGC;

int     theScreen;              /* which screen on display */
int     theDepth;               /* number of color planes */
int     ymaxim;
int ww,wh;
char c;

unsigned long theBlackPixel, theWhitePixel;

Visual *GetVisual();

/* globals for the graphing functions */

int orgx = 20;
int orgy = 20;
int x_inited = 0;

float   xmin,xmax,ymin,ymax,xscalerange,scalex,yscalerange,scaley;
int     ix,iy;


int ploton(int height,int width)

{
    int openwindow(int,int);
    
    if (height < 10) height = 10;
    if (width < 10) width = 10;
    
    scalex = width;
    scaley = height;
    ymaxim = height;
    
    if(!openwindow(width,height))
	return(0);
    //XSelectInput(theDisplay, theWindow, ButtonPressMask|KeyPressMask);
    XSelectInput(theDisplay, theWindow, ButtonPressMask|Button1MotionMask|Button3MotionMask);
    return(1);
}

void reset()
{
    
    XFlush(theDisplay);
    XCloseDisplay( theDisplay);
}



void scalx(float x,float y)
{
    xmin = x;
    xmax = y;
    xscalerange = scalex/(xmax-xmin);
}

void scaly(float x,float y)
{
    ymin = x;
    ymax = y;
    yscalerange = scaley/(ymax-ymin);
}


void set(float x,float y)
{
    ix = (int)((x-xmin)*xscalerange);
    iy = (int)((y-ymin)*yscalerange);
    iy = ymaxim - iy;
}

void dvect(float x,float y)
{
    
    int ix0,iy0;
    
    ix0 = ix;
    iy0 = iy;
    
    ix = (int)((x-xmin)*xscalerange);
    iy = (int)((y-ymin)*yscalerange);
    iy = ymaxim - iy;
    
    XDrawLine(theDisplay, theWindow, theGC, ix0, iy0, ix,iy);
    XFlush(theDisplay);
    
    
}

#define LX 4
void cross(float x,float y)
{
    ix = (int)((x-xmin)*xscalerange);
    iy = (int)((y-ymin)*yscalerange);
    iy = ymaxim - iy;
    
    XDrawLine(theDisplay, theWindow, theGC, ix-LX, iy-LX, ix+LX,iy+LX);
    XDrawLine(theDisplay, theWindow, theGC, ix-LX, iy+LX, ix+LX,iy-LX);
    XFlush(theDisplay);
    
}

/* _______________________ X related stuff ________________________*/
#define NUM_COLOR 4
#define SHADES 256
unsigned long colors[NUM_COLOR][SHADES];
int shade = SHADES-1;
unsigned long black, white,fore,back;


void getcolors()
{
    XColor rgb[SHADES];
    int colorindex,i;
    unsigned long camb = 256*75;     // start the shades here -- provides ambient lighting
    
    black = fore = BlackPixel( theDisplay,theScreen );
    white = back = WhitePixel( theDisplay,theScreen );
    
    // Do grayscale
    colorindex = 0;
    for(i=0; i<SHADES; i++){
	rgb[i].red = rgb[i].green = rgb[i].blue =camb+(256*255-camb)/SHADES*(i+1);
	rgb[i].pixel = i;
	rgb[i].flags = DoRed|DoGreen|DoBlue;
	XAllocColor( theDisplay, DefaultColormap(theDisplay, theScreen),
		    &rgb[i]);
	colors[colorindex][i] = rgb[i].pixel;
    }
    // Do Blue
    colorindex=1;
    for(i=0; i<SHADES; i++){
	rgb[i].red = rgb[i].green =255;
	rgb[i].blue =camb+(256*255-camb)/SHADES*(i+1);
	rgb[i].pixel = i;
	rgb[i].flags = DoRed|DoGreen|DoBlue;
	XAllocColor( theDisplay, DefaultColormap(theDisplay, theScreen),
		    &rgb[i]);
	colors[colorindex][i] = rgb[i].pixel;
    }
    // Do Red
    colorindex=2;
    for(i=0; i<SHADES; i++){
	rgb[i].red =camb+(256*255-camb)/SHADES*(i+1);
	rgb[i].green = rgb[i].blue = 255;
	rgb[i].pixel = i;
	rgb[i].flags = DoRed|DoGreen|DoBlue;
	XAllocColor( theDisplay, DefaultColormap(theDisplay, theScreen),
		    &rgb[i]);
	colors[colorindex][i] = rgb[i].pixel;
    }
    // Do Green
    colorindex=3;
    for(i=0; i<SHADES; i++){
	rgb[i].green=camb+(256*255-camb)/SHADES*(i+1);
	rgb[i].red  = rgb[i].blue = 255;
	rgb[i].pixel = i;
	rgb[i].flags = DoRed|DoGreen|DoBlue;
	XAllocColor( theDisplay, DefaultColormap(theDisplay, theScreen),
		    &rgb[i]);
	colors[colorindex][i] = rgb[i].pixel;
    }
    
}

int openwindow(int swidth,int sheight)

{
    
    static int firsttime = 1;
    
    void initX(),beep();
    
    int createGC(Window,GC*);
    
    XSetWindowAttributes    theWindowAttributes;
    XSizeHints              theSizeHints;
    unsigned        long    theWindowMask;
    
    if(firsttime) {
	firsttime = 0;
	initX();
    }
    ww = swidth;
    wh = sheight;
    
    if(swidth <= 0) swidth = 10;
    if(sheight <= 0) sheight = 10;
    
    theWindowMask = CWBackPixel | CWBorderPixel | CWOverrideRedirect;
    
    theWindowAttributes.border_pixel = theBlackPixel
    = BlackPixel( theDisplay,theScreen );
    theWindowAttributes.background_pixel = theWhitePixel
    = WhitePixel( theDisplay,theScreen );
    theWindowAttributes.override_redirect = False;
    
    
    theWindow = XCreateWindow(theDisplay,
			      RootWindow(theDisplay,theScreen),
			      orgx,orgy,
			      swidth,sheight,
			      BORDER_WIDTH,
			      theDepth,
			      InputOutput,
			      CopyFromParent,
			      theWindowMask,
			      &theWindowAttributes);
    
    theSizeHints.flags      =       PPosition | PSize;
    theSizeHints.x          =       orgx;
    theSizeHints.y          =       orgy;
    theSizeHints.width      =       swidth;
    theSizeHints.height     =       sheight;
    
    XSetNormalHints( theDisplay, theWindow, &theSizeHints );
    
    if (createGC(theWindow, &theGC) == 0 ) {
	XDestroyWindow(theDisplay, theWindow);
	beep();
	return(0);
    }
    
    XMapWindow( theDisplay, theWindow );
    
    XFlush( theDisplay );
    getcolors();
    return( 1 );
}

void initX()
{
    theDisplay = XOpenDisplay(NULL);
    
    if( theDisplay == NULL) {
	fprintf( stderr, "Can't esatblish connection to server %s.\n"
		,XDisplayName(NULL) );
	//exit(1);
    }
    
    theScreen = DefaultScreen( theDisplay );
    theDepth = DisplayPlanes(theDisplay,theScreen);
    x_inited = 1;
    
}

void beep()
{
    if(x_inited)
	XBell(theDisplay,100);
    
}

int createGC(Window theNewWindow,GC* theNewGC )
{
    XGCValues       theGCValues;
    *theNewGC = XCreateGC( theDisplay, theNewWindow,
			  (unsigned long) 0, &theGCValues);
    
    if (*theNewGC == 0) {
	return(0);
    } else {
	XSetForeground( theDisplay, *theNewGC, theBlackPixel);
	XSetBackground( theDisplay, *theNewGC, theWhitePixel);
	return(1);
    }
    
}

void clear()
{
    XSetForeground( theDisplay, theGC, theWhitePixel);	
    XFillRectangle(theDisplay,theWindow,theGC,0,0,ww,wh);
    XSetForeground( theDisplay, theGC,theBlackPixel);
    XFlush(theDisplay);
}

void delay(int hunsecs)
{
    clock_t start;
    start = clock();
    while ( (clock()- start)*100/CLOCKS_PER_SEC < hunsecs);
}


void set_color()
{
    
    XSetFunction(theDisplay, theGC,GXcopy);
    XSetForeground( theDisplay, theGC,fore);
    XSetBackground( theDisplay, theGC,white);
    
    
}

XPoint screen_coord(vertex p)
{
    XPoint sp;
    float x,y;
    
    // scale the x and y components of a vertex to screen
    
    sp.x = (int)((p.x-xmin)*xscalerange);
    sp.y = (int)((p.y-ymin)*yscalerange);
    sp.y = ymaxim - sp.y;
    
    return(sp);
}

void draw_poly(vertex* polygon,int num, int fc, float intensity)
{
    int j;
    XPoint ppoly[11];
    
    for(j=0;j<num;j++){
        ppoly[j] = screen_coord(polygon[j]);
    }
    ppoly[num] = ppoly[0];

    if (fc >= 0) {
        shade = (int)(intensity*SHADES);
        if(shade>SHADES-1)shade=SHADES-1;   // shouldn't happen
        fore = colors[fc][shade];
        set_color();
        XFillPolygon(theDisplay, theWindow, theGC,ppoly,num, Convex, CoordModeOrigin);
        //draw the borders of this polygon in black
    }
    fore = black;
    set_color();
    XDrawLines(theDisplay, theWindow, theGC,ppoly,num+1, 0);
    XFlush(theDisplay);
    
}

void draw_rect(float x, float y, float width, float height, int fc, float intensity)
{
    
    XPoint ppoly[5];
    vertex polygon[4];
    
    polygon[0].x = x;
    polygon[0].y = y;
    
    polygon[1].x = x+width;
    polygon[1].y = y;
    
    polygon[2].x = x+width;
    polygon[2].y = y+height;
    
    polygon[3].x = x;
    polygon[3].y = y+height;
    
    shade = (int)(intensity*SHADES);
    if(shade>SHADES-1)shade=SHADES-1;   // shouldn't happen
    fore = colors[fc][shade];
    set_color();
    ppoly[0] = screen_coord(polygon[0]);
    ppoly[1] = screen_coord(polygon[1]);
    ppoly[2] = screen_coord(polygon[2]);
    ppoly[3] = screen_coord(polygon[3]);
    ppoly[4] = ppoly[0];
    XFillPolygon(theDisplay, theWindow, theGC,ppoly,4, Convex, CoordModeOrigin);
    XFlush(theDisplay);
    
    
}


int getmouse(float *x, float *y, int *notdone)
{
    
    static int thebut;
    XEvent theEvent;
    if( x_inited == 0){
        beep();
        return 0;
    }
    XNextEvent( theDisplay, &theEvent); 
    switch(theEvent.type){
            
        case ButtonPress:   /* A mouse button has been pressed... */
            *x = theEvent.xbutton.x/xscalerange + xmin;
            *y = (ymaxim-theEvent.xbutton.y)/yscalerange + ymin;
            
            
            
            if( (*x > xmin + (xmax-xmin)*.1) ||
                (*y > ymin + (ymax-ymin)*.1)) *notdone = 1;
            else *notdone = 0;
            
            if( theEvent.xbutton.button == Button3 ){
                thebut = 3;
                return 0;
            }
            thebut = 1;
            //printf("mouse at %d %d\n",theEvent.xbutton.x,theEvent.xbutton.y);
            return 1;
            break;
            
        case ButtonRelease: /* A mouse button has been released... */
            
            break;
            
        case MotionNotify: /* Pointer in motion */
            *x = theEvent.xbutton.x/xscalerange + xmin;
            *y = (ymaxim-theEvent.xbutton.y)/yscalerange + ymin;

            if( (*x > xmin + (xmax-xmin)*.1) ||
               (*y > ymin + (ymax-ymin)*.1)) *notdone = 1;
            else *notdone = 0;

            //printf("mouse at %.2f %.2f %.2f %.2f%d\n",*x,*y,xmin,xmax,*notdone);
            if( thebut==3 ) return 0;
            return 1;
            
            break;
            
    }
    return 0;
}

// this function responds to mouse movement and provides information on how the object should be rotated
// axis returns 1 or 2 (rotation about x or y axis)
// dir returns 1 or -1, indicating the direction of rotation
// return value of notdone = 1 except for click or drag in the lower left corner of the window
int get_axis(int *axis, float *dir)
{   
    float x0,x1,y0,y1;
    int notdone;
    
    getmouse(&x0,&y0,&notdone);
    if (!notdone) return notdone;
    getmouse(&x1,&y1,&notdone);
    if (fabs(x1-x0) > fabs(y1-y0)) {
        *axis=2;
        if (x1>x0) {
            *dir = 1.;
        } else {
            *dir = -1;
        }
    }else {
        *axis=1;
        if (y1>y0) {
            *dir = -1.;
        } else {
            *dir = 1;
        }
    }
    return notdone;
}

/*
int not_right_click()
{
    int i;
    XEvent theEvent;
    if( x_inited == 0){
	beep();
	return 0;
    }
    
    //if(XCheckWindowEvent(theDisplay, theWindow, ButtonPressMask,&theEvent));
    
    
    if( XCheckMaskEvent( theDisplay, ButtonPressMask,&theEvent)){
	
	//XNextEvent( theDisplay, &theEvent); 
	switch(theEvent.type){
		
	    case ButtonPress:   // A mouse button has been pressed... 
		if( theEvent.xbutton.button == Button3 )
		    return 0;
		//printf("mouse at %d %d\n",theEvent.xbutton.x,theEvent.xbutton.y);
		return 1;
		break;
		
	    case ButtonRelease: // A mouse button has been released... 
		return 1;
		break;
		
	    case MotionNotify: // Pointer in motion 
		return 1;
		
		break;
		
	}
    } else
	return 1;
    
}    
*/
/*
 char getkey()
 {
 XEvent theEvent;
 KeySym theKeySym;
 char	keybuf[128];
 XComposeStatus cstat;
 
 if( x_inited == 0){
 beep();
 return 0;
 }
 XNextEvent( theDisplay, &theEvent); 
 //beep();
 switch(theEvent.type){
 
 case KeyPress:   // 
 if(theEvent.type == KeyPress){
 XLookupString(&theEvent,keybuf,128,&theKeySym,&cstat);
 //beep();
 return theKeySym;
 }
 
 
 break;
 
 }
 
 }
 */
