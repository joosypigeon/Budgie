#include <stdio.h>
#include "aapp.h"
#include "../budgie/oop.h"

void main() {
    printf("main\n");
    Application *app = getApplication();
    INSTANCE_METHOD(app, initGraphics);
    INSTANCE_METHOD(app, init);
    INSTANCE_METHOD(app, loop);
    INSTANCE_METHOD(app, deinit);
    CLASS_METHOD(&applicationClass, free, app);
}