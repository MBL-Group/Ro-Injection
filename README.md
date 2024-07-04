# Injection Project

## Overview
This project entails the development of a software application utilizing Visual Studio 2013 and MFC. The application integrates various hardware components, including a camera, microscope, motion stage, injector, pump and two micro manipulators, to establish a comprehensive control system.

<p align="center">
  <img src="Docs/images/Worm injection_GUI.jpg" alt="UI">
  <br>
  <em>UI of injection</em>
</p>


## Development Environment
- **IDE**: Visual Studio 2013
- **Framework**: MFC (Microsoft Foundation Class)

## Dependencies
- **OpenCV2**: For image processing and computer vision tasks.
- **Model 826**: For managing MP285 and MX7600 motion controllers.
- **Pylon**: For camera integration and control.
- **OLYMPUS.sdk**: For microscope settings and control. *Note: Due to licensing issues, this SDK and its source code are not included in this repository. Please contact OLYMPUS directly to obtain the SDK.*

## Directory Structure

### Hardware
- `Schematic.pdf`: Schematic files.
- `System setup.jpg`: Images of the hardware setup.

<p align="center">
  <img src="Hardware\System setup.jpg" alt="System Setup">
  <br>
  <em>System Setup</em>
</p>

### Software
- **PUMP_Driver_Arduino**:
  - `PUMP_ARDUINO.ino`: Files related to the Arduino driver for the pump.
- **Ro-Injection**:
  - `PicturecontrolDlg.cpp`: UI and main function.
  - `camera.cpp`: Camera driver and thread.
  - `mp285.cpp`: MP285 position and speed control.
  - `mhi.cpp`: MHI driver.
  - `injector_pump.cpp`: Injector and pump driver.
  - `mx7600_utils.cpp`: MX7600 utilities.
  - `mx7600.cpp`: MX7600 driver.
  - `SerialPort.cpp`: Serial communication base.
  - `tip_detection.cpp`: Injection pipette tip detection algorithm.
  - `xy_stage_proscan.cpp`: XY stage microscope driver.

### Docs
- `docs`: System setup and UI images.

## Installation and Setup

### Prerequisites
- Visual Studio 2013
- OpenCV2
- Model 826 drivers and libraries
- Pylon SDK
- OLYMPUS SDK (please contact OLYMPUS for access)

### Building the Project
1. Clone the repository:
    ```sh
    git clone https://github.com/MBL-Group/Ro-Injection.git
    cd Ro-Injection
    ```
2. Open the solution file in Visual Studio 2013.
3. Configure the dependencies in your project settings.
4. Build the solution.

### Running the Application
1. Ensure all hardware components are connected and powered on.
2. Launch the application from Visual Studio or by running the executable.
3. Follow the on-screen instructions to operate the system.

## Usage
Detailed instructions for using the application will be provided here. This may include steps for calibrating the hardware, capturing images, and controlling the motion controllers.

## License
This project is licensed under the MIT License. See the `LICENSE` file for more information.

## Acknowledgements
- [OpenCV](https://opencv.org/)
- [Basler Pylon](https://www.baslerweb.com/en/software/pylon/)
- [OLYMPUS](https://www.olympus-lifescience.com/en/)
- [Model826](https://www.sensoray.com/PCI_Express_digital_output_826.htm)
- [MP285](https://www.sutter.com/MICROMANIPULATION/mp285.html)

## Contact
For any inquiries, please submit issues on the GitHub repository.