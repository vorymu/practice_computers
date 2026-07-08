#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QMenu>
#include <QLabel>
#include <QCollator>
#include <QIntValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"Картинка", "Модель", "HDD", "CPU"});

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    listViewModel = new QStringListModel(this);
    ui->listView->setModel(listViewModel);

    ui->sizeEdit->setValidator(new QIntValidator(1, 100, this));

    connect(ui->listWidget, &QTableWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenu);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::shellSort(QVector<Computer>& arr) {
    int n = arr.size();

    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);

    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            Computer temp = arr[i];
            int j;

            for (j = i; j >= gap && collator.compare(arr[j - gap].model, temp.model) > 0; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

void MainWindow::on_btnSortShell_clicked() {
    if (computers.isEmpty()) {
        QMessageBox::information(this, "Внимание", "Таблица пуста! Сначала сгенерируйте данные.");
        return;
    }

    shellSort(computers);
    updateTable();

    this->statusBar()->showMessage("База данных успешно отсортирована методом Шелла.", 3000);
}

void MainWindow::on_btnGenerate_clicked() {
    bool ok;
    int n = ui->sizeEdit->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка ввода", "Поле ввода пустое! Укажите размер массива.");
        return;
    }

    if (n <= 0) {
        QMessageBox::warning(this, "Ошибка ввода", "Размер массива должен быть больше нуля!");
        return;
    }

    if (n > 100) {
        QMessageBox::warning(this, "Защита от зависания", "Максимальный размер массива - 100 элементов во избежание зависания программы!");
        ui->sizeEdit->setText("100");
        return;
    }

    computers.clear();

    QStringList availableIcons = {
        ":/pc.png",
        ":/pc2.png",
        ":/pc3.png",
        ":/pc4.png",
        ":/pc5.png"
    };

    QStringList models = {"Asus", "Acer", "Lenovo", "HP", "Dell"};
    QVector<qreal> hddSizes = {256.0, 512.0, 1024.0};
    QStringList cpus = {"Intel i5", "Intel i7", "AMD Ryzen 5"};

    while (computers.size() < n) {
        Computer newComp;
       newComp.model = models[QRandomGenerator::global()->bounded(models.size())] + "_" + QString::number(QRandomGenerator::global()->bounded(100) + 1);
        newComp.hdd = hddSizes[QRandomGenerator::global()->bounded(hddSizes.size())];
        newComp.cpu = cpus[QRandomGenerator::global()->bounded(cpus.size())];

        int randomIconIndex = QRandomGenerator::global()->bounded(availableIcons.size());
        newComp.iconPath = availableIcons[randomIconIndex];

        bool isDuplicate = false;
        for (const Computer &c : computers) {
            if (c.model == newComp.model && c.hdd == newComp.hdd && c.cpu == newComp.cpu) {
                isDuplicate = true;
                break;
            }
        }

        if (!isDuplicate) {
            computers.append(newComp);
        }
    }

    updateTable();
    this->statusBar()->showMessage("Сгенерирована база данных из " + QString::number(n) + " уникальных ПК.", 3000);
}

void MainWindow::updateTable() {
    ui->tableWidget->setRowCount(0);

    for (int i = 0; i < computers.size(); ++i) {
        ui->tableWidget->insertRow(i);

        QLabel *imgLabel = new QLabel();
        QPixmap pixmap(computers[i].iconPath);

        if (!pixmap.isNull()) {
            imgLabel->setPixmap(pixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imgLabel->setText("Нет фото");
        }
        imgLabel->setAlignment(Qt::AlignCenter);
        ui->tableWidget->setCellWidget(i, 0, imgLabel);

        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(computers[i].model));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(computers[i].hdd) + " ГБ"));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(computers[i].cpu));

        QStringList onlyNames;
        for (const Computer &c : computers) {
            onlyNames.append(c.model);
        }
        listViewModel->setStringList(onlyNames);
    }
}

void MainWindow::on_btnAddToList_clicked() {
    int row = ui->tableWidget->currentRow();

    if (row >= 0 && row < computers.size()) {

        QString info = QString("ПК: %1 | %2 ГБ | %3")
                           .arg(computers[row].model)
                           .arg(computers[row].hdd)
                           .arg(computers[row].cpu);

        QList<QListWidgetItem*> foundItems = ui->listWidget->findItems(info, Qt::MatchExactly);

        if (!foundItems.isEmpty()) {
            QMessageBox::warning(this, "Элемент уже добавлен",
                                 QString("Компьютер %1 (%2 ГБ) с процессором %3 уже есть в корзине!")
                                     .arg(computers[row].model)
                                     .arg(computers[row].hdd)
                                     .arg(computers[row].cpu));
            return;
        }

        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setIcon(QIcon(computers[row].iconPath));
        listItem->setText(info);

        ui->listWidget->addItem(listItem);
        this->statusBar()->showMessage("Добавлено в список: " + computers[row].model, 3000);
    } else {
        QMessageBox::information(this, "Внимание", "Пожалуйста, сначала кликните на нужную строку в таблице!");
    }
}
void MainWindow::on_btnDelete_clicked() {
    delete ui->listWidget->currentItem();
}

void MainWindow::showContextMenu(const QPoint &pos) {
    QMenu menu(this);

    menu.addAction("Сортировать список стандартно", [this]() {
        ui->listWidget->sortItems(Qt::AscendingOrder);
    });

    menu.exec(ui->listWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        this->close();
    }
}
