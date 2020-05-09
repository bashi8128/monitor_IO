# monitor_IO
C implementation to monitoring I/O speed.
A substitute for dd command.

## QuickStart
```sh
% git clone https://github.hpe.com/masahiro-itabashi/monitor_IO.git
% cd monitor_IO/src
% make
% ./monitor_IO -i /dev/urandom -o /tmp/test -s 256
Write 256 Byte to /tmp/test.
date,time
2020/05/08 16:29:06,0.002290
2020/05/08 16:29:07,0.001115
2020/05/08 16:29:08,0.001799
[Ctrl-C]
I/O statistics
3 output task was executed, 3 suceeded, 0 failed.
min/avg/max = 0.001115/0.001735/0.002290
```

## Usage
```
Usage: monitor_IO [-i inputfile] [-o outputfile]
                  [-s blocksize] [-t interval] [-h]
```

## Precautions
You might not want to specify too big blocksize.
In HP-UX, 256 Byte is appropriate.
