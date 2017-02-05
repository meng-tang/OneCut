#pragma once
#include "Table2D.h"
// two helpful global functions
template <class T> inline unsigned char fit(T s) {return (s<255) ? ((s>0) ? ((unsigned char) s) : 0) : 255;}
template <class T, class RGB> inline T avr(RGB s) {return (T) ((((double)s.r)+((double)s.g)+((double)s.b) )/3.0);}

////////////////////////////////////////////////////////////////////////////////////
// THIS FILE CONTAINS DEFINITIONS OF GLOBAL FUNCTIONS AND SOME EXTRA CLASSES THAT //
// ALLOW TO USE 2D TABLE OBJECTS AS GREY OR COLOR IMAGES THAT CAN BE DISPLAYED,   //
// LOADED FROM IMAGE FILES, PROCESSED, OR TRANSFORMED TO A DIFFERENT TYPE         //
////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// DEFINITION OF TYPE "RGB" THAT CAN BE USED TO DECLARE COLOR IMAGES as  //
//                ---   Table2D<RGB> myimage;   ----                     //
//                                                                       //
// Such color images, and many gray-scale images of basic types,  e.g.   //
//   ----  Table2D<int> a;  Table2D<float> b; Table2D<unsigned> c;  ---  // 
// can be displayed in a window or initialized from BMP image file using //
// global functions "drawImage()" and "loadImage()" defined below.       //
//                                                                       //
// NOTE: it is easy to convert different image types.  For example,      //
// to convert colored RGB image "Table2D<RGB> im" to a gray-scale image  //
// one can use Table2D conversion constructors/copying (in Table2D.h) as //	
//      ---  Table2D<double> s = im;   (uses RGB->double cast, see below)//
//      ---  Table2D<int> t; t = im;   (uses RGB->int cast, see below)   //
// Wise-versa, one can convert gray-scale images to RGB image as in      //
//      ---  Table2D<int> d;... im = d; (uses int->RGB cast, see bellow) //
// Conversion between basic gray-scale images is done similarly, e.g.    //
//      ---  Table2D<float> c;  c = d;  (uses standard int->float cast)  //
//                                                                       //
// Instead of default transformations (copying/constructor) that use     //
// type casting, for scalar-valued tables (int, double,...) one can also //
// use linear-scaling "convert" method (in Table2D):                     //
//     ---  Table2D<int> h = convert<int>(im,-4,500); (h-range [-4,500]) //
//     ---  Table2D<float> f = convert<float>(s,0,1);   (f-range [0,1])  //
// where "convert(arr,a,b)" maps range of "arr" [min,max] onto [a,b].    //
// In contrast, tables s,t (set by casting, above) got range [0,255].    //
//                                                                       //
// If "RGB->type" or "type->RGB" cast is not defined below then one can  //
// add the corresponding cast operators to class RGB (below) or use      //
// transformation methods ("convertTo" or "convert" in Table2D.h) with   //
// specialized conversion functors  (see FUNCTOR examples below).        //
// Conversion functors can be designed to handle any special situation.  //
///////////////////////////////////////////////////////////////////////////

struct   RGB  {         // member variables and constructors
                  unsigned char r; unsigned char g;  unsigned char b;  // red, green, and blue color channels
                  RGB(const unsigned char& r1=0, const unsigned char& g1=0, const unsigned char& b1=0) : r(r1), g(g1), b(b1) {}
                  RGB(const RGB& s) {r=s.r; g=s.g; b=s.b;}   // copy constructor
                       // casting T->RGB   (enables casting Table2D<T> objects into Table2D<RGB>, see "Table2D.h")
                  RGB(const bool& s) {r=g=b=255*s;}        // "bool->RGB" (examples: RGB c = false, d = (RGB) true;)
                  RGB(const unsigned char& s) {r=g=b=s;}   // "unsigned char->RGB" (examples: RGB c = 5;)
                  template <class T> RGB(const T& s) {r=g=b=fit(s);} // "T->RGB" for any type "T" that can be cast to "unsigned char"
                      // casting RGB->T    (enables casting Table2D<RGB> objects into Table2D<T>, see "Table2D.h")
                  operator unsigned char() const {return avr<unsigned char>(*this);} // "RGB->unsigned char" 
                  operator unsigned short() const {return avr<unsigned short>(*this);} // "RGB->unsigned short"
                  operator unsigned int() const {return avr<unsigned int>(*this);} // "RGB->unsigned int"
                  operator short() const {return avr<short>(*this);} // "RGB->short"
                  operator int() const {return avr<int>(*this);} // "RGB->int"   (example: RBG c(10,134,203); int i; i=c;)
                  operator float() const {return avr<float>(*this);} // "RGB->float"   (example: float t=c;)
                  operator double() const {return avr<double>(*this);} // "RGB->double"
                       // RGB operators enabling all arithmetic operations on Table2D<RGB> objects (see "Table2D.h") 
                  RGB& operator*=(const double& scalar) {r=fit(r*scalar); g=fit(g*scalar); b=fit(b*scalar); return (*this);}
                  RGB& operator+=(const RGB& c) {r=fit(((double)r)+c.r); g=fit(((double)g)+c.g); b=fit(((double)b)+c.b); return (*this);}
                  RGB& operator-=(const RGB& c) {r=fit(((double)r)-c.r); g=fit(((double)g)-c.g); b=fit(((double)b)-c.b); return (*this);}
				  bool operator==(const RGB& c) {return(r==c.r)&&(g==c.g)&&(b==c.b);}
				  RGB& operator=(const RGB& c) {r=c.r; g=c.g; b=c.b; return (*this);}
                  bool operator<(const RGB& c) const {return ((double)(*this))<((double)c);}
                  bool operator>(const RGB& c) const {return ((double)(*this))>((double)c);}
				  RGB& reset(const RGB& c) {r=c.r; g=c.g; b=c.b; return (*this);}
              };      // COMMENT: some operators in "Math2D.h" (if applied to Table2D<RGB>) may require 2 additional arithmetic operators for type RGB. 
                      // For example, to enable "convolution" for Table2D<RGB> images, operators "RGB*double" and "RGB+RGB" would have to be defined.
                      // Yet, this is probably not a good idea as type "unsigned char" (r, g, b) would not work well in many cases. Better
                      // solutions are either to convert RGB image to Table2D<double> array, or to split RGB image (see "splitRGB" method below)
                      // into 3 "double" arrays and "process" them separately. One can also define a new type "RGB_double" with operators * and +.


////////////////////////////////////////////////////////////////
// Some useful predefined constants for standard color values //
////////////////////////////////////////////////////////////////

const RGB red(255,0,0),    lime(0,255,0),      blue(0,0,255),     black(0,0,0);
const RGB maroon(128,0,0), green (0,255,0),    navy(0,0,128),     gray(128,128,128);
const RGB cyan(0,255,255), magenta(255,0,255), yellow(255,255,0), white(255,255,255);
const RGB teal(0,128,128), purple(128,0,128),  olive(128,128,0),  silver(192,192,192);


///////////////////////////////////////////////////////////////////////////////
// Basic global functions for type RGB and for Table2D<RGB> objects.   E.g.: //
//                                                                           //
// 1) to get intensity difference between 2 pixels in image Table2D<RGB> a:  //
//     --- Point p1(56,23), p2(72,39);   cout << dI(a[p1],a[p2]);            // 
// 2) to blur Table2D<RGB> image, can use splitRGB/combineRGB methods below: //
//     --- Table2D<double> r,g,b;   splitRGB(image,r,g,b);                   //
//     --- image = combineRGB(r*Gauss(),g*Gauss(),b*Gauss()); (see Math2D.h) //
// 3) to convert any gray-scale (int, unsigned, double,..) array to an RGB   //
//    image while automatically scaling the range to interval [0,255]        //
//     --- Table2D<double> a = image;              (  see "Math2D.h" for   ) //
//     --- Table2D<RGB> c = autoScale( a*dX() );   (operator* and kernel dX) //
///////////////////////////////////////////////////////////////////////////////

                    // computes (gray-scale) intensity of given RGB value
inline double I(const RGB &a) {return (double)a;}

                    // computes difference between intensities of two given RGB values
inline double dI(const RGB &a, const RGB& b) 
{
	//return I(a)-I(b);
	//return (abs(a.r-b.r)+abs(a.g-b.g)+abs(a.b-b.b))/3;
	return pow((double)(a.r-b.r),2.0)+pow((double)(a.g-b.g),2.0)+pow((double)(a.b-b.b),2.0);
	//return (a.r-b.r)*(a.r-b.r)+(a.g-b.g)*(a.g-b.g)+(a.b-b.b)*(a.b-b.b);
}

template <class T> // splits one RGB image into three scalar-valued tables (casts "unsigned char" into type "T")
void splitRGB(const Table2D<RGB>& src, Table2D<T>& R, Table2D<T>& G, Table2D<T>& B);

template <class T> // combines three scalar-valued tables into one RGB image  (uses "fit" to cast each "T" value to "unsigned char")
Table2D<RGB> combineRGB(const Table2D<T>& R, const Table2D<T>& G, const Table2D<T>& B);

template <class T> // "scales" the range of scalar-valued table to [0,255]
Table2D<RGB> autoScale(const Table2D<T>& a)        {return convert<RGB>(a,0,255);}


///////////////////////////////////////////////////////////////////////
// Basic functions for "drawing", "loading", and "saving" 2D arrays. //
// These global functions allow to treat Table2D objects as images.  //
//                                                                   //
// If RGB->T casting is defined (see class RGB above) then one can   //
// load Table2D<T> objects from BMP image files as in these examples://
//    Table2D<RGB>    a  = loadImage<RGB>("file.bmp");               //
//    Table2D<double> b  = loadImage<double>("file.bmp");            //
//    Table2D<int> r,g,b; splitRGB(loadImage<RGB>("file.bmp"),r,g,b);//
//    Kernel2D<float> template = loadImage<float>("file2.bmp");      //
//                                                                   //
// If T->RGB casting is defined (see constructors for class RGB),    //
// then one can easily display  Table2D<T> objects as in examples:   //
//       drawImage(a);            (for a, b,... objects above)       //
//       drawImage(b);                                               //
//       drawImage(autoScale(b*template));        (see Math2D.h)     //
//       drawImage(autoScale(b*SobolX()));        (see Math2D.h)     //
//       drawImage(combineRGB(r,g,b));                               //
//       drawImage( (Table2D<float>) template );                     //
///////////////////////////////////////////////////////////////////////

template <class T>  // creates Table2D of type "T" from BMP file, uses RGB->T cast orerator (if defined for RGB class above)
Table2D<T> loadImage(const char * bmp_file_name);  // loaded table gets range of values in [0,255]. 

template <class T>  // saves Table2D of type "T" to BMP file, uses T->RGB cast orerator (if defined for RGB class above)
bool saveImage(const Table2D<T>& arr, const char * bmp_file_name); // arr's values outside [0,255] are truncated at 0 or 255. 



////////////////////////////////////////////////////////////////
// More general drawing/saving functions (see examples below) //
////////////////////////////////////////////////////////////////

template<class T, class ConversionToRGB>  // saves Table2D of any type "T" to BMP file, but requires T-to-RGB conversion functor
bool saveImage(const Table2D<T>& arr, const ConversionToRGB& f, const char * bmp_file_name);

template<class T, class ConversionToRGB>  // saves Table2D of any type "T" to BMP file, uses RGB conversion functor and alpha_mat with values in interval [0,1]
bool saveImage(const Table2D<T>& arr, const ConversionToRGB& f, const Table2D<double>& alpha_mat, const char * bmp_file_name);
              //PRECONDITION: "alpha-mat" must have the same size as "arr" and values between 0 and 1.


///////////////////////////////////////////////////////////////////////////
//     EXAMPLES: "CONVERSIONS", "CONVERSION FUNCTORS", and "PALETTES"    //
//                                                                       //
// Here are examples of "drawImage", "loadImage", and transformations.   //
// The examples use conversion functors (defined in Basics2D.h or below).//
// Conversion functors must have public method convert(T& t, const S& s) //
// setting variable t (target) of type "T" from s (source) of type "S".  //
// --------------------------------------------------------------------- //
// 1) To extract "red" component of an RGB image into a separate table:  //
//       Table2D<RGB>    im  = loadImage<RGB>("myfile.bmp");             //
//       Table2D<double> red = convert<double>(im,GetRed()); (see below) //
//       or (alternatively)...   im.convertTo(red,GetRed());             //
// Similarly, one can write functors extracting other colors, hue, etc.  //
// --------------------------------------------------------------------- //
// 2) To draw a gray-scale image in blue color can use code like         //
//       Table2D<double> gr = im;                                        //
//       drawImage(gr,ToBlue());                     ("ToBlue" is below) //
// --------------------------------------------------------------------- //
// 3) To draw a negative of image "im" one can use code like             //
//       drawImage(255-im);             (uses operators in Math2D.h)     //
// The same code works for gray-scale tables,  e.g  drawImage(255-gr);   //
// NOTE: while gray-scale images can alternatively use "scaling" methods //
//       drawImage(convert<double>(gr,Scaling(-1,255))); (see Basic2D.h) //
// or    drawImage(convert<double>(gr,255,0));           (see Table2D.h) //
// "Scaling" functor or interval-based "convert" create gray RGB images. //
// --------------------------------------------------------------------- //
// 4) To draw gray-scale table with range different from [0,255] use     //
// "ShowRange" functor mapping selected range onto intensities [0,255]   //
//       drawImage(gr*LoG(), ShowRange(-30,+30));    ("LoG" in Math2D.h) //
// Instead, can also use "autoScale" function (above) to see full range  //
//       drawImage( autoScale(gr*LoG()) );                               //
// It is also possible to use linear scaling functor (given gain-&-bias) //
//       drawImage(gr*LoG(), Scaling(2,127) ); ("Scaling" in Basics2D.h) //
// or, equivalently, scalar multiplication/addition operators            //
//       drawImage(gr*LoG() * 2 + 127);      (*/+ operators in Math2D.h) //
// --------------------------------------------------------------------- //
// 5) To draw a semi-transparent mask (e.g. over previously drawn image) //
// use version of drawImage that takes look-up tables ("palettes")       //
//       drawImage(im);                                                  //
//       Table2D<int> mask; ...                                          //
//       RGB c[5]    = {red,  lime,  black,  maroon,  RGB(0,20,100)};    //
//       double t[5] = {0,2,  1.0,   0.0,    0.2,     0.0};              //
//       drawImage(mask,c,t);                                            //
// where we assume that table "mask" contains only values 0,1,2,3,4 and  //
// c[0]-c[4], t[0]-t[4] specify colors and transparencies for them.      //
// Equivalently, one can draw or save semi-transparant "mask" over       //
// image "im" using "Palette" functor defined in Basics2D.h              //
//       Table2D<RGB>    cMat = convert<RGB>(mask,Palette(c));           //
//       Table2D<double> aMat = convert<double>(mask,Palette(t));        //
//       drawImage( cMat + im%(1-aMat)); (% -pointwise scaling, Math2D.h)//
//       saveImage( cMat + im%(1-aMat), "file.bmp"); (saves to BMP file) //
// --------------------------------------------------------------------- //
// 6) To add "Table2D<type>" layer with given alpha mat "aMat", can use  //
//       drawImage( layer, Casting(), aMat);   ("Casting" in Basics2D.h) //
// --------------------------------------------------------------------- //
// --------------------------------------------------------------------- //
//  IMPORTANT: ONE CAN ALWAYS WRITE AND USE OTHER CONVERSION FUNCTORS    //
//    FOR SPECIFIC TYPES "T", OR TO ADDRESS ANY SPECIAL SITUATION!!!     //
///////////////////////////////////////////////////////////////////////////

class GetRed {   // can be used to initialize scalar-valued (e.g. int, double) tables
public:          // based on red color component of a given RGB image   
    template <class T>
    inline static void  convert(T& t, const RGB& s) { t = (T) s.r; } 
};

class ToBlue {  // can be used to draw scalar-valued (e.g. int, double) tables
public:         // in Blue color   
    template <class T>
    inline static void  convert(RGB& t, const T& s) { t.r=t.g=0; t.b=fit(s); } 
};

// linar-scaling functor that maps specified range [a,b] onto interval [0,255], ("Scaling" is in Basics2D.h)
inline Scaling ShowRange(double a, double b) {return Scaling(255.0/(b-a),255.0*a/(a-b));}

#include "Image2D.template"
