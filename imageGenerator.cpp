#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

void displayHelp(){
  cout << "Usage: imageGenerator [type] [max size] [refresh rate] [path to image]" << endl;
  cout << endl;
  cout << "type: { rectangle, triangle, circle, line }" << endl;
  cout << "max size: maximum size in pixels of randomly drawn geometry" << endl;
  cout << "refresh rate: after how many succesful iterations the preview is redrawn" << endl;
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

int main( int argc, char** argv )
{

  if(argc != 5){
    cout <<  "Not the correct amount of arguments" << endl;
    displayHelp();
    return -1;
  }

  srand((int)time(0));
  const string type = string(argv[1]);
  const int GEOMETRIC_SIZE = atoi(argv[2]);
  const int refresh = atoi(argv[3]);

  //original image
  Mat original;
  original = imread(argv[4], CV_LOAD_IMAGE_COLOR);

  if(!image.data){
    cout <<  "Could not open or find the image" << endl;
    return -1;
  }

  const int width = original.cols;
  const int height = original.rows;

  Mat currentCanvas;
  currentCanvas = Mat::zeros(width, height, CV_8UC3);

  Mat newCanvas;
  newCanvas = Mat::zeros(width, height, CV_8UC3);

  //make images white
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      currentCanvas.data[currentCanvas.channels()*(width*y + x) + 0] = 255;
      currentCanvas.data[currentCanvas.channels()*(width*y + x) + 1] = 255;
      currentCanvas.data[currentCanvas.channels()*(width*y + x) + 2] = 255;

      newCanvas.data[newCanvas.channels()*(width*y + x) + 0] = 255;
      newCanvas.data[newCanvas.channels()*(width*y + x) + 1] = 255;
      newCanvas.data[newCanvas.channels()*(width*y + x) + 2] = 255;
    }
  }

  int counter = 0;

  int oldDiff = 0;
  int newDiff = 0;

  int minY;
  int maxY;
  int minX;
  int maxX;

  //main loop
  while(1){

    //random color
    int b = (rand() % 255) + 1;
    int g = (rand() % 255) + 1;
    int r = (rand() % 255) + 1;

    if(type == "triangle"){
      //random location
      int ax = (rand() % width) + 1;
      int ay = (rand() % height) + 1;
      int bx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
      int by = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;
      int cx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
      int cy = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;

      //draw triangle
      Point rook_points[1][3];
      rook_points[0][0] = Point( ax, ay );
      rook_points[0][1] = Point( bx, by );
      rook_points[0][2] = Point( cx, cy );
      const Point* ppt[1] = { rook_points[0] };
      int npt[] = { 3 };
      fillPoly( newCanvas, ppt, npt, 1, Scalar( b, r, g ), 8 );

      //define borders for comparison
      minY = min3(ay, by, cy, 0);
      maxY = max3(ay, by, cy, height);
      minX = min3(ax, bx, cx, 0);
      maxX = max3(ax, bx, cx, width);
    }

    else if(type == "circle"){
      //random location
      int cx = (rand() % width) + 1;
      int cy = (rand() % height) + 1;
      int rad = (rand() % GEOMETRIC_SIZE/2) + 1;

      //draw circle
      circle(newCanvas, Point(cx, cy), rad, Scalar( b, r, g ), -1, 8, 0);

      //define borders for comparison
      minY = max(cy-rad, 0);
      maxY = min(cy+rad, height);
      minX = max(cx-rad, 0);
      maxX = min(cx+rad, width);
    }

    else if(type == "rectangle"){
      //random location
      int ax = (rand() % width) + 1;
      int ay = (rand() % height) + 1;
      int bx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
      int by = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;

      //draw rectangle
      rectangle(newCanvas, Point(ax, ay), Point(bx, by), Scalar( b, r, g ), -1, 8, 0);

      //define borders for comparison
      minY = min2(ay, by, 0);
      maxY = max2(ay, by, height);
      minX = min2(ax, bx, 0);
      maxX = max2(ax, bx, width);
    }

    else if(type == "line"){
      //random location
      int ax = (rand() % width) + 1;
      int ay = (rand() % height) + 1;
      int bx = (rand() % GEOMETRIC_SIZE) + ax - GEOMETRIC_SIZE/2 + 1;
      int by = (rand() % GEOMETRIC_SIZE) + ay - GEOMETRIC_SIZE/2 + 1;

      int length = sqrt(pow(ax - bx, 2) + pow(ay - by, 2));

      //draw line
      line(newCanvas, Point(ax, ay), Point(bx, by), Scalar( b, r, g ), length/8, 8, 0);

      //define borders for comparison
      minY = min2(ay, by, 0);
      maxY = max2(ay, by, height);
      minX = min2(ax, bx, 0);
      maxX = max2(ax, bx, width);
    }

    else{
      cout <<  "Not a valid type" << endl;
      displayHelp();
      return -1;
    }

    newDiff = 0;
    oldDiff = 0;

    //compare images
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

        //distance current and original
        oldDiff += sqrt(pow(original_b - current_b, 2) + pow(original_g - current_g, 2) + pow(original_r - current_r, 2));

        //distance new and original
        newDiff += sqrt(pow(original_b - new_b, 2) + pow(original_g - new_g, 2) + pow(original_r - new_r, 2));
      }
    }

    if(oldDiff > newDiff){
      currentCanvas = newCanvas.clone();
      counter++;

      //show
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
