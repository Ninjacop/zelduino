# Zelduino tutorial

## Comments

Create single line comments with `%`.
Create multiline comments with `[` and `]`.

## Instructions

Valid instructions are as follows:

- UP, DOWN, LEFT, RIGHT
- X, Y, A, B, L, R, ZL, ZR
- LCLICK, RCLICK, TRIGGERS
- MINUS, PLUS, HOME, CAPTURE
- THROW (Macro from the Medium article)
- NOTHING

By default, instructions are uppercase, but they can be written in any way.

## Durations

With each instruction, there needs to be a duration that the instruction is performed. This is in the form of milliseconds.

Ex.
    A 1000 % Presses A for 1000ms

## Loops

To execute a loop, you need to specify how many times it will execute, the body of the loop, and end it with `end`.

Ex.
    % Press A for 1000ms, then Up for 500ms, then repeat that action 5 times
    loop 5
        A 1000
        UP 500
    end

## Going Forward

Hopefully this can be expanded, but there are more examples in the `examples` folder.
