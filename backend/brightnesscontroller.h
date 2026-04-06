#ifndef BRIGHTNESSCONTROLLER_H
#define BRIGHTNESSCONTROLLER_H

#include <QObject>
#include <QQmlEngine>

class BrightnessController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit BrightnessController(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void brightnessUp();
    void brightnessDown();

signals:
    void brightnessChanged(int value);

private:
    int currentBrightness = 40;
};

#endif // BRIGHTNESSCONTROLLER_H
