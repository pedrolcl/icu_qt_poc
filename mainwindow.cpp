/*
    ICUQtPOC - Integrating libICU for Unicode Conversion in Qt apps
    Copyright (C) 2022 Pedro Lopez-Cabanillas
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <QFileDialog>
#include <QDebug>
#include <QDataStream>
#include <QFile>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "icuconverter.h"
#include "icudetector.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    ui->comboBox->clear();
    QList<QByteArray> names = ICUConverter::charsetNames();
    std::sort(names.begin(), names.end());
    foreach(const QByteArray& cs, names) {
        ui->comboBox->addItem(QString::fromLatin1(cs), cs);
    }
    ui->comboBox->setCurrentIndex(-1);
    connect(ui->comboBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::slotDecode);
    resize(640, 480);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setNameFilter(tr("Text Files (*.txt)"));
    dlg.setViewMode(QFileDialog::List);

    if (dlg.exec()) {
        QStringList fileNames = dlg.selectedFiles();
        QFileInfo fi(fileNames.first());
        QFile f(fi.absoluteFilePath());
        if (f.open(QIODevice::ReadOnly)) {
            m_data = f.readAll();
            ui->textEdit->clear();
            setWindowTitle("ICUQtPOC: " + fi.fileName());
            f.close();
            
            ICUDetector det;
            QByteArray charset = det.detect(m_data);
            if (charset.isEmpty()) {
                ui->comboBox->setCurrentIndex(-1);
            } else {
                int cur = ui->comboBox->currentIndex();
                int idx = ui->comboBox->findData(charset);
                if (cur == idx) {
                    slotDecode(idx);
                } else {
                    ui->comboBox->setCurrentIndex(idx);
                }
            }
        }
    }
}

void MainWindow::slotDecode(const int idx)
{
    if (idx < 0) {
        qDebug() << Q_FUNC_INFO << idx;
        ui->textEdit->clear();
    } else {
        QByteArray codecName = ui->comboBox->itemData(idx).toByteArray();
        qDebug() << Q_FUNC_INFO << codecName;
        ICUConverter cnv(codecName);
        ui->textEdit->setText(cnv.convertToUnicode(m_data));
    }
}
