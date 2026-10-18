#include "userformwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

UserFormWidget::UserFormWidget(QWidget *parent)
    : QWidget(parent)
    , m_username(new QLineEdit(this))
    , m_email(new QLineEdit(this))
    , m_addBtn(new QPushButton(tr("Добавить"), this))
    , m_refreshBtn(new QPushButton(tr("Обновить список"), this))
{
    m_username->setPlaceholderText(tr("Введите имя пользователя"));
    m_email->setPlaceholderText(tr("Введите адрес электронной почты"));

    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(tr("Имя пользователя:"), this), 0, 0);
    layout->addWidget(m_username, 0, 1);
    layout->addWidget(new QLabel(tr("Электронная почта:"), this), 1, 0);
    layout->addWidget(m_email, 1, 1);
    layout->addWidget(m_addBtn, 2, 0);
    layout->addWidget(m_refreshBtn, 2, 1);
    layout->setColumnStretch(1, 1);

    connect(m_addBtn, &QPushButton::clicked, this, &UserFormWidget::onAddClicked);
    connect(m_refreshBtn, &QPushButton::clicked, this, &UserFormWidget::refreshRequested);
    connect(m_email, &QLineEdit::returnPressed, this, &UserFormWidget::onAddClicked);
    connect(m_username, &QLineEdit::returnPressed, this, [this] { m_email->setFocus(); });
}

void UserFormWidget::setEnabledState(bool enabled)
{
    m_addBtn->setEnabled(enabled);
    m_refreshBtn->setEnabled(enabled);
}

void UserFormWidget::clearInputs()
{
    m_username->clear();
    m_email->clear();
}

void UserFormWidget::focusUsername()
{
    m_username->setFocus();
}

void UserFormWidget::onAddClicked()
{
    const QString username = m_username->text().trimmed();
    const QString email = m_email->text().trimmed();

    if (username.isEmpty()) {
        m_username->setFocus();
        emit inputRejected(tr("Введите имя пользователя"));
        return;
    }
    if (email.isEmpty()) {
        m_email->setFocus();
        emit inputRejected(tr("Введите адрес электронной почты"));
        return;
    }

    emit submitted(username, email);
}