# FridgeM8

Arduino code for my FridgeM8 project. 

The FridgeM8 is a fridge magnet that consists of an Arduino Mega and a 4 inch touch screen, along with an ESP8266 module for WiFi connection. 

The sketch uses the MCUFRIEND-kbv and Adafruit GFX core graphics libraries, and implements a touch menu system that lists everyday grocery items over several screens. When an item is selected, it remians highlighted and a HTTP request is sent to the Todoist (list app) API to be added to a shopping list. When the item is marked as purchased on the Todoist app, the highlighting is removed on the FridgeM8. 

See: https://hackaday.io/project/10679-not-so-smart-fridge
