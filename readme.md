# MED Task Server

## Description :
This simple GUI  application is designed to read data from server in specific format via networking port and draw a plot for the data.

## Dependencies

* QT 4.8
* qcustomplot (included in the project, no need to do anything)

## Procedure followed
1. Upon connection, the client sends an index telling the server from where to start.
2. Then, the client receives the data in the following format `timestamp:value`
3. Upon receiving, parsing takes an action and Da da da, you have your plot.

