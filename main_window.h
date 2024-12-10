#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QFuture>
#include <QFutureWatcher>

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
    void on_pushButton_2_clicked();

private:
    void StartSearch();
    void PerformSearch();
    void OnSearchFinished();

    Ui::MainWindow *ui;

    QFuture<void> computationFuture_;
    QFutureWatcher<void> computationWatcher_;
};
#endif // MAIN_WINDOW_H
