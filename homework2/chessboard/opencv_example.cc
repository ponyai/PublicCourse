// Copyright @2018 Pony AI Inc. All rights reserved.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main() {
  cv::Mat image;
  // ATTENTION!!! : please use absolute path for reading the data file.
  image = imread("/home/ocean/PublicCourse/homework2/chessboard/chessboard.png", CV_LOAD_IMAGE_COLOR);
  namedWindow("chessboard");
  imshow("chessboard", image);
  waitKey(0);
  return 0;
}
