# Injection Project

## Overview
This project involves the development of a software application using Visual Studio 2013 and MFC. The application integrates various hardware components, including a camera, microscope, and motion controllers, to create a comprehensive control system.

## Development Environment
- **IDE**: Visual Studio 2013
- **Framework**: MFC (Microsoft Foundation Class)

## Dependencies
- **OpenCV2**: For image processing and computer vision tasks.
- **Model 826**: For MP285 and MX7600 motion controller management.
- **Pylon**: For camera integration and control.
- **OLYMPUS.sdk**: For microscope settings and control. *Note: Due to licensing issues, this SDK and its source code are not included in this repository. Please contact OLYMPUS directly to obtain the SDK.*

## Directory Structure

### Hardware
- `xxx.sch`: Schematic files.
- `xxx.jpg`: Images of the hardware setup.

### Software
- **PUMP_Driver_Arduino**:
  - `xxx`: Files related to the Arduino driver for the pump.
- **PC_Controller**:
  - `PicturecontrolDlg.cpp`: UI and main function.
  - `camera.cpp`: camera driver and thread.
  - `mp285.cpp`: MP285 position and speed control mode.
  - `mhi.cpp`: .
  - `injector_pump.cpp`: injector and pump driver.
  - `mx7600_utils.cpp`: mx7600 utils.
  - `mx7600.cpp`: mx7600 driver.
  - `SerialPort.cpp`: serial base. 
  - `tip_detection.cpp`: injection pipette tip detection algorithm.
  - `xy_stage_proscan.cpp`: XY stage of microscope driver. 

### Docs
- `Todo`: Documentation and future work.

## Installation and Setup

### Prerequisites
- Install Visual Studio 2013.
- Ensure you have the following dependencies installed:
  - OpenCV2
  - Model 826 drivers and libraries
  - Pylon SDK
  - OLYMPUS.sdk (contact OLYMPUS for access)

### Building the Project
1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/projectname.git
    cd projectname
    ```
2. Open the solution file in Visual Studio 2013.
3. Configure the dependencies in your project settings.
4. Build the solution.

### Running the Application
1. Ensure all hardware components are connected and powered on.
2. Launch the application from Visual Studio or by running the executable.
3. Follow the on-screen instructions on the ui to operate the system.

## Usage
Detailed instructions for using the application will be provided here. This may include steps for calibrating the hardware, capturing images, and controlling the motion controllers.

## License
This project is licensed under the MIT License. See the `LICENSE` file for more information.

## Acknowledgements
- [OpenCV](https://opencv.org/)
- [Basler Pylon](https://www.baslerweb.com/en/products/software/basler-pylon-camera-software-suite/)
- [OLYMPUS](https://www.olympus-lifescience.com/en/)

## Contact
For any inquiries, please submit issues.
