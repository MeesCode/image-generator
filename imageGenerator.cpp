#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

void displayHelp(){
  cout << "Usage: imageGenerator [option] [parameter] [path]\n" << endl;
  cout << "options:" << endl;
  cout << "-t: \"rectangle\", \"triangle\", \"circle\", \"line\"" << endl;
  cout << "\twhich shapes will be randomly generated" << endl;
  cout << "\tdefault: triangle\n" << endl;
  cout << "-s: maximum size in pixels of randomly drawn geometry" << endl;
  cout << "\tdefault: 50\n" << endl;
  cout << "-r: after how many succesful iterations the preview is redrawn" << endl;
  cout << "\tdefault: 50\n" << endl;
  cout << "-c: colorspace" << endl;
  cout << "\t0: black and white\n\t1: grayscale\n\t2: full color" << endl;
  cout << "\tdefault: 2" << endl;
}

int min2(int a, int b, int m){
  int r = min(a, b);
  if(r < m){
    return m;
  } else {
    return r;
  }
}

int max2(int a, int b, int m){
  int r = max(a, b);
  if(r > m){
    return m;
  } else {
    return r;
  }
}

int min3(int a, int b, int c, int m){
  int r = min2(a, min(b, c), m);
}

int max3(int a, int b, int c, int m){
  int r = max2(a, max(b, c), m);
}

Scalar getColor(){
  // random color
  int b = (rand() % 255) + 1;
  int g = (rand() % 255) + 1;
  int r = (rand() % 255) + 1;

  return Scalar(b, r, g);
}

Scalar getGrayScale(){
  // random color
  int g = (rand() % 255) + 1;

  return Scalar(g, g, g);
}

Scalar getBlack(){
  return Scalar(0, 0, 0);
}

void drawTriangle(Mat &newCanvas, int &minX, int &maxX, int &minY, int &maxY, Scalar color, const int width, const int height, const int GEOMETRIC_SIZE){
  // random location
  int ax = (rand() % width) + 1;
  int ay = (rand() % height) + 1;
  int bx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
  int by = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;
  int cx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
  int cy = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;

  // draw triangle
  Point rook_points[1][3];
  rook_points[0][0] = Point( ax, ay );
  rook_points[0][1] = Point( bx, by );
  rook_points[0][2] = Point( cx, cy );
  const Point* ppt[1] = { rook_points[0] };
  int npt[] = { 3 };
  fillPoly(newCanvas, ppt, npt, 1, color, 8 );

  // define borders for comparison
  minY = min3(ay, by, cy, 0);
  maxY = max3(ay, by, cy, height);
  minX = min3(ax, bx, cx, 0);
  maxX = max3(ax, bx, cx, width);
}

void drawCircle(Mat &newCanvas, int &minX, int &maxX, int &minY, int &maxY, Scalar color, const int width, const int height, const int GEOMETRIC_SIZE){
  // random location
  int cx = (rand() % width) + 1;
  int cy = (rand() % height) + 1;
  int rad = (rand() % GEOMETRIC_SIZE/2) + 1;

  // draw circle
  circle(newCanvas, Point(cx, cy), rad, color, -1, 8, 0);

  // define borders for comparison
  minY = max(cy-rad, 0);
  maxY = min(cy+rad, height);
  minX = max(cx-rad, 0);
  maxX = min(cx+rad, width);
}

void drawRectangle(Mat &newCanvas, int &minX, int &maxX, int &minY, int &maxY, Scalar color, const int width, const int height, const int GEOMETRIC_SIZE){
  // random location
  int ax = (rand() % width) + 1;
  int ay = (rand() % height) + 1;
  int bx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
  int by = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;

  // draw rectangle
  rectangle(newCanvas, Point(ax, ay), Point(bx, by), color, -1, 8, 0);

  // define borders for comparison
  minY = min2(ay, by, 0);
  maxY = max2(ay, by, height);
  minX = min2(ax, bx, 0);
  maxX = max2(ax, bx, width);
}

void drawLine(Mat &newCanvas, int &minX, int &maxX, int &minY, int &maxY, Scalar color, const int width, const int height, const int GEOMETRIC_SIZE){
  // random location
  int ax = (rand() % width) + 1;
  int ay = (rand() % height) + 1;
  int bx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
  int by = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;

  // draw line
  line(newCanvas, Point(ax, ay), Point(bx, by), color, 1, 8, 0);

  // define borders for comparison
  // return points instead of boundary since we are using a line iterator
  minX = ax;
  minY = ay;
  maxX = bx;
  maxY = by;
}

void generalCompare(Mat &original, Mat &currentCanvas, Mat &newCanvas, int minX, int maxX, int minY, int maxY, const int width, int &oldDiff, int &newDiff){
  for(int y = minY; y < maxY; y++){
    for(int x = minX; x <maxX; x++){
      uchar original_b = original.data[original.channels()*(width*y + x) + 0];
      uchar original_g = original.data[original.channels()*(width*y + x) + 1];
      uchar original_r = original.data[original.channels()*(width*y + x) + 2];

      uchar new_b = newCanvas.data[newCanvas.channels()*(width*y + x) + 0];
      uchar new_g = newCanvas.data[newCanvas.channels()*(width*y + x) + 1];
      uchar new_r = newCanvas.data[newCanvas.channels()*(width*y + x) + 2];

      uchar current_b = currentCanvas.data[currentCanvas.channels()*(width*y + x) + 0];
      uchar current_g = currentCanvas.data[currentCanvas.channels()*(width*y + x) + 1];
      uchar current_r = currentCanvas.data[currentCanvas.channels()*(width*y + x) + 2];

      // distance current and original
      oldDiff += sqrt(pow(original_b - current_b, 2) + pow(original_g - current_g, 2) + pow(original_r - current_r, 2));

      // distance new and original
      newDiff += sqrt(pow(original_b - new_b, 2) + pow(original_g - new_g, 2) + pow(original_r - new_r, 2));
    }
  }
}

void lineCompare(Mat &original, Mat &currentCanvas, Mat &newCanvas, int minX, int maxX, int minY, int maxY, const int width, int &oldDiff, int &newDiff){

  LineIterator it(original, Point(minX, minY), Point(maxX, maxY), 8);

  for(int i = 0; i < it.count; i++, ++it){

    Point pt = it.pos();
    int x = pt.x;
    int y = pt.y;

    uchar original_b = original.data[original.channels()*(width*y + x) + 0];
    uchar original_g = original.data[original.channels()*(width*y + x) + 1];
    uchar original_r = original.data[original.channels()*(width*y + x) + 2];

    uchar new_b = newCanvas.data[newCanvas.channels()*(width*y + x) + 0];
    uchar new_g = newCanvas.data[newCanvas.channels()*(width*y + x) + 1];
    uchar new_r = newCanvas.data[newCanvas.channels()*(width*y + x) + 2];

    uchar current_b = currentCanvas.data[currentCanvas.channels()*(width*y + x) + 0];
    uchar current_g = currentCanvas.data[currentCanvas.channels()*(width*y + x) + 1];
    uchar current_r = currentCanvas.data[currentCanvas.channels()*(width*y + x) + 2];

    // distance current and original
    oldDiff += sqrt(pow(original_b - current_b, 2) + pow(original_g - current_g, 2) + pow(original_r - current_r, 2));

    // distance new and original
    newDiff += sqrt(pow(original_b - new_b, 2) + pow(original_g - new_g, 2) + pow(original_r - new_r, 2));
  }
}

int main( int argc, char** argv ){

  string typeString = "triangle";
  int color = 2;
  int refresh = 50;
  int GEOMETRIC_SIZE = 50;
  string imagePath = "";
  int c;

  while(optind < argc){
    if((c = getopt(argc, argv, "t:c:s:r:")) != -1){
      switch (c){
        case 't':
          typeString = string(optarg);
          break;
        case 'c':
          color = atoi(optarg);
          break;
        case 's':
          GEOMETRIC_SIZE = atoi(optarg);
          break;
        case 'r':
          refresh = atoi(optarg);
          break;
        case '?':
          cout << "Option -" << optopt << " requires an argument." << endl;
          return -1;
        default:
          displayHelp();
          return -1;
      }
    }
    else {
      imagePath = string(argv[optind]);
      optind++;
    }
  }

  if(imagePath == ""){
    cout << "No image path given" << endl;
    displayHelp();
    return -1;
  }

  Scalar (*colorFunction)();

  switch(color){
    case 0:
      colorFunction = &getBlack;
      break;
    case 1:
      colorFunction = &getGrayScale;
      break;
    case 2:
      colorFunction = &getColor;
      break;
    default:
      cout << "Not a valid colorspace" << endl;
      displayHelp();
      return -1;
  }

  srand((int)time(0));

  // original image
  Mat original;
  original = imread(imagePath, CV_LOAD_IMAGE_COLOR);

  if(!original.data){
    cout <<  "Could not open or find the image" << endl;
    displayHelp();
    return -1;
  }

  const int width = original.cols;
  const int height = original.rows;

  Mat currentCanvas(width, height, CV_8UC3, Scalar::all(255));
  Mat newCanvas(width, height, CV_8UC3, Scalar::all(255));

  int counter = 0;

  int oldDiff = 0;
  int newDiff = 0;

  int minX, maxX, minY, maxY;

  void (*drawFunction)(Mat&, int&, int&, int&, int&, Scalar, const int, const int, const int);
  void (*compareFunction)(Mat&, Mat&, Mat&, int, int, int, int, const int, int&, int&);

  compareFunction = &generalCompare;

  if(typeString == "triangle"){
    drawFunction = &drawTriangle;
  } else if(typeString == "circle"){
    drawFunction = &drawCircle;
  } else if(typeString == "rectangle"){
    drawFunction = &drawRectangle;
  } else if(typeString == "line"){
    compareFunction = &lineCompare;
    drawFunction = &drawLine;
  } else {
    cout <<  "Not a valid type" << endl;
    displayHelp();
    return -1;
  }

  // main loop
  while(1){

    drawFunction(newCanvas, minX, maxX, minY, maxY, colorFunction(), width, height, GEOMETRIC_SIZE);

    newDiff = 0;
    oldDiff = 0;

    // compare images
    compareFunction(original, currentCanvas, newCanvas, minX, maxX, minY, maxY, width, oldDiff, newDiff);

    if(oldDiff > newDiff){
      currentCanvas = newCanvas.clone();
      counter++;

      // show
      if(counter >= refresh){
        namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
        imshow( "Display window", currentCanvas );
        waitKey(1);
        counter = 0;
      }
    } else {
      newCanvas = currentCanvas.clone();
    }

  }

  return 0;
}
