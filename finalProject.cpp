#include <iostream>
#include <fstream>
#include "final.h"
#include <math.h>
using namespace std;

const float PI = 2.0*asin(1.0);

// Use this class to define points in 3D and do some basic operations
// None of the functions modify the calling object
class vertex{	
private:
    float x;
    float y;
    float z;
public:
    vertex();
    vertex(float,float,float); // set x, y, and z
    vertex operator-(vertex);	// subtract vertices
    vertex operator+(vertex);    // add vertices
    vertex operator*(float);	// the * operator -- multiply by a scalar
    float operator*(vertex);	// overload the * operator to define a dot product
    vertex operator%(vertex);	// the cross product operator
    
    vertex project(vertex cop, float z0); // do projection with this cop and z0
    vertex rotz(float);		// rotate by given amount about the z axis
    vertex rotx( float);		// rotate by given amount about the x axis
    vertex roty(float);		// rotate by given amount about the y axis
    vertex unit();			// return a unit vector 
    
    friend class solid;	    // give the solid class access to our coordinates
};

vertex::vertex()
{
	x = 0;
	y = 0;
	z = 0;	
}

vertex::vertex(float a, float b, float c)
{
	x = a;
	y = b;
	z = c;
}

vertex vertex::operator-(vertex a)
{
	vertex result (x - a.x,y - a.y,z - a.z);
	return result;	
}

vertex vertex::operator+(vertex a)
{
	vertex result (x + a.x, y + a.y, z + a.z);
	return result;	
}

vertex vertex::operator*(float scalar)
{
	vertex result (scalar*x,scalar*y,scalar*z);
	return result;
}

float vertex::operator*(vertex a)
{
	return (x*a.x + y*a.y + z*a.z); 	
}

vertex vertex::operator%(vertex a)
{
	vertex result (0,0,0);
	result.x = y*a.z - z*a.y;
	result.y = z*a.x - x*a.z;
	result.z = x*a.y - y*a.x;
	return result;
}

vertex vertex::project(vertex cop, float z0)
{
	vertex result (x,y,z);
	return result+((result-cop)*((z0-z)/(z-cop.z)));
}

vertex vertex::rotz(float theta)
{
	vertex result ((x*cos(theta)-y*sin(theta)),(x*sin(theta)+y*cos(theta)),z);
	return result;
}

vertex vertex::rotx(float theta)
{
	vertex result (x,(y*cos(theta)-z*sin(theta)),(y*sin(theta)+z*cos(theta)));
	return result;
}

vertex vertex::roty(float theta)
{
	vertex result ((x*cos(theta)+z*sin(theta)),y,(-x*sin(theta)+z*cos(theta)));
	return result;
}

vertex vertex::unit()
{
	float magn = (x*x + y*y + z*z);
	vertex result (x/(pow(magn,0.5)),y/(pow(magn,0.5)),z/(pow(magn,0.5)));
	return result;
}

class solid{
private:
    vertex *points;         // an array of points that define the object
    vertex cop;             // the center of projection
    float z0;               // specifies where the projection plane cuts the z axis
    poly *polys;            // an array of polygon data structures (see definition in final.h)
    int npts;               // number of points
    int npoly;              // number of polygons
public:
    solid();                // constructor, set things to zero
    void readin(char* filename);    // read in point and polygon data from the specified file; initialize the COP
    void projectAndPlot();    // project data points,  initialize a window (if needed), plot the result
                                    
    void rotate(int axis,float angle);	// rotate all points in the object by the specified angle (in radians)
                                        // axis = 1, 2, or 3 for x, y, or z axis
    vertex polyNorm(int pnum);         // return a normal to polygon pnum
    
};

solid::solid()
{
	points = 0;
	cop = vertex(0,0,0);
	z0 = 0;
	polys = 0;
	npts = 0;
	npoly = 0;	
}

void solid::readin(char* filename)
{
	ifstream infile;
	infile.open(filename);	// open the file
	int i,j;
	infile >> npts;
	points = new vertex[npts];
	for(i=0;i<npts;i++)
		infile >> points[i].x >> points[i].y >> points[i].z;
	infile >> npoly;
	polys = new poly[npoly];
	for(i=0;i<npoly;i++){
		infile >> polys[i].numVert;
		for(j=0;j<polys[i].numVert;j++)
			infile >> polys[i].vertexList[j];
	}
	for(i=0;i<npoly;i++)
		infile >> polys[i].color;
	float zmax=points[0].z;
	for(i=0;i<npoly;i++){
		if (points[i].z>zmax)
			zmax = points[i].z;
	}
	cop = vertex(0,0,8*zmax);
	z0 = -1*cop.z;
}

void solid::projectAndPlot()
{
	static int first_call=1;
	int i,j;
	if(first_call){//opening, scaling window on first call
		float xmax,xmin,ymax,ymin;
		xmax = points[polys[0].vertexList[0]].x;
		xmin = points[polys[0].vertexList[0]].x;
		ymax = points[polys[0].vertexList[0]].y;
		ymin = points[polys[0].vertexList[0]].y;
		for(i=0;i<npts;i++){
			if (points[i].x>xmax) 
				xmax = points[i].x;
			if (points[i].x<xmin) 
				xmin = points[i].x;
			if (points[i].y>ymax) 
				ymax = points[i].y;
			if (points[i].y<ymin) 
				ymin = points[i].y;
		}
		ploton(500,500);
		scalx(xmin-2*(xmax-xmin),xmax+2*(xmax-xmin));
		scaly(ymin-2*(ymax-ymin),ymax+2*(ymax-ymin));
		first_call--;
	}
	for(i=0;i<npoly;i++){//polygon projection, shading, drawing
		if(polyNorm(i).unit()*cop.unit() <= 0.1){
			continue;
		}
		vertex polygon[polys[i].numVert];
		for(j=0;j<polys[i].numVert;j++){
			polygon[j] = points[polys[i].vertexList[j]].project(cop,z0);
		}
		float intensity;
		intensity = polyNorm(i).unit()*cop.unit();
		draw_poly(polygon, polys[i].numVert, polys[i].color, intensity);
	}
}

void solid::rotate(int axis, float angle)
{
	int i,j;
	if(axis==1){
		for(i=0;i<npts;i++){
			points[i] = points[i].rotx(angle);
		}
	}
	if(axis==2){
		for(i=0;i<npts;i++){
			points[i] = points[i].roty(angle);
		}
	}
	if(axis==3){
		for(i=0;i<npts;i++){
			points[i] = points[i].rotz(angle);
		}
	}
}

vertex solid::polyNorm(int pnum)
{
	return ((points[polys[pnum].vertexList[0]]-points[polys[pnum].vertexList[1]])
		%(points[polys[pnum].vertexList[2]]-points[polys[pnum].vertexList[1]]));
}



int main()
{
    char filename[255];
    float theta = 3/180.0*PI,dir;		// rotate in 3 degree increments
    int axis;
    solid object;
    
    cout << "Type file name.\n";
    cin >> filename;
    object.readin(filename);        // read in from file
    cout << "Click in lower left of window to exit.\n";
    object.projectAndPlot();		// open the window and project the object
   while(get_axis(&axis,&dir)){     
        object.rotate(axis, dir*theta);
        clear();
        object.projectAndPlot();
    }
    reset(); 
    return 1;
}