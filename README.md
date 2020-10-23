# linux smile device
linux kernel module for char device with similar to __/dev/zero__ functionality, but instead of nulls generates __smiles__ ^^

# build
run
```
$ make
```
to build the module
> __NOTE__
> you need some kernel headers to build the module, if it fails on your system see __Chapter 2__ of the book mentioned below

# load module 
use 
```
$ ./load_smile.sh
```
this loads the module into the running kernel and creates fs node __/dev/smile__ representing the device

# try the device
use 
``` 
$ cat /dev/smile
```
to see it work. `Ctrl+C` to stop.

# undload module
use

```
$ ./unload_smile.sh
```

to unload the module and remove __/dev/smile__ device

# important
this example was made by following this __awesome__ [Linux Device Drivers, Third Edition](https://lwn.net/Kernel/LDD3/) book.<br />
__you should really check it out !__<br />
also [here](https://github.com/martinezjavier/ldd3) you can find __up-to-date examples__ for this book<br />

