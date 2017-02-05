#ifndef _TABLE2D_H_
#define _TABLE2D_H_
#include "Basics2D.h"

//////////////////////////////////////////////////////////////////////////////////////////
// THIS FILE DEFINES (TEMPLATED) CLASS "Table2D" REPRESENTING 2D ARRAYS WITH ELEMENTS   //
// OF ANY TYPE "T".   BASIC MATHEMATICAL OPERATIONS FOR Table2D OBJECTS ARE IN Math2D.h //
// GLOBAL FUNCTIONS FOR DISPLAYING/LOADING Table2D (AS IMAGE) ARE PROVIDED IN Image2D.h //
//////////////////////////////////////////////////////////////////////////////////////////


template <class T>
class Table2D {
public:
    // constructing/copying of 2d arrays 
    Table2D();    // creates an empty array of zero width and height 
    Table2D(unsigned width, unsigned height); // initialized array of given width and height (values are not initialized)
    Table2D(unsigned width, unsigned height, T val); // as above, but also initializes all values in the array
    Table2D(const Table2D<T>& src); // copy constructor
    Table2D<T>& operator=(const Table2D<T>& src); // copy operator
    ~Table2D(); // destructor

    template <class type>              // conversion constructor that works
    Table2D(const Table2D<type>& src); // works if casting from "type" to "T" is defined
    template <class type>                            // conversion operator that works
    Table2D<T>& operator=(const Table2D<type>& src); // if casting "type->T" is defined
	bool operator==(const Table2D<T>& src); // if casting "type->T" is defined
 
        // basic quiry functions
    bool isEmpty() const {return (m_container==NULL);} // checks if array is empty (zero width and/or height)
    unsigned getWidth() const {return m_width;} 
    unsigned getHeight() const {return m_height;}
    T getMin() const; // uses operator "<" for type "T"  PRECONDITION: Table should be non empty
    T getMax() const; // uses operator ">" for type "T"  PRECONDITION: Table should be non empty
	T getMean() const; // uses operator ">" for type "T"  PRECONDITION: Table should be non empty
	T sum() const; // uses operator ">" for type "T"  PRECONDITION: Table should be non empty
	T sum(Table2D<bool> ROI) const;          
        // functions for checking "in-range" array indexes
    bool pointIn(unsigned x, unsigned y) const {return (x<m_width && y<m_height);}
    bool pointIn(int x, int y) const {return (0<=x && ((unsigned)x)<m_width && 0<=y && ((unsigned)y)<m_height);}
	bool pointIn(Point p) const {return pointIn(p.x,p.y);}

        // operators allowing access of items in "Table2D<T> a" using syntax` "a[x][y]" or "a[p]"
    T* operator[](unsigned x) const;  // Note: "a[x]" returns address of the first item in column "x"
    T* operator[](int x) const;       // PRECONDITION: column index "x" must be in-range
    T& operator[](Point p) const;     // PRECONDITION: coordinates of p=(x,y) must be in-range

        // functions for resizing/resetting arrays
    Table2D<T>& resize(unsigned width, unsigned height); // creates new container (but values are not initialized)
    Table2D<T>& resize(int zoom); // positive "zoom" zooms-up, negative "zoom" zooms-down, zero generates empty Table
    Table2D<T>& reset(T val); // assigns new value to all items
    Table2D<T>& reset(unsigned width, unsigned height, T val) {resize(width,height); return reset(val);}
	T * copytoArray(); // copy data to an array

        // basic arithmetic operators
    Table2D<T>& operator+=(const Table2D<T>& arg); // adding another Table2D
    Table2D<T>& operator-=(const Table2D<T>& arg); // subtracting another Table2D
    Table2D<T>& operator+=(const T& val); // adding a constant to each element of the table
    Table2D<T>& operator-=(const T& val); // subtracting a constant from each element of the table
    Table2D<T>& operator*=(const double& s); // multiplication by a constant scalar 
    Table2D<T>& operator%=(const Table2D<double>& s); // point-wise scaling, PRECONDITION: array s should match the size of Table2D object
    Table2D<T> operator~() const; // returns a new transposed matrix

        // member functions for transforming into new arrays via "point-processing"
    template <class type> // via linear-scaling to a new range of values [min_val,max_val] (works for scalar "type") 
    void convertTo(Table2D<type>& trg, const double new_min, const double new_max) const;
    template <class type>   // using "function pointer" f,   e.g. Table2D<double> b(10,10,4), a; b.convertTo(a,&sqrt);
    void convertTo(Table2D<type>& trg, type (*f)(T item)) const; 
    template <class type, class Conversion> // using "conversion functor" f with "convert(type,T)" method
    void convertTo(Table2D<type>& trg, const Conversion& f) const;

private:
    T* m_container;
    unsigned m_width;
    unsigned m_height;

};

////////////////////////////////////////////////////////////////////////////////////////

    // global functions for transforming Table2D objects via "point-processing" --- trg[x][y] = f( arr[x][y] )
template <class type, class T>  // using linear rescaling to new range [new_min,new_max] (works for scalar "type") 
Table2D<type> convert(const Table2D<T>& src, const double new_min, const double new_max);

template <class type, class T>   // f - "function pointer", e.g.  Table2D<double> b(10,10,4), a = convert<double>(b,&sqrt);
Table2D<type> convert(const Table2D<T>& src, type (*f) (T)); 

template <class type, class T, class Conversion>  // f - "conversion functor" with "convert(type,T)" method
Table2D<type> convert(const Table2D<T>& src, const Conversion& f);

    // global function for cropping Table2D objects
template <class T> // returns new sub-array of the same type, PRECONDITION: corner points should be within the range of src table
Table2D<T> crop(const Table2D<T>& src, const Point corner1, const Point corner2);

template <class T> // scalar multiplication
Table2D<T> operator*(const Table2D<T>& a, const double& scalar); 

template <class T> // scalar multiplication
Table2D<T> operator*(const double& scalar, const Table2D<T>& a); 

template <class T> // adding a (scalar-valued) constant to all elements 
Table2D<T> operator+(const Table2D<T>& a, const double& val); 

template <class T> // adding a (scalar-valued) constant to all elements
Table2D<T> operator+(const double& val, const Table2D<T>& a); 

template <class T> // subtracting a (scalar-valued) constant from all elements
Table2D<T> operator-(const Table2D<T>& a, const double& val); 

template <class T> // subtracting from a (scalar-valued) constant
Table2D<T> operator-(const double& val, const Table2D<T>& a); 

template <class T> // matrix summation, PRECONDITION; arrays should have the same size
Table2D<T> operator+(const Table2D<T>& a, const Table2D<T>& b); 

template <class T> // matrix subtraction, PRECONDITION; arrays should have the same size
Table2D<T> operator-(const Table2D<T>& a, const Table2D<T>& b); 

template <class T> // matrix multiplication, PRECONDITION; width of "a" must match height of "b"
Table2D<T> operator*(const Table2D<T>& a, const Table2D<T>& b); 

template <class T> // POINTWISE scaling, PRECONDITION; arrays should have the same size
Table2D<T> operator%(const Table2D<T>& a, const Table2D<double>& s); 


#include "Table2D.template"
#endif
