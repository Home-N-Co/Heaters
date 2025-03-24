# Heaters
Gestion Chauffage

sequenceDiagram
autonumber
box Teal Device
participant S as Setup
participant A as Application
participant B as Broker
participant PD as PresenceDetector
participant TS as TemperatureSensor
participant CS as ConsumptionSensor
participant HC as HeaterController
end
box Green Server
participant MQTT as MQTT Broker
participant Server as Go Application
participant DB
end
S ->> A: Instanciate
A ->> B: Instanciate
B ->> MQTT: Connect
A ->> B: Prepare `startup` message
B ->> MQTT: Send `startup` message
MQTT ->> Server: Relay `startup` message from Device
Server ->> DB: Check if Device exists and creates it if necessary
DB ->> Server: Send Device data back to prepare `setup` message
Server ->> MQTT: Send `setup` message
MQTT ->> B: Relay `setup` message
B ->> A: Parse `setup` message
A ->> A: Update `location` and `locationID`
A ->> PD: Update value
A ->> TS: Update value
A ->> CS: Update value
A ->> HC: Update value (temperature, time-based control)
A ->> S: Setup complete
A ->> PD: Detect Presence (Who is in the room?)
PD ->> A: Return "mam" or "mister"
A ->> HC: Set Temperature (higher for "mam", lower for "mister")
A ->> HC: Schedule Heater Start Time
A ->> HC: Activate Heater (based on scheduled time or presence)
HC ->> TS: Monitor Temperature
TS ->> HC: Room temperature status
HC ->> A: Adjust Heater Output (On/Off, increase/decrease)
A ->> HC: Maintain desired temperature
A ->> S: Heater control complete
