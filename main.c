#include "mlp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 40
#define HEIGHT 20
#define LEARNING_RATE 0.5
#define FPS_DELAY 40000
#define STUCK_THRESHOLD 50
#define POSITION_HISTORY_SIZE 10

typedef struct {
    double x, y;
    double vx, vy;
} Ball;

typedef struct {
    double y;
    double speed;
} Paddle;

typedef struct {
    double x[POSITION_HISTORY_SIZE];
    double y[POSITION_HISTORY_SIZE];
    int count;
    int index;
} PositionHistory;

static double clamp(double x, double min, double max) {
    return (x < min) ? min : (x > max ? max : x);
}

static double normalize(double val, double max) {
    return val / max;
}

void init_position_history(PositionHistory *history) {
    history->count = 0;
    history->index = 0;
    for (int i = 0; i < POSITION_HISTORY_SIZE; i++) {
        history->x[i] = 0;
        history->y[i] = 0;
    }
}

void add_position(PositionHistory *history, double x, double y) {
    history->x[history->index] = x;
    history->y[history->index] = y;
    history->index = (history->index + 1) % POSITION_HISTORY_SIZE;
    if (history->count < POSITION_HISTORY_SIZE) {
        history->count++;
    }
}

int is_ball_stuck(PositionHistory *history, Ball *ball, int *stuck_counter) {
    if (history->count < POSITION_HISTORY_SIZE) {
        return 0;
    }

    double avg_x = 0, avg_y = 0;
    for (int i = 0; i < history->count; i++) {
        avg_x += history->x[i];
        avg_y += history->y[i];
    }
    avg_x /= history->count;
    avg_y /= history->count;

    double distance = sqrt(pow(ball->x - avg_x, 2) + pow(ball->y - avg_y, 2));

    if (distance < 2.0) {
        (*stuck_counter)++;
    } else {
        *stuck_counter = 0;
    }

    return (*stuck_counter >= STUCK_THRESHOLD);
}

void respawn_ball(Ball *ball) {

    int corner = rand() % 2;

    if (corner == 0) {

        ball->x = 1;
        ball->y = 1;
        ball->vx = (rand() % 3 + 1) * 0.5;
        ball->vy = (rand() % 3 + 1) * 0.5;
    } else {

        ball->x = 1;
        ball->y = HEIGHT - 2;
        ball->vx = (rand() % 3 + 1) * 0.5;
        ball->vy = -(rand() % 3 + 1) * 0.5;
    }

    double speed = sqrt(ball->vx * ball->vx + ball->vy * ball->vy);
    ball->vx /= speed;
    ball->vy /= speed;
}

void train_on_example(MLP *net, Ball *ball, Paddle *paddle) {
    double inputs[3] = {
        normalize(ball->y, HEIGHT),
        normalize(ball->vy, 1.0),
        normalize(paddle->y, HEIGHT)
    };

    double target[1];
    if (ball->y > paddle->y + 1)
        target[0] = 1.0;
    else if (ball->y < paddle->y - 1)
        target[0] = 0.0;
    else
        target[0] = 0.5;

    for (int i = 0; i < 5; i++)
        mlp_train(net, inputs, target, 1);
}

int decide_action(MLP *net, Ball *ball, Paddle *paddle) {
    double inputs[3] = {
        normalize(ball->y, HEIGHT),
        normalize(ball->vy, 1.0),
        normalize(paddle->y, HEIGHT)
    };

    double output[1];
    mlp_predict(net, inputs, output);

    if (output[0] > 0.6) return 1;
    if (output[0] < 0.4) return -1;
    return 0;
}

void reset_game(Ball *ball, Paddle *paddle) {
    respawn_ball(ball);
    paddle->y = HEIGHT / 2.0;
    paddle->speed = 1.0;
}

void draw(const Ball *ball, const Paddle *paddle, int score, int stuck_counter) {
    printf("\033[H");
    printf("\033[2J");

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if ((int)ball->x == x && (int)ball->y == y)
                printf("O");
            else if (x == WIDTH - 1 && fabs(y - paddle->y) < 1.0)
                printf("|");
            else if (x == 0)
                printf("|");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("Score: %d | Stuck counter: %d/%d\n", score, stuck_counter, STUCK_THRESHOLD);
    fflush(stdout);
}

int main() {
    srand(time(NULL));

    size_t layers[] = {3, 8, 1};
    MLP *net = mlp_create(layers, 3, LEARNING_RATE);

    Ball ball;
    Paddle paddle;
    PositionHistory pos_history;
    int stuck_counter = 0;

    reset_game(&ball, &paddle);
    init_position_history(&pos_history);
    int score = 0;

    printf("\033[2J");

    for (;;) {
        add_position(&pos_history, ball.x, ball.y);

        if (is_ball_stuck(&pos_history, &ball, &stuck_counter)) {
            respawn_ball(&ball);
            stuck_counter = 0;
            init_position_history(&pos_history);
        }

        draw(&ball, &paddle, score, stuck_counter);

        int action = decide_action(net, &ball, &paddle);
        paddle.y += action * paddle.speed;
        paddle.y = clamp(paddle.y, 0, HEIGHT - 1);

        ball.x += ball.vx;
        ball.y += ball.vy;

        if (ball.y <= 0 || ball.y >= HEIGHT - 1)
            ball.vy *= -1;

        if (ball.x <= 0) {
            ball.vx *= -1;
            ball.x = 0;
        }

        if ((int)ball.x >= WIDTH - 2 && fabs(ball.y - paddle.y) < 1.0) {
            ball.vx *= -1;
            score++;
        }

        if (ball.x >= WIDTH - 1) {
            score = 0;
            reset_game(&ball, &paddle);
            init_position_history(&pos_history);
        }

        if (ball.vx > 0)
            train_on_example(net, &ball, &paddle);

        usleep(FPS_DELAY);
    }

    mlp_free(net);
    return 0;
}