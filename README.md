# BFCI

A Brainf*** interpreter written in C. This project is in it's early stages;
I will be adding more to it in the future but for now it has all of the stuff
you would need from an interpreter in terms of functionality.


## Build

You can build the project one of two ways below:

```bash
  # Just build the project locally in the working directory.
  $ cd bfci && make

  # Build the project and then copy it to /usr/bin/.
  $ cd bfci && sudo make install
```


## Use

Below are the two main ways of interpreting Brainf*** code with bfci:

```bash
  # This will parse and interpret the code from a specified file.
  $ bfci -f examples/hello_world.bf

  # This will take the entered argument from the -i option and interpret it.
  $ bfci -i "+[--------->+<]>+.++[--->++<]>+.>++++++++++."
```
