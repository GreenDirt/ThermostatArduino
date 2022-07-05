#include "mainWindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QVector>


QSerialPort *serial = new QSerialPort();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    QLabel *label_img = new QLabel(this);
    QPixmap *pixmap_img = new QPixmap("./chauffeOff.png");
    label_img->setPixmap(pixmap_img->scaled(100, 100));
    ui->layoutGestionTemperature->insertWidget(1, label_img);

    int tempObjectif = 0;
    int nbrUpdates = 0;
    bool etatChauffage = 0;


    //timeline
    QVector <QVector <int>> etapes;
    QVector <QLabel*> afficheursEtapes;
    int indexTimeline = 0;
    int nbrSecondes = 0;


    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(updateInfos()));
    QObject::connect(ui->sliderTemperature, SIGNAL(valueChanged(int)), this, SLOT(setTemperatureSlider(int)));
    QObject::connect(ui->choixTemperature, SIGNAL(valueChanged(int)), this, SLOT(setTemperatureChoix(int)));
    QObject::connect(ui->btnAddEtape, SIGNAL(clicked()), this, SLOT(addEtape()));
    QObject::connect(ui->btnDeleteElmt, SIGNAL(clicked()), this, SLOT(deleteEtape()));
    QObject::connect(ui->btnStart, SIGNAL(clicked()), SLOT(startTimeline()));
    QObject::connect(ui->groupBoxTimeline, SIGNAL(clicked(bool)), this, SLOT(timelineActive(bool)));

    arduinoConnect();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTemperatureSlider(int temperature)
{
    ui->choixTemperature->setValue(temperature);

    tempObjectif = temperature;
    serial->write((QString::number(temperature) + "\n").toUtf8());
}

void MainWindow::setTemperatureChoix(int temperature)
{
    ui->sliderTemperature->setValue(temperature);
    tempObjectif = temperature;
    serial->write((QString::number(temperature) + "\n").toUtf8());
}

void MainWindow::updateInfos()
{
    if(serial->canReadLine())
    {
        serial->write((QString::number(tempObjectif) + "\n").toUtf8());

        QString infosBrutes = serial->readAll();

        QStringList listeInfos = infosBrutes.split(QLatin1Char(','));

        if(listeInfos.length() == 4)
        {
            bool valide;
            int temp = listeInfos[0].toInt(&valide);
            bool validePrec;
            int tempPrec = ui->labelTemperature->text().toInt(&validePrec);
            if(1 < temp < 100 and valide and temp > 10)
            {

                ui->labelTemperature->setText(listeInfos[0]);
            }
            else
            {
                if(validePrec)
                    ui->labelTemperature->setText(ui->labelTemperature->text() + "*");
            }

            if(nbrUpdates%20 == 0)
            {
                ui->sliderTemperature->setValue(listeInfos[1].toInt());
                ui->choixTemperature->setValue(listeInfos[1].toInt());
            }

            if(listeInfos[3].toInt(&valide))
            {
                label_img = new QLabel(this);
                pixmap_img = new QPixmap("./chauffeOn.png");
                label_img->setPixmap(pixmap_img->scaled(100, 100));

                ui->layoutGestionTemperature->removeItem(ui->layoutGestionTemperature->itemAt(1));
                ui->layoutGestionTemperature->insertWidget(1, label_img);
            }
            else
            {
                label_img = new QLabel(this);
                pixmap_img = new QPixmap("./chauffeOff.png");
                label_img->setPixmap(pixmap_img->scaled(100, 100));

                ui->layoutGestionTemperature->removeItem(ui->layoutGestionTemperature->itemAt(1));
                ui->layoutGestionTemperature->insertWidget(1, label_img);
            }

        }
        nbrUpdates += 1;
    }
}

void MainWindow::addEtape()
{
    ui->btnStart->setEnabled(true);
    ui->btnDeleteElmt->setEnabled(true);
    QVBoxLayout *etape = new QVBoxLayout;
    QLabel *duree = new QLabel;
    duree->setText(ui->setDureeEtape->text());
    QLabel *temperature = new QLabel;
    temperature->setText(ui->setTemperatureEtape->text());
    etape->addWidget(duree);
    etape->addWidget(temperature);

    bool valide;
    etapes.push_back(QVector<int>(2));
    etapes[etapes.length()-1][0] = ui->setDureeEtape->time().minute()*60 + ui->setDureeEtape->time().second();
    etapes[etapes.length()-1][1] = ui->setTemperatureEtape->text().toInt(&valide);

    afficheursEtapes.push_back(duree);


    ui->layoutTimeline->addLayout(etape);


}

void MainWindow::deleteEtape()
{
    ui->btnStart->setEnabled(false);
    ui->btnDeleteElmt->setEnabled(false);
    for(int i = 0; i < etapes.length(); i++)
        ui->layoutTimeline->removeItem(ui->layoutTimeline->itemAt(0));
    indexTimeline = 0;
    etapes.clear();
}

void MainWindow::startTimeline()
{
    nbrSecondes = 0;
    indexTimeline = 0;
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeline()));
    timer->start(1000);
}

void MainWindow::updateTimeline()
{
    if(nbrSecondes <= etapes[indexTimeline][0])
    {
        int avancee = etapes[indexTimeline][0] - nbrSecondes;
        QString nouvelleValeur = QString::number(avancee/60) + " : " + QString::number(avancee%60);
        afficheursEtapes[indexTimeline]->setText(nouvelleValeur);

        ui->choixTemperature->setValue(etapes[indexTimeline][1]);
    }
    else if(indexTimeline < etapes.length()-1)
    {
        indexTimeline += 1;
        nbrSecondes = 0;
    }

    nbrSecondes += 1;
}

int MainWindow::arduinoConnect()
{
    QString portName = "";
    qDebug() << "Ports disponibles : " << QSerialPortInfo::availablePorts().length();
    foreach (const QSerialPortInfo &serialPortinfo, QSerialPortInfo::availablePorts()) {
        qDebug() << "Port : " << serialPortinfo.portName();
        portName = serialPortinfo.portName();
    }
    if(QSerialPortInfo::availablePorts().length())
    {
        serial->setPortName(portName);
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->open(QIODevice::ReadWrite);

        return 0;
    }
    else
    {
        messageBoxReconnexion();
    }
}

void MainWindow::messageBoxReconnexion()
{
    int reponse = QMessageBox::information(this, "Erreur de connexion", "Impossible de se connecter à l'arduino, réessayer ?", QMessageBox::Yes | QMessageBox::No);
    if(reponse == QMessageBox::Yes)
        arduinoConnect();
}

void MainWindow::timelineActive(bool active)
{
    active = !active;
    ui->choixTemperature->setEnabled(active);
    ui->sliderTemperature->setEnabled(active);
    if(!active)
    {
        ui->choixTemperature->setStyleSheet("color: \"grey\"");
        ui->sliderTemperature->setStyleSheet("color: \"grey\"");
    }
    else
    {
        ui->choixTemperature->setStyleSheet("color: \"green\"");
        ui->sliderTemperature->setStyleSheet("color: \"green\"");
    }
}
