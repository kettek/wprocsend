# wprocsend
Simple Windows console program to either kill or signal SIGINT or SIGBREAK to a specific `pid` or executable name.

Hacked together to send a SIGINT to `ffmpeg.exe` to stop desktop capturing.

## Usage
    wprocsend (int|break|kill) (pid|program.exe)
  
## Examples

  * `wprocsend kill firefox.exe`
  * `wprocsend int ffmpeg.exe`
  * `wprocesnd break 8723`
