//
// Created by nicholas on 15.03.25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGridLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QFutureWatcher>
#include <QLabel>
#include <QProgressBar>
#include "MicrofluidicNucleation/VideoAnalyzer.h"
#include <MicrofluidicNucleation/Experiment.h>
#include <MicrofluidicNucleation/TemperatureReader.h>
#include <queue>
#include <tuple>
#include <filesystem>


namespace mfngui
{
    class MainWindow : public QWidget
    {
        Q_OBJECT
        public:
            explicit MainWindow(QWidget *parent = nullptr);

        private:
            typedef std::tuple<mfn::Experiment, mfn::TemperatureReader> analyzer_argument_t;
            QGridLayout *layout;
            QPushButton *open_videos;
            QPushButton *calibrate;
            QPushButton *start_analysis;
            QTableWidget *table_widget;
            QTextEdit *process_log;
            QLabel *calib_constant;
            QProgressBar *progress_bar;

            double calib_value = 0;

            void add_video(std::filesystem::path path) const;
            mfn::AnalysisConfig analysis_config;
            std::queue<analyzer_argument_t> video_queue;
            QFutureWatcher<void> * analysis_watcher;
            QFutureWatcher<double> * calibration_watcher;

        private slots:
            void openVideosSlot() const;
            void startAnalysisSlot();
            void scrollLogSlot() const;
            void startNextComputationSlot();
            void calibrateSlot();
            void isCalibratedSlot();
    };
}

#endif //MAINWINDOW_H
