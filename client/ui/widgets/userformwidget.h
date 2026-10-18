#pragma once

#include <QWidget>

class QLineEdit;
class QPushButton;

class UserFormWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserFormWidget(QWidget *parent = nullptr);

    void setEnabledState(bool enabled);

public slots:
    void clearInputs();
    void focusUsername();

signals:
    void submitted(const QString &username, const QString &email);
    void refreshRequested();
    void inputRejected(const QString &reason);

private slots:
    void onAddClicked();

private:
    QLineEdit *m_username = nullptr;
    QLineEdit *m_email = nullptr;
    QPushButton *m_addBtn = nullptr;
    QPushButton *m_refreshBtn = nullptr;
};
