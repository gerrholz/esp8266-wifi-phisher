# esp8266-wifi-phisher
Demonstration of Captive Portal Phishing on a ESP8266-Controller

## Disclaimer
This program is for educational purposes only. Only use it on your own networks or on networks you have permission for. I'm not responsible for anything that you do with this program.

## About
This project is supposed to demonstrate how on how perform wifi phishing attacks with cheap ESP8266 controllers.
After deploying this software on a NodeMCU or a similar board it will create an Open WiFi Access Point. This access point redirects all DNS requests to its own webserver. This will cause the captive portal to "pop open" on the client. A fake login site will be shown, which captures the clients credentials.

## Installation (Arduino IDE)
1. Download the source code of this project and open the esp8266phishing.ino in the Arduino IDE
2. Go to Preferences and add this url to your Additional Board Managers: http://arduino.esp8266.com/stable/package_esp8266com_index.json
3. Restart the IDE
4. Go to Tools->Board->Board Manager and search for "esp8266". Install the current version.
5. Connect your ESP8266 Controller to your device
6. Now go to Tools->Board and select your board (normally "Generic ESP8266 Module"). Go to Tools->Port and select your USB-Port
7. Click on "Upload" to deploy the Software to your module. After deployment an Access Point will start.
