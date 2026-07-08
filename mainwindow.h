#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QKeyEvent>
#include <QVector>

struct Computer {
    QString model;
    qreal hdd;
    QString cpu;
    QString iconPath;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_btnGenerate_clicked();
    void on_btnSortShell_clicked();
    void on_btnAddToList_clicked();
    void on_btnDelete_clicked();
    void showContextMenu(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    QVector<Computer> computers;
    QStringListModel *listViewModel;

    void updateTable();
    void shellSort(QVector<Computer>& arr);
};

#endif // MAINWINDOW_H
