#ifndef DESKTOPPET_H
#define DESKTOPPET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui {
class DesktopPet;
}
QT_END_NAMESPACE

enum FunctionType {
    Alarm,
    Talk,
    Play,
};

class DesktopPet : public QWidget
{
    Q_OBJECT

public:
    DesktopPet(QWidget *parent = nullptr);
    ~DesktopPet();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void addButton(FunctionType type, const QString &text);

private slots:
    void updatePetImage();
    void handleLongPressTimeout();
    void setAlarm();

private:
    Ui::DesktopPet *ui;

    QHBoxLayout *m_layout;
    QLabel *m_petLabel;
    QMap<FunctionType, QPushButton*> m_functionButtons;

    QTimer *m_longPressTimer;
    bool m_longPressTriggered;
    QPoint m_mouseGrabOffset;
    int m_currentImageIndex;
};
#endif // DESKTOPPET_H
