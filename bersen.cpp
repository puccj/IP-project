#include <chrono>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>

void bersen(int** padded, int rows, int cols, int w, int d, std::fstream& fout) {
  auto t1 = std::chrono::high_resolution_clock::now();   //start time

  //Threshold (Bersen (3))

  for (int x = 0; x < rows; ++x) {
    for (int y = 0; y < cols; ++y) {
      int min = 255;
      int max = 0;

      //remind that padded is shifted by d
      for (int i = 0; i < 2*d; ++i) {
        for (int j = 0; j < 2*d; ++j) {
          if (padded[x+i][y+j] > max)
            max = padded[x+i][y+i];
          if (padded[x+i][y+j] < min)
            min = padded[x+i][y+i];
        }
      }

      double Txy = 0.5*(max + min);
    }
  }

  //stop time
  auto t2 = std::chrono::high_resolution_clock::now();  
  std::chrono::duration<double, std::milli> computTime = t2 - t1; 
  std::cout << "Bersen: w = " << w << "\t Execution time = " << computTime.count() << "ms\n";
  fout << w << '\t' << computTime.count() << '\n';

  //return output;
}

cv::Mat bersen(int** padded, int rows, int cols, int w, int d, std::fstream& fout, int imgType) {
  auto t1 = std::chrono::high_resolution_clock::now();   //start time

  //Threshold (Bersen (3))

  cv::Mat output(rows, cols, imgType);

  for (int x = 0; x < rows; ++x) {
    for (int y = 0; y < cols; ++y) {
      int min = 255;
      int max = 0;

      //remind that padded is shifted by d
      for (int i = 0; i < 2*d; ++i) {
        for (int j = 0; j < 2*d; ++j) {
          if (padded[x+i][y+j] > max)
            max = padded[x+i][y+i];
          if (padded[x+i][y+j] < min)
            min = padded[x+i][y+i];
        }
      }

      double Txy = 0.5*(max + min);
      
      if (padded[x+d][y+d] < Txy)
        output.at<uchar>(x,y) = 0;
      else
        output.at<uchar>(x,y) = 255;
    }
  }

  return output;
}