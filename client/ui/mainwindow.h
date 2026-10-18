#pragma once

#include <QMainWindow>

class AppController;
class UserTableModel;
class UserFormWidget;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(AppController *controller, UserTableModel *model, QWidget *parent = nullptr);

private slots:
    void onConnectionStateChanged(bool connected);
    void onInfoMessage(const QString &text);
    void onErrorMessage(const QString &text);
    void onFormInputRejected(const QString &reason);

private:
    void setupUi();
    void setupConnections();
    void updateStatusIndicator(bool connected);

    AppController *m_controller = nullptr;
    UserTableModel *m_model = nullptr;
    UserFormWidget *m_form = nullptr;
    QLabel *m_status = nullptr;
};
