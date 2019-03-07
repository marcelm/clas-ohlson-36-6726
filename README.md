# Protocol of Clas Ohlson 36-6726 wireless thermometer/hygrometer

The [Clas Ohlson product 36-6726](https://www.clasohlson.com/se/Tr%C3%A5dl%C3%B6s-hygrometer-termometer/36-6726) is an extra temperature/humidity wireless sensor for [Clas Ohlson product 36-6725](https://www.clasohlson.com/se/Tr%C3%A5dl%C3%B6s-hygrometer-termometer/36-6725) and [36-6725-1](https://www.clasohlson.com/se/Tr%C3%A5dl%C3%B6s-hygrometer-termometer/36-6725-1).

The device sends on the usual frequency of 434 MHz. It can be set to one of
three channels via a switch in the battery compartment. It transmits once every
57 seconds when on channel 1, once every ~70 seconds on channel 2, and once
every ~80 seconds on channel 3.

Transmissions use OOK (on-off keying). Each transmission consists of 12
identical copies of the message, with 3.8 ms OFF between messages.

Each message consists of 36 bits, encoded in the following way:

- 0 is transmitted as ~500 µs ON, ~850 µs OFF
- 1 is transmitted as ~500 µs ON, ~1800 µs OFF

In other words: There are 37 ON pulses in each message. The information is encoded in the pauses
between them.

The bits have the following meaning. ("Bit 1" is the bit transmitted first.)

* Bits 1-10: This appears to always be 0101 1011 10.
* Bits 11-12: The channel as set by the switch in the device. 00 is channel 1;
  01 is channel 2; 10 is channel 3.
* Bits 13-24: 10 times temperature in degree celsius, two’s complement (see below)
* Bits 25-28: Seems to always be binary 1111
* Bits 29-36: Humidity (% relative humidity)

The temperature is transmitted as a 12-bit value representing the temperature in
degrees celsius multiplied by 10. For example, 17.9°C is transmitted as
0000 1011 0011 binary, 179 decimal. Negative temperatures are transmitted in
normal two’s complement. That is, -1.3°C is transmitted as 1111 1111 0011
(12-bit two’s complement of 13).

There is possibly a “low battery” bit in the bits described above as being
constant.
