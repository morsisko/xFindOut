**! NOTE: Please use x64dbg version released after 30 Nov, 2020. Older versions of x64dbg got hardware breakpoint callback bug and thus the plugin won't work!**

# xFindOut
A plugin to x64dbg that lets you find out what writes to/accesses particular address. Based on the similar feature available in Cheat Engine.

# Installation
Copy xFindOut.dp32 into x32/plugins and xFindOut.dp64 into x64/plugins

# Usage
Right click the dump, then select corresponding option in the contex menu:
![Dump context menu preview](https://i.imgur.com/wqXhz3W.png)

alternatively you can use commands:
## `findout address, [r]/w`
For example:
* `findout 0x1234` - will check out what accesses address `0x1234`
* `findout 0x1234, w` - will check out what writes to address `0x1234`

## `findoutstop address`
For example:
* `findoutstop 0x1234` - will stop and shutdown the window connected to address `0x1234` (provided it is currently opened and started)

# Demo
![Demo gif](https://i.imgur.com/1L0RTYu.gif)
