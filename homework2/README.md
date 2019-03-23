# Homework2

Now, you guys have learnt about calibrations. Let's implement something cool.

## New dependency
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) is a header only linear algebra library, which supports matrices, vectors, numerical solvers, and related algorithms. It will be used in multiple places through out the course. **Note:** You don't need to install this library by yourself, Bazel will do that for you.

[opencv](https://opencv.org/) is a package for image processing. It provides useful computation utilities and display functions. An example is provided under the chessboard folder. It will be used in problem 2 & 3 in this homework. **Note: you will need to install OpenCV yourself following [this tutorial](https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html) or simply `sudo apt-get install libopencv-dev python-opencv`.** The second method may be easier for using the BUILD file directly.

## Homework

### 1. Get familiar with Pointcloud data structure
Generally speaking, the Pointcloud is a collection of 3D points. Each point lies in Lidar's local coordinate system where the origin is defined as Lidar center and X axis is pointing forward. The rotation and translation to transform the pointcloud to world coordinate system are also provided. You don't need to worry about those fields for now, they will be covered in later lectures. A simple application has been provided to read the Pointcloud from file (`bazel run -c opt //homework2:pointcloud_reader_main`).

Try to visualize the Pointcloud first (you can refer the icp_viewer implementation provided under icp folder) and get familiar with the 3D world perceived by Lidar, since you will work on the Pointcloud a lot during this course. Our first task is using a histogram to find out the distribution over point's **range** and **height**. Here the range is defined as the length of each point's 3d vector (origin is (0,0,0)) and height is simply its z value. Try to plot those histograms and get deeper insight about how those lidar points distribute.

**What to submit:** Your code (you can use any language for generating the histogram) with two histogram figures and write down a short description about what you find from those histograms.


### 2. Get familiar with transformation representation.
When we want to rotate something in 3D space, we have many representations for the rotation. For example, roll-pitch-yaw is intuitive and used in airplanes; angle-axis is intuitive and clean, and rotation matrix is easy to use in computation. Among these three, rotation matrix is typically not easy for people to imagine. However, if you convert it to another format, you would be clear what it means.

Your task is to implement the conversion from rotation matrix to roll-pitch-yaw and angle-axis representations. For roll-pitch-yaw representation, the rotation intrinsic is roll->pitch->yaw. That is, the rotation sequence is **R(yaw) * R(pitch) * R(roll)**. There may be multiple solutions for the input. You can add a reasonable constraint to the result to get the test passed. For example, you can require `roll >= 0`.

**What to submit:** Your C++ implementation of rotation/rotation_test.cc to pass the unit tests non-trivially. Note, you should implement the conversion by yourself instead of using any existing APIs.


### 3. Chessboard distortion

Chessboard is commonly used in camera calibration. Before calibration, the photo on the chessboard has some distortions like shown in the [chessboard/chessboard.png](https://github.com/ponyai/PublicCourse/blob/master/homework3/chessboard/chessboard.png). Getting the intrinsic parameters is complex. However, if you have the intrinsic parameters, you could get a distorted image easily.

Your task is as follows:
- Load chessboard/chessboard_undistorted.png.
- Apply only radial distortion with k1 = 0.1 and k2 = 0.1 on it. For details distortion model, please refer to [this tutorial](https://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html).
- Output the distorted image.

**What to submit:** Your C++ code to generate chessboard and distorted image, and both images.

### 4. Implement Point-To-Point ICP

During the lecture, we have spent quite effort in learning the ICP algorithm. Now it is time to implement
it by yourself so that you can understand the algorithm more thoroughly.

We have provided you guys the basic template and your task is to follow the comments in **icp.cc** and implement
those two important functions: **FindCorrespondence** and **EstimatePose**. We also provide a viewer tool
for you to better visualize the iterative process. (`bazel run -c opt //homework2/icp:icp_viewer_main`). 
If your implementation is correct, you should be able to see the source pointcloud aligned with the target one after a few iterations (Press **N** for next iteration). 

**What to submit:** Your C++ code for those two functions mentioned above.

**Note that there is one mistake in the slide about the formula of H matrix. Please refer the handout for the correct formula.**
