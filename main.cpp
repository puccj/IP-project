#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int kTrack = 20;
double k = 0.2;
int wTrack = 5;
int w = 3;
bool print = false;

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
  cv::Mat image = cv::imread("kiddo.tif", cv::IMREAD_GRAYSCALE);
  int rows = image.rows;
  int cols = image.cols;

  int min = rows;
  if (cols < rows)
    min = cols;

  cv::namedWindow("Trackbars");
  cv::createTrackbar("k (/100)", "Trackbars", &kTrack, 100, onkChanged);
  cv::createTrackbar("w", "Trackbars", &wTrack, min, onwChanged);
  cv::setTrackbarMin("w", "Trackbars", 3);

  cv::imshow("Input", image);
  
  if (print) {
    std::cout << image <<"\n\n";
  }
  
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
    for (int x = 0; x < d; ++x) {
      for (int y = 0; y < d; ++y) {
        padded[x][y] = 0;
        padded[rows + d + x][cols + d + y] = 0;
      }
    }
    
    int** intS = new int*[rows+2*d];
    for (int i = 0; i < rows+2*d; i++)
      intS[i] = new int[cols+2*d];

    //Integral Sum (g)

    intS[0][0] = padded[0][0];
    
    for (auto i = 1; i < cols+2*d; ++i) {
      intS[0][i] = padded[0][i] + intS[0][i-1];
    }
    for (auto i = 1; i < rows+2*d; ++i) {
      intS[i][0] = padded[i][0] + intS[i-1][0];
    }
    for (int x = 1; x < rows+2*d; ++x) {
      for (int y = 1; y < cols+2*d; ++y) {
        intS[x][y] = padded[x][y] + intS[x][y-1] + intS[x-1][y] - intS[x-1][y-1];
      }
    }

    //Local sum (s)

    int** locS = new int*[rows];
    for (int i = 0; i < rows; ++i)
      locS[i] = new int[cols];

    double** mean = new double*[rows];
    for (int i = 0; i < rows; ++i)
      mean[i] = new double[cols];


    for (auto x = 0; x < rows; ++x) {
      for (auto y = 0; y < cols; ++y) {
        locS[x][y] = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];

        mean[x][y] = locS[x][y] / (w*w);
      }
    }

    if (print) {
      std::cout << "\n\nLOCAL SUM: ";
      std::cout << "[ ";
      for (auto j = 0; j < cols; ++j) {
        for (auto i = 0; i < rows; ++i) {
          std::cout << locS[i][j] << ", ";
        }
        std::cout << '\n';
      }
      std::cout << "]\n\n";

      std::cout << "\n\nMEAN: ";
      std::cout << "[ ";
      for (auto j = 0; j < cols; ++j) {
        for (auto i = 0; i < rows; ++i) {
          std::cout << mean[i][j] << ", ";
        }
        std::cout << '\n';
      }
      std::cout << "]\n\n";
    }
    


    //Proposed (T) and final

    cv::Mat output(rows, cols, image.type());

    for (int x = 0; x < rows; ++x) {
      for (int y = 0; y < cols; ++y) {
        double meanDevAtXY = image.at<uchar>(x,y) - mean[x][y];
        double Txy = mean[x][y] * (1+ k*(meanDevAtXY / (1-meanDevAtXY) -1));
        if (image.at<uchar>(x,y) < Txy)
          output.at<uchar>(x,y) = 0;
        else
          output.at<uchar>(x,y) = 255;
      }
    }  
    
    cv::imshow("Output", output);
    int k = cv::waitKey(60);
    if (k == 'q')
      break;
  }

  return 0;
}