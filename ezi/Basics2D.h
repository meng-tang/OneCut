#ifndef _BASIC2D_H_
#define _BASIC2D_H_
#include <math.h>
#include <iostream>
using namespace std;
/////////////////////////////////////////////////////////////////////////////////
// THIS FILE DEFINES SOME GENERAL BASIC TYPES USEFUL FOR ARRAYS (e.g. Table2D) //
//  AND STANDARD CONVERSION FUNCTORS THAT CAN BE USED FOR TRANSFORMING ARRAYS  //  
/////////////////////////////////////////////////////////////////////////////////

class Vect;

////////////////////////
// BASIC USEFUL TYPES //
////////////////////////
class Point {
public:
    int x, y;  // x and y coordinates of this point
	Point( ); // default constructor that builds the origin, p=(0,0)
	Point(const Vect& v);
    Point(int x_init, int y_init); 
	double norm() const { return sqrt((double)(x*x+y*y));}
    Point operator*(const double& s) const {return Point((int)(s*x),(int)(s*y));}
    Point operator+(const Point& a) const {return Point(x+a.x,y+a.y);}
    Point operator-(const Point& a) const {return Point(x-a.x,y-a.y);}
    bool operator==(const Point& a) const {return (x==a.x && y==a.y);}
};

class PointPair {
	public:  
    PointPair()
		:p1(),p2(){}     
    PointPair(int x1_,int y1_,int x2_,int y2_)
		:p1(x1_,y1_),p2(x2_,y2_){}  
	PointPair(Point p1_,Point p2_)
		:p1(p1_),p2(p2_){}
	Point p1;
	Point p2;
};


class Vect {
public:
    double x, y;  // x and y coordinates of this vector
    Vect( ); // default constructor that builds null vector, v=(0,0)
	Vect(const Point& p);
    Vect(double x_init, double y_init); 
	double norm() const {return sqrt(x*x+y*y);}
    Vect operator*(const double& s) const {return Vect(s*x,s*y);}
    Vect operator+(const Vect& a) const {return Vect(x+a.x,y+a.y);}
    Vect operator-(const Vect& a) const {return Vect(x-a.x,y-a.y);}
};

Point :: Point( ) { x=0; y=0; }

Point :: Point( int x_init, int y_init ) {// construct the point specified by the parameters
    x = x_init;
    y = y_init;
}

Vect :: Vect( ) { x=0; y=0; }

Vect :: Vect( double x_init, double y_init ) {// construct the vector specified by the parameters
    x = x_init;
    y = y_init;
}

class Vect3D {
public:
    double x, y, z;  // x, y and z coordinates of this vector
    Vect3D( ); // default constructor that builds null vector, v=(0,0,0)
	Vect3D(const Vect3D& v){x = v.x; y = v.y; z = v.z;}
    Vect3D(double x_init, double y_init, double z_init); 
	double norm() const {return sqrt(x*x+y*y+z*z);}
    Vect3D operator*(const double& s) const {return Vect3D(s*x,s*y,s*z);}
    Vect3D operator/(const double& s) const {return Vect3D(x/s,y/s,z/s);}
    Vect3D operator+(const Vect3D& a) const {return Vect3D(x+a.x,y+a.y,z+a.z);}
    Vect3D operator-(const Vect3D& a) const {return Vect3D(x-a.x,y-a.y,z-a.z);}
};

Vect3D :: Vect3D( ) { x=0; y=0; z=0;}
Vect3D :: Vect3D( double x_init, double y_init, double z_init ) {// construct the vector specified by the parameters
    x = x_init;
    y = y_init;
    z = z_init;
}

//////////////////////////////////
// STANDARD CONVERSION FUNCTORS //
//////////////////////////////////

    // conversion functor using type casting   (also used in Table2D constructors, see Table2D.h)
class Casting {   // Example:  Table2D<unsigned char> a; ...  Table2D<double> b;
public:           //           b = a;    <=>    b = convert<double>(a,Casting());   
    template <class T, class S>      // PRECONDITION: casting S->T should be defined
    inline static void  convert(T& t, const S& s) { t = (T) s; } 
};

    // conversion functor for scalar-types (int, double, unsigned,...) using gain-&-bias linear scaling
class Scaling {   // Examples: Table2D<double>   a = image;
    double g,b;   //           Table2D<short>    b = convert<short>(a*LoG(),Scaling(20));
public:           //           Table2D<unsigned> c = convert<unsigned>(a*DoG(),Scaling(10,1000));
    Scaling(const double& gain, const double& bias=0.0) { g = gain;  b = bias; }
    template <class type, class T>
	inline void  convert(type& t, const T& s) const  {t = (type)( ((double)s)*g + b );}
};

    // conversion functor for integer types (int, char, unsigned,...) using look-up tables ("palettes")
class Palette {    // PRECONDITION: look_up table "palette[i]" must be valid for all possible values of argument "i"
public:            // Example:   double alpha[3] = {0.2, 1, 0};  Table2D<int> mask; ... // assume mask's values 0,1,2  
    void* look_up; //            Table2D<double> alpha_mat = convert<double>(mask,Palette(alpha));
    Palette(void* palette) {look_up = palette;}
    template <class T>
    inline void  convert(T& t, const int& i) const { t = ((T*)look_up)[i];
    }
};
#endif

