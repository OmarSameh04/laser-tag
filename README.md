# laser-tag
laser tag microelectronics project:
  - each player has an esp32 as the brains
  - each player has a gun with a mounted lcd to display life points and ammo points
  - the gun shoots VISIBLE red laser
  - each player has a vest with a custom light sensor made from an array of LDRs
  - the esp must report back changes to a central "game master" server
-----------------------------------------------------------------------------------------
  - the sensor is made from 9 parallel 12mm LDRs connected in a 5V voltage divider with a resistor(the value should be chosen after testing the LDRs)
  - the vest houses the esp and the rfid module for reload with the gun connected to it with a wire
