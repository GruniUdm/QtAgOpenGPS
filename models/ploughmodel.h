#ifndef PLOUGHMODEL_H
#define PLOUGHMODEL_H

#include <QAbstractListModel>
#include <QVector>

class PloughModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        CurrentWidthRole,
        TargetWidthRole,
        PloughModeRole,
        PloughModeNameRole,
        PloughEnabledRole,
        AutoWidthEnabledRole,
        AutoLiftEnabledRole,
        IsCalibratedRole,
        MinWidthRole,
        MaxWidthRole,
        DeadBandRole,
        SectionOnRole,
        LineDistanceRole,
        ErrorRole,
        CommandRole
    };
    Q_ENUM(Roles)

    struct PloughData {
        int id;                 // ID плуга (всегда 0 для одного плуга, но можно расширить)
        int currentWidth;       // Текущая ширина в см
        int targetWidth;        // Целевая ширина в см
        int ploughMode;         // Текущий режим работы (0-8)
        QString ploughModeName; // Название режима
        bool ploughEnabled;     // Плуг включен
        bool autoWidthEnabled;  // Автоширина включена
        bool autoLiftEnabled;   // Автоподъем включен
        bool isCalibrated;      // Калиброван ли
        int minWidth;           // Минимальная ширина
        int maxWidth;           // Максимальная ширина
        int deadBand;           // Мертвая зона в см
        bool sectionOn;         // Секция включена
        int lineDistance;       // Расстояние до линии
        int error;              // Ошибка (current - target)
        int command;            // Последняя команда
    };

    explicit PloughModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для доступа по индексу
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE int getPloughId(int index) const;

    // Data management
    void setPloughs(const QVector<PloughData> &ploughs);
    void addPlough(const PloughData &plough);
    void clear();

    // Методы изменения данных по индексу
    void updateCurrentWidth(int index, int width);
    void updateTargetWidth(int index, int width);
    void updatePloughMode(int index, int mode, const QString &modeName = QString());
    void updatePloughEnabled(int index, bool enabled);
    void updateAutoWidthEnabled(int index, bool enabled);
    void updateAutoLiftEnabled(int index, bool enabled);
    void updateCalibration(int index, bool calibrated, int minWidth, int maxWidth);
    void updateDeadBand(int index, int deadBand);
    void updateSectionState(int index, bool sectionOn);
    void updateLineDistance(int index, int distance);
    void updateError(int index, int error);
    void updateCommand(int index, int command);

    // Обновление всех данных сразу
    void updateAllData(int index, const PloughData &data);

    // Utility
    int count() const { return m_ploughs.count(); }
    bool isValidIndex(int index) const { return index >= 0 && index < m_ploughs.count(); }

    // Получение данных по индексу
    PloughData ploughAt(int index) const;

signals:
    void countChanged();
    void currentWidthChanged(int index, int width);
    void targetWidthChanged(int index, int width);
    void ploughModeChanged(int index, int mode, const QString &modeName);
    void ploughEnabledChanged(int index, bool enabled);
    void autoWidthEnabledChanged(int index, bool enabled);
    void autoLiftEnabledChanged(int index, bool enabled);
    void calibrationChanged(int index, bool calibrated, int minWidth, int maxWidth);
    void deadBandChanged(int index, int deadBand);
    void sectionStateChanged(int index, bool sectionOn);
    void lineDistanceChanged(int index, int distance);
    void errorChanged(int index, int error);
    void commandChanged(int index, int command);
    void ploughDataUpdated(int index);

private:
    QVector<PloughData> m_ploughs;

    // Вспомогательный метод для получения названия режима
    QString getModeName(int mode) const;
};

#endif // PLOUGHMODEL_H
