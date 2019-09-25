# Arduino Weather Station
## Design Document

##### Pitt Vibrant Media Lab


### Introduction

The purpose of this document is to outline the design process of the Pitt Vibrant Media Lab's Arduino Weather Station modules. It lays out the requirements and goals of the project, as well as the basic design and a rough estimation of the schedule for completion.

The Arduino Weather Station module is intended to be a compact, portable, fully functioning weather monitoring system for distributed tracking applications. It should record a number of different weather metrics per the needs of the individual user, and be able to store a history of these records both locally and remotely. Each module will consist of a central Arduino Uno or Mega, which will receive readings from peripheral sensors and process the data to an onboard memory system, as well as send the data via Ethernet connection to a remote server.

The project aims to be easy for non-technical people to implement their own versions of, while transparent enough for more knowledgeable users to make changes to the source code and hardware. As an open source effort, all programs, instructions, and custom part templates will be made publicly available without cost.

This endeavor has a number of potential applications aside basic weather monitoring. Fire safty monitoring and distributed storm tracking could both benefit from mass public distribution, which allows for both greater acquisition and improved dissemination of information. With the onset of more unprdictable weather patterns due to climate change, the need for greater public involvement in monitoring will be further highlighted.

### Goals

At the highest level, the goal of this project is to provide means for the general public to access hyperlocal weather information readily, as well as to form monitoring networks over areas where traditional station might not be feasible to construct and maintain. We also aim to create a system that is easy to create and prepare, and which requires little to no regular maintence.

### Functional Requirements

System requirements are broken into the following categories:

#### Sensors and Monitoring

The default module should have capabilities for monitoring the following:

- Ambient Temperature
- Barometric Pressure
- Relative Humidity
- Air Quality

Additional support should be made avalable for additional sensing, including:

- Rain Rate
- Wind Speed and Direction
- Fire Hazard


These sensors should all have variable update times, accessible to the user for fine tuning. Additionally, while certain models will be directly programmed for, monitoring software should be modularized such that equivalently functioning code for a given sensor replacement can directly replace existing code. Such replacements should be regularly inculded with the project codebase to allow greater reach to users.

All sensors should also be fully separable from the central unit. This is to say that any sensor connection failure occuring at runtime should be detected by the system, which should respond by terminating data queries to that sensor, logging the failure, and notifying the user.

#### Local Storage

The module should maintain local storage using SD or MicroSD format, which can be removed and manually inspected if the device becomes disconnected. The system should log data points with frequency specified by the user, exportable as CSV. It should also maintain a textual log of system events, such as sensor faiures, DHCP lease renewals, and network changes.

Local storage should be separable from the central unit, provided a valid network connection allows for data recording. In the case that the network becomes disconnected and their is no SD card present to record data, the unit should attempt to reestablish connections for a period, then power down to protect the lifetimes of its sensors. This will be outlined further in the follownig section.

#### Ethernet/Internet Capabilities

The unit should have capabilities for connecting to the internet to upload data and messages to a backend server application. Users should be able to remotely view recent measurements, and receive notification when system failures occur. The network should be configurable using either DHCP protocol or a static IP address provided by the user, and should perform its own configuration and maintenance calls to keep a live connection.

Every unit should be client to a single server, and multiple units should access the same server. Servers should also transmit data from certain devices between one another on request.

### Design Overview

#### Hardware Specifications

The core unit of each module consists of an Arduino Uno/Mega, paired with a compatible ethernet shield and SD card reader. Attached serially to the Arduino are a number of peripheral sensors and devices, which provide environmental measurements using SPI protocol. All peripherals are hardwired into the main control and the entire unit is housed in custom modeled housing printed in PLA.

Voltage is provided to the sensors via the Arduino, the input to which should be 7-12 V. Using an onboard voltage regulator, the Arduino is able to appropriately power a number of devices concurrently, giving each its necessary amperage. See the schematics folder for [wiring diagrams](../schematics/wiring/) and [3D models](../schematics/models/).

#### Onboard Software Specifications

Deployed units store a single program, called [onboard.ino](../code/onboard/onboard.ino), which directs all the data collection, storage, and communication. This program has a number of [dependencies](../code/Libraries), which must be included for proper function. Critical dependencies include `Ethernet`, `SD`, and `SDI`. All dependencies ship with the final project, and can also be accessed remotely via the Arduino IDE.

Documentation of the onboard program can be found [here](../code/documentation/onboard_documentation.md).

#### Backend Server Specifications

As of 6 September 2019, backend specifications and capabilities have not been determined. Future code will be found [here](../code/server).

### Schedule and Deliverables

As of 6 September 2019, no schedule has been determined for this project.
