#include <iostream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "tictoc.h"
#include "cvmodified.h"

// flag to determine which GFTT to use
constexpr bool useModifiedCV = true;

// EuRoC MH_01_easy image from cam0
constexpr char imgFile[] = "../res/1403636641013555456.png";

int main(int argc, char const *argv[])
{
  cv::Mat img = cv::imread(imgFile, cv::IMREAD_COLOR);

  /// GFTT Parameters
  constexpr int maxCorners = 500;
  constexpr double qualityLevel = 0.001;
  constexpr double minDist = 30;

  // convert to greyscale for GFTT
  cv::Mat grey;
  cv::cvtColor(img, grey, cv::COLOR_BGR2GRAY);

  std::vector<cv::Point2f> corners;
  std::vector<float> scores;
  
  TicToc t_detect;
  if (useModifiedCV) {
    cvmodified::goodFeaturesToTrack(grey, corners, scores, maxCorners, qualityLevel, minDist);
  } else {
    cv::goodFeaturesToTrack(grey, corners, maxCorners, qualityLevel, minDist);

    // we don't have score info
    scores.resize(corners.size(), 1.0);
  }

  std::cout << "Detected " << corners.size() << " features in " << t_detect.toc() << " ms." << std::endl;

  // Score statistics
  float maxScore = *std::max_element(scores.begin(), scores.end());
  std::cout << "Maximum feature score: " << maxScore << std::endl;

  // draw features
  for (size_t i=0; i<corners.size(); ++i) {
    // convenience
    auto pt = corners[i];
    auto score = scores[i] / maxScore;

    // color alpha mixing
    auto color = cv::Scalar(0, 255*score, 255*(1 - score));
    cv::circle(img, pt, 2, color, 2);
  }

  cv::imshow("Detections", img);
  while(1) {
    char key = static_cast<char>(cv::waitKey(0));
    if (key == static_cast<char>(27)) break; // esc
  }
}