# AI automatic C++ application development framework
![Build Status](https://img.shields.io/badge/build-passing-brightgreen) ![License](https://img.shields.io/badge/license-MIT-blue)

The current framework allows creation large C++ applications using modules development approach.

## Overview

This open-source project provides a framework for building large-scale C++ applications using a modular design approach. The framework leverages both AI-generated and handwritten modules that interconnect via a well-defined API. It automates the development process by using AI to generate code for specific modules based on context prompts, simplifying both software design and implementation.

- Modular approach for scalable and maintainable software
- AI-assisted coding for rapid module development
- Simple integration with Python for testing and debugging (optional)

## Table of Contents
- [Architecture](#architecture)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Architecture

### Architecture Diagram

![Architecture Diagram](https://github.com/sig2566/app_framework/blob/main/AI_devel_framework.png)


### Main components

The AI SW development framework consists of the following components:  
- **Modules**. This modules actully performs the work. They expose common API, which allows to use them together in the application. Modules may expose additional APIs to support some specific features to other modules or they may be connected with other modules using FIFO and double buffer objects. Modules may be linked together as shared objects in run time or as static libraries during linking.Using the modules design approach has the following advantages:  
	-- Support for **automatic** C++ code generation with **AI** tools, like **chatGPT**. Since the module structure and its API are well defined, it is possible to define the general module code generation with the common **context** prompt. The second **task** prompt defines specific module features. Together these two prompts allows **automatic module code generation**.  
	-- The system design and redesign is simple. It is just the connection and reconnection of modules.  
	-- The modules may be developed separately, in parallel, some of them by the third party providers. In that case the third parties providers may not be exposed to the all system code.  
	-- The module design simplifies verification. It is possible easy to implement the module level and system level verification.  
	-- Support fast prototyping.  
- **Running Support Environment (RSE)**. This component implements the following features:  
	-- It supports loading modules, their interconnection, initialization and running.  
	-- The system configuration with the initialization XML file. This file contain definition of the used modules and their initialization data.  
	-- It exports the control and debug APIs. The upper (frontend) application may control the system behavior using these APIs.  
- **Python frontend** application PyStub is connected with RSE with its APIs. PyStub allows integration of RSE with Python script. It is loaded as PyStub package. Example:  
```  
import PyStub  
ver = PyStuv.ver()  
PyStub.init("../FRAMEWORK/RUN_ENV/sys_configure.xml")        
#prints/get list of modules names  
print(PyStub.modulenames())  
#prints/get list of mem areas names  
print(PyStub.areanames())  
#Call module stand alone  
PyStub.callmod("SAMPLE_MODULE")  
# prints/get profiling results  
PyStub.stop()  
```


## Installation

### Requerements

The program was tested with the following environment:  
- Linux Ubuntu.24.04, x86_64  
- Build tools gcc V13  
- Python 3.12.3  
- Boost 1.82 
- chatGPT (Copilot or 4o) 


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

```
cd bin
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
python 
import PyStub

PyStub.init("path/to/configure.xml")
PyStub.callmod("EXAMPLE_MODULE")
PyStub.prof()
```

### Module automatic generation using AI tools
This feature was tested with chatGPT 4o or Copilot.  
- Open chatGPT prompt  
- Copy and past the FRAMEWORK/AI_PROMPTS/module_context.txt file into the chat.  
- Check that the module code is generated for the sort module.  
- Write your task prompt. **Example:** "[task] Please implement the API and module for FFT calculation"  
- Copy/past the generated code into the sources.  



## Contributing

Contributions are welcome! Please submit pull requests or file issues for bug reports.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact
If you have any questions, suggestions, or issues, feel free to contact the project maintainers at:  
Email: sig2566@gmail.com    
We welcome feedback and will try to respond as quickly as possible.  

## Warranty
This software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non-infringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.

