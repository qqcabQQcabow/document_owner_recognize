#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QLineEdit>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QString>
#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Recognizer.h"


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Recognizer recognizer_tool;
    QPushButton* check_signature;
    QPushButton* settings_button;
    QPushButton* manual_select_area;
    QPushButton* get_image;
    QPushButton* clear_area;
    QLineEdit* thresh_input;

    QLabel *image_viewer;

    QLabel *signature_crop_viewer;
    QLabel *result_text;

    cv::Mat source_image;
    //cv::Mat drawing_image;
    //cv::Mat image_to_viewer;

    QHBoxLayout* buttons_layouts;
    QVBoxLayout* main_layout;
    QHBoxLayout* result_layout;
    QWidget *result_container;
    void showMainImage(const cv::Mat& image_);
    void setCropViewer(const cv::Mat& image_);

    int one_five_height;
    int one_half_width;

private slots:
    void settingsHandler();
    void chooseDocument();
    void processDocument();
    void manualSelectHandler();
    void clearHandler();
};
namespace myFunc{
    size_t getIndxMaxElement(const std::vector<float>& source);
    bool inRoi(const std::vector<cv::Point>& points, const cv::Point& roi_point);
    void normixRectanglePoints(std::vector<cv::Point>& points);
    cv::Mat getCropRect(const cv::Mat& source, const std::vector<cv::Point>& points);
    std::vector<cv::Point> getSignatureCoords(const cv::Mat& canny_output, const cv::Point& roi);
}
#endif // MAINWINDOW_H
