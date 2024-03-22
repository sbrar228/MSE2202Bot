# MSE2202Bot
Code for the programming of the robot and sorting station for the MSE2202 Final project.


# Operating Principles

The system is comprised of two parts, which I will refer to as the robot and the station.

The robot collects gems and returns to the station, at which the gems are sorted by color. 
The workings of each the robot and the station are described in more detail in their code, but below is a quick overview:

## The Robot

The robot operates on a "grid" or step-based system. it moves only in multiples of this "grid" and ensures positional accuracy by using the encoders located on the robot's wheels.
It collects gems by using a "plow" that it uses to push the gems along in front of it, eventually to the station.

## The Station

The station is comprised of a basket, a pulley system, a funnel, a sorter arm, and a collection basket that all work in unison. First, the bot leaves the station, having 
deposited a number of gems into it, then sends a signal to the station to start the sorting sequence. The station then pulls the basket up, which tips it into a funnel which only feeds one gem at a time into the sorter arm. The sorter arm has an LED in it for light, and a color sensor on the other side which detects the color of each gem. Depending on the color, the sorter arm either deposits them into the basket or discards them out the other side.
