#include "mainwindow.h"

#include "app/appcontroller.h"
#include "usertablemodel.h"
#include "widgets/userformwidget.h"

#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>

MainWindow::MainWindow(AppController *controller, UserTableModel *model, QWidget *parent)
    : QMainWindow(parent)
    , m_controller(controller)
    , m_model(model)
{
    setupUi();
    setupConnections();
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("Клиент пользователей"));
    resize(720, 520);

    auto *formBox = new QGroupBox(tr("Новый пользователь"), this);
    auto *formLayout = new QVBoxLayout(formBox);
    m_form = new UserFormWidget(formBox);
    formLayout->addWidget(m_form);

    auto *tableBox = new QGroupBox(tr("Список пользователей"), this);
    auto *tableLayout = new QVBoxLayout(tableBox);
    auto *table = new QTableView(tableBox);
    table->setModel(m_model);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    tableLayout->addWidget(table);

    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);
    root->addWidget(formBox);
    root->addWidget(tableBox, 1);
    setCentralWidget(central);

    m_status = new QLabel(this);
    statusBar()->addPermanentWidget(m_status);
    updateStatusIndicator(false);
    m_form->setEnabledState(false);
    statusBar()->showMessage(tr("Ожидание подключения к серверу…"));
}

void MainWindow::setupConnections()
{
    connect(m_form, &UserFormWidget::submitted, m_controller, &AppController::requestAddUser);
    connect(m_form, &UserFormWidget::refreshRequested, m_controller, &AppController::requestRefresh);
    connect(m_form, &UserFormWidget::inputRejected, this, &MainWindow::onFormInputRejected);
    connect(m_controller, &AppController::connectionStateChanged, this, &MainWindow::onConnectionStateChanged);
    connect(m_controller, &AppController::addSucceeded, m_form, &UserFormWidget::clearInputs);
    connect(m_controller, &AppController::addSucceeded, m_form, &UserFormWidget::focusUsername);
    connect(m_controller, &AppController::infoMessage, this, &MainWindow::onInfoMessage);
    connect(m_controller, &AppController::errorMessage, this, &MainWindow::onErrorMessage);
}

void MainWindow::onConnectionStateChanged(bool connected)
{
    updateStatusIndicator(connected);
    m_form->setEnabledState(connected);
}

void MainWindow::onInfoMessage(const QString &text)
{
    statusBar()->showMessage(text, 5000);
}

void MainWindow::onErrorMessage(const QString &text)
{
    statusBar()->showMessage(text, 8000);
}

void MainWindow::onFormInputRejected(const QString &reason)
{
    QMessageBox::warning(this, tr("Проверка данных"), reason);
}

void MainWindow::updateStatusIndicator(bool connected)
{
    m_status->setText(connected ? tr("Подключено") : tr("Отключено"));
    m_status->setStyleSheet(connected ? QStringLiteral("color: green;") : QStringLiteral("color: gray;"));
}
