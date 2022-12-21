#include <opencv2/highgui.hpp>

#include "proposed.cpp"
#include "niblack.cpp"
#include "sauvola.cpp"
#include "bersen.cpp"

int kTrack = 20;
double k = 0.2;
int wTrack = 5;
int w = 3;
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
    std::cin >> path;

    image = cv::imread(path, cv::IMREAD_GRAYSCALE);
    if (image.empty())
      std::cout << "Error: can't open image, check name and try again: ";
  }
  while(image.empty());


  int rows = image.rows;
  int cols = image.cols;
  int imgType = image.type();

  //maxW is the min between rows and cols
  int maxW = rows;
  if (cols < rows)
    maxW = cols;

  bool showImage;
  std::cout << "Do you want to check the computation time (0) or do you want to see the thresholded images (1)? ";
  std::cin >> showImage;

  int method = 0;
  std::fstream fout;

  if (showImage) {
    cv::namedWindow("Output", 0);
    cv::createTrackbar("k (/100)", "Output", &kTrack, 100, onkChanged);
    cv::createTrackbar("w", "Output", &wTrack, maxW, onwChanged);
    cv::setTrackbarMin("w", "Output", 3);
    cv::createTrackbar("Padding", "Output", &paddingMode, 1);
    cv::createTrackbar("Method", "Output", &method, 3);

    cv::imshow("Input", image);

    std::cout << "\n\nWith focus on the windows, press 'q' to quit and close them, 's' to save the output image.\n";
  }
  else {
    std::cout << "-- Methods --\n";
    std::cout << "0: proposed\n";
    std::cout << "1: Fast Niblack\n";
    std::cout << "2: Fast Sauvola\n";
    std::cout << "3: Fast Bersen\n";
    std::cout << "4: Niblack\n";
    std::cout << "5: Sauvola\n";
    std::cout << "6: Bersen\n";

    std::cout << "Type number to choose method: ";
    std::cin >> method;

    if (method == 0)
      fout.open("proposed.txt", std::ios::out);
    else if(method == 1)
      fout.open("fast-niblack.txt", std::ios::out);
    else if(method == 2)
      fout.open("fast-sauvola.txt", std::ios::out);
    else if(method == 3)
      fout.open("fast-bersen.txt", std::ios::out);
    else if(method == 4)
      fout.open("niblack.txt", std::ios::out);
    else if(method == 5)
      fout.open("sauvola.txt", std::ios::out);
    else if(method == 6)
      fout.open("bersen.txt", std::ios::out);
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
    if (paddingMode == 0) {      
      for (int x = 0; x < rows + 2*d; ++x) {
        for (int y = 0; y < d; ++y) {
          padded[x][y] = 0;             //up
          padded[x][y + cols + d] = 0;  //down
        }
      }

      for (int x = 0; x < d; ++x) {
        for (int y = d; y < rows; ++y) {
          padded[x][y] = 0;           //left
          padded[x + rows + d] = 0;   //right
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
    cv::Mat output;

    if (showImage) {
      if (method == 0)
        output = proposed(padded, rows, cols, k, w, d, fout, imgType);
      else if (method == 1)
        output = fast_niblack(padded, rows, cols, k, w, d, fout, imgType);
      else if (method == 2)
        output = fast_sauvola(padded, rows, cols, k, w, d, fout, imgType);
      else if (method == 3)
        output = bersen(padded, rows, cols, w, d, fout, imgType);
    }
    else {
      if (method == 0)
        proposed(padded, rows, cols, k, w, d, fout);
      else if (method == 1)
        fast_niblack(padded, rows, cols, k, w, d, fout);
      else if (method == 2)
        fast_sauvola(padded, rows, cols, k, w, d, fout);
      else if (method == 3)
        bersen(padded, rows, cols, w, d, fout);
      else if (method == 4)
        niblack(padded, rows, cols, k, w, d, fout);
      /*else if (method == 5)
        sauvola(padded, rows, cols, k, w, d, fout);
      else if (method == 6)
        bersen(padded, rows, cols, w, d, fout);*/
    }

    
    if (showImage) {
      cv::imshow("Output", output);
      int k = cv::waitKey(100);
      if (k == 'q')         //quit
        break;
      if (k == 's') {       //save
        std::string mName;
        if (method == 0)      mName = "proposed";
        else if (method == 1) mName = "fast-niblack";
        else if (method == 2) mName = "fast-sauvola";
        else if (method == 3) mName = "fast-bersen";

        cv::imwrite(mName + " - k=" + std::to_string(k) + " - w=" + std::to_string(w) + ".png", output);
      }
    }
    else {
      w += 2;
      if (w > maxW)
        break;
    }
  }

  fout.close();
  std::cout << "\nThank you for using this little program. Bye bye!\n";

  return 0;
}
