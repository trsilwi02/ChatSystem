#include "ui_components.h"
#include <QWidget>

void UIComponents::ApplyStylesheet(QWidget* widget) {
    QString stylesheet = R"(
        QMainWindow {
            background-color: #2C2F33;
            color: #FFFFFF;
        }
        
        QWidget {
            background-color: #2C2F33;
            color: #FFFFFF;
        }
        
        QLineEdit {
            background-color: #23272A;
            color: #FFFFFF;
            border: 1px solid #23272A;
            border-radius: 3px;
            padding: 5px;
        }
        
        QTextEdit {
            background-color: #23272A;
            color: #FFFFFF;
            border: 1px solid #23272A;
            border-radius: 3px;
        }
        
        QListWidget {
            background-color: #23272A;
            color: #FFFFFF;
            border: 1px solid #23272A;
            border-radius: 3px;
        }
        
        QPushButton {
            background-color: #7289DA;
            color: #FFFFFF;
            border: none;
            border-radius: 3px;
            padding: 5px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #8AA3E0;
        }
        
        QPushButton:pressed {
            background-color: #5B7FC7;
        }
        
        QLabel {
            color: #FFFFFF;
        }
    )";
    
    widget->setStyleSheet(stylesheet);
}
