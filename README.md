# A C implementation of RTISI-LA

This program uses the Real-Time Iterative Spectrum Inversion with Look-Ahead
(RTISI-LA) algorithm to stretch an audio file.
It is was developed for experimentation purposes within the context of a
R&D collaboration project between engineering students of TELECOM Bretagne
and Renesas Design France.

The time-stretching factor is defined in the header RTISI.h:
```
#define  SCALE 1.3
```

## References

- [Real-Time Iterative Spectrum Inversion with Look-Ahead (IEEE article)](https://ieeexplore.ieee.org/document/4036578)
