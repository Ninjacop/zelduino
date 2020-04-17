# Zelduino

A Nintendo Switch controller language!

## IMPORTANT

Currently in testing phase, so there are bound to still be some things to work out. Please create a ticket if you are
having trouble with using Zelduino.

## Wait, a language for Switch controllers?

This language talks to microcontrollers, such as the Teensy++ 2.0, and it emulates a Switch Pro Controller into doing the things you tell it to do!

## How did I even get here?

After searching online one day, I saw that it was possible to create super detailed sketches in Splatoon (repository [here](https://github.com/shinyquagsire23/Switch-Fightstick)). After doing some more research, I came across an article (found [here](https://medium.com/@bertrandom/automating-zelda-3b37127e24c8)) that used a Teensy++ 2.0 microcontroller that a emulated Switch controller to bowl strikes everytime in The Legend of Zelda: Breath of the Wild. The writer also thought of a little script idea that would handle all of the controller inputs, but he didn't get the time around to implement it. So, as a fun side project, I decided to take that challenge and do it myself!

## Tutorial

I've included a tutorial file (TUTORIAL.md), so you can see how Zelduino works.

## Installing

- run `git clone --recursive https://github.com/Ninjacop/zelduino.git`, to clone the repository and LUFA submodule.

## Running Zelduino

- Compile `zelduino.cpp` by executing `make zelduino` at the top level of the repository.

- Create a script to hold all the instructions.

- Take your script and execute `./zelduino [your file]` to create the hex output.

- Go into the `controller` folder and execute `make`. If everything goes well, a `Joystick.hex` file should be produced.

- Using TeensyLoader (For an Arduino Teensy 2.0++), you can flash the `Joystick.hex` file easily through there.

- If you're using a different microcontroller, please view shinyquagsire's repository ([here](https://github.com/shinyquagsire23/Switch-Fightstick)) for instructions on how to flash the `Joystick.hex` file to your microcontroller.

- Finally, plug in your Teensy or other microcontroller into your Switch, and have fun!

## License

Ninjacop (c) MIT

## Special Thanks

dot, Edudmas & TheSteveEngine for testing.
shinyquagsire23 & Bertand Fan for inspiration and base code.
Dean Camera for the LUFA library.