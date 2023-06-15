#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QTimer> 

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_showImage_clicked();
    void on_dilatation_clicked();
    void on_mergeimages_clicked();
    void on_print_clicked();
    void on_Stitching_clicked();
    void on_Brighten_clicked();
    void on_darken_clicked();
    void on_pushButton_clicked();

    void on_Detection_clicked();
    void updateFrame(); // Add new slot function declaration

private:
    Ui::MainWindow *ui;
    cv::Mat mCVimage;
    int liangdu_level = 0; // Used to track brightness levels
    int andu_level = 5;    // for tracking darkness levels

    cv::CascadeClassifier faceDetector;
    cv::VideoCapture camera;
    QTimer *timer;
};

#endif // MAINWINDOW_H
