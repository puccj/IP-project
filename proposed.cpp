#include <chrono>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>

void proposed(int** padded, int rows, int cols, double k, int w, int d, std::fstream& fout) {
  auto t1 = std::chrono::high_resolution_clock::now();   //start time

  //Integral Sum (g)
  int** intS = new int*[rows+2*d];
  for (int i = 0; i < rows+2*d; i++)
    intS[i] = new int[cols+2*d];

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

  //Local sum (s) and local sum of the squared image, mean and std deviation
  for (auto x = 0; x < rows; ++x) {
    for (auto y = 0; y < cols; ++y) {
      int locSumAtXY = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];
      
      double meanAtXY = locSumAtXY*1.0 / (w*w);
      double meanDevAtXY = padded[x+d][y+d] - meanAtXY;
      
      //Proposed Threshold (0)
      double Txy = meanAtXY * (1+ k*(meanDevAtXY / (1-meanDevAtXY) -1));
    }
  }

  //stop time
  auto t2 = std::chrono::high_resolution_clock::now();
  //auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  std::chrono::duration<double, std::milli> computTime = t2 - t1;
  std::cout << "Proposed: w = " << w << "\t Execution time = " << computTime.count() << "ms\n";
  fout << w << '\t' << computTime.count() << '\n';
}


cv::Mat proposed(int** padded, int rows, int cols, int k, int w, int d, std::fstream& fout, int imgType) {
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

  cv::Mat output(rows, cols, imgType);

  //Local sum (s) and mean

  for (auto x = 0; x < rows; ++x) {
    for (auto y = 0; y < cols; ++y) {
      int locSatXY = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];
      double meanAtXY = locSatXY*1.0 / (w*w);
      double meanDevAtXY = padded[x+d][y+d] - meanAtXY;

      //Proposed (T)
      double Txy = meanAtXY * (1+ k*(meanDevAtXY / (1-meanDevAtXY) -1));
      
      if (padded[x+d][y+d] < Txy)
        output.at<uchar>(x,y) = 0;
      else
        output.at<uchar>(x,y) = 255;
    }
  }

  return output;
}