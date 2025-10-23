#include "controllers/MainController.h"
#include <locale.h>

int main() {
    setlocale(LC_ALL, "");
    
    MainController app;
    
    if (!app.init()) {
        return 1;
    }
    
    app.run();
    
    return 0;
}