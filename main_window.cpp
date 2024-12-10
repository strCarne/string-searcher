#include "main_window.h"
#include "./ui_main_window.h"

#include <QMovie>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pixmap("/home/strcarne/Documents/qt-based/string-searcher/logo.png");
    ui->label->setPixmap(pixmap);

    QMovie *movie = new QMovie("/home/strcarne/Documents/qt-based/string-searcher/loading.gif");
    movie->setScaledSize(QSize(180, 180));
    movie->setSpeed(80);
    ui->label_wait->setMovie(movie);
    movie->start();

    connect(&computationWatcher_, &QFutureWatcher<void>::finished, this, &MainWindow::OnSearchFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(PAGE_WAIT);

    StartSearch();
}

void MainWindow::StartSearch() {
    computationFuture_ = QtConcurrent::run([this]() {this->PerformSearch();});
    computationWatcher_.setFuture(computationFuture_);
}

void MainWindow::PerformSearch() {
    QThread::sleep(3);
}

void MainWindow::OnSearchFinished() {
    ui->stackedWidget->setCurrentIndex(PAGE_RESULTS);
}
