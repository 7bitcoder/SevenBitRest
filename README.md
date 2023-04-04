[![Ubuntu](https://github.com/7bitcoder/SevenBitRest/actions/workflows/Ubuntu.yml/badge.svg?branch=main)](https://github.com/7bitcoder/SevenBitRest/actions/workflows/Ubuntu.yml)
[![Windows](https://github.com/7bitcoder/SevenBitRest/actions/workflows/Windows.yml/badge.svg?branch=main)](https://github.com/7bitcoder/SevenBitRest/actions/workflows/Windows.yml)
[![MacOs](https://github.com/7bitcoder/SevenBitRest/actions/workflows/MacOs.yml/badge.svg?branch=main)](https://github.com/7bitcoder/SevenBitRest/actions/workflows/MacOs.yml)

<div align="center">

  <img src="7bitrest - logo.svg" alt="logo" width="500" height="auto" />
  <p>
    C++20 simple http rest framework! 
  </p>
   
<h4>
    <a href="https://github.com/7bitcoder/SevenBitRest/">Home</a>
  <span> · </span>
    <a href="https://github.com/7bitcoder/SevenBitRest/tree/main/Docs">Documentation</a>
  <span> · </span>
    <a href="https://github.com/7bitcoder/SevenBitRest/issues/">Report Bug</a>
  <span> · </span>
    <a href="https://github.com/7bitcoder/SevenBitRest/issues/">Request Feature</a>
  </h4>
</div>

<br />
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#build-framework-locally">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a>
      <ul>
        <li><a href="#hello-world">Hello World</a></li>
        <li><a href="#dependency-injector">Dependency Injector</a></li>
        <li><a href="#middlewares">Middlewares</a></li>
      </ul>
    </li>
    <li><a href="#prebuild-binaries">Prebuild Binaries</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
</details>

## About The Project

SevenBitRest is a simple C++ http rest framework, designed to be as easy to use as possible, the main inspiration was asp net core minimal api.

### Built With

* [Boost Beast](https://github.com/boostorg/beast)
* [Boost Url](https://github.com/boostorg/url)
* [Taocpp Json](https://github.com/taocpp/json)
* [cli11](https://github.com/CLIUtils/CLI11)
  
#### Testing Utilities
* [Google Test](https://github.com/google/googletest)
* [Google Banchmark](https://github.com/google/benchmark)
* [Curl](https://github.com/curl/curl)

## Getting Started

To use library download prebuild binaries and link dynamic library, or build project locally 

### Prerequisites

Install Cmake if not already installed:

https://cmake.org/

Install Conan:

https://conan.io/

### Build Framework Locally

Framework uses BoostUrl library, it is not yet part of official boost library so it must be installed separately (clone git repo in BoostUrl directory), also to build project benchmarks/examples/tests cmake cache flags should be set (using cmake-gui or in cli):

* Benchmarks - BUILD_BENCHMARKS
* Examples - BUILD_EXAMPLES
* Tests - BUILD_TESTS

#### Prepare

1. Clone the repo
    ```sh
    git clone https://github.com/7bitcoder/SevenBitRest.git
    ```

#### Install Conan Packages

1. Create and navigate to build directory
    ```sh
    mkdir build && cd ./build
    ```
3. Install conan packages
    ```sh
    conan install --build=missing ..
    ```
4. Navigate up
    ```sh
    cd ..
    ```

#### Install BoostUrl Library

1. Create and navigate to BoostUrl directory
    ```sh
    mkdir BoostUrl && cd ./BoostUrl
    ```
2. Clone BoostUrl repository
    ```sh
    git clone https://github.com/boostorg/url.git
    ```
3. Navigate up
    ```sh
    cd ..
    ```

#### Build Framework

1.  Configure project
    ```sh
    cmake -B ./build 
    ```
2.  Build project
    ```sh
    cmake --build ./build
    ```

## Usage

Framework uses asp net core minimal api approach, to build endpoints use Map functions on SevenBitRest object. If not specified app will run on 9090 port by default.

_For more examples, please refer to the [Documentation](https://github.com/7bitcoder/SevenBitRest/tree/main/Docs)_

### Hello World

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=./Examples/HelloWorld/Basic.cpp) -->
<!-- The below code snippet is automatically added from ./Examples/Basic/HelloWorld.cpp -->
```cpp
#include "SevenBitRest.hpp"

using namespace std::string_literals;
using namespace sd;

int main()
{
    SevenBitRest rest;

    rest.mapGet("/", []() { return "Hello, world!"s; });

    rest.run();
}
```
<!-- MARKDOWN-AUTO-DOCS:END -->

Open link http://localhost:9090 to see results 

### Dependency Injection

Framework has build in dependency injection system, usage looks similar to asp net core

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=./Examples/ServiceProvider/Basic.cpp) -->
<!-- The below code snippet is automatically added from ./Examples/ServiceProvider/HelloWorld.cpp -->
```cpp
#include "SevenBitRest.hpp"

using namespace std;
using namespace sd;

struct Service
{
    string helloFromService() { return "Hello from service."; }
};

int main()
{
    SevenBitRest rest;

    auto &services = rest.getServices();

    services.addScoped<Service>();

    rest.mapGet("/", [](Service &service) { return service.helloFromService(); });

    rest.run();
}
```
<!-- MARKDOWN-AUTO-DOCS:END -->

### Middlewares

7BitRest has also middleware system that is well known from other frameworks

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=./Examples/Middlewares/Basic.cpp) -->
<!-- MARKDOWN-AUTO-DOCS:END -->

## Prebuild Binaries

<!-- MARKDOWN-AUTO-DOCS:START (WORKFLOW_ARTIFACT_TABLE) -->
<!-- MARKDOWN-AUTO-DOCS:END -->

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

## Contact

Project Link: [https://github.com/7bitcoder/SevenBitRest](https://github.com/7bitcoder/SevenBitRest)

@7bitcoder 2022
