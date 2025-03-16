//
// Created by nicholas on 15.03.25.
//
#include <MicrofluidicNucleationGui/MainWindow.h>
#include <QHeaderView>
#include <QStringList>
#include <QFileDialog>
#include <QCheckBox>
#include <QFuture>
#include <QtConcurrent/qtconcurrentrun.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <MicroCalibrator/MicroCalibrator.h>

mfngui::MainWindow::MainWindow(QWidget *parent)
{
    resize(800, 600);
    setWindowTitle("Microfluidic Nucleation Software");

    open_videos = new QPushButton("Open Videos");

    table_widget = new QTableWidget();
    table_widget->setColumnCount(5);
    table_widget->setHorizontalHeaderLabels({
        "Video",
        "Temperature",
        "Fixed Temperature",
        "Frame rate",
        "Heater temperature"
    });
    table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    process_log = new QTextEdit();
    process_log->setReadOnly(true);
    if (spdlog::get("mfn_logger") == nullptr)
    {
        auto qt_logger_sink = std::make_shared<spdlog::sinks::qt_color_sink_mt>(process_log, 500);
        auto logger_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/qtlog.txt", true);
        const auto logger = std::make_shared<spdlog::logger>("mfn_logger", spdlog::sinks_init_list({logger_file_sink, qt_logger_sink}));
        spdlog::register_logger(logger);
    }
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter started");

    start_analysis = new QPushButton("Start Analysis");
    start_analysis->setEnabled(false);

    progress_bar = new QProgressBar();
    progress_bar->setRange(0, 1);

    calibrate = new QPushButton("Calibrate");
    calibrate->setEnabled(false);

    calib_constant = new QLabel();
    calib_constant->setText(QString::number(calib_value));


    layout = new QGridLayout();
    layout->addWidget(open_videos, 0, 0, 1, 1);
    layout->addWidget(calibrate, 0, 1, 1, 1);
    layout->addWidget(calib_constant, 0, 2, 1, 1);
    layout->addWidget(table_widget, 1, 0, 1, 3);
    layout->addWidget(process_log, 3, 0, 3, 3);
    layout->addWidget(start_analysis, 2, 0, 1, 1);
    layout->addWidget(progress_bar, 2, 1, 1, 1);
    setLayout(layout);

    analysis_watcher = new QFutureWatcher<void>();
    calibration_watcher = new QFutureWatcher<double>();

    connect(open_videos, &QPushButton::clicked, this, &MainWindow::openVideosSlot);
    connect(start_analysis, &QPushButton::clicked, this, &MainWindow::startAnalysisSlot);
    connect(process_log, &QTextEdit::textChanged, this, &MainWindow::scrollLogSlot);
    connect(analysis_watcher, &QFutureWatcher<void>::finished, this, &MainWindow::startNextComputationSlot);
    connect(calibrate, &QPushButton::clicked, this, &MainWindow::calibrateSlot);
    connect(calibration_watcher, &QFutureWatcher<double>::finished, this, &MainWindow::isCalibratedSlot);
    analysis_config.net_file = "/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/resources/fixed_100_model.onnx";
    analysis_config.frame_stop = 100;
}

void mfngui::MainWindow::openVideosSlot() const
{
    const QStringList filters = {
        "Video Files (*.mp4 *.avi *.mov *.mkv *.wmv *.flv *.webm)"
    };

    QStringList files = QFileDialog::getOpenFileNames(
        nullptr,
        "Open Videos",
        QString(),
        filters.join(";;")
        );

    if (!files.isEmpty())
    {
        for (const QString &filename : files)
        {
            add_video(filename.toStdString());
        }
        calibrate->setEnabled(true);
    }
}

void mfngui::MainWindow::startAnalysisSlot()
{
    start_analysis->setEnabled(false);
    if (table_widget->rowCount() <= 0)
    {
        spdlog::get("mfn_logger")->error("No videos selected");
        start_analysis->setEnabled(true);
        return;
    }
    progress_bar->setRange(0, table_widget->rowCount());
    for (int row = 0; row < table_widget->rowCount(); ++row)
    {
        std::filesystem::path video_file = (table_widget->item(row, 0))->text().toStdString();
        const double frame_rate = std::stod((table_widget->item(row, 3))->text().toStdString());
        const double calib = 1e-6;
        const double heater_temperature = std::stod((table_widget->item(row, 4))->text().toStdString());
        mfn::Experiment experiment(video_file, frame_rate, calib, heater_temperature);
        mfn::TemperatureReader temperature_reader;

        const QWidget *widget = table_widget->cellWidget(row, 2);
        const QCheckBox *checkBox = widget->findChild<QCheckBox *>();
        if (checkBox->checkState() == Qt::Unchecked)
        {
            const std::filesystem::path tc_file = (table_widget->item(row, 1))->text().toStdString();
            temperature_reader = mfn::TemperatureReader(tc_file);
        }
        else
        {
            double temperature = 0;
            try
            {
                temperature = std::stod((table_widget->item(row, 1))->text().toStdString());
            }
            catch (...)
            {
                temperature = 0;
                spdlog::get("mfn_logger")->error("Error getting temperature. Using 0°C as default value");
            }
            temperature_reader = mfn::TemperatureReader(temperature);
        }
        analyzer_argument_t video_tuple = std::make_tuple(experiment, temperature_reader);
        video_queue.push(video_tuple);
    }
    startNextComputationSlot();

}

void mfngui::MainWindow::startNextComputationSlot()
{
    if (!video_queue.empty())
    {
        analyzer_argument_t video = video_queue.front();
        progress_bar->setValue(table_widget->rowCount() - video_queue.size());
        video_queue.pop();
        mfn::VideoAnalyzer analyzer(std::get<0>(video), analysis_config, std::get<1>(video));
        QFuture<void> future = QtConcurrent::run(analyzer, &mfn::VideoAnalyzer::analyze);
        analysis_watcher->setFuture(future);
    }
    else
        start_analysis->setEnabled(true);
}

void mfngui::MainWindow::scrollLogSlot() const
{
    QTextCursor cursor = process_log->textCursor();
    cursor.movePosition(QTextCursor::End);
    process_log->setTextCursor(cursor);
}

void mfngui::MainWindow::add_video(std::filesystem::path path) const
{
    const int row = table_widget->rowCount();
    table_widget->insertRow(row);

    table_widget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(path.string())));

    QString temperature_string;
    const std::filesystem::path tc_path = path.replace_extension(".tc");
    if (std::filesystem::exists(tc_path))
        temperature_string = QString::fromStdString(tc_path.string());
    else
        temperature_string = QString::number(0);
    table_widget->setItem(row, 1, new QTableWidgetItem(temperature_string));

    auto checkboxWidget = new QWidget(table_widget);
    auto checkBox = new QCheckBox(checkboxWidget);
    if (temperature_string == "0")
    {
        checkBox->setChecked(true);
        checkBox->setEnabled(false);
    }
    else
        checkBox->setChecked(false);

    auto *layout = new QHBoxLayout(checkboxWidget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    checkboxWidget->setLayout(layout);
    table_widget->setCellWidget(row, 2, checkboxWidget);

    table_widget->setItem(row, 3, new QTableWidgetItem(QString::number(30)));
    table_widget->setItem(row, 4, new QTableWidgetItem(QString::number(50)));

}

void mfngui::MainWindow::calibrateSlot()
{
    auto calibrator = new MicroCalibrator("/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000.mp4");
    QFuture<double> future = QtConcurrent::run(calibrator, &mfngui::MicroCalibrator::getCalibrationConstant);
    calibration_watcher->setFuture(future);
}

void mfngui::MainWindow::isCalibratedSlot()
{
    calib_value = calibration_watcher->result();
    start_analysis->setEnabled(true);
    calib_constant->setText(QString::number(calib_value));
}