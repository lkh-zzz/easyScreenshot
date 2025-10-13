#include "ScreenshotWidget.h"
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QFileDialog>
#include <QPainter>
#include <QRegion>

// 构造函数初始化截图小部件，设置布局和按钮连接。
// explain: 初始化 ScreenshotWidget 的成员变量，设置窗口属性，加载屏幕截图并准备绘制界面。
ScreenshotWidget::ScreenshotWidget(QWidget *parent)
    : QWidget(parent),
      screenshotLabel(new QLabel(this)),
      statusLabel(new QLabel(this)),
      isDrawing(false),
      isDragging(false),
      dragOffset(QPoint(0, 0)),
      confirmButton(new QPushButton(this)),
      cancelButton(new QPushButton(this)),
      saveButton(new QPushButton(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    mainLayout->addWidget(screenshotLabel);
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    confirmButton->hide();
    cancelButton->hide();
    saveButton->hide();

    connect(confirmButton, &QPushButton::clicked, this, &ScreenshotWidget::onConfirmButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ScreenshotWidget::onCancelButtonClicked);
    connect(saveButton, &QPushButton::clicked, this, &ScreenshotWidget::onSaveButtonClicked);

    takeScreenshot();
    setMyStyle();

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int width = screenGeometry.width();
        int height = screenGeometry.height();
        resize(width, height);
    }

    setWindowModality(Qt::WindowModal);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowState(Qt::WindowActive | Qt::WindowFullScreen);

    //    setMouseTracking(true); // 不捕捉鼠标不点击时的轨迹
}

ScreenshotWidget::~ScreenshotWidget()
{
}

// 设置界面组件的样式。
// explain: 该函数用于初始化或更新界面中按钮和状态标签的样式。
void ScreenshotWidget::setMyStyle()
{
    // 设置按钮
    confirmButton->setIcon(QIcon(":/new/img/Img/yes.png"));
    cancelButton->setIcon(QIcon(":/new/img/Img/close.png"));
    saveButton->setIcon(QIcon(":/new/img/Img/save.png"));
    confirmButton->setStyleSheet("border: none; padding: 0px;");
    cancelButton->setStyleSheet("border: none; padding: 0px;");
    saveButton->setStyleSheet("border: none; padding: 0px;");
    confirmButton->setFixedSize(32, 32);
    cancelButton->setFixedSize(32, 32);
    saveButton->setFixedSize(32, 32);

    // 设置位置标签
    statusLabel->setStyleSheet("QLabel { background-color : white; color : black; border: 1px solid grey; padding: 2px; }");
    statusLabel->setText("Position: (0, 0)");
    statusLabel->setAlignment(Qt::AlignRight);
    statusLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    statusLabel->setFont(QFont("Arial", 10));
    statusLabel->setMaximumWidth(200);
    statusLabel->setFixedWidth(200);
    statusLabel->setFixedHeight(20);
    statusLabel->setStyleSheet("QLabel { background-color: transparent; border: none; color: white; padding: 0px; }");
}

// 更新按钮位置以适应选择区域。
// explain: 当用户完成一个矩形的选择后，此函数会更新确认、取消和保存按钮的位置，使它们靠近选择区域的底部中心。
void ScreenshotWidget::updateButtonPositions()
{
    if (!isDrawing || selectionRect.isEmpty()) {
        confirmButton->hide();
        cancelButton->hide();
        saveButton->hide();
        return;
    }

    confirmButton->show();
    cancelButton->show();
    saveButton->show();

    int buttonHeight = confirmButton->height();
    int buttonsWidth = confirmButton->width() + cancelButton->width() + saveButton->width() + 20;
    int rectBottom = selectionRect.bottom();
    int rectRight = selectionRect.right();
    int rectLeft = selectionRect.left();

    int x = qMax(rectLeft, qMin(rectRight - buttonsWidth, width() - buttonsWidth));
    int y = qMin(rectBottom + 5, height() - buttonHeight);

    saveButton->move(x, y);
    cancelButton->move(x + saveButton->width() + 5, y);
    confirmButton->move(x + saveButton->width() + cancelButton->width() + 10, y);

    confirmButton->raise();
    saveButton->raise();
    cancelButton->raise();
}


// 捕获屏幕截图。
// explain: 使用当前主屏幕捕获整个屏幕的图像，并将其存储在成员变量中。
void ScreenshotWidget::takeScreenshot()
{
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        screenshot = screen->grabWindow(0);
    }
}

// 窗口大小改变时触发。
// explain: 处理窗口大小改变事件，确保组件正确响应窗口大小变化。
void ScreenshotWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

// 鼠标按下事件处理。
// explain: 当鼠标左键被按下时，判断是开始绘制新矩形还是拖拽现有矩形。
void ScreenshotWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isDrawing && selectionRect.contains(event->pos())) {
            isDragging = true;
            dragOffset = event->pos() - selectionRect.topLeft();
        } else {
            isDrawing = true;
            startPos = event->pos();
            endPos = startPos;
            isDragging = false;
            confirmButton->hide();
            cancelButton->hide();
            saveButton->hide();
        }
        update();
    }

    statusLabel->show();
    updateStatusLabel(event->pos());
}

// 鼠标移动事件处理。
// explain: 在鼠标左键按下的情况下，更新选择区域的终点或拖拽矩形，并重绘界面。
void ScreenshotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isDrawing) {
        if (isDragging) {
            QPoint newPos = event->pos() - dragOffset;
            selectionRect.moveTopLeft(newPos);
            confirmButton->hide();
            cancelButton->hide();
            saveButton->hide();
        } else {
            endPos = event->pos();
            selectionRect = QRect(startPos, endPos).normalized();
        }
        update();
    }


    // 添加调试信息
    //    qDebug() << "mouseMoveEvent: isDrawing=" << isDrawing
    //             << ", isDragging=" << isDragging
    //             << ", selectionRect=" << selectionRect;

    updateStatusLabel(event->pos());
}

// 鼠标释放事件处理。
// explain: 当鼠标左键被释放时，结束绘制或拖拽矩形，并根据选择区域更新按钮位置。
void ScreenshotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isDragging) {
            isDragging = false;
        } else if (isDrawing) {
            endPos = event->pos();
            selectionRect = QRect(startPos, endPos).normalized();

        }
        update();
        updateButtonPositions();
    }
}
// 绘制事件处理。
// explain: 负责绘制截屏图像以及选择区域的遮罩和边框。
void ScreenshotWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 绘制背景截图
    QPixmap screenshotPixmap(this->size());
    screenshotPixmap.fill(Qt::transparent);
    {
        QPainter p(&screenshotPixmap);
        QRect targetRect = rect();
        QPixmap scaledScreenshot = this->screenshot.scaled(targetRect.size(),
                                                           Qt::KeepAspectRatio,
                                                           Qt::SmoothTransformation);

        int x = (targetRect.width() - scaledScreenshot.width()) / 2;
        int y = (targetRect.height() - scaledScreenshot.height()) / 2;
        p.drawPixmap(x, y, scaledScreenshot);
    }

    // 绘制阴影效果
    QPixmap shadowPixmap(this->size());
    shadowPixmap.fill(Qt::transparent);
    if (isDrawing) {
        QPainter p(&shadowPixmap);
        QColor shadowColor(0, 0, 0, 128);
        p.setBrush(QBrush(shadowColor));
        p.setPen(Qt::NoPen);

        QRect selectRect = selectionRect;
        if (!selectRect.isEmpty()) {
            QRegion nonSelectionRegion(rect());
            nonSelectionRegion -= QRegion(selectRect);

            p.setClipRegion(nonSelectionRegion);
            p.drawRect(rect());
        } else {
            p.drawRect(rect());
        }
    }

    // 绘制截图和阴影
    painter.drawPixmap(0, 0, screenshotPixmap);
    painter.drawPixmap(0, 0, shadowPixmap);

    // 绘制选择矩形的边框
    if (isDrawing) {
        QRect selectRect = selectionRect;
        if (!selectRect.isEmpty()) {
            QPen pen(Qt::green, 3);
            pen.setStyle(isDragging ? Qt::DashLine : Qt::SolidLine);
            painter.setPen(pen);
            painter.drawRect(selectRect);
        }
    }
}


// 确认按钮点击槽。
// explain: 当用户点击确认按钮时，裁剪选中的区域，并发出信号通知截图已捕获。
void ScreenshotWidget::onConfirmButtonClicked()
{
    QPixmap croppedPixmap = screenshot.copy(selectionRect);

    emit screenshotCaptured(croppedPixmap);

    confirmButton->hide();
    cancelButton->hide();
    saveButton->hide();
    isDrawing = false;
    update();

    this->close();
}

// 取消按钮点击槽。
// explain: 当用户点击取消按钮时，隐藏所有操作按钮，取消绘制状态，并关闭窗口。
void ScreenshotWidget::onCancelButtonClicked()
{
    confirmButton->hide();
    cancelButton->hide();
    saveButton->hide();
    isDrawing = false;
    update();
    this->close();
}

// 异步保存文件。
// explain: 用户点击保存按钮时，弹出文件对话框让用户选择保存路径，然后异步保存截图到磁盘。
void ScreenshotWidget::onSaveButtonClicked()
{
    QPixmap croppedPixmap = screenshot.copy(selectionRect);

    QString filePath = QFileDialog::getSaveFileName(
                this,
                tr("Save Screenshot"),
                "untitled.png",
                tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;All Files (*)")
                );

    if (!filePath.isEmpty()) {
        QtConcurrent::run([=]() {
            croppedPixmap.save(filePath);
            qDebug() << "Screenshot saved to" << filePath;
        });

        confirmButton->hide();
        cancelButton->hide();
        saveButton->hide();
        isDrawing = false;
        update();

        this->close();
    }
}

// 更新状态标签的方法。
// explain: 根据鼠标位置更新状态标签的文本，并在绘制状态下调整其位置以不遮挡选择区域。
void ScreenshotWidget::updateStatusLabel(const QPoint &pos)
{
    statusLabel->setText(QString("Position: (%1, %2)").arg(pos.x()).arg(pos.y()));

    if (isDrawing || isDragging) {

        int labelHeight = statusLabel->height();
        int labelWidth = statusLabel->width();

        if (isDrawing && !isDragging) {
            QRect selectRect = QRect(startPos, pos).normalized();

            int x = qMax(selectRect.left(), 0);
            int y = qMax(selectRect.top() - labelHeight, 0);

            if (y < 0) {
                y = qMin(selectRect.bottom() + 5, height() - labelHeight);
            }

            x = qBound(0, x, width() - labelWidth);
            y = qBound(0, y, height() - labelHeight);

            statusLabel->move(x, y);
            statusLabel->raise();
        } else {
            QRect selectRect = selectionRect.normalized();

            int x = qMax(selectRect.left(), 0);
            int y = qMax(selectRect.top() - labelHeight, 0);

            if (y < 0) {
                y = qMin(selectRect.bottom() + 5, height() - labelHeight);
            }

            x = qBound(0, x, width() - labelWidth);
            y = qBound(0, y, height() - labelHeight);

            statusLabel->move(x, y);
            statusLabel->raise();
        }
    } else {
        statusLabel->hide();
    }
}
