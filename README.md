This project demonstrates a self-learning paddle game powered by a custom Multilayer Perceptron (MLP) implemented from scratch in C.
The neural network gradually learns to track and hit a bouncing ball through trial and error — adapting in real time without external dependencies.

The project consists of two core components:

mlp.h Library – A compact and efficient C implementation of a feedforward neural network.

main.c – A minimalist arcade simulation showcasing real-time neural learning.



🚀 Build & Run

gcc - o2 main.c mlp.c -o paddle -lm
./paddle
