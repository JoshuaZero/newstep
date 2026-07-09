#include "testengine.h"

#include <QFile>
#include <QRandomGenerator>
#include <QTextStream>

TestEngine::TestEngine(QObject *parent) : QObject(parent) {
    m_specs = {
        {"power_voltage", "上电电压测试", "功能测试", "V", 27.0, 29.0, "power_voltage", "采集上电母线电压并与整机自检电压窗口比对"},
        {"ram", "RAM测试", "功能测试", "%", 99.0, 100.0, "ram", "读取内存自检覆盖率、校验和与错误计数"},
        {"flash", "FLASH测试", "功能测试", "%", 99.0, 100.0, "flash", "读取FLASH自检擦写/校验状态"},
        {"mil1553b", "1553B通信测试", "功能测试", "%", 98.0, 100.0, "mil1553b", "校验1553B总线收发成功率和协议响应"},
        {"rs422", "RS422通信测试", "功能测试", "%", 98.0, 100.0, "rs422", "校验RS422链路帧收发和误码统计"},
        {"ethernet", "以太网通信测试", "功能测试", "Mbps", 90.0, 1000.0, "ethernet", "校验以太网吞吐、连通性和丢包率"},
        {"switch_in", "开关量输入测试", "功能测试", "%", 100.0, 100.0, "switch_in", "比对开关量输入采样矩阵"},
        {"switch_out", "开关量输出测试", "功能测试", "%", 100.0, 100.0, "switch_out", "比对开关量输出回读矩阵"},
        {"timing_hv", "时序高压测试", "功能测试", "ms", 0.0, 5.0, "timing_hv", "校验高压时序偏差"},
        {"timing_current", "时序电流测试", "功能测试", "ms", 0.0, 5.0, "timing_current", "校验电流建立时序偏差"},
        {"analog_low_voltage", "模拟低压通路", "功能测试", "V", 4.8, 5.2, "analog_low_voltage", "校验低压模拟通路采样值"},
        {"load_distribution", "配电模拟负载", "功能测试", "A", 0.8, 1.2, "load_distribution", "校验配电模拟负载电流"},
        {"safety_command", "安控指令模拟测试", "功能测试", "%", 100.0, 100.0, "safety_command", "校验安控指令模拟链路响应"},
        {"single_mode_fault", "一模故障注入测试", "多模可靠性测试", "%", 100.0, 100.0, "single_mode_fault", "注入一模故障后检查系统重构和工作状态"},
        {"dual_mode_fault", "两模故障注入测试", "多模可靠性测试", "%", 100.0, 100.0, "dual_mode_fault", "注入两模故障后检查系统降级与工作状态"},
    };
}

const QVector<TestSpec> &TestEngine::testSpecs() const { return m_specs; }

QMap<QString, double> TestEngine::readWholeMachineSelfCheck() const {
    return {
        {"power_voltage", 28.1}, {"ram", 100.0}, {"flash", 100.0}, {"mil1553b", 99.7},
        {"rs422", 99.5}, {"ethernet", 940.0}, {"switch_in", 100.0}, {"switch_out", 100.0},
        {"timing_hv", 2.1}, {"timing_current", 2.4}, {"analog_low_voltage", 5.0},
        {"load_distribution", 1.0}, {"safety_command", 100.0}, {"single_mode_fault", 100.0},
        {"dual_mode_fault", 100.0},
    };
}

TestResult TestEngine::runTest(const TestSpec &spec) const {
    const auto selfCheck = readWholeMachineSelfCheck();
    const double selfValue = selfCheck.value(spec.selfCheckKey, -9999.0);
    TestResult result;
    result.spec = spec;
    result.measuredValue = simulateMeasurement(spec, selfCheck);
    result.selfCheckMatched = qAbs(result.measuredValue - selfValue) <= qMax(0.05, qAbs(selfValue) * 0.03);
    result.passed = fuzzyInRange(result.measuredValue, spec.expectedMin, spec.expectedMax) && result.selfCheckMatched;
    result.timestamp = QDateTime::currentDateTime();
    result.detail = result.passed
        ? QStringLiteral("通过：测量值与整机自检值一致，工作状态正常")
        : QStringLiteral("未通过：测量值、阈值或整机自检比对异常");
    return result;
}

QVector<TestResult> TestEngine::runAllTests() const {
    QVector<TestResult> results;
    for (const auto &spec : m_specs) results.push_back(runTest(spec));
    return results;
}

QString TestEngine::exportCsv(const QVector<TestResult> &results, const QString &filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return file.errorString();
    QTextStream out(&file);
    out << "time,category,name,measured,unit,expected_min,expected_max,self_check_matched,result,detail\n";
    for (const auto &r : results) {
        out << r.timestamp.toString(Qt::ISODate) << ',' << r.spec.category << ',' << r.spec.name << ','
            << QString::number(r.measuredValue, 'f', 3) << ',' << r.spec.unit << ','
            << r.spec.expectedMin << ',' << r.spec.expectedMax << ','
            << (r.selfCheckMatched ? "YES" : "NO") << ',' << (r.passed ? "PASS" : "FAIL") << ','
            << r.detail << '\n';
    }
    return QString();
}

double TestEngine::simulateMeasurement(const TestSpec &spec, const QMap<QString, double> &selfCheck) const {
    const double base = selfCheck.value(spec.selfCheckKey, (spec.expectedMin + spec.expectedMax) / 2.0);
    if (spec.expectedMin == spec.expectedMax) return base;
    const double span = qMax(0.1, spec.expectedMax - spec.expectedMin);
    const double noise = (QRandomGenerator::global()->bounded(2001) - 1000) / 1000.0 * span * 0.01;
    return base + noise;
}

bool TestEngine::fuzzyInRange(double value, double min, double max) {
    const double epsilon = 0.0001;
    return value + epsilon >= min && value - epsilon <= max;
}
