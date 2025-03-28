#include "mainwindow.h"
#include <iostream> 
#define IMAGE_WIDTH_PX 800
#define IMAGE_HEIGHT_PX 600
#define CORNERS_COUNT 4
#define DEFAULT_THRESH 100
#define INPUT_THRESH_WIDTH 50
#define PAPER_PARTS_HEIGHT 5
#define PAPER_PARTS_WIDTH 5

const std::string model_path = "sem_model_tf";
const std::string input_layer = "serving_default_xception_input:0";
const std::string output_layer = "StatefulPartitionedCall:0";
const int img_h = 128;
const int img_w = 128;
const int color_chanels = 3;
const std::vector<int64_t> inp_tensor_shape = {1, img_h, img_w, color_chanels};
const std::vector<std::string> labels_names = {"Объект 1", "Объект 2", "Объект 3", "Объект 4", "Неопознанный объект", "Объект 6"};

const double resize_scale = 0.65;
const int gap_crop_px = 3;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent, Qt::Window),
    one_five_height(INTMAX_MAX),
    one_half_width(INTMAX_MAX),

    image_viewer(new QLabel(this)),
    signature_crop_viewer(new QLabel(this)),
    result_text(new QLabel(this)),

    result_container(new QWidget(this)),
    //thresh_input(new QLineEdit(this)),

    buttons_layouts(new QHBoxLayout()),
    result_layout(new QHBoxLayout()),
    main_layout(new QVBoxLayout()),


    check_signature(new QPushButton(this)),
    //settings_button(new QPushButton(this)),
    //manual_select_area(new QPushButton(this)),
    clear_area(new QPushButton(this)),
    get_image(new QPushButton(this)),
    recognizer_tool
        (
            model_path,
            input_layer,
            output_layer,
            img_h,
            img_w,
            inp_tensor_shape,
            labels_names
        )
{

    image_viewer->setAlignment(Qt::AlignHCenter);
    image_viewer->setScaledContents(false);

    image_viewer->clear();

    cv::Mat image_ = cv::imread("empty_doc.png");

    cv::Mat image_to_viewer;
    cv::resize(image_, image_to_viewer, cv::Size(0, 0), resize_scale, resize_scale);

    image_viewer->setPixmap(QPixmap::fromImage(QImage(
                    image_to_viewer.data,
                    image_to_viewer.cols,
                    image_to_viewer.rows,
                    static_cast<int>(image_to_viewer.step),
                    QImage::Format_RGB888
                    )));

    //thresh_input->setFixedWidth(INPUT_THRESH_WIDTH);
    //thresh_input->setText("100");


    check_signature->setText("Проверить");

    //settings_button->setText("Настройки");

    //manual_select_area->setText("Ручной режим");

    get_image->setText("Выбрать документ");

    clear_area->setText("Очистить");

    //buttons_layouts[0] = new QHBoxLayout(this);
    //buttons_layouts->addWidget(thresh_input);
    //buttons_layouts->addWidget(settings_button);
    //buttons_layouts->addWidget(manual_select_area);
    buttons_layouts->addWidget(clear_area);
    buttons_layouts->addWidget(get_image);


    result_text->setStyleSheet("QLabel { color : rgb(215,215,215); }");

    result_layout->addWidget(signature_crop_viewer);
    result_layout->addWidget(result_text);

    result_container->setLayout(result_layout);
    result_container->setVisible(false);

    main_layout->addLayout(buttons_layouts);
    main_layout->addWidget(image_viewer);
    main_layout->addWidget(check_signature);
    main_layout->addWidget(result_container);

    this->setLayout(main_layout);
    this->adjustSize();

    // Add buttons
    connect(check_signature, &QPushButton::released, this, &MainWindow::processDocument);
    //connect(settings_button, &QPushButton::released, this, &MainWindow::settingsHandler);
    //connect(manual_select_area, &QPushButton::released, this, &MainWindow::manualSelectHandler);
    connect(clear_area, &QPushButton::released, this, &MainWindow::clearHandler);
    connect(get_image, &QPushButton::released, this, &MainWindow::chooseDocument);

}

MainWindow::~MainWindow() = default;

namespace myFunc{

    size_t getIndxMaxElement(const std::vector<float>& source)
    {
        auto cmp = std::max_element(source.begin(), source.end());
        std::cout << "CMP IS : " << *cmp << std::endl;
        size_t indx = 0;
        for(auto it = source.begin(); it != source.end(); ++it){
            std::cout << "source[" << indx << "] = " << *it << std::endl;
            if(it == cmp){
                return indx;
            }
            indx++;
        }
        throw std::runtime_error("FUCK: source size is: " + std::to_string(source.size()));

    }
    bool inRoi(const std::vector<cv::Point>& points, const cv::Point& roi_point)
    {
        for(const auto& point : points){
            if(point.x < roi_point.x){
                return false;
            }
            if(point.y < roi_point.y){
                return false;
            }
        }
        return true;

    }
    void normixRectanglePoints(std::vector<cv::Point>& points) 
    {
        if (points.size() != 4) {
            throw std::runtime_error("Input must contain exactly 4 points!");
        }

        // Сортируем по сумме (x + y), чтобы найти левый верхний и правый нижний углы
        std::sort(points.begin(), points.end(), [](const cv::Point& first_, const cv::Point& second_) {
            return (first_.x + first_.y) < (second_.x + second_.y);
        });

        cv::Point leftTop = points[0];  // Минимальная сумма x + y (верхний левый угол)
        cv::Point rightBottom = points[3];  // Максимальная сумма x + y (нижний правый угол)

        // Оставшиеся две точки сортируем по разности (x - y), чтобы найти верхний правый и нижний левый углы
        if ((points[1].x - points[1].y) > (points[2].x - points[2].y)) {
            std::swap(points[1], points[2]);
        }

        cv::Point rightTop = points[1];
        cv::Point leftBottom = points[2];


        rightTop.x = leftTop.x = std::max(leftTop.x, rightTop.x);
        rightBottom.y = rightTop.y = std::min(rightTop.y, rightBottom.y);
        leftBottom.y = leftTop.y = std::max(leftTop.y, leftBottom.y);
        leftBottom.x = rightBottom.x = std::min(rightBottom.x, leftBottom.x);

        points[0] = leftTop;
        points[1] = rightTop;
        points[2] = rightBottom;
        points[3] = leftBottom;

        points[0].y += gap_crop_px;
        points[0].x += gap_crop_px;

        points[1].y -= gap_crop_px;
        points[1].x += gap_crop_px;

        points[2].y -= gap_crop_px;
        points[2].x -= gap_crop_px;

        points[3].y += gap_crop_px;
        points[3].x -= gap_crop_px;

    }
    cv::Mat getCropRect(const cv::Mat& source, const std::vector<cv::Point>& points)
    {
        if (points.size() != 4) {
            throw std::runtime_error("Input must contain exactly 4 points!");
        }
            
        // Создаем маску, такую же по размеру, как и изображение, и заполняем её черным цветом
        cv::Mat mask = cv::Mat::zeros(source.size(), CV_8UC1);

        // Заполняем область внутри четырехугольника белым цветом на маске
        const cv::Scalar white_color = cv::Scalar(255);
        cv::fillConvexPoly(mask, points, white_color);

        cv::Mat result;
        source.copyTo(result, mask);

        cv::Rect boundingBox = cv::boundingRect(points);

        return result(boundingBox).clone();
    }

    std::vector<cv::Point> getSignatureCoords(const cv::Mat& canny_output, const cv::Point& roi)
    {
        const double approximate_k = 0.01;

        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
                                                            // search parametrs
        cv::findContours(canny_output, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for(const auto& contour : contours){

            std::vector<cv::Point> approxRes;
            cv::approxPolyDP(contour, approxRes, approximate_k*arcLength(contour, true), true);

            if((approxRes.size() == 4) && (cv::isContourConvex(approxRes) && inRoi(approxRes, roi))){
                return approxRes;
            }
        }
        return {};
    }
}
void MainWindow::processDocument()
{
    clearHandler();

    if(source_image.empty()){
        QMessageBox::warning(nullptr, "Ошибка", "Обнаружено пустое изображение");
        return;
    }

    int thresh = DEFAULT_THRESH;
    //try{
    //    thresh = thresh_input->text().toInt();
    //}
    //catch(...){
    //    thresh = DEFAULT_THRESH;
    //    std::cout << "Cant use thresh from input" << "\n";
    //}

    cv::Mat src_gray;
    cv::cvtColor(source_image, src_gray, cv::COLOR_BGR2GRAY );
    cv::blur(src_gray, src_gray, cv::Size(3,3) );

    cv::Mat canny_output;
    cv::Canny(src_gray, canny_output, thresh, thresh*2);

    std::cout << "Call getSignatureCoords with thresh " << thresh << "\n";
    cv::Point roi_p = cv::Point(one_half_width, one_five_height*4);
    std::cout << "Roi " << one_half_width << "x" << one_five_height*4 << "\n";

    auto res = myFunc::getSignatureCoords(canny_output, roi_p);

    if(res.size() != 4){
        result_text->setText("Не найдена область для распознавания");
        result_container->setVisible(true);
        return;
    }

    myFunc::normixRectanglePoints(res);

    cv::Mat crop = myFunc::getCropRect(source_image, res);


    auto predictions = recognizer_tool.getPredictList(crop);
    QString recognize_rezult = QString::fromStdString(labels_names.at(myFunc::getIndxMaxElement(predictions)));

    result_text->setText("Документ утвердил: " + recognize_rezult);
    setCropViewer(crop);
    result_container->setVisible(true);

    std::cout << "Find tops of rectangle:" << "\n";

    for(const auto& item : res){
        std::cout << "      " << item.x << "x" << item.y << "\n";
    }

    cv::Mat drawing_image = source_image.clone();

    for(const auto& item : res){
        const cv::Scalar green_color = cv::Scalar(0, 255, 0);
        cv::circle(drawing_image, item, 3, green_color, -1, cv::LINE_8, 0);
    }

    showMainImage(drawing_image);
}
void MainWindow::settingsHandler()
{
}
void MainWindow::manualSelectHandler()
{
}
void MainWindow::clearHandler()
{
    result_text->clear();
    signature_crop_viewer->clear();
    result_container->setVisible(false);
    if(!source_image.empty()){
        showMainImage(source_image);
    } 
    this->adjustSize();
    this->update();
}
void MainWindow::showMainImage(const cv::Mat& image_)
{
    image_viewer->clear();

    cv::Mat image_to_viewer;
    cv::resize(image_, image_to_viewer, cv::Size(0, 0), resize_scale, resize_scale);

    image_viewer->setPixmap(QPixmap::fromImage(QImage(
                    image_to_viewer.data,
                    image_to_viewer.cols,
                    image_to_viewer.rows,
                    static_cast<int>(image_to_viewer.step),
                    QImage::Format_RGB888
                    )));
    this->adjustSize();
    this->update();
}

void MainWindow::setCropViewer(const cv::Mat& image_)
{

    signature_crop_viewer->clear();
    signature_crop_viewer->setPixmap(QPixmap::fromImage(QImage(
                    image_.data,
                    image_.cols,
                    image_.rows,
                    static_cast<int>(image_.step),
                    QImage::Format_RGB888
                    )));
}

void MainWindow::chooseDocument()
{
    QString doc_path = QFileDialog::getOpenFileName(this, "Выберите файл с документом для идентификации", QDir::homePath(), "Файл с изображением (*.png *.jpg)");
    if(doc_path.isEmpty()){
        return;
    }

    source_image = cv::imread(doc_path.toStdString());
    if(source_image.empty()){
        QMessageBox::warning(nullptr, "Ошибка", "Обнаружено пустое изображение");
        return;
    } 

    clearHandler();

    one_five_height = source_image.rows / PAPER_PARTS_HEIGHT;
    one_half_width = source_image.cols / PAPER_PARTS_WIDTH;

    showMainImage(source_image);
}
