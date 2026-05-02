#include "chat_window.h"
#include "../common/message.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QFont>

ChatWindow::ChatWindow(QWidget* parent)
    : QMainWindow(parent), stacked_widget_(nullptr), socket_client_(nullptr) {
    setWindowTitle("Chat Application");
    setGeometry(100, 100, 900, 600);
    
    socket_client_ = std::make_unique<SocketClient>();
    
    stacked_widget_ = new QStackedWidget(this);
    setCentralWidget(stacked_widget_);
    
    SetupLoginScreen();
    SetupChatScreen();
    CreateConnections();
    
    stacked_widget_->addWidget(login_widget_);
    stacked_widget_->addWidget(chat_widget_);
    stacked_widget_->setCurrentWidget(login_widget_);
}

ChatWindow::~ChatWindow() {
    if (socket_client_ && socket_client_->IsConnected()) {
        socket_client_->Disconnect();
    }
}

void ChatWindow::SetupLoginScreen() {
    login_widget_ = new QWidget();
    QVBoxLayout* main_layout = new QVBoxLayout(login_widget_);
    
    // Title
    QLabel* title = new QLabel("Chat Application");
    QFont title_font = title->font();
    title_font.setPointSize(16);
    title_font.setBold(true);
    title->setFont(title_font);
    title->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(title);
    
    // Username field
    main_layout->addWidget(new QLabel("Username:"));
    login_username_edit_ = new QLineEdit();
    login_username_edit_->setPlaceholderText("Enter username");
    main_layout->addWidget(login_username_edit_);
    
    // Password field
    main_layout->addWidget(new QLabel("Password:"));
    login_password_edit_ = new QLineEdit();
    login_password_edit_->setPlaceholderText("Enter password");
    login_password_edit_->setEchoMode(QLineEdit::Password);
    main_layout->addWidget(login_password_edit_);
    
    // Buttons
    QHBoxLayout* button_layout = new QHBoxLayout();
    login_button_ = new QPushButton("Login");
    register_button_ = new QPushButton("Register");
    button_layout->addWidget(login_button_);
    button_layout->addWidget(register_button_);
    main_layout->addLayout(button_layout);
    
    // Status label
    login_status_label_ = new QLabel("");
    login_status_label_->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(login_status_label_);
    
    main_layout->addStretch();
}

void ChatWindow::SetupChatScreen() {
    chat_widget_ = new QWidget();
    QHBoxLayout* main_layout = new QHBoxLayout(chat_widget_);
    
    // Left sidebar - User list
    QVBoxLayout* sidebar_layout = new QVBoxLayout();
    
    QLabel* users_title = new QLabel("Online Users");
    QFont title_font = users_title->font();
    title_font.setBold(true);
    users_title->setFont(title_font);
    sidebar_layout->addWidget(users_title);
    
    online_users_list_ = new QListWidget();
    sidebar_layout->addWidget(online_users_list_);
    
    refresh_users_button_ = new QPushButton("Refresh");
    sidebar_layout->addWidget(refresh_users_button_);
    
    current_user_label_ = new QLabel("User: Not logged in");
    current_user_label_->setWordWrap(true);
    sidebar_layout->addWidget(current_user_label_);
    
    QWidget* sidebar = new QWidget();
    sidebar->setLayout(sidebar_layout);
    sidebar->setMaximumWidth(200);
    main_layout->addWidget(sidebar);
    
    // Right side - Chat area
    QVBoxLayout* chat_layout = new QVBoxLayout();
    
    // Header
    selected_user_label_ = new QLabel("Select a user to chat");
    QFont header_font = selected_user_label_->font();
    header_font.setBold(true);
    header_font.setPointSize(12);
    selected_user_label_->setFont(header_font);
    chat_layout->addWidget(selected_user_label_);
    
    // Message display
    message_display_ = new QTextEdit();
    message_display_->setReadOnly(true);
    chat_layout->addWidget(message_display_);
    
    // Message input
    QHBoxLayout* input_layout = new QHBoxLayout();
    message_input_ = new QLineEdit();
    message_input_->setPlaceholderText("Type message...");
    send_button_ = new QPushButton("Send");
    send_button_->setMaximumWidth(80);
    input_layout->addWidget(message_input_);
    input_layout->addWidget(send_button_);
    chat_layout->addLayout(input_layout);
    
    // Bottom buttons
    QHBoxLayout* bottom_layout = new QHBoxLayout();
    logout_button_ = new QPushButton("Logout");
    bottom_layout->addStretch();
    bottom_layout->addWidget(logout_button_);
    chat_layout->addLayout(bottom_layout);
    
    QWidget* chat_area = new QWidget();
    chat_area->setLayout(chat_layout);
    main_layout->addWidget(chat_area);
}

void ChatWindow::CreateConnections() {
    connect(login_button_, &QPushButton::clicked, this, &ChatWindow::OnLoginClicked);
    connect(register_button_, &QPushButton::clicked, this, &ChatWindow::OnRegisterClicked);
    connect(logout_button_, &QPushButton::clicked, this, &ChatWindow::OnLogoutClicked);
    connect(send_button_, &QPushButton::clicked, this, &ChatWindow::OnSendMessageClicked);
    connect(refresh_users_button_, &QPushButton::clicked, this, &ChatWindow::OnRefreshOnlineUsers);
    connect(online_users_list_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        selected_user_ = item->text().toStdString();
        selected_user_label_->setText(QString::fromStdString("Chatting with: " + selected_user_));
        message_display_->clear();
    });
    
    // Connect signals for thread-safe UI updates
    connect(this, &ChatWindow::SignalLoginSuccess, this, &ChatWindow::OnSignalLoginSuccess, Qt::QueuedConnection);
    connect(this, &ChatWindow::SignalRegistrationSuccess, this, &ChatWindow::OnSignalRegistrationSuccess, Qt::QueuedConnection);
    connect(this, &ChatWindow::SignalLoginError, this, &ChatWindow::OnSignalLoginError, Qt::QueuedConnection);
    connect(this, &ChatWindow::SignalUpdateStatus, this, &ChatWindow::OnSignalUpdateStatus, Qt::QueuedConnection);
    connect(this, &ChatWindow::SignalSwitchToChatScreen, this, &ChatWindow::OnSignalSwitchToChatScreen, Qt::QueuedConnection);
    connect(this, &ChatWindow::SignalUpdateUserList, this, &ChatWindow::OnSignalUpdateUserList, Qt::QueuedConnection);
    connect(this, &ChatWindow::SignalReceiveMessage, this, &ChatWindow::OnSignalReceiveMessage, Qt::QueuedConnection);
    connect(this, &ChatWindow::SignalDisconnected, this, &ChatWindow::OnSignalDisconnected, Qt::QueuedConnection);
}

void ChatWindow::OnRegisterClicked() {
    QString username = login_username_edit_->text();
    QString password = login_password_edit_->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter username and password");
        return;
    }
    
    if (!socket_client_->IsConnected()) {
        if (!socket_client_->Connect(SERVER_IP, SERVER_PORT,
                [this](const std::vector<uint8_t>& data) { OnMessageReceived(data); },
                [this]() { OnSocketDisconnected(); })) {
            QMessageBox::critical(this, "Connection Error", "Failed to connect to server");
            return;
        }
    }

    auto msg = Message::CreateRegisterMessage(username.toStdString(), password.toStdString());
    socket_client_->SendMessage(msg);
    login_status_label_->setText("Registering...");
}

void ChatWindow::OnLoginClicked() {
    QString username = login_username_edit_->text();
    QString password = login_password_edit_->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter username and password");
        return;
    }
    
    if (!socket_client_->IsConnected()) {
        if (!socket_client_->Connect(SERVER_IP, SERVER_PORT,
                [this](const std::vector<uint8_t>& data) { OnMessageReceived(data); },
                [this]() { OnSocketDisconnected(); })) {
            QMessageBox::critical(this, "Connection Error", "Failed to connect to server");
            return;
        }
    }

    auto msg = Message::CreateLoginMessage(username.toStdString(), password.toStdString());
    socket_client_->SendMessage(msg);
    login_status_label_->setText("Logging in...");
}

void ChatWindow::OnLogoutClicked() {
    if (!current_username_.empty()) {
        auto msg = Message::CreateLogoutMessage(current_username_);
        socket_client_->SendMessage(msg);
        socket_client_->Disconnect();
        
        current_username_.clear();
        selected_user_.clear();
        online_users_list_->clear();
        message_display_->clear();
        message_input_->clear();
        login_username_edit_->clear();
        login_password_edit_->clear();
        login_status_label_->setText("");
        
        stacked_widget_->setCurrentWidget(login_widget_);
    }
}

void ChatWindow::OnSendMessageClicked() {
    QString message = message_input_->text();
    
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a message");
        return;
    }
    
    if (selected_user_.empty()) {
        QMessageBox::warning(this, "Selection Error", "Please select a user to chat with");
        return;
    }
    
    auto msg = Message::CreateChatMessage(current_username_, selected_user_, message.toStdString());
    socket_client_->SendMessage(msg);
    DisplayMessage(QString::fromStdString(current_username_), message, true);
    message_input_->clear();
}

void ChatWindow::OnRefreshOnlineUsers() {
    auto msg = Message::CreateGetOnlineUsersMessage();
    socket_client_->SendMessage(msg);
}

void ChatWindow::OnMessageReceived(const std::vector<uint8_t>& data) {
    HandleServerResponse(data);
}

void ChatWindow::OnSocketDisconnected() {
    emit SignalDisconnected();
}

void ChatWindow::HandleServerResponse(const std::vector<uint8_t>& data) {
    MessageType type = Message::GetMessageType(data);
    
    switch (type) {
        case MessageType::SUCCESS: {
            std::string detail;
            Message::ParseErrorMessage(data, detail);
            
            if (detail.find("Login") != std::string::npos) {
                emit SignalLoginSuccess();
            } else if (detail.find("Registration") != std::string::npos) {
                emit SignalRegistrationSuccess();
            }
            break;
        }
        case MessageType::ERROR: {
            std::string error;
            Message::ParseErrorMessage(data, error);
            emit SignalLoginError(QString::fromStdString(error));
            break;
        }
        case MessageType::ONLINE_USERS_LIST: {
            std::vector<std::string> users;
            if (Message::ParseOnlineUsersList(data, users)) {
                emit SignalUpdateUserList(users);
            }
            break;
        }
        case MessageType::RECEIVE_MESSAGE: {
            std::string from, to, content;
            if (Message::ParseChatMessage(data, from, to, content)) {
                // Emit without checking selected_user_ to avoid data race
                // Handler in main thread will check if message is for current chat
                emit SignalReceiveMessage(QString::fromStdString(from), QString::fromStdString(content));
            }
            break;
        }
        case MessageType::SEND_MESSAGE: {
            std::string from, to, content;
            if (Message::ParseChatMessage(data, from, to, content)) {
                // Emit without checking selected_user_ to avoid data race
                emit SignalReceiveMessage(QString::fromStdString(from), QString::fromStdString(content));
            }
            break;
        }
        default:
            break;
    }
}

void ChatWindow::DisplayMessage(const QString& sender, const QString& content, bool is_sent) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString style = is_sent ? 
        "<div style='text-align: right; color: blue;'><b>You</b> (%1): %2</div>" :
        "<div style='text-align: left; color: green;'><b>%0</b> (%1): %2</div>";
    
    if (is_sent) {
        message_display_->append(QString(style).arg(timestamp, content));
    } else {
        message_display_->append(QString(style).arg(sender, timestamp, content));
    }
    
    message_display_->verticalScrollBar()->setValue(message_display_->verticalScrollBar()->maximum());
}

void ChatWindow::OnSignalLoginSuccess() {
    current_username_ = login_username_edit_->text().toStdString();
    current_user_label_->setText(QString::fromStdString("User: " + current_username_));
    
    // Get online users
    auto msg = Message::CreateGetOnlineUsersMessage();
    socket_client_->SendMessage(msg);
    
    stacked_widget_->setCurrentWidget(chat_widget_);
}

void ChatWindow::OnSignalRegistrationSuccess() {
    login_status_label_->setText("Registration successful! Please login.");
}

void ChatWindow::OnSignalLoginError(const QString& error) {
    login_status_label_->setText(QString("Error: ") + error);
}

void ChatWindow::OnSignalUpdateStatus(const QString& status) {
    login_status_label_->setText(status);
}

void ChatWindow::OnSignalSwitchToChatScreen() {
    stacked_widget_->setCurrentWidget(chat_widget_);
}

void ChatWindow::OnSignalUpdateUserList(const std::vector<std::string>& users) {
    online_users_list_->clear();
    for (const auto& user : users) {
        if (user != current_username_) {
            online_users_list_->addItem(QString::fromStdString(user));
        }
    }
}

void ChatWindow::OnSignalReceiveMessage(const QString& from, const QString& content) {
    // Only display message if it's from the currently selected user
    if (from.toStdString() == selected_user_) {
        DisplayMessage(from, content, false);
    }
}

void ChatWindow::OnSignalDisconnected() {
    current_username_.clear();
    selected_user_.clear();
    online_users_list_->clear();
    message_display_->clear();
    message_input_->clear();
    
    stacked_widget_->setCurrentWidget(login_widget_);
    QMessageBox::information(this, "Disconnected", "Connection to server lost");
}

void ChatWindow::RefreshUserList() {
    if (socket_client_ && socket_client_->IsConnected()) {
        auto msg = Message::CreateGetOnlineUsersMessage();
        socket_client_->SendMessage(msg);
    }
}
