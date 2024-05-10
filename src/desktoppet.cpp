#include "desktoppet.h"
#include "ui_desktoppet.h"

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QInputDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QSpinBox>

DesktopPet::DesktopPet(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DesktopPet)
    , m_currentImageIndex(1)
    , m_petLabel(new QLabel(this))
    , m_layout(new QHBoxLayout(this))
    , m_longPressTimer(new QTimer(this))
    , m_longPressTriggered(false)
{
    ui->setupUi(this);

    // 设置窗口
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlags (windowFlags () | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    // 设置宠物标签尺寸
    m_petLabel->setMinimumSize(600, 800);
    m_petLabel->setMaximumSize(600, 800);

    // 设置布局
    m_layout->addWidget(m_petLabel);
    m_layout->setStretchFactor(m_petLabel, 1);
    m_layout->setAlignment(Qt::AlignTop);

    // 添加功能
    addButton(Alarm, "Alarm");
    addButton(Talk, "Talk");
    addButton(Play, "Play");
    for (QPushButton* button : m_functionButtons.values()) {
        button->setVisible(button->isVisible());
    }
    connect(m_functionButtons[Alarm], &QPushButton::clicked, this, &DesktopPet::setAlarm);

    // 配置长按定时器
    m_longPressTimer->setSingleShot(true);
    connect(m_longPressTimer, &QTimer::timeout, this, &DesktopPet::handleLongPressTimeout);

    // 更新宠物图像
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DesktopPet::updatePetImage);
    timer->start(50);
}
DesktopPet::~DesktopPet()
{
    delete m_layout;
    delete m_longPressTimer;
    qDeleteAll(m_functionButtons);
    delete ui;
}
void DesktopPet::addButton(FunctionType type, const QString &text)
{
    QPushButton* button = new QPushButton(this);
    button->setMinimumSize(80, 20);
    button->setMaximumSize(80, 20);
    button->setText(text);
    m_functionButtons[type] = button;
    m_layout->addWidget(button);
}
void DesktopPet::updatePetImage()
{
    m_currentImageIndex = (m_currentImageIndex % 2970) + 1;
    m_petLabel->setStyleSheet(QString("background: url(://nilu/models/nilu/%1.png) no-repeat;").arg(m_currentImageIndex));
    update();
}

void DesktopPet::mousePressEvent(QMouseEvent *event)
{
    m_mouseGrabOffset = event->globalPos() - this->pos();
    if (event->button() == Qt::LeftButton) {
        if (m_petLabel->geometry().contains(event->pos())) {
            m_longPressTriggered = false;
            m_longPressTimer->start(100);
        }
    }
}
void DesktopPet::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseGrabOffset = QPoint();
    if (event->button() == Qt::LeftButton) {
        if (!m_longPressTriggered) {
            for (QPushButton* button : m_functionButtons.values()) {
                button->setVisible(!button->isVisible());
            }
        }
    }
}
void DesktopPet::mouseMoveEvent(QMouseEvent *event)
{
    if (m_longPressTriggered) {
        QPoint newPos = event->globalPos() - m_mouseGrabOffset;
        move(newPos);
    }
}

void DesktopPet::handleLongPressTimeout()
{
    m_longPressTriggered = true;
}
void DesktopPet::setAlarm()
{
    qDebug() << "设置闹钟按钮点击！";

    // 创建设置闹钟时间的对话框
    QDialog dialog(this);
    dialog.setWindowTitle("设置闹钟时间");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("选择闹钟时间：", &dialog);
    layout->addWidget(label);

    // 添加小时、分钟、秒数输入框
    QHBoxLayout *timeLayout = new QHBoxLayout;
    QSpinBox *hourSpinBox = new QSpinBox(&dialog);
    hourSpinBox->setRange(0, 23);
    hourSpinBox->setValue(QTime::currentTime().hour());
    timeLayout->addWidget(hourSpinBox);
    timeLayout->addWidget(new QLabel("时", &dialog));

    QSpinBox *minuteSpinBox = new QSpinBox(&dialog);
    minuteSpinBox->setRange(0, 59);
    minuteSpinBox->setValue(QTime::currentTime().minute());
    timeLayout->addWidget(minuteSpinBox);
    timeLayout->addWidget(new QLabel("分", &dialog));

    QSpinBox *secondSpinBox = new QSpinBox(&dialog);
    secondSpinBox->setRange(0, 59);
    secondSpinBox->setValue(0);
    timeLayout->addWidget(secondSpinBox);
    timeLayout->addWidget(new QLabel("秒", &dialog));

    layout->addLayout(timeLayout);

    // 添加确定按钮
    QPushButton *okButton = new QPushButton("确定", &dialog);
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(okButton);

    // 显示对话框并等待用户输入
    if (dialog.exec() == QDialog::Accepted) {
        int hours = hourSpinBox->value();
        int minutes = minuteSpinBox->value();
        int seconds = secondSpinBox->value();

        qDebug() << "用户选择的时间：" << hours << "时" << minutes << "分" << seconds << "秒";

        QTime alarmTime(hours, minutes, seconds);

        qDebug() << "当前时间：" << QTime::currentTime();
        qDebug() << "闹钟时间：" << alarmTime;

        // 计算距离闹钟时间的毫秒数
        int millisecondsToAlarm = QTime::currentTime().msecsTo(alarmTime);
        qDebug() << "距离闹钟时间的毫秒数：" << millisecondsToAlarm;

        if (millisecondsToAlarm > 0) {
            // 输出闹钟设置信息
            qDebug() << "闹钟已设置：" << alarmTime.toString("hh:mm:ss");

            // 定时器设定闹钟触发时间
            QTimer *alarmTimer = new QTimer(this);
            connect(alarmTimer, &QTimer::timeout, this, [this]() {
                qDebug() << "时间到！闹钟响了！";
                QMessageBox::information(this, "闹钟", "时间到！闹钟响了！");
                QTimer *timer = qobject_cast<QTimer*>(sender());
                if (timer) {
                    timer->deleteLater();
                }
            });
            alarmTimer->setSingleShot(true);
            alarmTimer->start(millisecondsToAlarm);
        } else {
            qDebug() << "无效的闹钟时间！";
        }
    }
}

