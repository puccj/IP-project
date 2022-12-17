#include <chrono>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>

//cv::Mat bersen(int** padded, int rows, int cols, int d, std::fstream& fout, int imgType) {
void bersen(int** padded, int rows, int cols, double k, int w, int d, std::fstream& fout) {
  auto t1 = std::chrono::high_resolution_clock::now();   //start time

  //Threshold (Bersen (3))

  //cv::Mat output(rows, cols, imgType);

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
  std::cout << "Bersen: w = " << w << "\t Execution time = " << ms_int.count() << "ms\n";
  fout << w << '\t' << ms_int.count() << '\n';

  //return output;
}