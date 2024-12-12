#include "main_window.h"
#include "./ui_main_window.h"

#include <QMovie>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <sys/stat.h>
#include <iostream>

#include "search/engine/kmp.h"
#include "search/engine/boyer_moore.h"
#include "search/engine/naive.h"

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

    this->ui->radioButton_kmp->setChecked(true);

    connect(&computationWatcher_, &QFutureWatcher<void>::finished, this, &MainWindow::OnSearchFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnSearchFinished() {
    auto display = ui->treeWidget_results;

    std::size_t total_files_found = search_results_.size();
    std::size_t total_entries_found = 0;

    for (auto const &result : search_results_) {
        total_entries_found += result.second.size();

        auto item = new QTreeWidgetItem();
        item->setText(0, result.first.c_str());

        int count = 0;
        for (auto &entry : result.second) {
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
            auto content = entry.line_content;
            content.pop_back();
            itemLineContent->setText(0, QString::fromUtf8("Content: ") + QString::fromStdString(content));
            itemEntry->addChild(itemLineContent);

            item->addChild(itemEntry);
        }

        display->addTopLevelItem(item);
    }

    std::cout << "Files: " << total_files_found << '\n';
    std::cout << "Entries: " << total_entries_found << '\n';
    std::cout.flush();

    display->setHeaderLabel((std::string("Files: ") + std::to_string(total_files_found) + ", Entries: " + std::to_string(total_entries_found)).c_str());

    search_results_.clear();

    ui->stackedWidget->setCurrentIndex(PAGE_RESULTS);
}

void MainWindow::on_pushButton_search_clicked()
{
    std::cout << "Search button clicker\n";

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

    if (this->ui->radioButton_kmp->isChecked()) {
        this->searcher_.ResetEngine(new search::engine::Kmp(target.toStdString()));
    } else if (this->ui->radioButton_bm->isChecked()) {
        this->searcher_.ResetEngine(new search::engine::BoyerMoore(target.toStdString()));
    } else {
        this->searcher_.ResetEngine(new search::engine::Naive(target.toStdString()));
    }

    std::cout << "Path: " << file_path.toStdString() << '\n';
    std::cout << "Target: " << target.toStdString() << '\n';
    std::cout.flush();

    ui->stackedWidget->setCurrentIndex(PAGE_WAIT);

    auto future = QtConcurrent::run([this, file_path, target]() { this->search_results_ = this->searcher_.Search(file_path.toStdString(), target.toStdString()).result; });

    computationWatcher_.setFuture(future);
}

void MainWindow::on_pushButton_go_to_main_clicked()
{
    ui->treeWidget_results->clear();
    ui->stackedWidget->setCurrentIndex(PAGE_SEARCH);
}


void MainWindow::on_pushButton_clear_clicked()
{
    ui->lineEdit_file_path->clear();
    ui->lineEdit_target->clear();
}


void MainWindow::on_pushButton_choose_file_clicked()
{
    auto file_path = QFileDialog::getOpenFileName(this, "Choose a file");

    if (!file_path.isEmpty()) {
        ui->lineEdit_file_path->setText(file_path);
    }
}


void MainWindow::on_pushButton_choose_folder_clicked()
{
    auto folder_path = QFileDialog::getExistingDirectory(this, "Choose a folder");

    if (!folder_path.isEmpty()) {
        ui->lineEdit_file_path->setText(folder_path);
    }
}

