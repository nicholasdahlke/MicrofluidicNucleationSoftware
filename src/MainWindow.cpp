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
#include <MicrofluidicNucleation/NucleationCalculator.h>
#include <MicrofluidicNucleation/ResultsWriter.h>
#include <MicrofluidicNucleation/CsvWriter.h>
#include <chrono>

mfngui::MainWindow::MainWindow(QWidget *parent)
{
    resize(800, 600);
    setWindowTitle("Microfluidic Nucleation Software");

    open_videos = new QPushButton("Open Videos");
    open_videos->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
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
    table_widget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    table_widget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);


    process_log = new QTextEdit();
    process_log->setReadOnly(true);
    std::filesystem::path log_path = std::string(std::getenv("HOME")) + "/MpembaLogs";
    std::filesystem::create_directories(log_path);
    if (spdlog::get("mfn_logger") == nullptr)
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

        auto qt_logger_sink = std::make_shared<spdlog::sinks::qt_color_sink_mt>(process_log, 1e5);
        std::stringstream filename;
        filename << log_path.string()
                 << "/log_"
                 << std::put_time(std::localtime(&t_c), "%Y-%m-%d_%H-%M-%S")
                 << ".txt";
        auto logger_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename.str(), true);
        const auto logger = std::make_shared<spdlog::logger>("mfn_logger", spdlog::sinks_init_list({logger_file_sink, qt_logger_sink}));
        spdlog::register_logger(logger);
    }
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter started");

    start_analysis = new QPushButton("Start Pre Analysis");
    start_analysis->setEnabled(false);

    progress_bar = new QProgressBar();
    progress_bar->setRange(0, 1);

    calibrate = new QPushButton("Calibrate");
    calibrate->setEnabled(false);

    calib_constant = new QLabel();
    calib_constant->setText(QString::number(calib_value));

    load_config = new QPushButton("Load Config");

    save_results = new QPushButton("Save Results");
    save_results->setEnabled(false);

    layout = new QGridLayout();
    layout->addWidget(open_videos, 0, 0, 2, 1);
    layout->addWidget(calibrate, 0, 1, 1, 1);
    layout->addWidget(calib_constant, 0, 2, 1, 1);
    layout->addWidget(load_config, 1, 1,1, 1 );
    layout->addWidget(table_widget, 2, 0, 1, 3);
    layout->addWidget(process_log, 4, 0, 3, 3);
    layout->addWidget(start_analysis, 3, 0, 1, 1);
    layout->addWidget(save_results, 3, 1, 1, 1);
    layout->addWidget(progress_bar, 3, 2, 1, 1);
    setLayout(layout);

    analysis_watcher = new QFutureWatcher<void>();
    calibration_watcher = new QFutureWatcher<double>();

    connect(open_videos, &QPushButton::clicked, this, &MainWindow::openVideosSlot);
    connect(start_analysis, &QPushButton::clicked, this, &MainWindow::startAnalysisSlot);
    connect(process_log, &QTextEdit::textChanged, this, &MainWindow::scrollLogSlot);
    connect(analysis_watcher, &QFutureWatcher<void>::finished, this, &MainWindow::startNextComputationSlot);
    connect(calibrate, &QPushButton::clicked, this, &MainWindow::calibrateSlot);
    connect(calibration_watcher, &QFutureWatcher<double>::finished, this, &MainWindow::isCalibratedSlot);
    connect(load_config, &QPushButton::clicked, this, &MainWindow::loadConfigSlot);
    connect(save_results, &QPushButton::clicked, this, &MainWindow::saveResultsSlot);
}

void mfngui::MainWindow::openVideosSlot()
{
    const QStringList filters = {
        "Video Files (*.mp4 *.avi *.mov *.mkv *.wmv *.flv *.webm)"
    };

    QStringList files = QFileDialog::getOpenFileNames(
        this,
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
    progress_bar->setRange(0, table_widget->rowCount()-1);
    for (int row = 0; row < table_widget->rowCount(); ++row)
    {
        std::filesystem::path video_file = table_widget->item(row, 0)->text().toStdString();
        const double frame_rate = std::stod((table_widget->item(row, 3))->text().toStdString());
        const double heater_temperature = std::stod((table_widget->item(row, 4))->text().toStdString());
        mfn::Experiment experiment(video_file, frame_rate, calib_value, heater_temperature);
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
        video_queue.pop();
        progress_bar->setValue(table_widget->rowCount() - video_queue.size()); // TODO: Fix progress bar
        auto analyzer = std::make_shared<mfn::VideoAnalyzer>(std::get<0>(video), analysis_config, std::get<1>(video));
        QFuture<void> future = QtConcurrent::run(analyzer.get(), &mfn::VideoAnalyzer::analyze);  // Unsafe ?
        analysis_watcher->setFuture(future);
        video_results.push_back(analyzer);
    }
    else
    {
        start_analysis->setEnabled(true);
        save_results->setEnabled(true);
    }
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
    MicroCalibrator calibrator(table_widget->item(0, 0)->text().toStdString()); // TODO: Fix memory leak
    QFuture<double> future = QtConcurrent::run(calibrator, &mfngui::MicroCalibrator::getCalibrationConstant);
    calibration_watcher->setFuture(future);
}

void mfngui::MainWindow::isCalibratedSlot()
{
    calib_value = calibration_watcher->result();
    start_analysis->setEnabled(true);
    calib_constant->setText(QString::number(calib_value));
    spdlog::get("mfn_logger")->info("Calibration constant set to {} m/px", calib_value);
}

void mfngui::MainWindow::loadConfigSlot()
{
    QString file_name = QFileDialog::getOpenFileName(
        this,
        "Open Config File",
        QString(),
        "Config Files (*.cf)"
        );
    if (!file_name.isEmpty())
        analysis_config = mfn::AnalysisConfig(file_name.toStdString());
    else
        spdlog::get("mfn_logger")->error("Error opening config file");
}

void mfngui::MainWindow::saveResultsSlot()
{
    for (const std::shared_ptr<mfn::VideoAnalyzer>& video : video_results)
    {
        auto writer = mfn::ResultsWriter(*video, video->getExperiment());
        std::filesystem::path video_file = video->getExperiment().getVideo();

        std::filesystem::path volume_file = video_file.parent_path().string() + "/" + video_file.stem().string() + "-volumes.csv";
        std::filesystem::path speed_file = video_file.parent_path().string() + "/" + video_file.stem().string() + "-speeds.csv";

        std::vector<std::vector<double>> speed_vectors;
        std::vector<std::vector<double>> volume_vectors;

        std::vector<double> speed_result = writer.getSpeeds();
        std::vector<double> volume_result = writer.getVolumes();
        for (const double& speed : speed_result)
            speed_vectors.push_back({speed});

        for (const double& volume : volume_result)
            volume_vectors.push_back({volume});

        mfn::CsvWriter::writeToCsvFile(speed_vectors, speed_file);
        mfn::CsvWriter::writeToCsvFile(volume_vectors, volume_file);

        spdlog::get("mfn_logger")->info("Analysis results saved for {}", video_file.stem().string());
    }
}