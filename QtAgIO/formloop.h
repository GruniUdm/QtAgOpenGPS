#ifndef FormLoop_H
#define FormLoop_H

#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkDatagram>
#include <QString>
#include <QByteArray>
#include "src/CTraffic.h"
#include "src/CScanReply.h"
#include <QUdpSocket>
#include <QQmlApplicationEngine>
#include "src/glm.h"
#include <cmath>
#include <QSettings>
#include "agiosettings.h"
#include <QTcpSocket>
#include <QTimer>
#include <QQueue>
#include <QDateTime>
#include <QElapsedTimer>
#include <QObject>
#include <QtSerialPort/QSerialPort>
//#include <QSerialPortInfo>
#include <QObject>



class FormUDP;
class BluetoothDeviceList;
class BluetoothManager;

//in cs, the ipEndPoint is both the ip address and port.
//cpp doesn't have that luxury, so we create a struct.
//we might as well put all 3 in here.
struct IPAndPort {
    QHostAddress address;
    int portToSend;
    int portToListen;

	IPAndPort() : address(""), portToSend(0), portToListen(0) {}; // Ajoutez un constructeur par défaut
    //IPAndPort(const QString &ip, int portToSend, int portToListen)
    //    : ip(ip), portToSend(portToSend), portToListen(portToListen) {}

    IPAndPort(const QString &ipAddress, int sendPort, int listenPort)
        : address(ipAddress), portToSend(sendPort), portToListen(listenPort) {}

};

class FormLoop : public QObject
{
    //This is added for future usage for Agio as C++ singleton plugin
	//  QML_ELEMENT
	Q_OBJECT
  //  QML_SINGLETON
   // Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)

	public:    
		void setContextProperty(const QString &name, const QVariant &value);
		QString status() const { return m_status; }
		void setStatus(const QString &value);

		// intialazing formloop as an instance
		static FormLoop* instance();
		// Initialize an engine pointer that it will be used by formloop instance
		void setEngine(QQmlApplicationEngine *engine);
		void setupGUI();
		// Getter to acess `m_engine` in formloop
		QQmlApplicationEngine* engine() const;  

    signals:
        void statusChanged();

    private:
	    explicit FormLoop(QObject *parent = nullptr);
        QString m_status;
        static FormLoop *m_instance;  // Store the singleton instance of `FormLoop`
        QQmlApplicationEngine *m_engine;  // Declare `m_engine` to store the engine reference

    	//~FormLoop();

		/* settings related stuff*/
		void loadSettings();


		//settings dialog callbacks
		void on_settings_reload();
		void on_settings_save();
		// end of settings related stuff

		QObject *qml_root;
        //QObject *agio; // moved to plubic:
        QWidget *qmlcontainer;

        IPAndPort wwwNtrip; //send ntrip to module
        IPAndPort ethUDP;//the main ethernet network
        IPAndPort ethModulesSet;

        quint16 sendNtripToModulePort = 2233; // sends to .255, so ip is the same as ethUDP

		//debug vars
        bool haveWeRecUDP = false;
        bool haveWeSentToParser = false;
		bool haveWeRecGGA = false;
		bool haveWeRecNDA = false;
		bool haveWeRecVTG = false;

        bool udpListenOnly;

        /*formloop_formUDP.cpp
         * formerly FormUDP.cs*/
    public slots:
        void btnSendSubnet_Click();
        void btnAutoSet_Click();


    public:

        QObject *agio; 
        //used to send communication check pgn= C8 or 200
        QByteArray sendIPToModules = QByteArray::fromRawData("\x80\x81\x7F\xC9\x05\xC9\xC9\xC0\xA8\x05\x47", 11);

        QByteArray ipCurrent;
        QByteArray ipNew;

        int tickCounter = 0;

        void SetFormLoop(FormLoop *formLoop);
        void FormUDp_Load();

        void ScanNetwork();

        void nudFirstIP_Click();

        /*formloop_ui.cpp*/


        //void setupGUI();
        void ShowAgIO();
		void UpdateUIVars();
        void TimedMessageBox(int timeout, QString s1, QString s2);

		/*formloop.cpp
		 * formerly Formloop.cs */
        int nmeaErrCounter = 0;

        bool isGPSSentencesOn = true, isSendNMEAToUDP;
        int focusSkipCounter = 310;
        bool isSendToSerial = false, isSendToUDP = true;

        bool lastHelloGPS, lastHelloAutoSteer, lastHelloMachine, lastHelloIMU, lastHelloBlockage;
        bool isConnectedIMU, isConnectedSteer, isConnectedMachine = true, isConnectedBlockage = true;

        int packetSizeNTRIP = 256;

        QTimer *ntripMeterTimer;

        QTimer *oneSecondTimer;

		QTimer *twoSecondTimer;

        QTimer *halfSecondTimer;

		void DoHelloAlarmLogic();
		void DoTraffic();
    private:
        int loopListenPort;
        int loopSendPort;

    public:
        void SendGPSPort(QByteArray byteData);
        void openSerialPort(QString comName, int baud);
        void writeToSerialPort(QString message);
        bool getConnectionStatus();
        void closeSerialPort();

    public slots:

        void openSerialPortSlot(QString comName, int baud);
        void writeToSerialPortSlot(QString message);
        void readFromSerialPort();
        bool getConnectionStatusSlot();
        void closeSerialPortSlot();

    private:
        QSerialPort *serialPort;

    signals:

        QString getData(QString data);


    public slots:
        void timer1_Tick();
        void oneSecondLoopTimer_Tick();
	    void TwoSecondLoop();
        void LookupNTripIP(QString url);
        void btnUDPListenOnly_Click(bool isIt);


		/* formloop_udpcomm.cpp
		 * formerly UDP.designer.cs */
	public:
		bool isUDPNetworkConnected;

		//QByteArray ipAutoSet = "192.168.5";
        QByteArray ipAutoSet;


		//set up the networks
		void LoadUDPNetwork();
		void LoadLoopback();

        void RestartUDPSocket();

		//send
        void SendUDPMessage(QByteArray byteData, QHostAddress address, uint portNumber);
		void SendDataToLoopBack(QByteArray byteData);

		QUdpSocket *udpSocket;
		QUdpSocket *loopBackSocket;
		CTraffic traffic;
		CScanReply scanReply;

	private:

       //QQmlApplicationEngine engine;

        QByteArray helloFromAgIO = QByteArray::fromRawData("\x80\x81\x7F\xC8\x03\x38\x00\x00\x47", 9);


		public slots:

			void ReceiveFromLoopBack();
		void ReceiveFromUDP();
		//end of formloop_udpcomm.cpp


		/* formloop_parseNMEA.cpp
		 * formerly NMEA.designer.cs */
	private:

        double nowHz;
        double gpsHz;
		QString rawBuffer = 0;

		QStringList words;
		QString nextNMEASentence = 0;

		bool isNMEAToSend = false;

	public:
        QElapsedTimer swFrame;
		QString ggaSentence, vtgSentence, hdtSentence, avrSentence, paogiSentence,
                hpdSentence, rmcSentence, pandaSentence, ksxtSentence, unknownSentence;

        float hdopData, altitude = glmAgIO::FLOAT_MAX, headingTrue = glmAgIO::FLOAT_MAX,
              headingTrueDual = glmAgIO::FLOAT_MAX, speed = glmAgIO::FLOAT_MAX, roll = glmAgIO::FLOAT_MAX;
		float altitudeData, speedData, rollData, headingTrueData, headingTrueDualData, ageData;

        double latitudeSend = glmAgIO::DOUBLE_MAX, longitudeSend = glmAgIO::DOUBLE_MAX, latitude, longitude;

        float previousAltitude = 0;
        bool nmeaError = false;

        unsigned short satellitesData, satellitesTracked = glmAgIO::USHORT_MAX, hdopX100 = glmAgIO::USHORT_MAX, ageX100 = glmAgIO::USHORT_MAX;


		//imu data
        unsigned short imuHeadingData, imuHeading = glmAgIO::USHORT_MAX;
        short imuRollData, imuRoll = glmAgIO::SHORT_MAX, imuPitchData, imuPitch = glmAgIO::SHORT_MAX,
              imuYawRateData, imuYawRate = glmAgIO::SHORT_MAX;

		quint8 fixQuality, fixQualityData = 255;


		float rollK, Pc, G, Xp, Zp, XeRoll, P = 1.0f;
		const float varRoll = 0.1f, varProcess = 0.0003f;

		double LastUpdateUTC = 0;

		QString FixQuality();
        QString Parse(QString& buffer);

        void ParseNMEA(QString& buffer);
		void ParseKSXT();
		void ParseGGA();
		void ParseVTG();
		void ParseHDT();
		void ParseAVR();
		void ParseOGI();
		void ParseHPD();
		void ParseRMC();
		void ParsePANDA();
		void ParseSTI032();
		void ParseTRA();

		bool ValidateChecksum(QString Sentence);

		/* end of formloop_parseNMEA.cpp */

		/* formloop_ntripcomm.cpp
		 * formerly NTRIPComm.designer.cs */
	public:

		QString brandCasterIP;
        bool isNTRIP_RequiredOn = false; //these are all settings-derived vars. Set in ntripcomm
		bool isNTRIP_Connected = false;
		bool isNTRIP_Starting = false;
		bool isNTRIP_Connecting = false;
		bool isNTRIP_Sending = false;
		bool isRunGGAInterval = false;
		bool isRadio_RequiredOn = false;
		bool isSerialPass_RequiredOn = false;

		unsigned int tripBytes = 0;

		void StartNTRIP();
        void OnAddMessage(QByteArray data);
        void SendNTRIP(QByteArray data);
		void SendGGA();
		void OnConnect();
		void OnReceivedData();
		QString CalculateChecksum(QString Sentence);
	private:
		//for the NTRIP Client counting
		int ntripCounter = 10;
        bool debugNTRIP = false;

        QTcpSocket *clientSocket; //server connection   line 21
        QByteArray casterRecBuffer; //received data buffer

        QByteArray trip;

        QTimer *tmr; //send gga back timer line 25

		QString mount;
		QString username;
		QString password;


		int broadCasterPort;

		int sendGGAInterval = 0;
		QString GGASentence;

		int toUDP_Port = 0;
		int NTRIP_Watchdog = 100;

		   QList<int> rList;
		   QList<int> aList;

		//NTRIP metering
		QQueue<quint8> rawTrip;

		QString sbGGA; //string builder

		//forward declarations
		void DoNTRIPSecondRoutine();
		void ReconnectRequest();
		void IncrementNTRIPWatchDog();
		void SendAuthorization();
        void ntripMeterTimer_Tick();
		void NtripPort_DataReceived();
		void ShutDownNTRIP();
		void SettingsShutDownNTRIP();
		void BuildGGA();
		QString ToBase64(QString str);
    public slots:
        void ResolveNTRIPURL(const QHostInfo &hostInfo);
    private slots:
        void ConfigureNTRIP();
        void NTRIPDebugMode(bool doWeDebug);

    private:
        BluetoothManager *bluetoothManager;
    public:
        BluetoothDeviceList *btDevicesList;// for sending the list to frontend
};


#endif
