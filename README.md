# Custom Real-Time Synthesizer
A real-time synthesizer that takes input from an arduino hooked up to metalic objects. The circuit is closed with a metalic glove that touches the triggers ( metalic objects ). A laptop / raspberry pi processes the input and outputs a square/sine/triangle/sawtooth wave to a speaker.

## Requirements & Implementation
### Physical requirements
- For the synth program
  * laptop / raspberry pi
- For the arduino keyboard
  * arduino board
  * usb cable
  * wires
  * resistors
  * metalic objects ( triggers ) - we used candles
  * glove ( on which to attach some of the wires )
  * LEDs for debugging
  * speaker

### Software requirements
- For the synth program
  * [SFML](https://www.sfml-dev.org/) installed
  * A C++ compiler
  * A text editor
- For the arduino keyboard
  * Arduino IDE

### Implementation
A main graphical window ( [sf::Window](https://www.sfml-dev.org/documentation/2.4.2/classsf_1_1Window.php) ) takes input from the arduino ( using a serial connection ), generates a waveform sample array ( [sf::SoundStream](https://www.sfml-dev.org/documentation/2.0/classsf_1_1SoundStream.php) ) and outputs it to the speaker.
The arduino is programmed to poll input from digital pins, encode information and send it via the serial connection to the raspberry pi / laptop.

## Todo List
- [x] C++ ( w/ SFML ) synth
  - [x] Custom Sound Stream
  - [x] Arduino Serial Input Interface
  - [x] Sine Wave
- [x] Arduino keyboard
  - [x] Wired glove
  - [x] Triggers
  - [x] Arduino Code
  - [x] Box that holds everything together
  - [ ] Extra details

- [ ] Article for it @ www.gdrh.ro
- [ ] Demo video
- [x] Pictures
- [ ] Project Schematic
- [ ] Function documentation / Code comments
