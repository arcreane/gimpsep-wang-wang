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
     brightnessLevel = 5;  // Initial brightness level is 5
     //Initialize timer and connect to updateFrame slot function
       timer = new QTimer(this);
       connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_showImage_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("show the photo"), ".", tr("Image Files(*.jpg *.png)"));

    // load image
    mCVimage = cv::imread(path.toStdString());

    // bgr to rgb
    cv::cvtColor( mCVimage, mCVimage, cv::COLOR_BGR2RGB);

    //Convert OpenCV image to Qt image
    QImage qimg = QImage((uchar*)mCVimage.data, mCVimage.cols, mCVimage.rows, QImage::Format_RGB888);

    // Set QLabel to automatically scale content
    ui->label->setScaledContents(true);

    // Set the Pixmap of QLabel
    ui->label->setPixmap(QPixmap::fromImage(qimg));
}



void MainWindow::on_dilatation_clicked()
{
    int dilatationSize = 5;

        // Create a kernel for dilatation
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilatationSize + 1, 2 * dilatationSize + 1),
                                                    cv::Point(dilatationSize, dilatationSize));

        // Perform dilatation operation
        cv::Mat dilatedImage;
        cv::dilate(mCVimage, dilatedImage, element);

       // Convert the processed image to QImage
        QImage qimg = QImage((uchar*)dilatedImage.data, dilatedImage.cols, dilatedImage.rows, QImage::Format_RGB888);

        // show it in qlabel
        ui->label->setPixmap(QPixmap::fromImage(qimg));
}

void MainWindow::on_mergeimages_clicked()
{
    // load the second image
        QString path2 = QFileDialog::getOpenFileName(this, tr("2nd photo"), ".", tr("Image Files(*.jpg *.png)"));
        cv::Mat img2 = cv::imread(path2.toStdString());

        // Convert color space from BGR to RGB
        cv::cvtColor(img2, img2, cv::COLOR_BGR2RGB);

        // If the height of the first image and the second image are different, adjust the height of the second image to be the same as the first image
        if (img2.rows != mCVimage.rows) {
            double scale = (double)mCVimage.rows / img2.rows;
            cv::resize(img2, img2, cv::Size(), scale, scale);
        }

        // Stitch two images together
        cv::Mat result;
        cv::hconcat(mCVimage, img2, result);

        // Convert OpenCV image to Qt image
        QImage qimg = QImage((uchar*)result.data, result.cols, result.rows, QImage::Format_RGB888);

        // Display the processed image on the QLabel
        ui->label->setPixmap(QPixmap::fromImage(qimg));

        // convert color space from RGB back to BGR before saving
        cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

        //save
            //cv::imwrite("/home/luo/pictures/result.jpg", result);
}


void MainWindow::on_print_clicked()
{

        if(mCVimage.empty())
        {
            QMessageBox::warning(this, tr("error"), tr("no photo"));
            return;
        }

        QString path = QFileDialog::getSaveFileName(this, tr("save"), ".", tr("Image Files(*.jpg)"));
        if (path.isEmpty())
        {
            return;
        }
        else
        {
            // rgb to bgr
            cv::Mat saveImg;
            cv::cvtColor(mCVimage, saveImg, cv::COLOR_RGB2BGR);
            // use imwrite() for save
            cv::imwrite(path.toStdString(), saveImg);
        }
}

void MainWindow::on_Stitching_clicked()
{

        if(mCVimage.empty())
        {
            QMessageBox::warning(this, tr("error"), tr("no photo"));
            return;
        }

        bool ok;
            int newWidth = QInputDialog::getInt(this, tr("new w"), tr("w:"), 1, 1, 10000, 1, &ok);
            int newHeight = QInputDialog::getInt(this, tr("new h"), tr("h:"), 1, 1, 10000, 1, &ok);

            if(ok)
            {
                cv::resize(mCVimage, mCVimage, cv::Size(newWidth, newHeight));

              // convert the processed image to QImage
                QImage qimg = QImage((uchar*)mCVimage.data, mCVimage.cols, mCVimage.rows, QImage::Format_RGB888);

                // Display the processed image on the QLabel
                ui->label->setPixmap(QPixmap::fromImage(qimg));

                QString path = QFileDialog::getSaveFileName(this, tr("save"), ".", tr("Image Files(*.jpg)"));
                if (path.isEmpty())
                {
                    return;
                }
                else
                {
                    //RGB to BGR
                    cv::Mat saveImg;
                    cv::cvtColor(mCVimage, saveImg, cv::COLOR_RGB2BGR);

                    cv::imwrite(path.toStdString(), saveImg);
                }
            }


}

void MainWindow::on_Brighten_clicked()
{

        if(mCVimage.empty())
        {
            QMessageBox::warning(this, tr("error"), tr("no photo"));
            return;
        }

        // Check if the brightness level has reached the maximum value
        if(brightnessLevel >= 10)
        {
            QMessageBox::warning(this, tr("attention"), tr("is max"));
            return;
        }

        // increase brightness level
        brightnessLevel++;

        // adjust brightness
        cv::Mat brighterImage = mCVimage + cv::Scalar((brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50);

        // show it in QLabel
        QImage qimg = QImage((uchar*)brighterImage.data, brighterImage.cols, brighterImage.rows, QImage::Format_RGB888);
        ui->label->setPixmap(QPixmap::fromImage(qimg));

        // Update mCVimage so that it can operate on new images later
        mCVimage = brighterImage;
}
void MainWindow::on_darken_clicked()
{

            if(mCVimage.empty())
            {
                QMessageBox::warning(this, tr("error"), tr("no photo"));
                return;
            }

            // check min
            if(brightnessLevel <= 0)
            {
                QMessageBox::warning(this, tr("attention"), tr("is min now"));
                return;
            }

            // decrease brightness level
            brightnessLevel--;

            // adjust brightness
            cv::Mat darkerImage = mCVimage + cv::Scalar((brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50, (brightnessLevel - 5) * 50);  

            // Display the processed image on the QLabel
            QImage qimg = QImage((uchar*)darkerImage.data, darkerImage.cols, darkerImage.rows, QImage::Format_RGB888);
            ui->label->setPixmap(QPixmap::fromImage(qimg));

            // Update mCVimage so that new images can be operated on later
            mCVimage = darkerImage;

}

void MainWindow::on_Detection_clicked()
{
    // Load face detector model
    if (!faceDetector.load("/home/luo/ku/mod.xml"))
    {
        QMessageBox::critical(this, "Error", "Failed to load face detector model.");
        return;
    }

    if(mCVimage.empty())
    {
        QMessageBox::warning(this, tr("error"), tr("no photo"));
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


/*void MainWindow::on_Detection_clicked()
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
    QString outputFileName = "output.mp4";

    QProcess* process = new QProcess(this);
    QString program = "/usr/bin/ffmpeg";
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

    std::string outputFile = "output.avi";

    clipVideo(videoFile.toStdString(), outputFile, startFrame, endFrame);
}
