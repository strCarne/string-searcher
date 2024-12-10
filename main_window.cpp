#include "main_window.h"
#include "./ui_main_window.h"

#include <QMovie>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <sys/stat.h>

#include "search/search.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(PAGE_SEARCH);

    QPixmap pixmap("/home/strcarne/Documents/qt-based/string-searcher/logo.png");
    ui->label->setPixmap(pixmap);

    QMovie *movie = new QMovie("/home/strcarne/Documents/qt-based/string-searcher/loading.gif");
    movie->setScaledSize(QSize(200, 200));
    movie->setSpeed(80);
    ui->label_wait->setMovie(movie);
    movie->start();

    connect(&computationWatcher_, &QFutureWatcher<void>::finished, this, &MainWindow::OnSearchFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnSearchFinished() {
    auto search_results = computationFuture_.result();

    auto display = ui->treeWidget_results;

    for (auto const &result : search_results) {
        if (!result.entries.empty()) {
            auto item = new QTreeWidgetItem();
            item->setText(0, result.file_path.c_str());

            int count = 0;
            for (auto &entry : result.entries) {
                count += 1;

                auto itemEntry = new QTreeWidgetItem();
                itemEntry->setText(0, (std::string("entry") + " " + std::to_string(count)).c_str());

                auto itemLineNumber = new QTreeWidgetItem();
                itemLineNumber->setText(0, QString::fromUtf8("Line number: ") + QString::number(entry.line_number));
                itemEntry->addChild(itemLineNumber);

                auto itemLineOffset = new QTreeWidgetItem();
                itemLineOffset->setText(0, QString::fromUtf8("Line offset: ") + QString::number(entry.line_offset));
                itemEntry->addChild(itemLineOffset);

                auto itemLineContent = new QTreeWidgetItem();
                itemLineContent->setText(0, QString::fromUtf8("Content: ") + QString::fromStdString(entry.line_content));
                itemEntry->addChild(itemLineContent);

                item->addChild(itemEntry);
            }

            display->addTopLevelItem(item);
        }
    }

    ui->stackedWidget->setCurrentIndex(PAGE_RESULTS);
}


void MainWindow::on_pushButton_choose_clicked()
{
    auto file_path = QFileDialog::getOpenFileName(this, "Choose a folder or file");

    if (!file_path.isEmpty()) {
        ui->lineEdit_file_path->setText(file_path);
    }
}


void MainWindow::on_pushButton_search_clicked()
{
    auto file_path = ui->lineEdit_file_path->text();
    if (file_path.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Search path is not set.");

        return;
    }

    struct stat stat_buf;
    if (stat(file_path.toUtf8(), &stat_buf) != 0) {
        QMessageBox::critical(this, "Error", "Given search path does not exist.");

        return;
    }

    auto target = ui->lineEdit_target->text();
    if (target.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Target string is not set.");

        return;
    }

    ui->stackedWidget->setCurrentIndex(PAGE_WAIT);

    computationFuture_ = QtConcurrent::run([file_path, target]() { return search::DoMultithreaded(file_path.toStdString(), target.toStdString()); });
    computationWatcher_.setFuture(computationFuture_);
}


void MainWindow::on_pushButton_go_to_main_clicked()
{
    ui->treeWidget_results->clear();
    ui->stackedWidget->setCurrentIndex(PAGE_SEARCH);
}

