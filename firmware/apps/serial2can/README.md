# USB2CAN

http://www.mictronics.de/projects/usb-can-bus/

## Device control commands

The device can be controlled with simple ASCII commands from any terminal tool (using the
VCP driver).

Each command consists of one character for command identification and, depending upon
command, several data bytes.

Each command must end with a carriage return [CR] ASCII code 13.

All command identifiers are case sensitive.

All other characters for data bytes are NOT case sensitive, but must be valid hex characters
(0-9 A-F a-f).

The firmware checks also required command length against given data.

Any detected error will be indicated with return of character [BEL] ASCII code 7.

The following control commands are implemented:

### A[CR]

Read last stored Arbitration Lost Capture register content from last AL interrupt.

Return: Axx[CR]

xx = One byte hexadecimal with register content.(00-FF)

### C[CR]

This command switches the CAN controller from operational in reset mode. The
controller is no longer involved in bus activities.

Command is only active if controller was set to operational mode with command “O”
before.

Return: [CR] or [BEL]

### E[CR]
Read last stored Error Capture register content from last bus error interrupt.
Return: Exx[CR]
xx = One byte hexadecimal register content.(00-FF)
### F[CR]
This command reads the error flags from CAN controller.
Return: Fxx[CR] or [BEL]
xx = one byte hexadecimal with error flags

| Bit | Function
|-----|-----------
|  0  | Not used
|  1  | Not used
|  2  | Error warning
|  3  | Data overrun
|  4  | Not used
|  5  | Error passive
|  6  | Arbitration Lost
|  7  | Bus error

The red error indication LED will blink if an error interrupt was triggered from
SJA1000.
A bus error will generate a constant red LED light.
Command “F” and command “S” will clear a bus error indication.
For detailed error description see the SJA1000 datasheet.
### Gxx[CR]
Read register content from SJA1000 controller.
xx = Register to read (00-7F)
Return: Gdd[CR]
dd = Data in register
### L[CR]
This command will switch the CAN controller in Listen Only mode. No channel open
command (“O”) is required after issuing “L”.
Use the close channel command “C” to return to reset mode, for re-init of SJA1000
send a set bit rate command “s” or “S”.
Return: [CR]
### Mxxxxxxxx[CR]
Set acceptance code register of SJA1000. This command works only if controller is
setup with command “S” and in reset mode.
xxxxxxxx = Acceptance Code in hexadecimal, order ACR0 ACR1 ACR2 ACR3
Default value after power-up is 0x00000000 to receive all frames.
Return: [CR] or [BEL]

### mxxxxxxxx[CR]

Set acceptance mask register of SJA1000. This command works only if controller is
setup with command “S” and in reset mode.
xxxxxxxx = Acceptance Mask in hexadecimal, order AMR0 AMR1 AMR2 AMR3
Default value after power-up is 0xFFFFFFFF to receive all frames.
Return [CR] or [BEL]
The acceptance filter is defined by the Acceptance Code Registers ACRn and the
Acceptance Mask Registers AMRn. The bit patterns of messages to be received are
defined within the acceptance code registers. The corresponding acceptance mask registers
allow to define certain bit positions to be ‘don’t care’.
This device uses dual filter configuration.
For details of ACR and AMR usage see the SJA1000 datasheet.

### N[CR]

Read serial number from device.
Return: Nxxxx[CR]
xxxx = Serial number in alphanumeric characters.

### O[CR]

This command switches the CAN controller from reset in operational mode. The
controller is then involved in bus activities. It works only if the initiated with “S” or “s”
command before, or controller was set to reset mode with command “C”.
Return: [CR] or [BEL]

### riiiL [CR]

This command transmits a standard remote 11 Bit CAN frame. It works only if
controller is in operational mode after command “O”.

  * `iii` Identifier in hexadecimal (000-7FF)
  * `L` Data length code (0-8)

Return: [CR] or [BEL]

### RiiiiiiiiL [CR]

This command transmits an extended remote 29 Bit CAN frame. It works only if
controller is in operational mode after command “O”.

  * iiiiiiii Identifier in hexadecimal (00000000-1FFFFFFF)
  * L Data length code (0-8)

Return: [CR] or [BEL]

### Sn[CR]

This command will set the CAN controller to a predefined standard bit rate.
It works only after power up or if controller is in reset mode after command “C”.
The following bit rates are available:

| cmd | speed      | cmd | speed
|-----|------------|-----|---------
| S0  | 10Kbps     | S5  | 250Kbps
| S1  | 20Kbps     | S6  | 500Kbps
| S2  | 50Kbps     | S7  | 800Kbps
| S3  | 100Kbps    | S8  | 1Mbps
| S4  | 125Kbps    | S9  | 95.238kbps
| Sa  | 8.333Kbps  | Sb  | 47.619kbps
| Sc  | 33.333Kbps | Sd  | 5kbps

Return: [CR] or [BEL]

### sxxyy[CR]
This command will set user defined values for the SJA1000 bit rate register BTR0 and
BTR1. It works only after power up or if controller is in reset mode after command “C”.
xx = hexadecimal value for BTR0 (00-FF)
yy = hexadecimal value for BTR1 (00-FF)
Return: [CR] or [BEL]
### tiiiLDDDDDDDDDDDDDDDD[CR]
This command transmits a standard 11 Bit CAN frame. It works only if controller is in
operational mode after command “O”.
iii Identifier in hexadecimal (000-7FF)
L Data length code (0-8)
DD Data byte value in hexadecimal (00-FF). Number of given data bytes will be
checked against given data length code.
Return: [CR] or [BEL]
### TiiiiiiiiLDDDDDDDDDDDDDDDD[CR]
This command transmits an extended 29 Bit CAN frame. It works only if controller is
in operational mode after command “O”.
iiiiiiii Identifier in hexadecimal (00000000-1FFFFFFF)
L Data length code (0-8)
DD Data byte value in hexadecimal (00-FF). Number of given data bytes will be
checked against given data length code.
Return: [CR] or [BEL]
### V[CR]
Read hardware and firmware version from device.
Return: Vhhff[CR]
hh = hardware version
ff = firmware version
### v[CR]
Read detailed firmware version from device.
Return: vmami[CR]
ma = major version number
mi = minor version number
### Wrrdd[CR]
Write SJA1000 register with data.
The data will be written to specified register without any check!
rr = Register number (00-7F)
dd = Data byte (00-FF)
Return: [CR]
### Z[CR]
This command will toggle the time stamp setting for receiving frames. Time stamping
is disabled by default, but a change of this setting will be stored in EEPROM and
remembered for the next time. So this command needs to be issued only if
necessary.

If time stamping is enabled for received frames, an incoming frame includes 2 more
bytes at the end which is a time stamp in milliseconds.

The time stamp starts at 0x0000 and overflows at 0xEA5F which is equal to 59999ms.

Each increment time stamp indicates 1ms within the 60000ms frame.

The time stamp counter resets if this setting is turned ON.

All incoming frames are sent via USB after successful receiving, optional with time stamp.

No polling is needed.

They will be sent in the following format:
11 Bit ID Frame
tiiiLDDDDDDDDDDDDDDDD[ssss][CR]
11 Bit ID Remote Frame
riiiL[ssss][CR]
29 Bit ID Frame
TiiiiiiiiLDDDDDDDDDDDDDDDD[ssss][CR]
29 Bit ID RemoteFrame
RiiiiiiiiL[ssss][CR]
r = Identifier for Remote 11 bit frame
R = Idenifier for Remote 29 bit frame
t = Identifier for 11 bit frame
T = Idenifier for 29 bit frame
i = ID bytes (000-7FF) or (00000000-1FFFFFFF)
L = Data length code (0-8)
DD = Data bytes (00-FF)
ssss = Optinal time stamp (0000-EA5F)
Overview about command status (active or invalid) depending on controller mode.
X = active/valid
