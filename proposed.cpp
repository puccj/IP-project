#include <chrono>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>

//cv::Mat proposed(int** padded, int rows, int cols, int k, int w, int d, std::fstream& fout, int imgType) {
void proposed(int** padded, int rows, int cols, int k, int w, int d, std::fstream& fout) {
  auto t1 = std::chrono::high_resolution_clock::now();   //start time

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

  //Local sum (s) and mean
  double** mean = new double*[rows];
  for (int i = 0; i < rows; ++i)
    mean[i] = new double[cols];


  for (auto x = 0; x < rows; ++x) {
    for (auto y = 0; y < cols; ++y) {
      int locSatXY = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];
      mean[x][y] = locSatXY / (w*w);
    }
  }

  //Proposed (T)

  //cv::Mat output(rows, cols, imgType);

  for (int x = 0; x < rows; ++x) {
    for (int y = 0; y < cols; ++y) {
      double meanDevAtXY = padded[x][y] - mean[x][y];
      double Txy = mean[x][y] * (1+ k*(meanDevAtXY / (1-meanDevAtXY) -1));
      
      /*
      if (image.at<uchar>(x,y) < Txy)
        output.at<uchar>(x,y) = 0;
      else
        output.at<uchar>(x,y) = 255;
      */
    }
  }

  //stop time
  auto t2 = std::chrono::high_resolution_clock::now();
  auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1); 
  std::cout << "Proposed: w = " << w << "\t Execution time = " << ms_int.count() << "ms\n";
  fout << w << '\t' << ms_int.count() << '\n';

  //return output;
}