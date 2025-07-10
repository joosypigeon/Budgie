#include "../app.h"
#include "../demo.h"

void main() {
    buAppInit();
    initDemo();
    buApp();
    deinitDemo();
}