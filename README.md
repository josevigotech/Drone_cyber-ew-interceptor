How does the simulation work?

 Imagine you are trying to hear a whisper in a room representing the GPS signal when suddenly, someone right next to you turns on a megaphone at full volume, blasting static the jammer. Although the whisper is still there, it becomes impossible to hear.

Our architecture simulates this scenario using a modern, modular, object-oriented approach in C++

1. Early Radar Detection
A central module monitors the airspace based on actual geometric distances to track the incoming drone.

2. Soft-Kill Electronic Warfare
If the drone enters the electromagnetic radius of the `JammerEW`, it loses its GPS signal [gps_quality = 0.0] and switches to inertial navigation, accumulating positional drift that can completely disorient it.

3. Hard-Kill & C-RAM Kinetic Response
If electronic countermeasures are insufficient and the threat manages to bypass the previous layers approaching a critical threshold near the base the system automatically initiates kinetic defensive fire using 20mm weaponry.
