#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QFuture>
#include <QFutureWatcher>
#include <vector>

#include "search/search.h"

#define PAGE_SEARCH 0
#define PAGE_WAIT 1
#define PAGE_RESULTS 2

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_search_clicked();

    void on_pushButton_go_to_main_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_choose_file_clicked();

    void on_pushButton_choose_folder_clicked();

private:
    void OnSearchFinished();

    Ui::MainWindow *ui;

    QFutureWatcher<void> computationWatcher_;
    std::vector<search::Result> search_results_;
};
#endif // MAIN_WINDOW_H
