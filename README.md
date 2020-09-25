# Airquality Sensor and Backend
The goal of this project was to monitor the airquality of a room and visualize it on a website.

The project consists of a sensor part and the webserver backend, each in their own subfolder.

[The sensor](./sensor/README.md) collects the data and sends it to the backend via a wifi connection.

[The backend](./server/README.md) saves the data and provides an interface to query the data and visualizes it.

The system is described in this blogpost: [https://life-is-a-project.de/airquality-sensor/](https://life-is-a-project.de/airquality-sensor/)

# Future possible improvements
Sensor:
* Power saving/deepsleep modes (the SGP30 sensor must stay on though)
* Local buffering of values to avoid constant WiFi connection

Server:
* Security/Authentication
* Grafana/Prometheus
* Some sort of database (sqlite)