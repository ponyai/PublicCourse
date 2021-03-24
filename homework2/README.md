# Homework2

We have learnt about the outline of perception on autonomous vehicles. Now, let's get hands dirty and implement some major parts in it.

In this homework, we provide some frames of pointcloud data. You can find the data at [this link](https://pan.baidu.com/s/1yJa9Mn3Rawu1F5c373_kjg) with extraction code `udzr`. To do this homework, you will need to run the binary single_frame_detector_main with the data placed at pony_data_dir. When your implementation has been done, you can find the obstacle polygons labelled on the provided viewer. You only need to implement the code for ground detection and segmentation inside `single_frame_detector.cc`.

To run the binary:
`bazel run -c opt //homework2:single_frame_detector_main -- --pony_data_dir ${Where you placed the extracted data files}`

You need to press `N` to go to the next frame and immediately after launching this program.

### 1. Ground detection.

The first step of detection is ground detection. You need to replace the fake implementation placed inside `singe_frame_detector.cc` inside function `GetGroundAndObstacles`. The ground points will be shown in blue. You can either estimate the ground heights based on the single frame data or use a pre-generated ground height set based on the whole data set. If you choose the latter way, Please also submit the code you wrote to get that.

Notes:

Actually, It is hard to generate ground from single frame 32c Lidar data.
Detecting ground typically requires multiple frames to be accurate.

Your task is to detect the ground based on multiple adjacent pointcloud frames.
This could be done by the following steps.
- convert the pointcloud of multiple continuous frames to world coordinate system and you will get an accumulated pointcloud with a lot more points compared to one single pointcloud.
- Try the following method to improve your ground detection method:
  1. Project the accumulated pointcloud to an image with each pixel represents a 0.5m by 0.5m square on XY plane in world coordinate.(You can use cv::Mat to represent an image.) For example, an image with size 200 by 200 and resolution 0.5m can represents the area 100m by 100m on XY plane in world coordinate;
  2. Use the lowest Z value in the world coordinate system as the value for each pixel;
  3. Given a reasonable threshold, the points which are below the lowest Z value plus this predefined threshold are considered as ground points.

### 2. Obstacle detection

Assuming that you have gotten a ground point set, you can exclude these points from point cloud and draw polygons for each obstacle around us. You will need to generate some polygons such that each polygon contains exactly one polygon (car, pedestrian, cyclist, tree ...). This is kind of difficult. Please try your best to get a good result.

Notes:

Here, I provide a basic algorithm to do this. You can try many better algorithms.

We can do this by flood fill.

1. First, we construct a cell grid from the bird view of the point cloud. Each cell has two states: occupied or empty. It is occupied if it has some point.
2. Then, we run a union-find algorithm on the cells. The cells should belong to the same set if they are close to each other.
3. Finally, we can construct a polygon for each set of cells.

If you are willing to get better result, you can try the following things:
1. Make use of previous frames' information.
2. Take the shape of a car into consideration.
3. If you have very much time, you can try to apply models trained on [KITTI](http://www.cvlibs.net/datasets/kitti/).



**What to submit:**
1. `single_frame_detector.cc`.
2. (optional) All the other files that are needed in your implementation.
3. At least 10 screenshots of your result on 10 different frames. **Please do use the screenshots from your own code**.
