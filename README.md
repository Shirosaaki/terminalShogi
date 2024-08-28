# Terminal Shogi
## Description

The goal of this project is to create a Shogi game playable between two terminals.

## WARNING

This project is in development,

### TODO

* Move on the map
* Airdrop of parts
* WIN / LOOSE

### IMPROVEMENT

* Make the code more readable
* Add move of pieces (Tower / Bishop)

## Requirements
* gcc / g++
* make
* Linux


## Compilation

You can run:

```
make
```

For a standard compilation, or:

```
make run
```

To compile and execute the code.

## Usage

You need to use two terminals to play.

After opening the terminals, in the first one, run:

```
./shogi
```

You will receive the PID of the first player. Then, in the second terminal, enter:

```
./shogi pid_of_the_first_player
```

The terminals will connect, and you can start playing!

## Help

If you need help, you can run 

```
./shogi -h
```
