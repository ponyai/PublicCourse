# Platform Manual

## Simulation System

SimulationSystem is the top-level class in our platform. It takes a simulation config as an input, and runs the defined simulation tasks and record simulation log and metrics. 

Basic Usage:
```
bazel run -c opt //pnc:simulation_main -- --multi_process_mode=false
```

By default, it will read the config file in `pnc/data/sample.config`, and run a simulation based on that configuration. The log file will be saved to `/tmp/simulation_log.bin` and metrics will be saved to `/tmp/simulation_metrics.txt`. 

### Useful flags

#### --simulation_config_path
The file path the configuration file. If this flag is not specified, config in `pnc/data/sample.config` will be read. 
#### --multi_process_mode
Whether to run simulation in multi-process mode
#### --enable_visualization
Whether to enable visualization UI window
#### --simulation_log_file_path
The file path of generated simulation log
#### --simulation_metrics_file_path
The file path of generated simulation metrics 
#### --srand_by_time
Whether to set random seed as system time

### Simulation Config

In simulation configs, you could define a simulation task. The proto definition of simulation config could be found at `common/proto/simulation_config.proto`. It contains the agents in a simulation task, the rules, and number of trips. 

Note that all of the rules are configurable. You could choose to enable some rules and disable some rules. It will be helpful when your vehicle is in development and it cannot handle all rules properly. In final evaluation, all of the rules will be enabled. 

A sample simulation config could be found at `pnc/data/sample.config`.

### Simulation Metric

Once simulation is finished, a simulation metric file will be generated at `FLAGS_simulation_log_file_path`. It should looks like this:
```
agent_metric {
  name: "xuan1"
  num_finished_trips: 2
  big_acc_sqr_sum_total: 1323.3140835087761
  big_acc_sqr_sum_per_trip: 661.657041754388
  curvature_sqr_sum_total: 13414375.794555988
  curvature_sqr_sum_per_trip: 6707187.8972779941
  time_cost_total: 256.06999999991211
  time_cost_per_trip: 128.03499999995606
}
agent_metric {
  name: "xuan2"
  num_finished_trips: 2
  big_acc_sqr_sum_total: 1337.5148113603545
  big_acc_sqr_sum_per_trip: 668.75740568017727
  curvature_sqr_sum_total: 1328383.8747587984
  curvature_sqr_sum_per_trip: 664191.9373793992
  time_cost_total: 174.86999999998594
  time_cost_per_trip: 87.434999999992968
}
```

### Replay

We provide a tool `replay_main` to help you easily replay a simulation. You could use it to replay a simulation task without running all agents again. Usage:

```
bazel run -c opt //pnc:replay_main -- --simulation_log_file_path={file_path}                    
```

Note that you could double-click on the slider to perform seek operation in replay. 

## Simulation Engine

Simulation engine is the internal engines that run agents and collect their control commands. Here we provide two simulation engines: 

### Single-threaded Simulation Engine

In single-threaded engine, all vehicles’ iterations are invoked sequentially in one thread. Since there is only one thread besides UI, no timeout interrupt will be supported. 

### Multi-processed Simulation Engine

In multi-processed engine, all vehicles are running in a dedicated process. They communicate with a simulation server via tcp socket. This will be used as final evaluation environment to avoid potential segment fault, timeout and memory leak in one agent. 

You could use multi-processed simulation engine by enable flag `--multi_process_mode`. 

## UI 

### Playback Commands

You could press following keyboard keys to control the playback progress. 

1. `SPACE`: Pause the simulation. 
2. `+`: Speed up the simulation. 
3. `-`: Slow down the simulation. 
4. `RIGHT ARROW`: Run next iteration (only available when simulation is paused). 
5. `CTRL + Q`: Terminate the simulation. 

### Observation Perspective

You could change your observation perspective in menu. 

1. God Perspective. You could see all vehicles and all pedestrians with this perspective. You could freely adjust the camera by mouse buttons and wheel. 
2. Vehicle Follow Perspective. The camera will follow the selected agent. Only obstacles that the vehicle can see will be drawn. That is to say, vehicles and pedestrians in 100 meters will be displayed. 

### Keyboard Controlled Agent

You could add a keyboard controlled agent into simulation. Just add `add_keyboard_controlled_agent: true` to enable it. And you could press `SHIFT + UP/DOWN/LEFT/RIGHT` to control the throttle, brake and steering angle. 

It's mostly useful to get familiar with the rules. You could control the agent by keyboard to test the exact elimination criteria. Another usage of this tool is to test your agent's reaction to this agent that you manually controlled.

### Variable View

Note that in the right side of the UI window, there is a variable lists which show all vehicle's status. 

You could also publish your own variables for debug purposes. The published variables will also be displayed in that UI widget. And the variables will be saved into simulation log, and could be viewed in replay systems. See `pnc/agents/sample/sample_agent.h` as an example. 

