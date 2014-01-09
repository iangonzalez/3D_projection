/*
 
 Routines defined:
 
 void ploton(int height,int width)	Open a window of specified size (in pixels)
 void scalx(float xmin,float xmax)	Scale window's x axis
 void scaly(float ymin,float ymax)	Scale window's y axis
 void set(float x,float y)			Place the pen at location x,y
 void dvect(float x,float y)		Draw a line to location x,y, leave the pen at the new point
 void cross(float x,float y)		Draw a cross at location x,y
 void clear()						Erase everything in the window
 void delay(int hunsecs)			Delay for the specified number of hundredths of seconds
 void reset()						Close the window
 
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

// Definitions for some data structures used in the routines

class vertex;

// This is a polygon structure 
// An polygon is defined by the vertices that bound it
// An vertex is specified by its index in the main vertex array
// We will consider polygons with 10 or fewer points
typedef struct {
    int numVert;     // number of vertices in this polygon (<=10)
    int vertexList[10];  // array containing the indices of the vertices that make up this polygon
    int color;  // the color of this polygon (0-3)
} poly;

// these are the prototypes for the functions defined in graph_final.c
int ploton(int,int);
void scalx(float,float), scaly(float,float), set(float,float);		
void dvect(float,float),cross(float,float), reset(void), clear(void), delay(int);

void draw_rect(float x, float y, float width, float height, int fc, float intensity);
int getmouse(float*,float*,int*);
void draw_poly(vertex* polygon,int num, int fc, float intensity);
int get_axis(int *axis, float *dir);
