#include "ui/roihistogramdialog.h"

#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QTableWidget>
#include <QVBoxLayout>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

RoiHistogramDialog::RoiHistogramDialog(
    const RoiStatistics& statistics,
    double imageMinValue,
    double imageMaxValue,
    QWidget* parent
    )
    : QDialog(parent)
{
    setWindowTitle("ROI Histogram");
    resize(500, 600);

    auto* layout = new QVBoxLayout(this);

    auto* summaryLabel = new QLabel(
        QString(
            "ROI: x=%1 y=%2 w=%3 h=%4\n"
            "Count: %5\n"
            "Mean: %6\n"
            "Min: %7\n"
            "Max: %8\n"
            "StdDev: %9"
            )
            .arg(statistics.rect.x())
            .arg(statistics.rect.y())
            .arg(statistics.rect.width())
            .arg(statistics.rect.height())
            .arg(statistics.pixelCount)
            .arg(statistics.mean, 0, 'f', 2)
            .arg(statistics.min, 0, 'f', 2)
            .arg(statistics.max, 0, 'f', 2)
            .arg(statistics.standardDeviation, 0, 'f', 2),
        this
        );

    const QVector<int>& histogram = statistics.histogram;
    const int binCount = histogram.size();
    const double range = imageMaxValue - imageMinValue;

    auto* table = new QTableWidget(this);
    table->setColumnCount(2);
    table->setRowCount(binCount);
    table->setHorizontalHeaderLabels(
        QStringList() << "Value Range" << "Count"
        );

    auto* series = new QLineSeries();

    for (int i = 0; i < binCount; ++i) {
        const double binCenter =
            imageMinValue + range * (i + 0.5) / binCount;

        series->append(binCenter, histogram[i]);

        const double binStart =
            imageMinValue + range * i / binCount;

        const double binEnd =
            imageMinValue + range * (i + 1) / binCount;

        table->setItem(
            i,
            0,
            new QTableWidgetItem(
                QString("%1 ~ %2")
                    .arg(static_cast<int>(binStart))
                    .arg(static_cast<int>(binEnd))
                )
            );

        table->setItem(
            i,
            1,
            new QTableWidgetItem(QString::number(histogram[i]))
            );
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("ROI Histogram");
    chart->legend()->hide();

    auto* axisX = new QValueAxis();
    axisX->setTitleText("Pixel Value");
    axisX->setRange(imageMinValue, imageMaxValue);
    axisX->setLabelFormat("%.0f");

    auto* axisY = new QValueAxis();
    axisY->setTitleText("Count");
    axisY->setLabelFormat("%d");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    auto* chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(260);

    table->horizontalHeader()->setStretchLastSection(true);

    layout->addWidget(summaryLabel);
    layout->addWidget(chartView);
    layout->addWidget(table);
}