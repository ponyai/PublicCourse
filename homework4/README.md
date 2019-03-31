# homework4

As you may know, one of the final projects will ask you to implement a self-driving car agent that can run in the virtual world. It is an exciting, but also complicated project. Thus, this homework and next homework are designed to help you understand the final project piece by piece. In this homework, you will get familiar with the map format in the final project. 

## Homework

### 1. Get familiar with the "grid2" map. 

The first task in this homework is to get familiar with the "grid2" map. The map is made up of a set of lanes. You could see the protobuf definition in `common/proto/map.proto`. Each lane is primarily defined by a unique id, a central line, left and right boundary and a speed limit. Note that all of the coordinates in the protobuf definition are in the world coordinate systems. 

SVG file `homework4/map/grid2/map.svg` could give you a first glance of the map. You could see that the map contains 4 (2*2) blocks. Each block is surrounded by two lanes in different directions. Note that all of the lanes are directed. Same as Chinese traffic rule, vehicles must drive in the right-side lane in the map. 

Then you could use the tool `map_visualizer` to visualize the map. Run following command to start the tool:

```
bazel run -c opt //homework4:map_visualizer_main
```

You could press the left or right mouse button to adjust the observing angle to the map, and use the mouse wheel to adjust the observing distance to the map. You could also click on the lane id in the right-side widget to highlight any lane. 

### 2. Find predecessors and successors of the lanes. 

In the lane's protobuf definition, there are two fields `predecessor` and `successor` defined. However, they are missing in the protobuf text file that provided to you. In this task, you are going to find predecessors and successors for all lanes. 

If lane `B` is a `successor` of another lane `A`, it means a vehicle could drive from lane `A` to lane `B` without passing another lane. And lane `A` is also considered as a predecessor of lane `B`. Note that one lane may have multiple successors or predecessors. For example, lane `L1`'s successors are `L25`, `L26`, and its predecessor is `L58`. Also note that lanes may have overlaps with each other, but they may not have predecessor/successor relationships, such as `L25` and `L26`. 

**What to submit:** 
1. A map's protobuf text file named `processed_map_proto.txt` in which `predecessor` and `successor` fields have been filled. 
2. Your source code that generates the predecessors and successors. Brief comments are required in the source code for others to understand your code. You could use `homework4::MapLib` in `homework4/map_lib.h` to get the map's original protobuf object and use `file::WriteProtoToTextFile` in `common/utils/file/file.h` to dump the protobuf object into text file. 


### 3. Find an appropriate route from one point to another point in the map. 

When you drive a car to a destination in real life, you should firstly figure out an appropriate route to the destination. In this task, you are going to find an appropriate route from a start point to a destination in `grid2` map. 

The protobuf definition of a route could be found in `common/proto/route.proto`. It contains a `start_point`, a `end_point` and several `route_point`s which are sampled points. The sample distance between two adjacent points is less than 4m. You could find two examples of routes at `homework4/data/routes/sample1.txt` and `homework4/data/routes/sample2.txt`. You could also use `map_visualizer` to visualize the route by following commands:

```
bazel run -c opt //homework4:map_visualizer_main -- --route_file_path {PATH_TO_CODEBASE}/PublicCourse/homework4/data/routes/sample1.txt
bazel run -c opt //homework4:map_visualizer_main -- --route_file_path {PATH_TO_CODEBASE}/PublicCourse/homework4/data/routes/sample2.txt
```

In the window, the green flag represents the `start_point`, and the red flag represents the `end_point`. The route connected by `route_point` is highlighted with green color. 

You could find five route requests named `route_request_x.txt` in the directory `homework4/data/routes/`. In these files, `start_point` and `end_point` of the route are specified, but the `route_point`s are missing. Your task is to find an appropriate route from `start_point` to `end_point`. Note that the route must follow the lane's direction. 

**What to submit:** 
1. Five protobuf text files named `route_result_x.txt` which contain the route from `start_point` to `end_point`, and the corresponding screenshots of the routes taken from `map_visualizer`. Please read the `common/proto/route.proto` to understand the requirement of the `route_point`. 
2. The source code that generates the routes. Brief comments are required in the source code for others to understand your code. 

