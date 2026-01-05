#ifndef BLOCKAGE_H
#define BLOCKAGE_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include "settingsmanager.h"
#include "pgnparser.h"
#include "blockagemodel.h"
#include "simpleproperty.h"

#define MAX_SECTIONS 64

class PGNParser;

class Blockage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QMutex mutex;

protected:
    //QVariantList rows;
    explicit Blockage(QObject *parent = nullptr);
    ~Blockage() override=default;

    //prevent copying
    Blockage(const Blockage &) = delete;
    Blockage &operator=(const Blockage &) = delete;

    static Blockage *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static Blockage *instance();
    static Blockage *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    Q_INVOKABLE void reset_count();

    int blockageSecCount1[16] = {0};
    int blockageSecCount2[16] = {0};
    int blockageSecCount3[16] = {0};
    int blockageSecCount4[16] = {0};
    int blockageseccount[64] = {0};
    int min=0;

    Q_PROPERTY(BlockageModel *blockageModel READ blockageModel CONSTANT)
    BlockageModel *blockageModel() const { return m_blockageModel; }
    BlockageModel *m_blockageModel;

    SIMPLE_BINDABLE_PROPERTY(int, avg)
    SIMPLE_BINDABLE_PROPERTY(int, min1)
    SIMPLE_BINDABLE_PROPERTY(int, min2)
    SIMPLE_BINDABLE_PROPERTY(int, max)
    SIMPLE_BINDABLE_PROPERTY(int, min1_i)
    SIMPLE_BINDABLE_PROPERTY(int, min2_i)
    SIMPLE_BINDABLE_PROPERTY(int, max_i)
    SIMPLE_BINDABLE_PROPERTY(int, blocked)
    //blockageSecCount will be bound in constructor to a lambda to
    //pull out information from the above int arrays. Must call
    //m_blockageSecCount.notify() to let the property system know
    //the data must be recalculated.
    SIMPLE_BINDABLE_PROPERTY(QVariantList, secCount)

signals:

public slots:
    void onBlockageDataReady(const PGNParser::ParsedData& data);
    void statistics(const double speed);

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_avg, 0, &Blockage::avgChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_min1, 0, &Blockage::min1Changed)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_min2, 0, &Blockage::min2Changed)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_max, 0, &Blockage::maxChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_min1_i, 0 ,&Blockage::min1_iChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_min2_i, 0, &Blockage::min2_iChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_max_i, 0, &Blockage::max_iChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Blockage, int, m_blocked, 0, &Blockage::blockedChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Blockage, QVariantList, m_secCount, &Blockage::secCountChanged)

    qint64 lastUpdate;

};
#endif // BLOCKAGE_H
