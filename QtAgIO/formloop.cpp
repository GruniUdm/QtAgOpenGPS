#include "formloop.h"
#include "agioproperty.h"
#include "qmlutil.h"
#include <QQuickWindow>
//#include <QQmlApplicationEngine>
#include "qmlsettings.h"
#include "src/bluetoothdevicelist.h"
#include "src/bluetoothmanager.h"
#include <QQmlContext>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QtQml>

AgIOSettings *agiosettings;
//extern QMLSettings qml_settings;

//FormLoop::FormLoop(QWidget *parent) : QQmlApplicationEngine(parent),
FormLoop::FormLoop(QObject *parent) : QObject(parent),
    m_status("Initializing..."),
     //qml_root(parent),
    wwwNtrip("192.168.1.100", 2101, 2102), // Example initial values
    ethUDP("192.168.1.101", 2201, 2202),
    ethModulesSet("255.255.255.255", 2301, 2302)
{

//     // qml_settings->setupKeys();
//     // qml_settings->loadSettings();
}


void FormLoop::setEngine(QQmlApplicationEngine *engine)
{
     qDebug() << "Appel de setEngine avec engine:" << engine;
    if (!m_engine) {
        m_engine = engine;
        qDebug() << "Engine correctement défini dans FormLoop";
        //emit engineReady();
    } else {
        qDebug() << "Attention: Engine déjà défini dans FormLoop!";
    }

    agiosettings = new AgIOSettings();
    AgIOProperty::init_defaults();
    agiosettings->sync();

    // ===== QMLSettings maintenant géré par FormGPS =====
    // L'instance et la configuration sont maintenant dans le module AOG principal
    QMLSettings* qmlsettings_instance = QMLSettings::instance();
    qmlsettings_instance->setupKeys();
    qmlsettings_instance->loadSettings();
    qDebug() << "FormLoop using QMLSettings instance:" << qmlsettings_instance;
    // qml_settings->setupKeys();
    // qml_settings->loadSettings();


    btDevicesList = new BluetoothDeviceList(this);// I don't like this, but right now, the class
    //has to be in place when the QML starts.

    bluetoothManager = new BluetoothManager(this, this);


    //tell the QML what OS we are using
    #ifdef __ANDROID__
        //rootContext()->setContextProperty("OS", "ANDROID");
        setContextProperty("OS", QString("ANDROID"));
    #elif defined(__WIN32)
        //setContextProperty("OS", "WINDOWS");
        setContextProperty("OS", QString("WINDOWS"));
    #else
        //rootContext()->setContextProperty("OS", "LINUX");
        setContextProperty("OS", QString("LINUX"));    
    #endif

    // ✅ agiosettings now available via QML_SINGLETON - no setContextProperty needed
    setContextProperty("bluetoothDeviceList", QVariant::fromValue(btDevicesList));

    // setupGUI();
    // loadSettings();

    // LoadLoopback();
    // LoadUDPNetwork();

    // if(property_setBluetooth_isOn)
    //     bluetoothManager->startBluetoothDiscovery();

    // ConfigureNTRIP();

    // halfSecondTimer = new QTimer(this);
    // halfSecondTimer->setInterval(500);
    // connect(halfSecondTimer, &QTimer::timeout, this, &FormLoop::timer1_Tick);

    // oneSecondTimer = new QTimer(this);
    // oneSecondTimer->setInterval(1000);
    // connect(oneSecondTimer, &QTimer::timeout, this, &FormLoop::oneSecondLoopTimer_Tick);
    // oneSecondTimer->start();


    // twoSecondTimer = new QTimer(this);
    // twoSecondTimer->setInterval(2000);
    // connect(twoSecondTimer, &QTimer::timeout, this, &FormLoop::TwoSecondLoop);
    // twoSecondTimer->start();

    // tmr = new QTimer(this);// the timer used in formloop_ntripcomm
    // tmr->setSingleShot(false);
    // tmr->setInterval(5000);
    // connect(tmr, &QTimer::timeout, this, &FormLoop::SendGGA);
    // tmr->start();

    // ntripMeterTimer = new QTimer(this);
    // ntripMeterTimer->setSingleShot(false);
    // ntripMeterTimer->setInterval(50);
    // connect(ntripMeterTimer, &QTimer::timeout, this, &FormLoop::ntripMeterTimer_Tick);


    // clientSocket = new QTcpSocket(this);

    // FormUDp_Load();
    // swFrame.start();
    QSerialPortInfo serialPortInfo;
    QList<QSerialPortInfo> ports = serialPortInfo.availablePorts();
    QList<qint32> bauds = serialPortInfo.standardBaudRates();
    QStringList portsName;
    QStringList baudsStr;


    foreach (QSerialPortInfo port, ports) {

        portsName.append(port.portName());

    }

    foreach (qint32 baud, bauds) {

        baudsStr.append(QString::number(baud));

    }

    //setContextProperty("serialTerminal",&serialTerminal);
    setContextProperty("portsNameModel",QVariant::fromValue(portsName));
    setContextProperty("baudsModel",QVariant::fromValue(baudsStr));

}

QQmlApplicationEngine* FormLoop::engine() const
{
    return m_engine;
}


void FormLoop::setContextProperty(const QString &name, const QVariant &value)
{
    if (m_engine) {
        m_engine->rootContext()->setContextProperty(name, value);
    } else {
        qDebug() << "Error: Engine not initialized, storing property: " << name;
    }
}

void FormLoop::setStatus(const QString &value)
{
    if (m_status != value) {
        m_status = value;
        emit statusChanged();
    }
}

FormLoop* FormLoop::instance()
{
    static FormLoop instance;  // ✅ Utilisation d'une instance statique locale
    return &instance;
}




// FormLoop::~FormLoop()
// {
//     /* clean up our dynamically-allocated
//      * objects.
//      */
// }
void FormLoop::oneSecondLoopTimer_Tick(){
    DoNTRIPSecondRoutine();
}

void FormLoop::TwoSecondLoop()
{
	//Hello Alarm logic
	DoHelloAlarmLogic();
	DoTraffic();

	//send a hello to modules
    SendUDPMessage(helloFromAgIO, ethUDP.address, ethUDP.portToSend);
}

void FormLoop::DoTraffic()
{
	traffic.helloFromMachine++;
	traffic.helloFromAutoSteer++;
	traffic.helloFromIMU++;
    traffic.helloFromBlockage++;
    traffic.cntrGPSOut = 0;
}

void FormLoop::DoHelloAlarmLogic()
{
    bool currentHello;
    //agio = qmlItem(qml_root, "agio");

    if (isConnectedMachine)
    {
        currentHello = traffic.helloFromMachine < 3;

        //check if anything changed, IE if currentHello is greater than
        //3 or not
        if (currentHello != lastHelloMachine)
        {
            agio->setProperty("machineConnected", currentHello); //set qml

            if (currentHello)
                qDebug() << "Connected to machine";
            else qDebug() << "Not connected to machine";

            lastHelloMachine = currentHello;
            ShowAgIO();
        }
    }
    // traffic.helloFromMachine starts at 99
    //so this is triggered when a machine board is connected
    else if (traffic.helloFromMachine < 90)
        isConnectedMachine = true;

    if (isConnectedSteer)
    {
        currentHello = traffic.helloFromAutoSteer < 3;

        if (currentHello != lastHelloAutoSteer)
        {
            agio->setProperty("steerConnected", currentHello);

            if (currentHello) qDebug() << "Connected to steer";
            else qDebug() << "Not connected to steer";

            lastHelloAutoSteer = currentHello;

            ShowAgIO();
        }
    }
    else if (traffic.helloFromAutoSteer < 90)
        isConnectedSteer = true;

    if (isConnectedIMU)
    {
        currentHello = traffic.helloFromIMU < 3;

        if (currentHello != lastHelloIMU)
        {
            agio->setProperty("imuConnected", currentHello);

            if (currentHello) qDebug() << "Connected to IMU";
            else qDebug() << "Not connected to IMU";

            lastHelloIMU = currentHello;
            ShowAgIO();
        }
    }
    else if (traffic.helloFromIMU < 90)
        isConnectedIMU = true;

    if (isConnectedBlockage)
    {
        currentHello = traffic.helloFromBlockage < 3;

        if (currentHello != lastHelloBlockage)
        {
            agio->setProperty("blockageConnected", currentHello);

            if (currentHello) qDebug() << "Connected to Blockage";
            else
            {
                qDebug() << "Not connected to Blockage";
                isConnectedBlockage = false;
            }


            lastHelloBlockage = currentHello;
            ShowAgIO();
        }
    }
    else if (traffic.helloFromBlockage < 90)
        isConnectedBlockage = true;

    currentHello = traffic.cntrGPSOut != 0;

    //I suppose we could do the logic to not go red if
    //no GPS  was ever connected. But what's the point of AgIO if
    //no GPS is connected?
    if (currentHello != lastHelloGPS)
    {
        agio->setProperty("gpsConnected", currentHello);

        if (currentHello) qDebug() << "Connected to GPS";
        else qDebug() << "Not connected to GPS";

        lastHelloGPS = currentHello;

        ShowAgIO();
    }
}

