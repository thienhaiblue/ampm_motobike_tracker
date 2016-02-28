using System;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Collections;

namespace Motor_Tracker_Configuration
{
    #region Framing struct
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct USER_INFO
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string userName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string phoneNo;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct GPS_CONFIG_DATA
    {
        // tracker info
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 18)]
        public string imei;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 18)]
        public string trackerId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string pass;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string whiteCallerNo;

        // gprs settings
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string gprsApn;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string gprsUser;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string gprsPass;
        public UInt16 runReportInterval;
        public UInt16 stopReportInterval;

        // data server
        public byte currentDataServer;
        // primary
        public UInt32 priDataServerIp;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string priDataServerDomain;
        public UInt16 priDataServerPort;
        // secondary
        public UInt32 secDataServerIp;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string secDataServerDomain;
        public UInt16 secDataServerPort;
        public byte dataServerUseIp;

        // firmware server
        public byte currentFirmwareServer;
        // primary
        public UInt32 priFirmwareServerIp;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string priFirmwareServerDomain;
        public UInt16 priFirmwareServerPort;
        // secondary
        public UInt32 secFirmwareServerIp;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string secFirmwareServerDomain;
        public UInt16 secFirmwareServerPort;
        public byte firmwareServerUseIp;

		// info server
		public byte currentInfoServer;
		// primary
		public UInt32 priInfoServerIp;
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
		public string priInfoServerDomain;
		public UInt16 priInfoServerPort;
		// secondary
		public UInt32 secInfoServerIp;
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
		public string secInfoServerDomain;
		public UInt16 secInfoServerPort;
		public byte infoServerUseIp;

        // user info
        public byte currentUser;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
        public USER_INFO[] userList;

        // vehicle info
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
        public string plateNo;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 20)]
        public string vin;
        public UInt16 speedSensorRatio;
        public UInt16 useGpsSpeed;

        // analog settings
        public UInt16 a1Mode;
        public UInt16 a1LowerBound;
        public UInt16 a1UpperBound;
        public UInt16 a2Mode;
        public UInt16 a2LowerBound;
        public UInt16 a2UpperBound;

        // other settings
        public UInt16 speedLimit;
        public UInt32 drivingTimeLimit;
        public UInt32 totalDrivingTimeLimit;
        public UInt32 baseMileage;
        
        public UInt16 featureSet;

        public byte securityOn;
        public byte securityAction;
        public byte SOSKeyPressTimes;
        public byte SOSKeyPressPeriod;

        public UInt32 removePowerWarningPeriod;
        public UInt32 batteryLowWarningPeriod;
        public byte powerLowWarningLevel;
        public byte batteryLowWarningLevel;

        public UInt32 autoSecurityTime;
        public UInt32 speedEngineOff;
        public UInt32 powerOffDelayTime;
		public byte accountAlarmCheck;

        UInt32 crc;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct SMS_RECORD
    { 
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst=16)]
        public string smsAddress;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst=16)]
        public string smsMessage;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct DATE_TIME
    {
        public UInt16 year;   // year with all 4-digit (2011)
        public byte month;   // month 1 - 12 (1 = Jan)
        public byte mday;   // day of month 1 - 31
        public byte wday;   // day of week 1 - 7 (1 = Sunday)
        public byte hour;   // hour 0 - 23
        public byte min;    // min 0 - 59
        public byte sec;    // sec 0 - 59
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct SPEED_RECORD
    {
        public DATE_TIME currentTime; //8 thoi gian hien tai
        public float lat;//12 kinh do
        public float lon;//16 vi do
        public float hdop;//20 sai so trong don vi met
        public float speed;//24 toc do
        public float dir;//28 huong di chuyen
        public float mileage; //32 so km hien thi tren dong ho do cua xe
        public float speedFreq;//36 tan so (dung cho cam bien xang)
        public UInt32 drivingTime; //38  thoi gian lai xe lien tuc
        public UInt32 totalDrivingTime;//42 tong thoi gian lai xe trong ngay
        public UInt16 IOStatus;//46  Trang thai cua cac IO nhu DOOR, AIRCON,Engine
        public UInt16 adc1Value;//48 Gia tri cua cam bien 1
        public UInt16 adc2Value;//50 Gia tri cua cam bien 2
        public UInt16 overSpeedCnt;//52 Toc do gioi han cua xe
        public UInt16 parkCnt;//54 so dan dung do xe
        public UInt16 openDoorCnt;//56 so lan dong mo cua xe
        public UInt16 onOffEngineCnt;//58 so lan tat mo may xe
        public UInt16 parkingTime;//60 thoi gian dau xe 
        public byte user;//61 ID cua tai xe
        public byte ew;//62 huong 'E' hoac "W'
        public byte ns;//63 huong 'N' hoac 'S'
        public byte crc;//64
    }

    #endregion

    public class IndividualReport
    {
        List<SPEED_RECORD> reportList;

        public IndividualReport()
        {
            reportList = new List<SPEED_RECORD>();
        }

        public void AddReport(SPEED_RECORD report)
        {
            reportList.Add(report);
        }

        public void AddReport(byte[] reportBytes)
        {
            SPEED_RECORD report;

            report = (SPEED_RECORD)Lib.ByteArrayToObject(reportBytes, typeof(SPEED_RECORD));
            AddReport(report);
        }

        public int GetReportCount()
        {
            return (reportList.Count);
        }

        public DateTime GetReportTime(SPEED_RECORD report)
        {
            return (new DateTime(report.currentTime.year, report.currentTime.month, report.currentTime.mday, 
                                    report.currentTime.hour, report.currentTime.min, report.currentTime.sec));
        }

        public void ExportToKML(GPS_CONFIG_DATA cfg, string path)
        {
            StreamWriter sw;

            if ((reportList == null) || (reportList.Count == 0))
            {
                return;
            }

            sw = new StreamWriter(path);
            sw.WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>");
            sw.WriteLine("<kml>");
            sw.WriteLine("<Document>");
            sw.WriteLine("<Style id=\"s_ylw-pushpin\"><IconStyle><scale>1.1</scale><Icon><href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href></Icon><hotSpot x=\"20\" y=\"2\" xunits=\"pixels\" yunits=\"pixels\"/></IconStyle></Style>");
            sw.WriteLine("<Style id=\"s_blue-pushpin\"><IconStyle><scale>1.1</scale><Icon><href>https://maps.google.com/mapfiles/kml/pushpin/blue-pushpin.png</href></Icon><hotSpot x=\"20\" y=\"2\" xunits=\"pixels\" yunits=\"pixels\"/></IconStyle></Style>");
            sw.WriteLine("<Style id=\"s_ylw-pushpin_hl\"><IconStyle><scale>1.3</scale><Icon><href>http://maps.google.com/mapfiles/kml/pushpin/red-pushpin.png</href></Icon><hotSpot x=\"20\" y=\"2\" xunits=\"pixels\" yunits=\"pixels\"/></IconStyle></Style>");
            sw.WriteLine("<StyleMap id=\"m_ylw-pushpin\"><Pair><key>normal</key><styleUrl>#s_ylw-pushpin</styleUrl></Pair><Pair><key>highlight</key><styleUrl>#s_ylw-pushpin_hl</styleUrl></Pair><Pair><key>highlight1</key><styleUrl>#s_blue-pushpin</styleUrl></Pair></StyleMap>");

            foreach (SPEED_RECORD report in reportList)
            {
                sw.WriteLine("<Placemark>");
                sw.WriteLine("<name>Biển số xe : " + cfg.plateNo + "</name>");
                sw.WriteLine("<description>");
                sw.WriteLine("Thời gian : " + GetReportTime(report).ToString("HH:mm:ss dd/MM/yyyy").Replace('-', '/') + "<br/>");
                sw.WriteLine("Lái xe : " + cfg.userList[report.user].userName + "<br/>");
                sw.WriteLine("Biển số : " + cfg.plateNo + "<br/>");
                sw.WriteLine("Vận tốc : " + report.speed + "<br/>");
                sw.WriteLine("Mileage : " + report.mileage + "br/>");
                sw.WriteLine("HDOP : " + report.hdop + "br/>");
                sw.WriteLine("Tần số : " + report.speedFreq + "br/>");
                sw.WriteLine("Thời gian lái xe : " + report.drivingTime + "br/>");
                sw.WriteLine("Tổng thời gian lái xe : " + report.totalDrivingTime + "br/>");
                sw.WriteLine("Số lần đóng/mở cửa : " + report.openDoorCnt + "<br/>");
                sw.WriteLine("Số lần vượt quá tốc độ : " + report.overSpeedCnt + "<br/>");
                sw.WriteLine("Số lần dừng đỗ : " + report.parkCnt + "<br/>");
                sw.WriteLine("</description>");
                sw.WriteLine("<styleUrl>#s_ylw-pushpin</styleUrl>");
                sw.WriteLine("<Point>");
                sw.WriteLine("<coordinates>" + Lib.latlngToString(report.lon) + "," + Lib.latlngToString(report.lat) + "</coordinates>");
                sw.WriteLine("</Point>");
                sw.WriteLine("</Placemark>");
            }

            sw.WriteLine("<Placemark>");
            sw.WriteLine("<color>ff00ffff</color>");
            sw.WriteLine("<width>6</width>");
            sw.WriteLine("<MultiGeometry>");
            sw.WriteLine("<LineString>");
            sw.WriteLine("<coordinates>");
            foreach (SPEED_RECORD report in reportList)
            {
                if ((report.lat != 0) || (report.lon != 0))
                {
                    sw.Write(Lib.latlngToString(report.lon) + "," + Lib.latlngToString(report.lat) + " ");
                }
            }
            sw.WriteLine("</coordinates>");
            sw.WriteLine("</LineString>");
            sw.WriteLine("</MultiGeometry>");
            sw.WriteLine("</Placemark>");

            sw.WriteLine("</Document>");
            sw.WriteLine("</kml>");

            sw.Close();
        }
    }
}
