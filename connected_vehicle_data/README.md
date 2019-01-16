# Connected Vehicles Data

This dataset contains the periodic log of 20+ connected vehicles.  

The logs are collected and processed into the data tables below.

Each vehicle is identified by a `{device}` number. 

Each commute of a vehicle is identified by a `{trip}` number.

The summary of all {device}s and {trip}s is aggregated in the `summary` table.

The dataset is accessible through a networked API. 

__Important Notes__:

When making requests to the API:

1. Make sure to include the provided `key` in the header of every request.
2. The output format is based on `json`
3. Some of the API endpoints give back so much data that you will get a 404 error if you try to reach them without specifying query attributes. Make sure you specify a {device} and/or {trip} to your request.

## Table of Contents
1. [Connection Requirements](#connection-requirements)
2. [Sample Code](#sample-code)
3. [Dataset Tables](#dataset-tables)
4. [Available APIs](#available-apis)
5. [Data Dictionaries](#data-dictionaries)


## Connection Requirements

Method: GET

Request Headers:

Key=

---

## Sample Code

```python
# Making a Python request

import requests

url = "https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/evtwarn"

headers = {
    'key' : "************",
    'cache-control' : "no-cache"
}

response = requests.request("GET", url, headers=headers)

print(response.text)
```



---

## Dataset Tables

| **Table Name**      | **Description**                                              | **Type**   |
| :------------------ | ------------------------------------------------------------ | ---------- |
| **Summary**         | Aggregated summary per vehicle per trip.   Each ignition cycle   generates only one summary record. | Log        |
| **Host**            | Periodic log of host vehicle’s internal signal               | Log        |
| **RvBsm**           | The messages **received** by the host vehicle from remote vehicle | Log        |
| **EvtWarn**         | Log of all the **events** observed by the host vehicle   Each event of interest creates a one-record   snapshot of the kinematics | Log        |
| **Spat**            | The messages received from intersection unit.   One record each 0.5 sec that is   received by the host | Log        |
| **RvZone**          | Relative location of the remote vehicle                      | Dictionary |
| **RvBasicVehClass** | Basic type/class of the remote vehicle                       | Dictionary |
| **AlertLevel**      | Level of the event                                           | Dictionary |
| **EventAppId**      | Type of event/alert                                          | Dictionary |

## Available APIs

Each table is accessible through the following APIs using {device} and {trip} numbers. 

### Log URIs:

---

#### EvtWarn          

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/evtwarn?device={device}

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/evtwarn?device={device}&trip={trip}


#### Host

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/host?device={device}

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/host?device={device}&trip={trip}
                     

#### RvBsm

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/rvbsm?device={device}

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/rvbsm?device={device}&trip={trip}
                         

#### Spat

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/spat?device={device}

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/spat?device={device}&trip={trip}
    

#### Summary

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/summary

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/summary?device={device}

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/summary?device={device}&trip={trip}

 

### Dictionary URIs:

---

#### AlertLevel

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/alertlevel

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/alertlevel/{id}



#### EventAppId

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/eventappid

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/eventappid/{id}

 

#### RvBasicVehClass

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/rvbasicvehclass

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/rvbasicvehclass/{id}

 

#### RvZone

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/rvzone

https://ipacdev.hondaresearch.com:8443/hackathon/hondadsrc/rvzone/{id}

---

## Data Dictionaries

Data dictionaries for each table are provided below.

| Summary Table | **Type** | Units      | **Description**                                              | **Range**      |
| ------------- | -------- | ---------- | ------------------------------------------------------------ | -------------- |
| Device        | int32    |            | Randomly   selected number between 2000 and 3000             | 2000 to 3000   |
| Trip          | int32    |            | Sequential   trip number                                     | 0 - 2147483647 |
| StartTime     | int32    | centi-secs | Start   of Trip Time in centi-seconds                        | 0 - 2147483647 |
| EndTime       | int32    | centi-secs | End   of Trip Time in centi-seconds                          | 0 - 2147483647 |
| UTCTime       | long64   | ms         | Coordinated   Universal Time                                 | N/A            |
| TripStart     | Double   | Days       | Serial Date/Time value (UTC) corresponding to trip time = 0   (Ref: January 1, 1900) | N/A            |
| TodTripStart  | Double   | Days       | Serial Date/Time value (Central Time corrected for DST)   corresponding to trip time = 0 (Ref: January 1, 1900) | N/A            |

 

---

| EvtWarn Table       | **Type** | Units      | **Description**                                              | **Range**                                |
| ------------------- | -------- | ---------- | ------------------------------------------------------------ | ---------------------------------------- |
| Device              | int32    |            | Randomly selected   number between 2000 and 3000             | 2000 to 3000                             |
| Trip                | int32    |            | Sequential trip   number                                     | 0 - 2147483647                           |
| Time                | int32    | centi-secs | Sequential time   start at 0 and increasing by 10 (10 hz)    | 0 - 2147483647                           |
| NativeFlag          | tinyint  |            | Flag indicating a   match between Time and Localtime (within a 0.1 s) | 0 - 1                                    |
| LocalTimeMS         |          | ms         |                                                              | 0 - 86400000                             |
| PsId                | uint32   |            |                                                              | 0 - 0xFFFFFFFF                           |
| RvBasicVehicleClass | enum     |            | car, truck, bicycle,   pedestrian, etc.                      |                                          |
| RvRandomId          | uint16   |            |                                                              | 0 - 0xFFFF                               |
| RvDevice            | uint16   |            |                                                              | 0 - 0xFFFF                               |
| AlertLevel          | tinyint  | enum       |                                                              |                                          |
| EventAppId          | enum     |            | Showing which   application transitioned to Inform or Warn   |                                          |
| RvZone              | enum     |            | Showing the location   of the Rv at the time of the Inform or Warn |                                          |
| RvLatitude          | double   | deg        |                                                              | -90.0 - 90.0                             |
| RvLongitude         | double   | deg        |                                                              | -180.0 - 180.0                           |
| RvHeading           | float    | deg        |                                                              | 0 - 360.0                                |
| RvElevation         | float    | m          |                                                              | -409.5 - 6143.9                          |
| RvSpeed             | float    | m/s        |                                                              | -55.5556 - 55.5556                       |
| RvBrakeStatus       | tinyint  |            |                                                              | 0 = Brake Not Active    1 = Brake Active |
| RvYawRate           | float    | deg/s      |                                                              | -327.68 - 327.67                         |
| RvLongitudinalAccel | float    | m/s2       |                                                              | -15.0 - 15.0                             |
| RvTurnSignal        | tinyint  | enum       | 0 = unknown/off; 1 =   Left; 2 = right; 3 = both             |                                          |
| RvEventFlags        | tinyint  | enum       | Bit 0 = Hazard   lights    Bit 1 = Stop line violation    Bit 2 = ABS activated    Bit 3 = Traction ctrl loss    Bit 4 = Stability ctrl activated    Bit 5 = Hazardous materials    Bit 6 = Reserved    Bit 7 = Hard braking    Bit 8 = Lights changed    Bit 9 = Wipers changed    Bit 10 = Flat tire    Bit 11 = Disabled vehicle    Bit 12 = Air Bag Deployed |                                          |
| RvRange             | float    | m          |                                                              | 0 - 655.35                               |
| RvRangeRate         | float    | m/s        |                                                              | -400.0    -  400.0                       |
| RvLongOffset        | float    | m          |                                                              |                                          |
| RvLatOffset         | float    | m          |                                                              |                                          |
| HvLatitude          | double   | deg        |                                                              | -90.0 - 90.0                             |
| HvLongitude         | double   | deg        |                                                              | -180.0 - 180.0                           |
| HvHeading           | float    | deg        |                                                              | 0 - 360.0                                |
| HvElevation         | float    | m          |                                                              | -409.5 - 6143.9                          |
| HvSpeed             | float    | m/s        |                                                              | -55.5556 - 55.5556                       |
| HvBrakeStatus       | tinyint  |            |                                                              | 0 = Brake Not Active    1 = Brake Active |
| HvYawRate           | float    | deg/s      |                                                              | -327.68 - 327.67                         |
| HvLongitudinalAccel | float    | m/s2       |                                                              | -15.0    -  15.0                         |
| HvTurnSignal        | tinyint  | enum       | 0 = unknown/off; 1 =   Left; 2 = right; 3 = both             |                                          |

 

---

| Host Table                                         | **Type** | Units                  | **Description**                                              | **Range**                                                    |
| -------------------------------------------------- | -------- | ---------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Device                                             | int32    |                        | Randomly number   between 2000 and 3000                      | 2000 to 3000                                                 |
| Trip                                               | int32    |                        | Sequential trip   number                                     | 0 - 2147483647                                               |
| Time                                               | int32    | centi-secs             | Sequential trip time   starting at 0 and increasing by 50 (2 hz) | 0 - 2147483647                                               |
| NativeFlag                                         | tinyint  |                        | Flag indicating a   match between Time and Localtime (within a 0.1 s) | 0 - 1                                                        |
| LocalTimeMS                                        | uint32   | ms                     |                                                              | 0 - 86400000                                                 |
| Latitude                                           | double   | deg                    |                                                              | -90.0 - 90.0                                                 |
| Longitude                                          | double   | deg                    |                                                              | -180.0 - 180.0                                               |
| Elevation                                          | float    | m                      |                                                              | -409.5 - 6143.9                                              |
| Heading                                            | float    | deg                    |                                                              | 0 - 360.0                                                    |
| GpsSpeed                                           | float    | m/s                    |                                                              | -55.5556 - 55.5556                                           |
| BrakeStatus                                        | tinyint  |                        | Boolean: (0 = off; 1   = on)                                 | 0 = Brake Not Active    1 = Brake Active                     |
| Speed                                              | float    | m/s                    |                                                              | -55.5556 - 55.5556                                           |
| YawRate                                            | float    | deg/s                  |                                                              | -327.68 - 327.67                                             |
| LongitudinalAccel                                  | float    | m/s2                   |                                                              | -15.0    -  15.0                                             |
| Throttle Position                                  | tinyint  | %                      |                                                              | 0 - 100                                                      |
| Steering Wheel Angle                               | float    | deg                    |                                                              | -450 - 450                                                   |
| TurnSignal                                         | enum     |                        | enum: 0 = off; 1 =   left; 2 = right; 3 = both               | 0 = Off    1 = Left    2 = Right    3 = Both                 |
| Headlamp                                           | enum     |                        | enum: 0 = off; 1 =   parking; 2 = low; 3 = high              | 0 = Off    1 = Parking    2 = Low    3 = Hight               |
| Wiper                                              | enum     |                        | enum: 0 = off; 1 =   delay; 2 = low; 3 = high                | 0 = Off    1 = Delay    2 = Low    3 = High                  |
| TransmissionState                                  | enum     |                        | enum: 0 = Neutral; 1   = Park; 2 = Forward gears; 3 = Reverse gears | 0 = Neutral    1 = Park    2 = Forward gears    3 = Reverse gears |
| Stability Control                                  | tinyint  |                        | Boolean: (0 = off; 1   = on)                                 | 0 = Off, 1 = On                                              |
| ABS                                                | tinyint  |                        | Boolean: (0 = off; 1   = on)                                 | 0 = Off, 1 = On                                              |
| Traction Control                                   | tinyint  |                        | Boolean: (0 = off; 1   = on)                                 | 0 = Off, 1 = On                                              |
| GpsFixType                                         | tinyint  |                        |                                                              | 0 = Invalid    1 = GPS fix    2 = DGPS fix    4 = RTK fix    5 = Float RTK fix    6 = Dead reckoning    7 = Manual fix    8 = Simulated fix    9 = WAAS |
| Semi-major Axis Accuracy                           | float    | m                      |                                                              | 0 - 12.7m                                                    |
| Semi-minor Axis Accuracy                           | float    | m                      |                                                              | 0 - 12.7m                                                    |
| Orientation of Semi-major axis of    Error Ellipse | float    | deg from    true north |                                                              | 0 - 360                                                      |

 

---

| RvBsm Table         | **Type** | Units      | **Description**                                              | **Range**                                                    |
| ------------------- | -------- | ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Device              | int32    |            | between 2000 and   3000                                      | 2000 to 3000                                                 |
| Trip                | int32    |            | Sequential trip   number                                     | 0 - 2147483647                                               |
| Time                | int32    | centi-secs | Sequential trip time   starting at 0 and increasing by 50 (2 hz) in centi-seconds | 0 - 2147483647                                               |
| NativeFlag          | tinyint  |            |                                                              | 0 - 1                                                        |
| LocalTimeMS         | uint32   | ms         |                                                              | 0 - 86400000                                                 |
| PsId                | uint32   | ms         |                                                              | 0 - 270,549,119                                              |
| RvBasicVehicleClass | enum     |            |                                                              | 0 = Car    20 = Light truck    25 = Heavy duty truck    40 = Motorcycle    60 = Emergency vehicle    82 = Pedestrian    85 = Bicycle |
| RvRandomId          | uint16   |            |                                                              | 0 - 0xFFFF                                                   |
| RvLatitude          | double   | deg        |                                                              | -90.0 - 90.0                                                 |
| RvLongitude         | double   | deg        |                                                              | -180.0 - 180.0                                               |
| RvElevation         | float    | deg        |                                                              | -409.5 - 6143.9                                              |
| RvHeading           | float    | deg        |                                                              | 0 - 360.0                                                    |
| RvGpsSpeed          | float    | m/s        |                                                              | -55.5556 - 55.5556                                           |
| RvBrakeStatus       | tinyint  |            |                                                              | 0 = Brake Not Active    1 = Brake Active                     |
| RvYawRate           | real     | deg/s      |                                                              | -327.68 - 327.67                                             |
| RvLongitudinalAccel | real     | m/s2       |                                                              | -15.0    -  15.0                                             |

 

---

| Spat Table         | Type    | Units           | **Description**                                              | **Range**                                                    |
| ------------------ | ------- | --------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Device             | int32   |                 | number between 2000   and 3000                               | 2000 to 3000                                                 |
| Trip               | int32   |                 | Sequential trip   number                                     | 0 - 2147483647                                               |
| Time               | int32   | centi-secs      | Sequential trip time   starting at 0 and increasing by 50 (2 hz) in centi-seconds | 0 - 2147483647                                               |
| NativeFlag         | tinyint |                 | Flag indicating a   match between Time and Localtime (within a 0.1 s) | 0 - 1                                                        |
| LocalTimeMS        | uint32  | ms              |                                                              | 0 - 86400000                                                 |
| IntersectionId     | uint16  |                 |                                                              | 0 - 65535                                                    |
| LaneId             | tinyint |                 |                                                              | 1 - 254                                                      |
| MovementPhaseState | tinyint |                 |                                                              | 0 - 9    0 = Dark    1 = Green    2 = Yellow    3 = Flashing Red    4 = Red |
| SignalGroupID      | tinyint |                 |                                                              | 1 - 255                                                      |
| MinEndTime         | uint16  | 1/10th UTC Time | Note: Tenths of a   second in the current or next hour when the phase is to change | 0 - 36000                                                    |

 

 

