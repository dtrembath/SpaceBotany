# SpaceBotany
Details of aeroponic mister control using esp8266

This is supplementary material for the Amateur Professionals team entry in the Grow Beyond Earth contest (Feb 3, 2020). 
The design uses a 113kHz piezoelectric mister w/ WG3166A board to aeroponically grow lettuce plants.
See https://www.instructables.com/contest/beyondEarth/ for details.

Files in this repository inlude 'mister-esp8266-wiring.pdf', which shows the wiring for controlling the mister with an ESP8266 microcontroller. A DHT22 temp/humidity sensor is also included. 
Data is uploaded to https://thingspeak.com/channels/948030

The Arduino IDE is used to upload the 'space_botany_contest_mister.ino' code to an Adafruit Feather Huzzah ESP 8266.
You will also need to download the 'secrets.h' file into the same directory as 'space_botany_contest_mister.ino'.
Edit the information in 'secrets.h' to add your wifi and ThingSpeak credentials.

The 'esp8266-mister.fzz' file is included if you'd like to open it with the Fritzing software tool.
