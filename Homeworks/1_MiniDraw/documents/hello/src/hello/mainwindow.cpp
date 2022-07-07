#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qmessagebox.h>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	CreateButtons();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::HelloWorld() {
	QMessageBox::about(this, tr("Hello world"),
		tr("Well done! Go ahead."));
}

void MainWindow::CreateButtons()
{
	hello_world_action_ = new QAction(tr("&Hello world"), this);
	hello_world_action_2 = new QAction(tr("&Hello world 2"), this);
	connect(hello_world_action_, &QAction::triggered, this, &MainWindow::HelloWorld);
	connect(hello_world_action_2, &QAction::triggered, this, &MainWindow::HelloWorld);
	main_menu_ = menuBar()->addMenu(tr("&menu"));
	main_menu_->addAction(hello_world_action_);
	main_menu_->addAction(hello_world_action_2);
	main_toolbar_ = addToolBar(tr("&toolbar"));
	main_toolbar_->addAction(hello_world_action_);
	main_toolbar_->addAction(hello_world_action_2);
}