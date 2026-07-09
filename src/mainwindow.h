#pragma once

#include "testengine.h"

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void runSelectedTest();
    void runAllTests();
    void exportReport();

private:
    TestEngine m_engine;
    QVector<TestResult> m_results;
    QTableWidget *m_table = nullptr;
    QLabel *m_status = nullptr;
    QLabel *m_summary = nullptr;
    QProgressBar *m_progress = nullptr;

    void buildUi();
    void populateTests();
    void applyStyle();
    void upsertResult(const TestResult &result);
    void updateSummary();
};
