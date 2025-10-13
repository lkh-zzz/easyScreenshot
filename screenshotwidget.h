#ifndef SCREENSHOTWIDGET_H
#define SCREENSHOTWIDGET_H

#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include <QPoint>
#include <QPixmap>
#include <QRect>
#include <QtConcurrent/QtConcurrent>

class ScreenshotWidget : public QWidget {
    Q_OBJECT

public:
    explicit ScreenshotWidget(QWidget *parent = nullptr);
    ~ScreenshotWidget();

protected:
    void takeScreenshot();
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onConfirmButtonClicked();
    void onCancelButtonClicked();
    void onSaveButtonClicked();

signals:
    void screenshotCaptured(const QPixmap &croppedPixmap);

private:
    QLabel *screenshotLabel;   //
    QLabel *statusLabel;
    QPixmap screenshot;
    QPoint startPos, endPos;
    bool isDrawing;
    bool isDragging;
    QPoint dragOffset;
    QRect selectionRect;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
    QPushButton *saveButton;

    void updateStatusLabel(const QPoint &pos);
    void updateButtonPositions();
    void setMyStyle();
};

#endif // SCREENSHOTWIDGET_H
