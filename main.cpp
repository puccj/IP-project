#include <opencv2/highgui.hpp>

#include "proposed.cpp"
#include "niblack.cpp"
#include "sauvola.cpp"
#include "bersen.cpp"

int kTrack = 20;
double k = 0.2;
int wTrack = 5;
int w = 3;
bool print = false;
int paddingMode = 1;

static void onkChanged(int value, void*) {
  k = value / 100.0;
  //print = true;
}
static void onwChanged(int value, void*) {
  if (value % 2 == 0) 
    w = value + 1;
  else
    w = value;

  //print = true;
}

int main() {
  std::string path;
  std::cout << "Insert file name (with extension): ";
  cv::Mat image;
  do {
    //std::cin >> path;
    path = "lenna.jpg";

    image = cv::imread(path, cv::IMREAD_GRAYSCALE);
    if (image.empty())
      std::cout << "Error: can't open image, check name and try again: ";
  }
  while(image.empty());

  int rows = image.rows;
  int cols = image.cols;

  int min = rows;
  if (cols < rows)
    min = cols;

  /*
  cv::namedWindow("Trackbars");
  cv::createTrackbar("k (/100)", "Trackbars", &kTrack, 100, onkChanged);
  cv::createTrackbar("w", "Trackbars", &wTrack, min, onwChanged);
  cv::createTrackbar("Padding", "Trackbars", &paddingMode, 1);
  cv::setTrackbarMin("w", "Trackbars", 3);

  cv::imshow("Input", image);
  */

  int method;
  std::fstream fout;

  std::cout << "-- Methods --\n";
  std::cout << "0: proposed\n";
  std::cout << "1: Niblack\n";
  std::cout << "2: Sauvola\n";
  std::cout << "3: Bersen\n";

  std::cout << "Type number to choose method: ";
  std::cin >> method;

  if (method == 0)
    fout.open("proposed.txt", std::ios::out);
  else if(method == 1)
    fout.open("niblack.txt", std::ios::out);
  else if(method == 2)
    fout.open("sauvola.txt", std::ios::out);
  else if(method == 3)
    fout.open("bersen.txt", std::ios::out);

  while (true) {
    int d = round(w/2);

    //Padded image
    int** padded = new int*[rows+2*d];
    for (int i = 0; i < rows+2*d; i++)
      padded[i] = new int[cols+2*d];
    
    for (int x = 0; x < rows; ++x) {
      for (int y = 0; y < cols; ++y) {
        padded[x+d][y+d] = image.at<uchar>(x,y);
      }
    }

    //padding: all zeros
    if (paddingMode == 0) {      
      for (int x = 0; x < rows + 2*d; ++x) {
        for (int y = 0; y < d; ++y) {
          padded[x][y] = 0;             //up
          padded[x][y + cols + d] = 0;  //down
        }
      }

      for (int x = 0; x < d; ++x) {
        for (int y = d; y < rows; ++y) {
          padded[x][y] = 0;
          padded[x + rows + d] = 0;
        }
      }
    }
    else {
      //padding (copy of the extreme values of the matrix)
      for (int x = 0; x < d; ++x) {
        for (int y = 0; y < d; ++y) {
          padded[x][y] = padded[d][d];
          padded[x][cols+d+y] = padded[d][cols+d-1];
          padded[rows+d+x][y] = padded[rows+d-1][d];
          padded[rows+d+x][cols + d + y] = padded[rows+d-1][cols+d-1];
        }
      }
      for (int x =d; x <rows+d; ++x) {
        for (int y = 0; y < d; ++y) {
          padded[x][y] = padded[x][d];
          padded[x][cols + d + y] = padded[x][cols + d-1];
        }
      }
      for (int y=d; y < cols+d; ++y) {
        for (int x = 0; x < d; ++x) {
          padded[x][y] =padded[d][y];
          padded[rows+d+x][y] = padded[rows+d-1][y];
        }
      }
    }

    //Choose between methods

    if (method == 0)
      proposed(padded, rows, cols, k, w, d, fout /*, image.type()*/);
    else if (method == 1)
      niblack(padded, rows, cols, k, w, d, fout /*, image.type()*/);
    else if (method == 2)
      sauvola(padded, rows, cols, k, w, d, fout /*, image.type()*/);
    else if (method == 3)
      bersen(padded, rows, cols, k, w, d, fout /*, image.type()*/);

    
    /*
    cv::imshow("Output", output);
    int k = cv::waitKey(0);
    if (k == 'q')
      break;
    */
    w += 2;

    if (w > 400)
      break;
  }

  fout.close();
  std::cout << "done.\n";

  return 0;
}
