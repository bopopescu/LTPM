#pragma once

#include <opencv/cv.h>

#include <vector>
#include <math.h>

using namespace std;

class Segmentation
{
public:
  int maxLabel;
  vector<int> labels;
  int width;
  int height;

  int getLabel(int x, int y) { return labels[x*height+y]; }
};

class Color
{
public:
  Color() {};
  Color(int _r, int _g, int _b) { r = _r; g = _g; b = _b; }
  static float distance(Color a, Color b) { return sqrtf((a.r-b.r)*(a.r-b.r) + (a.g-b.g)*(a.g-b.g) + (a.b-b.b)*(a.b-b.b)); }
  unsigned char r;
  unsigned char g;
  unsigned char b;
};


template<class T> class Image
{
  private:
  IplImage* imgp;
  public:
  Image(IplImage* img=0) {imgp=img;}
  ~Image(){imgp=0;}
  void operator=(IplImage* img) {imgp=img;}
  inline T* operator[](const int rowIndx) {
    return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
};

typedef struct{
  float b,g,r;
} RgbPixelFloat;

typedef Image<Color>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

