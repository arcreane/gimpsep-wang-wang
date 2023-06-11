#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QInputDialog>

#include <QProcess>
//Q_UNUSED(_a)
    int brightnessLevel;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     brightnessLevel = 5;  // 初始亮度等级为5
     // 初始化 timer，并连接到 updateFrame 槽函数
       timer = new QTimer(this);
       connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_showImage_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择图片"), ".", tr("Image Files(*.jpg *.png)"));

    // 加载图片
    mCVimage = cv::imread(path.toStdString());

    // 转换颜色空间从BGR到RGB
    cv::cvtColor( mCVimage, mCVimage, cv::COLOR_BGR2RGB);

    // 将OpenCV图像转换为Qt图像
    QImage qimg = QImage((uchar*)mCVimage.data, mCVimage.cols, mCVimage.rows, QImage::Format_RGB888);

    // 设置QLabel自动缩放内容
    ui->label->setScaledContents(true);

    // 设置QLabel的Pixmap
    ui->label->setPixmap(QPixmap::fromImage(qimg));
}



void MainWindow::on_dilatation_clicked()
{
    int dilatationSize = 5; // 你可以根据需求设置这个值

        // 创建用于膨胀的内核
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilatationSize + 1, 2 * dilatationSize + 1),
                                                    cv::Point(dilatationSize, dilatationSize));

        // 膨胀操作
        cv::Mat dilatedImage;
        cv::dilate(mCVimage, dilatedImage, element);

        // 将处理后的图像转换为QImage
        QImage qimg = QImage((uchar*)dilatedImage.data, dilatedImage.cols, dilatedImage.rows, QImage::Format_RGB888);

        // 在 QLabel 上显示处理后的图像
        ui->label->setPixmap(QPixmap::fromImage(qimg));
}

void MainWindow::on_hebing_clicked()
{
    // 加载第二张图片
        QString path2 = QFileDialog::getOpenFileName(this, tr("选择第二张图片"), ".", tr("Image Files(*.jpg *.png)"));
        cv::Mat img2 = cv::imread(path2.toStdString());

        // 转换颜色空间从BGR到RGB
        cv::cvtColor(img2, img2, cv::COLOR_BGR2RGB);

        // 如果第一张图片和第二张图片的高度不同，将第二张图片的高度调整为与第一张图片相同
        if (img2.rows != mCVimage.rows) {
            double scale = (double)mCVimage.rows / img2.rows;
            cv::resize(img2, img2, cv::Size(), scale, scale);
        }

        // 拼接两张图片
        cv::Mat result;
        cv::hconcat(mCVimage, img2, result);

        // 将OpenCV图像转换为Qt图像
        QImage qimg = QImage((uchar*)result.data, result.cols, result.rows, QImage::Format_RGB888);

        // 在 QLabel 上显示处理后的图像
        ui->label->setPixmap(QPixmap::fromImage(qimg));

        // 在保存之前，将颜色空间从 RGB 转换回 BGR
        //cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

        // 将结果保存为文件
            cv::imwrite("/home/luo/pictures/result.jpg", result);
}


void MainWindow::on_print_clicked()
{
    // 检查mCVimage是否为空，如果为空，说明没有加载任何图片，就不必保存
        if(mCVimage.empty())
        {
            QMessageBox::warning(this, tr("错误"), tr("没有图片可保存"));
            return;
        }

        QString path = QFileDialog::getSaveFileName(this, tr("保存图片"), ".", tr("Image Files(*.jpg)"));
        if (path.isEmpty())
        {
            return;
        }
        else
        {
            // 在保存之前，将颜色空间从 RGB 转换回 BGR
            cv::Mat saveImg;
            cv::cvtColor(mCVimage, saveImg, cv::COLOR_RGB2BGR);
            // 使用imwrite()保存图片
            cv::imwrite(path.toStdString(), saveImg);
        }
}

void MainWindow::on_chicun_clicked()
{
    // 检查mCVimage是否为空，如果为空，说明没有加载任何图片，就不必保存
        if(mCVimage.empty())
        {
            QMessageBox::warning(this, tr("错误"), tr("没有图片可处理"));
            return;
        }

        bool ok;
            int newWidth = QInputDialog::getInt(this, tr("输入新宽度"), tr("宽度:"), 1, 1, 10000, 1, &ok);
            int newHeight = QInputDialog::getInt(this, tr("输入新高度"), tr("高度:"), 1, 1, 10000, 1, &ok);

            if(ok) // 如果用户点击了"OK"，那么就更改图像大小
            {
                cv::resize(mCVimage, mCVimage, cv::Size(newWidth, newHeight));

                // 将处理后的图像转换为QImage
                QImage qimg = QImage((uchar*)mCVimage.data, mCVimage.cols, mCVimage.rows, QImage::Format_RGB888);

                // 在 QLabel 上显示处理后的图像
                ui->label->setPixmap(QPixmap::fromImage(qimg));

                QString path = QFileDialog::getSaveFileName(this, tr("保存图片"), ".", tr("Image Files(*.jpg)"));
                if (path.isEmpty())
                {
                    return;
                }
                else
                {
                    // 在保存之前，将颜色空间从 RGB 转换回 BGR
                    cv::Mat saveImg;
                    cv::cvtColor(mCVimage, saveImg, cv::COLOR_RGB2BGR);
                    // 使用imwrite()保存图片
                    cv::imwrite(path.toStdString(), saveImg);
                }
            }


}

void MainWindow::on_liangdu_clicked()
{
    // 检查mCVimage是否为空，如果为空，说明没有加载任何图片，就不必调整亮度
        if(mCVimage.empty())
        {
            QMessageBox::warning(this, tr("错误"), tr("没有图片可处理"));
            return;
        }

        // 检查亮度等级是否已达到最大值
        if(brightnessLevel >= 10)
        {
            QMessageBox::warning(this, tr("警告"), tr("已达到最大亮度等级"));
            return;
        }

        // 增加亮度等级
        brightnessLevel++;

        // 调整亮度
        cv::Mat brighterImage = mCVimage + cv::Scalar((brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50);  // 每个等级增加50的亮度

        // 在 QLabel 上显示处理后的图像
        QImage qimg = QImage((uchar*)brighterImage.data, brighterImage.cols, brighterImage.rows, QImage::Format_RGB888);
        ui->label->setPixmap(QPixmap::fromImage(qimg));

        // 更新mCVimage以便以后可以对新的图像进行操作
        mCVimage = brighterImage;
}
void MainWindow::on_andu_clicked()
{
     // 检查mCVimage是否为空，如果为空，说明没有加载任何图片，就不必调整亮度
            if(mCVimage.empty())
            {
                QMessageBox::warning(this, tr("错误"), tr("没有图片可处理"));
                return;
            }

            // 检查亮度等级是否已达到最小值
            if(brightnessLevel <= 0)
            {
                QMessageBox::warning(this, tr("警告"), tr("已达到最小亮度等级"));
                return;
            }

            // 减少亮度等级
            brightnessLevel--;

            // 调整亮度
            cv::Mat darkerImage = mCVimage + cv::Scalar((brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50);  // 每个等级减少50的亮度

            // 在 QLabel 上显示处理后的图像
            QImage qimg = QImage((uchar*)darkerImage.data, darkerImage.cols, darkerImage.rows, QImage::Format_RGB888);
            ui->label->setPixmap(QPixmap::fromImage(qimg));

            // 更新mCVimage以便以后可以对新的图像进行操作
            mCVimage = darkerImage;

}

void MainWindow::on_renlian_clicked()
{
    // Load face detector model
    if (!faceDetector.load("/home/luo/ku/renlian.xml"))
    {
        QMessageBox::critical(this, "Error", "Failed to load face detector model.");
        return;
    }

    if(mCVimage.empty())
    {
        QMessageBox::warning(this, tr("错误"), tr("没有图片可处理"));
        return;
    }

    // Convert image to grayscale
    cv::Mat gray;
    cv::cvtColor(mCVimage, gray, cv::COLOR_BGR2GRAY);

    // Detect faces
    std::vector<cv::Rect> faces;
    faceDetector.detectMultiScale(gray, faces);

    // Draw detected faces on frame
    for (const auto& face : faces)
    {
        cv::rectangle(mCVimage, face, cv::Scalar(255, 0, 0));
    }

    // Convert image to QImage and display it on QLabel
    QImage image(mCVimage.data, mCVimage.cols, mCVimage.rows, mCVimage.step, QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(image.rgbSwapped()));
}


/*void MainWindow::on_renlian_clicked()
{
    // Load face detector model
     if (!faceDetector.load("/home/luo/ku/renlian.xml"))
    {
        QMessageBox::critical(this, "Error", "Failed to load face detector model.");
        return;
    }

    // Open camera
    if (!camera.open(2))
    {
        QMessageBox::critical(this, "Error", "Failed to open camera.");
        return;
    }

    // Start timer
    timer->start(1000/30); // 30 frames per second
}

void MainWindow::updateFrame()
{
    cv::Mat frame;
    camera >> frame;
    if (!frame.empty())
    {
        // Convert frame to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detect faces
        std::vector<cv::Rect> faces;
        faceDetector.detectMultiScale(gray, faces);

        // Draw detected faces on frame
        for (const auto& face : faces)
        {
            cv::rectangle(frame, face, cv::Scalar(255, 0, 0));
        }

        // Convert frame to QImage and display it on QLabel
        QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        ui->label->setPixmap(QPixmap::fromImage(image.rgbSwapped()));
    }
}*/
void MainWindow::updateFrame() {
    // 函数体
}


/*void MainWindow::on_pushButton_clicked()
{
    QString videoFile = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.mp4 *.avi *.mov)"));
    if(videoFile.isEmpty()) {
        return;
    }

    bool ok;
    int start = QInputDialog::getInt(this, tr("Start Time"), tr("Enter start time (in seconds):"), 0, 0, 10000, 1, &ok);
    if (!ok) {
        return;
    }

    int end = QInputDialog::getInt(this, tr("End Time"), tr("Enter end time (in seconds):"), 0, 0, 10000, 1, &ok);
    if (!ok) {
        return;
    }

    int duration = end - start;
    QString outputFileName = "output.mp4"; // 设置输出文件名，你可能需要让用户选择这个文件名

    QProcess* process = new QProcess(this);
    QString program = "/usr/bin/ffmpeg";  // 注意这里使用了完整路径
    QStringList arguments;
    arguments << "-ss" << QString::number(start)
              << "-i" << videoFile
              << "-t" << QString::number(duration)
              << "-c:v" << "copy"
              << "-c:a" << "copy"
              << outputFileName;

    process->start(program, arguments);
}*/
void clipVideo(const std::string &inputFile, const std::string &outputFile, int startFrame, int endFrame) {
    cv::VideoCapture cap(inputFile);

    if (!cap.isOpened()) {
        std::cout << "Error opening video file" << std::endl;
        return;
    }

    int frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int fps = cap.get(cv::CAP_PROP_FPS);

    cv::VideoWriter writer;
    writer.open(outputFile, cv::VideoWriter::fourcc('M','J','P','G'), fps, cv::Size(frameWidth,frameHeight));

    if (!writer.isOpened()) {
        std::cout << "Could not open the output video file for write\n";
        return;
    }

    cv::Mat frame;
    int frameNumber = 0;
    while (true) {
        cap >> frame;

        if (frame.empty()) {
            break;
        }

        if (frameNumber >= startFrame && frameNumber <= endFrame) {
            writer.write(frame);
        }

        frameNumber++;
    }

    cap.release();
    writer.release();
}
void MainWindow::on_pushButton_clicked() {
    QString videoFile = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.mp4 *.avi *.mov)"));
    if(videoFile.isEmpty()) {
        return;
    }

    bool ok;
    int startSecond = QInputDialog::getInt(this, tr("Start Time"), tr("Enter start time (in seconds):"), 0, 0, 10000, 1, &ok);
    if (!ok) {
        return;
    }

    int endSecond = QInputDialog::getInt(this, tr("End Time"), tr("Enter end time (in seconds):"), 0, 0, 10000, 1, &ok);
    if (!ok) {
        return;
    }

    cv::VideoCapture cap(videoFile.toStdString());
    int fps = cap.get(cv::CAP_PROP_FPS);

    int startFrame = startSecond * fps;
    int endFrame = endSecond * fps;

    std::string outputFile = "output.avi"; // 你可能需要让用户选择输出文件名

    clipVideo(videoFile.toStdString(), outputFile, startFrame, endFrame);
}
