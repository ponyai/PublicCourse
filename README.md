# Public course from pony.ai
In this course you'll learn basic knowledge about autonomous car and complete essential algorithms
in autonomous car simulation system. In the following weeks, you'll get some real data recorded during
our road test. Your task is writing code to recognise around obstacles, plan a best path to
avoid them and make autonomous car run along the road for its destination.

You can access the code and document for homework here.

Let's begin now!!!

# System setup

A shared display library with some utility classes has been introduced into the codebase. We will provide several visualization tools based on this tool. To use this tool, you need to run following commands to install several system dependencies. 

```
sudo apt install qtdeclarative5-dev clang-6.0 nasm
```

**QT** is a cross-platform application framework. Our visualization tools will be developed based on it. 

**Clang** is the compiler we use to build the libraries. We require you to install `clang-6.0` to avoid any potential issues caused by compiler version. 

**nasm** is a required library for an introduced third-party library. 


# Visualization Issue Troubleshooting

Our visualization tool is based on OpenGL 3D rendering. We recommend using Ubuntu16.04 native system to get the best visualization effect, since 3D acceleration inside virtual machines is just experimental features. If you must use virtual machine for some reasons, we recommend [VirtualBox](https://www.virtualbox.org/wiki/Downloads), which is free and cross-platform.  

If you are using VMWare and 3D rendering is not correct, please try command `export SVGA_VGPU10=0` to set an environment variable, and re-run the binary again to check if it solves your issue. 


