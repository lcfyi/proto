# Proto

Code for the Particle Electron (would compile on an Arduino as well tbh).

The VESC app/motor config is in `config/`. The main thing to pay attention to is that app control should use `Current No Reverse`, and that the BDLC startup boost should be a bit higher (I haven't figured out exactly what, but 0.040 works).