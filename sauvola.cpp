#include <chrono>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>

void fast_sauvola(int** padded, int rows, int cols, double k, int w, int d, std::fstream& fout) {
  auto t1 = std::chrono::high_resolution_clock::now();   //start time

  //Integral Sum (g) and integral sum of the squared image
  int** intS = new int*[rows+2*d];
  for (int i = 0; i < rows+2*d; i++)
    intS[i] = new int[cols+2*d];

  int** squareIntS = new int*[rows+2*d];
  for (int i = 0; i < rows+2*d; i++)
    squareIntS[i] = new int[cols+2*d];

  intS[0][0] = padded[0][0];
  squareIntS[0][0] = padded[0][0];
  
  for (auto i = 1; i < cols+2*d; ++i) {
    intS[0][i] = padded[0][i] + intS[0][i-1];
    squareIntS[0][i] = padded[0][i] + squareIntS[0][i-1];
  }
  for (auto i = 1; i < rows+2*d; ++i) {
    intS[i][0] = padded[i][0] + intS[i-1][0];
    squareIntS[i][0] = padded[i][0] + squareIntS[i-1][0];
  }
  for (int x = 1; x < rows+2*d; ++x) {
    for (int y = 1; y < cols+2*d; ++y) {
      intS[x][y] = padded[x][y] + intS[x][y-1] + intS[x-1][y] - intS[x-1][y-1];
      squareIntS[x][y] = padded[x][y] + squareIntS[x][y-1] + squareIntS[x-1][y] - squareIntS[x-1][y-1];
    }
  }

  //Local sum (s) and local sum of the squared image, mean and std deviation
  for (auto x = 0; x < rows; ++x) {
    for (auto y = 0; y < cols; ++y) {
      int locSumAtXY = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];
      int squareLocSumAtXY = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];
      
      double meanAtXY = locSumAtXY / (w*w);
      double stdDevAtXY = squareLocSumAtXY / (w*w) - meanAtXY*meanAtXY;
      
      //Threshold (Sauvola (2))
      double Txy = meanAtXY*(1 + k*(stdDevAtXY/128 - 1));
    }
  }

  //stop time
  auto t2 = std::chrono::high_resolution_clock::now();  
  std::chrono::duration<double, std::milli> computTime = t2 - t1; 
  std::cout << "Sauvola: w = " << w << "\t Execution time = " << computTime.count() << "ms\n";
  fout << w << '\t' << computTime.count() << '\n';
}

cv::Mat fast_sauvola(int** padded, int rows, int cols, int k, int w, int d, std::fstream& fout, int imgType) {
  
  //Integral Sum (g) and integral sum of the squared image
  int** intS = new int*[rows+2*d];
  for (int i = 0; i < rows+2*d; i++)
    intS[i] = new int[cols+2*d];

  int** squareIntS = new int*[rows+2*d];
  for (int i = 0; i < rows+2*d; i++)
    squareIntS[i] = new int[cols+2*d];

  intS[0][0] = padded[0][0];
  squareIntS[0][0] = padded[0][0];
  
  for (auto i = 1; i < cols+2*d; ++i) {
    intS[0][i] = padded[0][i] + intS[0][i-1];
    squareIntS[0][i] = padded[0][i] + squareIntS[0][i-1];
  }
  for (auto i = 1; i < rows+2*d; ++i) {
    intS[i][0] = padded[i][0] + intS[i-1][0];
    squareIntS[i][0] = padded[i][0] + squareIntS[i-1][0];
  }
  for (int x = 1; x < rows+2*d; ++x) {
    for (int y = 1; y < cols+2*d; ++y) {
      intS[x][y] = padded[x][y] + intS[x][y-1] + intS[x-1][y] - intS[x-1][y-1];
      squareIntS[x][y] = padded[x][y] + squareIntS[x][y-1] + squareIntS[x-1][y] - squareIntS[x-1][y-1];
    }
  }

  //Local sum (s) and mean
  double** mean = new double*[rows];
  for (int i = 0; i < rows; ++i)
    mean[i] = new double[cols];

  for (auto x = 0; x < rows; ++x) {
    for (auto y = 0; y < cols; ++y) {
      double locSatXY = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];
      mean[x][y] = locSatXY / (w*w);
    }
  }

  //Integral of the squared sum(sg)
  squareIntS[0][0] = padded[0][0];
  
  for (auto i = 1; i < cols+2*d; ++i) {
    squareIntS[0][i] = padded[0][i] + squareIntS[0][i-1];
  }
  for (auto i = 1; i < rows+2*d; ++i) {
    squareIntS[i][0] = padded[i][0] + squareIntS[i-1][0];
  }
  for (int x = 1; x < rows+2*d; ++x) {
    for (int y = 1; y < cols+2*d; ++y) {
      squareIntS[x][y] = padded[x][y] + squareIntS[x][y-1] + squareIntS[x-1][y] - squareIntS[x-1][y-1];
    }
  }

  //Local squared sum (ss) and mean

  double** squareMean = new double*[rows];
  for (int i = 0; i < rows; ++i)
    squareMean[i] = new double[cols];

  for (auto x = 0; x < rows; ++x) {
    for (auto y = 0; y < cols; ++y) {
      int squareLocSumAtXY = intS[x+d-1 +d][y+d-1 +d] + intS[x-d +d][y-d +d] - intS[x-d +d][y+d-1 +d] - intS[x+d-1 +d][y-d +d];
      squareMean[x][y] = squareLocSumAtXY / (w*w);
    }
  }


  //Threshold (Sauvola (2))

  cv::Mat output(rows, cols, imgType);

  for (int x = 0; x < rows; ++x) {
    for (int y = 0; y < cols; ++y) {
      double meanAtXY = mean[x][y];
      double stdDevAtXY = squareMean[x][y] - meanAtXY*meanAtXY;
      double Txy = meanAtXY*(1 + k*(stdDevAtXY/128 - 1));

      if (padded[x+d][y+d] < Txy)
        output.at<uchar>(x,y) = 0;
      else
        output.at<uchar>(x,y) = 255;
    }
  }

  return output;
}