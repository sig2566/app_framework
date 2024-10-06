# AI automatic C++ application development framework
![Build Status](https://img.shields.io/badge/build-passing-brightgreen) ![License](https://img.shields.io/badge/license-MIT-blue)

The current framework allows creation large C++ applications using modules development approach.

## Overview

This open-source project provides a framework for building large-scale C++ applications using a modular design approach. The framework leverages both AI-generated and handwritten modules that interconnect via a well-defined API. It automates the development process by using AI to generate code for specific modules based on context prompts, simplifying both software design and implementation.

- Modular approach for scalable and maintainable software
- AI-assisted coding for rapid module development
- Simple integration with Python for testing and debugging (optional)

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Architecture](#architecture)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## Architecture

The framework consists of a Python front-end, a Running Support Environment (RSE), and modules that interact via APIs. Below is a diagram of the system:

![Architecture Diagram](https://github.com/sig2566/app_framework/blob/main/AI_devel_framework.png


## Installation

### Requerements

The program was tested with the following environment:  
- Linux Ubuntu.24.04, x86_64  
- Build tools gcc V13  
- Python 3.12.3  
- Boost 1.82  


### Clone the repositories and build:
    ```
   	     
    	mkdir my_proj; cd my_proj  
    	git clone git@github.com:sig2566/app_framework.git  
    	git clone git@github.com:sig2566/rt_debug_lib.git      	
    	cd rt_debug_lib/rt_debug  
      	
    	#Build RT_Debug library  
    	./build.sh
  
    	# Build Application Framework + modules  
    	cd ../../app_framework; make mode==DEBUG  
    	
    ```

## Usage  
### Running 

Modules can be created by providing AI-based prompts or manually coding them. Below is an example of using the Python stub to control the framework:

```python
import PyStub

PyStub.init("path/to/configure.xml")
PyStub.callmod("EXAMPLE_MODULE")
PyStub.prof()


#### 6. **Architecture Diagram**
Use diagrams to visually describe the system architecture. For example, you can add the flowchart or component diagram we generated earlier:

```

## Contributing

Contributions are welcome! Please submit pull requests or file issues for bug reports.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

