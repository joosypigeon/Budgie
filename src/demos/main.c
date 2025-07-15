#include <stdio.h>
#include "aapp.h"
#include "../budgie/oop.h"

void main() {
    printf("main:enter\n");
    Object *app = getApplication();
    printf("main: app=%p\n", app);
    INSTANCE_METHOD_AS(ApplicationVTable, (Application *)app, initGraphics);
    INSTANCE_METHOD_AS(ApplicationVTable, (Application *)app, init);
    INSTANCE_METHOD_AS(ApplicationVTable, (Application *)app, loop);
    INSTANCE_METHOD_AS(ApplicationVTable, (Application *)app, deinit);

    CLASS_METHOD(&applicationClass, free, app);
    printf("main:leave\n");
}