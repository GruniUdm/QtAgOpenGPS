#include "formloop.h"
#include "agioproperty.h"
#include <QHostInfo>

//set up connection to caster
// main routine
void FormLoop::DoNTRIPSecondRoutine()
{
    agio->setProperty("ntripCounter", ntripCounter);

	//count up the ntrip clock only if everything is alive
	if (isNTRIP_RequiredOn || isRadio_RequiredOn || isSerialPass_RequiredOn)
	{
		IncrementNTRIPWatchDog();
	}

	//Have we NTRIP connection?
    //start if it is required
	if (isNTRIP_RequiredOn && !isNTRIP_Connected && !isNTRIP_Connecting)
	{
		if (!isNTRIP_Starting && ntripCounter > 20)
		{
			StartNTRIP();
		}
	}

    /*radio and serial
	//check if we need to send...?
	if ((isRadio_RequiredOn || isSerialPass_RequiredOn) && !isNTRIP_Connected && !isNTRIP_Connecting)
	{
		if (!isNTRIP_Starting)
		{
			StartNTRIP();
		}
    }*/

	if (isNTRIP_Connecting)
	{
		if (ntripCounter > 29)
		{
			qDebug() << "Connection Problem. Not Connecting To Caster";
            TimedMessageBox(3000, tr("Connection Problem"), tr("Not Connecting To Caster"));
			ReconnectRequest();
		}
        if (clientSocket != NULL && clientSocket->state() == QAbstractSocket::ConnectedState)
		{
			SendAuthorization();
		}
	}

	if (isNTRIP_RequiredOn || isRadio_RequiredOn)
	{

		//Bypass if sleeping
        if (focusSkipCounter != 0)//I suspect this is cs leftovers... Skip if AgIO is not in
        //"focus", or visible. However, I want the debug vars for now
		{

            /*
             * qml "ntripStatus" :
             * 0 = invalid
             * 1 = Authorizing
             * 2 = Waiting
             * 3 = Send GGA
             * 4 = Listening NTRIP
             * 5 = Wait GPS
             */
			//watchdog for Ntrip
            if (isNTRIP_Connecting){
                agio->setProperty("ntripStatus", 1);//authorizing
            }
            else
			{
                if (isNTRIP_RequiredOn && NTRIP_Watchdog > 10){
                    agio->setProperty("ntripStatus", 2);//waiting
                }
                else agio->setProperty("ntripStatus", 4); //listening GGA

			}

			if (sendGGAInterval > 0 && isNTRIP_Sending)
			{
                agio->setProperty("ntripStatus", 3);// Send GGA
                isNTRIP_Sending = false;
			}

		}
	}
}

void FormLoop::ConfigureNTRIP() //set the variables to the settings
{
    if(debugNTRIP) qDebug() << "Configuring NTRIP";
    agio->setProperty("ntripStatus", 5); //Wait GPS
	aList.clear();
	rList.clear();

	//start NTRIP if required
    isNTRIP_RequiredOn = property_setNTRIP_isOn;
    isRadio_RequiredOn = property_setRadio_isOn;
    isSerialPass_RequiredOn = property_setSerialPass_isOn;

	/*I'm not worrying about either one for now
	 * if (isRadio_RequiredOn || isSerialPass_RequiredOn)
	{
		// Immediatly connect radio
		ntripCounter = 20;
	}*/
}

void FormLoop::StartNTRIP()
{
    if(debugNTRIP) qDebug() << "Starting NTRIP";
	if (isNTRIP_RequiredOn)
	{
		//load the settings
        wwwNtrip.portToSend = property_setNTRIP_casterPort; //Select correct port (usually 80 or 2101)
        mount = property_setNTRIP_mount; //Insert the correct mount
        username = property_setNTRIP_userName; //insert your username!
        password = property_setNTRIP_userPassword; //Insert your password!
        toUDP_Port = property_setNTRIP_sendToUDPPort; //send rtcm to which udp port
        sendGGAInterval = property_setNTRIP_sendGGAInterval; //how often to send fixes

		//if we had a timer already, kill it
        if (tmr)
		{
            tmr->stop();
		}

		//create new timer at fast rate to start
		if (sendGGAInterval > 0)
		{
            tmr = new QTimer(this);
            tmr->setSingleShot(false);
            tmr->setInterval(5000);
            connect(tmr, &QTimer::timeout, this, &FormLoop::SendGGA);
            tmr->start();


		}

			// Close the socket if it is still open
            if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState)
            {
                clientSocket->disconnectFromHost();

                clientSocket->close();
            }

            //NTRIP caster
            QString ipAddress = property_setNTRIP_ipAddress;

            wwwNtrip.address.setAddress(ipAddress);


            //this is the socket that sends to the receiver
			// Create the socket object
            if(debugNTRIP) qDebug() << "Creating new tcp socket";
			clientSocket = new QTcpSocket(this);

			//set socket to non-blocking mode
            clientSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

			// Connect to server
            if(debugNTRIP) qDebug() << "Connecting to server... Connecting";
            clientSocket->connectToHost(wwwNtrip.address, wwwNtrip.portToSend);
            connect(clientSocket, &QTcpSocket::readyRead, this, &FormLoop::OnReceivedData);

		isNTRIP_Connecting = true;
	}
    /*radio
	 * If someone wants to attempt this, go ahead. However, I don't think many
	 * if anyone uses it. 
	 * I think it is for a radio connected to AgIO over usb, not a radio plugged
	 * into the receiver. David
	 * Later: Maybe a plugin or extension for this
	else if (isRadio_RequiredOn)
	{
		if (!string.IsNullOrEmpty(Properties.Settings.Default.setPort_portNameRadio))
		{
			// Disconnect when already connected
			if (spRadio != NULL)
			{
				spRadio.Close();
				spRadio.Dispose();
			}

			// Setup and open serial port
			spRadio = new SerialPort(Properties.Settings.Default.setPort_portNameRadio);
			spRadio.BaudRate = int.Parse(Properties.Settings.Default.setPort_baudRateRadio);
			spRadio.DataReceived += NtripPort_DataReceived;
			isNTRIP_Connecting = false;
			isNTRIP_Connected = true;

			try
			{
				spRadio.Open();
			}
			catch (...)
			{
				isNTRIP_Connecting = false;
				isNTRIP_Connected = false;
				isRadio_RequiredOn = false;

				TimedMessageBox(2000, "Error connecting to radio", $"{ex.Message}");
			}
		}
	}*/

	/*
	 * forget about serial for right now. David
	else if (isSerialPass_RequiredOn)
	{
		toUDP_Port = Properties.Settings.Default.setNTRIP_sendToUDPPort; //send rtcm to which udp port
		epNtrip = new IPEndPoint(IPAddress.Parse(
					Properties.Settings.Default.etIP_SubnetOne.toString() + "." +
					Properties.Settings.Default.etIP_SubnetTwo.toString() + "." +
					Properties.Settings.Default.etIP_SubnetThree.toString() + ".255"), toUDP_Port);

		if (!string.IsNullOrEmpty(Properties.Settings.Default.setPort_portNameRadio))
		{
			// Disconnect when already connected
			if (spRadio != NULL)
			{
				spRadio.Close();
				spRadio.Dispose();
			}

			// Setup and open serial port
			spRadio = new SerialPort(Properties.Settings.Default.setPort_portNameRadio);
			spRadio.BaudRate = int.Parse(Properties.Settings.Default.setPort_baudRateRadio);
			spRadio.DataReceived += NtripPort_DataReceived;
			isNTRIP_Connecting = false;
			isNTRIP_Connected = true;
			//lblWatch.Text = "RTCM Serial";


			try
			{
				spRadio.Open();
			}
			catch (...)
			{
				isNTRIP_Connecting = false;
				isNTRIP_Connected = false;
				isSerialPass_RequiredOn = false;

				TimedMessageBox(2000, "Error connecting to Serial Pass", $"{ex.Message}");
			}
		}
	}*/
}

void FormLoop::ReconnectRequest()
{
    TimedMessageBox(2000, "NTRIP Not Connected", " Reconnect Request");
    qDebug() << "NTRIP Not Connected. Reconnect Request";
    QString url = property_setNTRIP_url;
    QHostInfo::lookupHost(url, this, SLOT(ResolveNTRIPURL(QHostInfo)));

	ntripCounter = 15;
	isNTRIP_Connected = false;
    agio->setProperty("ntripConnected", false);
    ShowAgIO();
	isNTRIP_Starting = false;
	isNTRIP_Connecting = false;

	//if we had a timer already, kill it
    if (tmr != nullptr)
	{
        tmr->stop();
	}
}

void FormLoop::IncrementNTRIPWatchDog()
{
	//increment once every second
	ntripCounter++;

	//Thinks is connected but not receiving anything
	if (NTRIP_Watchdog++ > 30 && isNTRIP_Connected) 
		ReconnectRequest();

	//Once all connected set the timer GGA to NTRIP Settings
    if (sendGGAInterval > 0 && ntripCounter == 40) tmr->setInterval(sendGGAInterval * 1000);
}

void FormLoop::SendAuthorization()
{
    if(debugNTRIP) qDebug() << "Attempting to send Athorization";
	// Check we are connected
    if (clientSocket == NULL || clientSocket->state() != QAbstractSocket::ConnectedState)
	{
        qDebug() << "SendAuthorization isn't connected. Reconnecting";
		ReconnectRequest();
		return;
	}

    // Read the message from settings and send it
    if (!property_setNTRIP_isTCP)//if we are not using TCP. Should that go in an extension also?
    //a sort of "ntrip weirdos" for all the unusual stuff to go.
    {
        //encode user and password
        QString auth = ToBase64(username + ":" + password);

        //grab location sentence
        BuildGGA();
        GGASentence = sbGGA;

        QString htt;
        if (property_setNTRIP_isHTTP10) htt = "1.0";
        else htt = "1.1";

        //Build authorization string
        QString str = "GET /" + mount + " HTTP/" + htt + "\r\n";
        str += "User-Agent: NTRIP AgOpenGPSClient/20221020\r\n";
        str += "Authorization: Basic " + auth + "\r\n"; //This line can be removed if no authorization is needed
        str += "Accept: */*\r\nConnection: close\r\n";
        str += "\r\n";

        QByteArray byteDateLine = str.toLatin1();

        if(clientSocket->write(byteDateLine, byteDateLine.length())){
            if(debugNTRIP)
                qDebug() << "NTRIP wrote to clientSocket";
        }
        else
            qDebug() << "NTRIP: Failed to write to clientSocket"
                     << "Error:" << clientSocket->errorString();  ;



        //enable to periodically send GGA sentence to server.
        if (sendGGAInterval > 0) tmr->start();

    }
    //say its connected
    isNTRIP_Connected = true;
    agio->setProperty("ntripConnected", true);
    isNTRIP_Starting = false;
    isNTRIP_Connecting = false;
}

void FormLoop::OnAddMessage(QByteArray data)
{

    //update gui with stats
    tripBytes += (uint)data.length();
    agio->setProperty("tripBytes", tripBytes);
    //reset watchdog since we have updated data
	NTRIP_Watchdog = 0;

    if (isNTRIP_RequiredOn)
	{
		//move the ntrip stream to queue
        for (int i = 0; i < data.length(); i++)
		{
            rawTrip.enqueue(data[i]);
		}
        //qDebug() << "RawTrip: " << rawTrip;

        ntripMeterTimer->start();
	}
	else
    {
		//send it
        //qDebug() << "onadd";
		SendNTRIP(data);
    }


}

void FormLoop::ntripMeterTimer_Tick()
{
	//we really should get here, but have to check
    if (rawTrip.size() == 0) return;

	//how many bytes in the Queue
    int cnt = rawTrip.size();

	//how many sends have occured
	traffic.cntrGPSIn++;

	//128 bytes chunks max
	if (cnt > packetSizeNTRIP) cnt = packetSizeNTRIP;

	//new data array to send
    QByteArray trip(cnt, 0);

	traffic.cntrGPSInBytes += cnt;

	//dequeue into the array
    for (int i = 0; i < cnt; i++) trip[i] = rawTrip.dequeue();

	//send it
	SendNTRIP(trip);

	//Are we done?
    if (rawTrip.size() == 0)
	{
        ntripMeterTimer->stop();

		if (focusSkipCounter != 0)
			traffic.cntrGPSInBytes = 0;
	}

	//Can't keep up as internet dumped a shit load so clear
    if (rawTrip.size() > 10000) {
        rawTrip.clear();
    }

    agio->setProperty("rawTripCount", rawTrip.count()); // tell the UI
}

void FormLoop::SendNTRIP(QByteArray data)
{
	//serial send out GPS port
	/* don't worry about serial
	 *
	if (isSendToSerial)
	{
		SendGPSPort(data);
	}*/

	//send out UDP Port
	if (isSendToUDP)
	{
        SendUDPMessage(data, ethUDP.address, sendNtripToModulePort);
        if(debugNTRIP) qDebug() << "NTRIP: Sending data with size " << data.size() << " to modules UDP network";
	}
}

void FormLoop::SendGGA()
{
	//timer may have brought us here so return if not connected
	if (!isNTRIP_Connected)
		return;
	// Check we are connected
    if (clientSocket == NULL || clientSocket->state() != QAbstractSocket::ConnectedState)
	{
		ReconnectRequest();
		return;
	}

	// Read the message from the text box and send it
		isNTRIP_Sending = true;
		BuildGGA();
        //I'm not sure about this. sbGGA is a stringbuilder in cs, however, I think it's a
        //regular qstring in qt.
		//or a QStringList?
        //QString str = sbGGA.toString();
        QString str = sbGGA;

        QByteArray byteDateLine = str.toLatin1();
        if(clientSocket->write(byteDateLine, byteDateLine.length())){
            if(debugNTRIP) qDebug() << "Sending GGA to caster";
        }else{
            TimedMessageBox(3000, tr("NTRIP Error"), tr("Failed to send GGA to caster. Reconnecting"));
            qDebug() << "Failed to send GGA to caster. Reconnecting";
            ReconnectRequest();
        }
}


 void FormLoop::OnReceivedData() //where we listen
{
	// Check if we got any data
    while (clientSocket->bytesAvailable() > 0){//not yet finished
        casterRecBuffer.resize(clientSocket->bytesAvailable());
        clientSocket->read(casterRecBuffer.data(), casterRecBuffer.size());

        qint64 nBytesRec = casterRecBuffer.size();
        //if(debugNTRIP) qDebug() << "Recieved NTRIP data: " << byteData << " Size: " << nBytesRec;

		if (nBytesRec > 0)
		{
            QByteArray localMsg;
            localMsg.resize(nBytesRec);
            localMsg.append(casterRecBuffer, nBytesRec);

            OnAddMessage(localMsg);
		}
		else
		{
			// If no data was recieved then the connection is probably dead
            qDebug() << "Shutting down clientSocket as we got no data";
            TimedMessageBox(3000, tr("NTRIP Error"), tr("Shutting down clientSocket as we got no data"));
            clientSocket->close();
		}
	}
}

void FormLoop::NtripPort_DataReceived()//this is the serial port, right?
{
	// Check if we got any data
	/*try
	{
		SerialPort comport = (SerialPort)sender;
		if (comport.BytesToRead < 32) 
			return;

		int nBytesRec = comport.BytesToRead;

		if (nBytesRec > 0)
		{
			byte[] localMsg = new byte[nBytesRec];
			comport.Read(localMsg, 0, nBytesRec);

			BeginInvoke((MethodInvoker)(() => OnAddMessage(localMsg)));
		}
		else
		{
			// If no data was recieved then the connection is probably dead
			// TODO: What can we do?
		}
	}
	catch (...)
	{
		qDebug() << "Unusual error druing Recieve!";
	}*/
	//do we need to return??
}

 QString FormLoop::ToBase64(QString str)
{
    // Get the ASCII-encoded QByteArray
    QByteArray byteArray = str.toLatin1(); // Latin-1 is equivalent to ASCII for plain English characters

    // Convert to Base64
    QString base64String = byteArray.toBase64();

    return base64String;
}

void FormLoop::ShutDownNTRIP()
{
    //qDebug() << "ShutDownNTRIP";
    if (clientSocket != NULL && clientSocket->state() == QAbstractSocket::ConnectedState)
	{
		//shut it down
		clientSocket->disconnectFromHost();
		if (clientSocket->state() != QAbstractSocket::UnconnectedState) {
			clientSocket->waitForDisconnected(3000); // Optional: Wait for up to 3 seconds to ensure the socket is disconnected
		}
		clientSocket->close();
        //start it up again
		ReconnectRequest();

		//Also stop the requests now
		isNTRIP_RequiredOn = false;
	}
	/*
	else if(spRadio != NULL)
	{
		spRadio.Close();
		spRadio.Dispose();
		spRadio = null;

		ReconnectRequest();

		//Also stop the requests now
		isRadio_RequiredOn = false;
	}*/
}

void FormLoop::SettingsShutDownNTRIP()
{
    if (clientSocket != NULL && clientSocket->state() == QAbstractSocket::ConnectedState)
    {
        clientSocket->disconnectFromHost();
        if (clientSocket->state() != QAbstractSocket::UnconnectedState) {
            clientSocket->waitForDisconnected(3000); // Optional: Wait for up to 3 seconds to ensure the socket is disconnected
        }
        clientSocket->close();
        ReconnectRequest();
    }

	/*
	if (spRadio != NULL && spRadio.IsOpen)
	{
		spRadio.Close();
		spRadio.Dispose();
		spRadio = NULL;
		ReconnectRequest();
	}*/
}

//calculate the NMEA checksum to stuff at the end
QString FormLoop::CalculateChecksum(QString Sentence)
{
	int sum = 0, inx;

    QVector<QChar> sentence_chars(Sentence.begin(), Sentence.end());
    QChar tmp;

	// All character xor:ed results in the trailing hex checksum
	// The checksum calc starts after '$' and ends before '*'
	for (inx = 1; ; inx++)
	{
		tmp = sentence_chars[inx];

		// Indicates end of data and start of checksum
		if (tmp == '*')
			break;
        //sum ^= tmp;    // Build checksum
        sum ^= tmp.unicode();    // Build checksum
    }

	// Calculated checksum converted to a 2 digit hex string
    //return String.Format("{0:X2}", sum);
    return QString::asprintf("%02X", sum);

}

//private readonly StringBuilder sbGGA = new StringBuilder();
//moved to formloop.h line 195

void FormLoop::BuildGGA()
{
	double latitude = 0;
	double longitude = 0;

    if (property_setNTRIP_isGGAManual)
	{
        latitude = property_setNTRIP_manualLat;
        longitude = property_setNTRIP_manualLon;
	}
	else
	{
        //latitude = this.latitude;
        latitude = this->latitude; //correct??
        longitude = this->longitude;
	}

	//convert to DMS from Degrees
	double latMinu = latitude;
	double longMinu = longitude;

	double latDeg = (int)latitude;
	double longDeg = (int)longitude;

	latMinu -= latDeg;
	longMinu -= longDeg;

	latMinu = qRound(latMinu * 60 * 1000000) / 1000000.0;
	longMinu = qRound(longMinu * 60 * 1000000) / 1000000.0;

	latDeg *= 100.0;
	longDeg *= 100.0;

	double latNMEA = latMinu + latDeg;
	double longNMEA = longMinu + longDeg;

	char NS = 'W';
	char EW = 'N';
	if (latitude >= 0) NS = 'N';
	else NS = 'S';
	if (longitude >= 0) EW = 'E';
	else EW = 'W';

	sbGGA.clear();
    sbGGA.append("$GPGGA,");
	sbGGA.append(QDateTime::currentDateTime().toString("HHmmss.00,"));
    sbGGA.append(QString::number(abs(latNMEA), 'f', 3)).append(',').append(NS).append(',');
	sbGGA.append(QString::number(abs(longNMEA), 'f', 3)).append(',').append(EW);
	sbGGA.append(",").append(QString::number(fixQualityData)).append(',');
	sbGGA.append(QString::number(satellitesData)).append(',');

	if (hdopData > 0) sbGGA.append(QString::number(hdopData, 'f', 2)).append(',');

    else sbGGA.append("1,");

	sbGGA.append(QString::number(altitudeData, 'f', 3)).append(',');
    sbGGA.append("M,");
    sbGGA.append("46.4,M,");  //udulation
    sbGGA.append(QString::number(ageData, 'f', 1)).append(','); //age
    sbGGA.append("0*");

    sbGGA.append(CalculateChecksum(sbGGA));
    sbGGA.append("\r\n");
	/*
	   $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,5,0*47
	   0     1      2      3    4      5 6  7  8   9    10 11  12 13  14
	   Time      Lat       Lon     FixSatsOP Alt */
}

void FormLoop::NTRIPDebugMode(bool doWeDebug){
    debugNTRIP = doWeDebug;
    qDebug() << "Debug mode is now" << (debugNTRIP ? "enabled" : "disabled");
}

void FormLoop::ResolveNTRIPURL(const QHostInfo &hostInfo) {
    if (hostInfo.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed:" << hostInfo.errorString();
        TimedMessageBox(3000, tr("NTRIP Failure"), tr("URL lookup failed: ") + hostInfo.errorString());
        return;
    }

    foreach (const QHostAddress &address, hostInfo.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            qDebug() << "IP Address:" << address.toString();
            property_setNTRIP_ipAddress = address.toString();
        }
    }
}
