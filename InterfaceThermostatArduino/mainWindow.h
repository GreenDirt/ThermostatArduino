#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <QtWebEngineWidgets/QtWebEngineWidgets>
#include <QVBoxLayout>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void messageBoxReconnexion();


public slots:
    void setTemperatureSlider(int temperature);
    void setTemperatureChoix(int temperature);
    void addEtape();
    void updateInfos();
    void deleteEtape();
    void startTimeline();
    void updateTimeline();
    int arduinoConnect();
    void timelineActive(bool active);

protected:
    int tempObjectif;
    int nbrUpdates;
    bool etatChauffage;
    QPixmap *pixmap_img;
    QLabel *label_img;
    QVector <QVector <int>> etapes;
    QVector <QLabel*> afficheursEtapes;
    int indexTimeline;
    int nbrSecondes;
    QTimer *timer;


private:
    Ui::MainWindow *ui;



};

#endif // MAIN_H
