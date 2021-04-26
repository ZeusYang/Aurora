<p align="center">
  <a href="https://github.com/ZeusYang/TinySoftRenderer">
    <img src="images/logo.jpg" alt="Logo" height="200">
  </a>
  <h3 align="center">Aurora</h3>
<p align="center">
<img src="images/license.svg">
</p>
  <p align="center">
    An offline ray tracing based renderer for fun
    <br />



## About The Project

Aurora, is a learning-oriented rendering system in the style of PBRT. Hence, this project were mostly ported from [pbrt-v3](https://github.com/mmp/pbrt-v3) with some customization and adjustment out of personal habits. I plan to develop this project as a long-term project. Learning by doing!

<img src="images/logo.jpg" alt="Logo" width="100%">



### Built With

This project is built with following third parties.
* [GLM](https://github.com/g-truc/glm) for fast algebra and math calculation
* [stb_image](https://github.com/nothings/stb) for image I/O
* [oneTBB](https://github.com/oneapi-src/oneTBB/releases/tag/v2021.1.1) for cpu-based parallel tiling rendering
* [glog](https://github.com/google/glog) for logging and debuging



## Getting Started

### Prerequisites

 I build this project on Windows platform. Please make sure your system is equipped with the following softwares.  

- [cmake](https://cmake.org/)：at least version 3.5

* Microsoft visual studio 2017 or 2019
  

### Installation

Please compile the project for **x64 platform**.

1. Clone the repo
   ```sh
   git clone --recursive-submodules https://github.com/ZeusYang/Aurora.git
   ```
   
2. Use cmake to build the project：
   
   ```
   cd build
   cmake ..
   make
   ```
   
   or using cmake-gui is ok.

Please make sure that clone the reposity with `--recursive-submodules` option. If you foget to run `git clone` with it, just type the command `git submodule update --init --recursive`.



## Usage

To be planned. At present, the basic framework was established.



## License

Distributed under the MIT License. See `LICENSE` for more information.



## Contact

yangwc3@mail2.sysu.edu.cn

