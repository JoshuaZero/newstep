#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    buildUi();
    applyStyle();
    populateTests();
}

void MainWindow::buildUi() {
    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);

    auto *title = new QLabel(QStringLiteral("NewStep 测控软件框架"), this);
    title->setObjectName("title");
    auto *subtitle = new QLabel(QStringLiteral("整机自检读取 · 测试阈值比对 · 多模可靠性故障注入 · 报告导出"), this);
    subtitle->setObjectName("subtitle");

    m_table = new QTableWidget(this);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels({QStringLiteral("类别"), QStringLiteral("测试项"), QStringLiteral("测量值"), QStringLiteral("单位"), QStringLiteral("合格范围"), QStringLiteral("自检比对"), QStringLiteral("结果"), QStringLiteral("说明")});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto *buttons = new QHBoxLayout();
    auto *runOne = new QPushButton(QStringLiteral("运行选中测试"), this);
    auto *runAll = new QPushButton(QStringLiteral("一键全项测试"), this);
    auto *exportBtn = new QPushButton(QStringLiteral("导出CSV报告"), this);
    buttons->addWidget(runOne);
    buttons->addWidget(runAll);
    buttons->addWidget(exportBtn);
    buttons->addStretch();

    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 100);
    m_status = new QLabel(QStringLiteral("就绪：等待测试指令"), this);
    m_summary = new QLabel(QStringLiteral("通过 0 / 0"), this);
    m_summary->setObjectName("summary");

    root->addWidget(title);
    root->addWidget(subtitle);
    root->addWidget(m_table, 1);
    root->addLayout(buttons);
    root->addWidget(m_progress);
    root->addWidget(m_summary);
    root->addWidget(m_status);
    setCentralWidget(central);
    resize(1280, 760);

    connect(runOne, &QPushButton::clicked, this, &MainWindow::runSelectedTest);
    connect(runAll, &QPushButton::clicked, this, &MainWindow::runAllTests);
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::exportReport);
}

void MainWindow::populateTests() {
    const auto &specs = m_engine.testSpecs();
    m_table->setRowCount(specs.size());
    for (int row = 0; row < specs.size(); ++row) {
        const auto &s = specs[row];
        m_table->setItem(row, 0, new QTableWidgetItem(s.category));
        m_table->setItem(row, 1, new QTableWidgetItem(s.name));
        m_table->setItem(row, 2, new QTableWidgetItem(QStringLiteral("未运行")));
        m_table->setItem(row, 3, new QTableWidgetItem(s.unit));
        m_table->setItem(row, 4, new QTableWidgetItem(QStringLiteral("%1 ~ %2").arg(s.expectedMin).arg(s.expectedMax)));
        m_table->setItem(row, 5, new QTableWidgetItem(QStringLiteral("待比对")));
        m_table->setItem(row, 6, new QTableWidgetItem(QStringLiteral("待测试")));
        m_table->setItem(row, 7, new QTableWidgetItem(s.description));
    }
    updateSummary();
}

void MainWindow::applyStyle() {
    qApp->setStyleSheet(R"(
        QMainWindow, QWidget { background: #0f172a; color: #e5e7eb; font-family: "Microsoft YaHei", "Noto Sans CJK SC", sans-serif; font-size: 14px; }
        QLabel#title { font-size: 30px; font-weight: 800; color: #67e8f9; padding: 14px 4px 2px; }
        QLabel#subtitle { color: #a5b4fc; padding: 0 4px 12px; }
        QLabel#summary { color: #bbf7d0; font-size: 18px; font-weight: 700; padding: 8px; }
        QTableWidget { background: #111827; alternate-background-color: #1f2937; gridline-color: #334155; border: 1px solid #334155; border-radius: 10px; }
        QHeaderView::section { background: #1d4ed8; color: white; padding: 9px; border: none; font-weight: 700; }
        QTableWidget::item { padding: 8px; }
        QTableWidget::item:selected { background: #2563eb; color: white; }
        QPushButton { background: #06b6d4; color: #082f49; border: none; border-radius: 8px; padding: 10px 18px; font-weight: 800; }
        QPushButton:hover { background: #22d3ee; }
        QProgressBar { border: 1px solid #334155; border-radius: 8px; text-align: center; background: #020617; height: 22px; }
        QProgressBar::chunk { border-radius: 8px; background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #22c55e, stop:1 #06b6d4); }
    )");
}

void MainWindow::runSelectedTest() {
    const int row = m_table->currentRow();
    if (row < 0) { QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择一个测试项。")); return; }
    upsertResult(m_engine.runTest(m_engine.testSpecs()[row]));
    m_progress->setValue(100);
}

void MainWindow::runAllTests() {
    m_results.clear();
    m_progress->setValue(0);
    const auto specs = m_engine.testSpecs();
    for (int i = 0; i < specs.size(); ++i) {
        upsertResult(m_engine.runTest(specs[i]));
        m_progress->setValue((i + 1) * 100 / specs.size());
        QApplication::processEvents();
    }
    m_status->setText(QStringLiteral("全项测试完成：已完成整机自检读取与逐项比对"));
}

void MainWindow::exportReport() {
    if (m_results.isEmpty()) { QMessageBox::warning(this, QStringLiteral("无法导出"), QStringLiteral("请先运行测试。")); return; }
    const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("导出测试报告"), "newstep-test-report.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    const QString error = m_engine.exportCsv(m_results, path);
    if (!error.isEmpty()) QMessageBox::critical(this, QStringLiteral("导出失败"), error);
    else QMessageBox::information(this, QStringLiteral("导出成功"), path);
}

void MainWindow::upsertResult(const TestResult &r) {
    bool replaced = false;
    for (auto &existing : m_results) {
        if (existing.spec.id == r.spec.id) { existing = r; replaced = true; break; }
    }
    if (!replaced) m_results.push_back(r);
    const auto specs = m_engine.testSpecs();
    int row = 0;
    for (; row < specs.size(); ++row) if (specs[row].id == r.spec.id) break;
    if (row >= specs.size()) return;
    m_table->item(row, 2)->setText(QString::number(r.measuredValue, 'f', 3));
    m_table->item(row, 5)->setText(r.selfCheckMatched ? QStringLiteral("一致") : QStringLiteral("不一致"));
    m_table->item(row, 6)->setText(r.passed ? QStringLiteral("通过") : QStringLiteral("失败"));
    m_table->item(row, 7)->setText(r.detail);
    const QColor color = r.passed ? QColor("#14532d") : QColor("#7f1d1d");
    for (int col = 0; col < m_table->columnCount(); ++col) m_table->item(row, col)->setBackground(color);
    m_status->setText(QStringLiteral("%1：%2").arg(r.spec.name, r.detail));
    updateSummary();
}

void MainWindow::updateSummary() {
    int pass = 0;
    for (const auto &r : m_results) if (r.passed) ++pass;
    m_summary->setText(QStringLiteral("通过 %1 / %2；一模/两模故障注入后要求工作状态正常").arg(pass).arg(m_engine.testSpecs().size()));
}
