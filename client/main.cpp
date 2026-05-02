#include <QApplication>
#include "chat_window.h"
#include "ui_components.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    ChatWindow window;
    UIComponents::ApplyStylesheet(&window);
    window.show();
    
    return app.exec();
}
