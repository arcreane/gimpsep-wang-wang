#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QTimer> // 加入QTimer头文件

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
    void on_hebing_clicked();
    void on_print_clicked();
    void on_chicun_clicked();
    void on_liangdu_clicked();
    void on_andu_clicked();
    void on_pushButton_clicked();

    void on_renlian_clicked();
    void updateFrame(); // 加入新的槽函数声明

private:
    Ui::MainWindow *ui;
    cv::Mat mCVimage;
    int liangdu_level = 0; // 用于跟踪亮度等级
    int andu_level = 5;    // 用于跟踪暗度等级

    // 新添加的私有成员
    cv::CascadeClassifier faceDetector;
    cv::VideoCapture camera;
    QTimer *timer;
};

#endif // MAINWINDOW_H
