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
 
 void draw_poly(vertex* polygon, int fc, float intensity)    
    Draw a polygon with color fc and specified intensity
	fc is 0-3 for grey, blue, red, green, intensity is between 0 - 1

 void draw_rect(float x, float y, float width, float height, int fc, float intensity)
	Draw a rectangle from x,y to x+width,y+height
	fc is 0-3 for grey, blue, red, green, intensity is between 0 - 1
 
 int getmouse(float *x, float *y, int *drag)	
 	Gets the x-y coordinates of a mouse click. 
	Scaling is as per scalx and scaly calls.
	drag is 0 for a mouse click, 1 for mouse drag
	Return value is 0 if the right button is pressed, 1 otherwise.
	
 int not_right_click();	
 	Returns 1 if no right click has been detected, 0 if there has been a right click
 	
*/

// Definitions for some data structures used in the routines

// Use this structure to define points in 3D
typedef struct {
	float x;
	float y;
	float z;
} vertex;

// This is an edge structure 
// An edge is defined by the verticies that it connects
// A vertex is specified by it's index in the main vertex array
typedef struct {
	int v1;
	int v2;
} edge;


// This is a polygon structure 
// An polygon is defined by the vertices that bound it
// An vertex is specified by its index in the main vertex array
// We will consider only polygons with 4 points
typedef struct {
	int p1;
	int p2;
	int p3;
	int p4;
} poly;

// these are the prototypes for the functions defined in graph_life.c
int ploton(int,int);
void scalx(float,float), scaly(float,float), set(float,float);		
void dvect(float,float),cross(float,float), reset(void), clear(void), delay(int);

void draw_rect(float x, float y, float width, float height, int fc, float intensity);
int getmouse(float*,float*,int*);
int not_right_click(void);	// return 1 if no right click has occurred

void draw_poly(vertex* polygon, int fc, float intensity);

void set_color(void);
void beep(void);
void initX(void);
int openwindow(int swidth,int sheight);
void getcolors(void);

