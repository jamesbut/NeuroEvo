<img align="left" width="400" title="NeuroEvo logo" src="https://github.com/jamesbut/NeuroEvo/blob/master/logo.png">
<br/><br/><br/><br/>

## Overview

NeuroEvo is a fast, easy-to-use Neuroevolution library written in C++.
It has been designed to be as extensible as possible with abstract interfaces
used throughout. Many genetic operators, network types and domains have
already been implemented, however it is straight forward to implement your own
in a plug-and-play fashion.

## Compilation

Boost is required in order to compile NeuroEvo.

```
git clone https://github.com/jamesbut/NeuroEvo.git
cd NeuroEvo
mkdir build
cd build
cmake -D BUILD_EXAMPLES=ON ..
make
```

NeuroEvo can be built without the examples by not including the above flag.

## Usage

There are a number of [examples](https://github.com/jamesbut/NeuroEvo/tree/master/examples)
that illustrate the usage of the library.

To run an evolutionary run of the AND example:

```
cd build/examples
./and_example
```

To run an individual run with the best winner of the evolutionary process
the population folder name needs to be copied from the data/ directory and
used as a command line argument:

```
./and_example *population folder name*
```

To graph the evolutionary progress:

```
cd scripts
python3 visualisation/pop_scores.py
```

## Contributions

Contributions are very welcome. Like all software libraries the design is not
perfect and there may still be some bugs. Suggested design changes are welcome,
so are C++ improvements. Comments are best communicated via opening an issue.

## License

This project is licensed under the MIT License,
see the [LICENSE]() file for details.

## Acknowledgments

I would first like to thank Chloe Carter for her creative logo design and
support through the writing of this library. I would also like to
acknowledge Gregory Palmer because I think somewhere buried in the neural
network classes are remnants of some of his original code. I would finally
like to thank my friend Benjamin Schnieders for all of the advice and help with
regards to C++ of which he truly was a master - without him, this library would
not have been possible.
