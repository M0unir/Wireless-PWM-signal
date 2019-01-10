# ESP to ESP PWM Forwarding over UDP Protocol

This repo contains sources for both the Access Point and Client. 

1. Connect the AP to the Pixhawk PWM OUT Pins.
2. Connect the Client to the Servo Motors.
3. Make sure to ground all devices properly to avoid noise and thus servos jittering.

The Client ESP should autoconnect to the AP and start writing to the servos right away.

