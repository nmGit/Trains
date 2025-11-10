#include "drafter/Canvas.h"

#include <iostream>

Drafter::Canvas canvas;

int main(int argc, char *argv) {

    canvas.Start();
    while (canvas.Service() == Drafter::Canvas::ServiceResult::Continue) {}

    return 0;
}