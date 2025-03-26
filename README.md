![Screenshot](screenshot.png)

# Yellow Snow (C - SDL2)
Earn points by collecting all the tasty white snow flakes that fall. But watch out for the yellow snow. The Neural Network can be loaded from a file if it exist, or a new random network will be created.When training finishes it will save the current top networks back into the neural-networks file. If ai_on is enabled the game will play but the neural-network will have the ablility to move the snowman. If ai_train is on the game will play without delay, with as many FPS as possible. If gfx_off is enabled then the game will not load a graphical window, or load SDL at all. It will play the game but you will not see it happening. This is much, much faster. If threads are used it will complete generations of training even faster. I was able to have it play 20 million games in 2.5 hours, using 16 threads on a laptop.

# Neural Network
This project can be played as a standard game, or as an AI enabled game, or to train the AI networks. This uses a simple Evelutionary Neural Network with up to 10 hidden layers.

# Build
To build the yellow-snow binary you will need all required build tools needed to compile via Makefile and gcc. You will also need SDL2 with image, ttf and mixer both the libraries and headerfiles.

ArchLinux instructions.
```
sudo pacman -S --needed base-devel
sudo pacman -S --needed sdl2 sdl2_image sdl2_mixer sdl2_ttf
make release
./yellow-snow --ai_on
```
Command line switches. Some switches will turn others on or off. `--ai_train` will enable `--ai_on` and if `--threads` are set then so is `--gfx_off`.
```
--ai_on                 // Enable AI to play or train.
--ai_train              // Train network and save to file.
--gfx_off               // Disable graphics mode for training.
--filename {string}     // Filename of network to load or save to.
                        // fitness by default is score at yellow collide.
--fit_ticks             // Switch fitness to number of ticks at yellow collide.
--fit_timed             // Switch fitness to score when max_ticks reached.
--white_inc {+-int}     // Training white flake collision fitness increment.
--yellow_inc {+-int}    // Training yellow flake collision fitness increment.
--mut_rate {int}        // Rate of random mutation in percentage.
--mut_rang {int}        // Range of random mutation in percentage.
--trials {int}          // How many trials to run the same game to average.
--max_ticks {int}       // How many game Ticks to run for each game.
--threads {int}         // How many cpu threads
--top_variants {int}    // How many of the Variants to mutate to the next Generation.
--variants {int}        // How many Variants in each generateion.
--gens {int}            // How many Generations to trains for.
--new {int} {int} {int} // Create a new network with up to 10 hidden layers.
```

# Controls
Left Arrow - Moves left.\
Right Arrow - Moves right.\
Space Bar - Resets the Game.\
Escape - Quits and closes game. \
F - Prints FPS to the console.

