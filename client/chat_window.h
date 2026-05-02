#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QComboBox>
#include <memory>
#include "socket_client.h"

class ChatWindow : public QMainWindow {
    Q_OBJECT
    
public:
    ChatWindow(QWidget* parent = nullptr);
    ~ChatWindow();
    
signals:
    // Thread-safe signals for UI updates
    void SignalLoginSuccess();
    void SignalRegistrationSuccess();
    void SignalLoginError(const QString& error);
    void SignalUpdateStatus(const QString& status);
    void SignalSwitchToChatScreen();
    void SignalUpdateUserList(const std::vector<std::string>& users);
    void SignalReceiveMessage(const QString& from, const QString& content);
    void SignalDisconnected();
    
private slots:
    void OnRegisterClicked();
    void OnLoginClicked();
    void OnLogoutClicked();
    void OnSendMessageClicked();
    void OnRefreshOnlineUsers();
    
    // Socket callbacks
    void OnMessageReceived(const std::vector<uint8_t>& data);
    void OnSocketDisconnected();
    
    // Signal handlers for thread-safe UI updates
    void OnSignalLoginSuccess();
    void OnSignalRegistrationSuccess();
    void OnSignalLoginError(const QString& error);
    void OnSignalUpdateStatus(const QString& status);
    void OnSignalSwitchToChatScreen();
    void OnSignalUpdateUserList(const std::vector<std::string>& users);
    void OnSignalReceiveMessage(const QString& from, const QString& content);
    void OnSignalDisconnected();
    
private:
    void SetupUI();
    void SetupLoginScreen();
    void SetupChatScreen();
    void CreateConnections();
    void HandleServerResponse(const std::vector<uint8_t>& data);
    void DisplayMessage(const QString& sender, const QString& content, bool is_sent);
    void RefreshUserList();
    
    // Network
    std::unique_ptr<SocketClient> socket_client_;
    std::string current_username_;
    std::string selected_user_;
    
    // UI Components
    QStackedWidget* stacked_widget_;
    
    // Login screen
    QWidget* login_widget_;
    QLineEdit* login_username_edit_;
    QLineEdit* login_password_edit_;
    QPushButton* login_button_;
    QPushButton* register_button_;
    QLabel* login_status_label_;
    
    // Chat screen
    QWidget* chat_widget_;
    QTextEdit* message_display_;
    QLineEdit* message_input_;
    QPushButton* send_button_;
    QPushButton* logout_button_;
    QListWidget* online_users_list_;
    QPushButton* refresh_users_button_;
    QComboBox* selected_user_combo_;
    QLabel* current_user_label_;
    QLabel* selected_user_label_;
};
