#pragma once

#include <QObject>
#include <QDateTime>
#include <QMap>
#include <QString>
#include <QVector>

struct TestSpec {
    QString id;
    QString name;
    QString category;
    QString unit;
    double expectedMin = 0.0;
    double expectedMax = 0.0;
    QString selfCheckKey;
    QString description;
};

struct TestResult {
    TestSpec spec;
    double measuredValue = 0.0;
    bool selfCheckMatched = false;
    bool passed = false;
    QString detail;
    QDateTime timestamp;
};

class TestEngine : public QObject {
    Q_OBJECT
public:
    explicit TestEngine(QObject *parent = nullptr);

    const QVector<TestSpec> &testSpecs() const;
    QMap<QString, double> readWholeMachineSelfCheck() const;
    TestResult runTest(const TestSpec &spec) const;
    QVector<TestResult> runAllTests() const;
    QString exportCsv(const QVector<TestResult> &results, const QString &filePath) const;

private:
    QVector<TestSpec> m_specs;

    double simulateMeasurement(const TestSpec &spec, const QMap<QString, double> &selfCheck) const;
    static bool fuzzyInRange(double value, double min, double max);
};
