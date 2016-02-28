using System;
using System.Threading;
using System.Windows.Forms;
using System.IO;
using Microsoft.Win32.SafeHandles;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Net;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.Linq.Expressions;
using System.Text;
using System.Drawing;
using System.IO.Ports;
using System.ComponentModel;
using System.Resources;
using System.Linq;

namespace Motor_Tracker_Configuration
{
    public partial class MainForm : Form
    {
        // variables defined for localized string
        // control strings
        ResourceString APP_CAPTION_STR = new ResourceString();
        ResourceString DATA_SERVER_CMBBOX_ITEM_STR = new ResourceString();
        ResourceString FIRMWARE_SERVER_CMBBOX_ITEM_STR = new ResourceString();
        ResourceString INFO_SERVER_CMBBOX_ITEM_STR = new ResourceString();
        ResourceString INTERVAL_TRIGGER_SOURCE_CHKLIST_ITEM_STR = new ResourceString();
        ResourceString CONNECTED_STR = new ResourceString();
        ResourceString DISCONNECTED_STR = new ResourceString();
        ResourceString RETRIEVING_FILE_STR = new ResourceString();
        ResourceString OPEN_FILE_DIALOG_TITLE_STR = new ResourceString();
        ResourceString SAVE_FILE_DIALOG_TITLE_STR = new ResourceString();
        ResourceString OPEN_FOLDER_DIALOG_TITLE_STR = new ResourceString();
        ResourceString DOWNLOADING_FIRMWARE_STR = new ResourceString();
        ResourceString LOG_INTERVAL_1_MIN_STR = new ResourceString();
        ResourceString LOG_INTERVAL_5_MIN_STR = new ResourceString();
        ResourceString LOG_INTERVAL_10_MIN_STR = new ResourceString();
        ResourceString LOG_INTERVAL_30_MIN_STR = new ResourceString();
        // message strings
        ResourceString READ_CONFIG_SUCCESS_STR = new ResourceString();
        ResourceString READ_CONFIG_FAILED_STR = new ResourceString();
        ResourceString WRITE_CONFIG_SUCCESS_STR = new ResourceString();
        ResourceString WRITE_CONFIG_FALIED_STR = new ResourceString();
        ResourceString FORMAT_SDCARD_WARNING_STR = new ResourceString();
        ResourceString KML_SAVE_SUCCESS_STR = new ResourceString();
        ResourceString CONFIG_SAVE_SUCCESS_STR = new ResourceString();
        ResourceString CONFIG_LOADED_STR = new ResourceString();
        ResourceString CANNOT_OPEN_COMPORT_STR = new ResourceString();
        ResourceString FIRMWARE_DOWNLOADED_SUCCESS_STR = new ResourceString();
        ResourceString FIRMWARE_DOWNLOADED_FAILED_STR = new ResourceString();
        ResourceString CANNOT_OPEN_FILE_STR = new ResourceString();
        ResourceString FILE_NAME_IS_INVALID_WARNING_STR = new ResourceString();
        ResourceString SET_CAMERA_ID_SUCCESS_STR = new ResourceString();
        ResourceString SET_CAMERA_ID_FAILED_STR = new ResourceString();
        List<ResourceString> localizedStrings;


        readonly int GPS_CONFIG_DATA_SIZE = Marshal.SizeOf(typeof(GPS_CONFIG_DATA));
        readonly int SPEED_RECORD_SIZE = Marshal.SizeOf(typeof(SPEED_RECORD));

        const int DEFAULT_COMM_TIMEOUT = 500;
        const byte COMM_START_CODE = 0xCA;
        const int MAX_COMM_RETRY = 1;

        const byte DISABLE_LOG_OPCODE = 0x01;
        const byte ENABLE_LOG_OPCODE = 0x02;
        const byte READ_LOG_OPCODE = 0x03;
        const byte ENABLE_GPS_LOG_OPCODE = 0x04;
        const byte ENABLE_GSM_LOG_OPCODE = 0x05;
        const byte ENABLE_MCU_LOG_OPCODE = 0x06;
        const byte READ_CONFIG_OPCODE = 0x0C;
        const byte WRITE_CONFIG_OPCODE = 0x0D;

        const byte READ_FIRMWARE_VERSION_OPCODE = 0x11;
        const byte DOWNLOAD_FIRMWARE_OPCODE = 0x12;
        const byte SET_CAMERA_ID_OPCODE = 0x13;
        const byte SEND_SMS_OPCODE = 0x14;

        const byte BOOT_MODE_OPCODE = 0x33;
        const byte JUMP_TO_APP_OPCODE = 0xA5;
        const byte UPLOAD_LOG_OPCODE = 0x0F;
        const byte RESET_FACTORY_OPCODE = 0xFD;
        const byte RESET_DEVICE_OPCODE = 0xFE;
        const byte PRINT_REPORT_OPCODE = 0x91;
        const byte FORMAT_SDCARD_OPCODE = 0xFB;

        const UInt32 DEFAULT_PACKET_NO = 0xA5A5A5A5;
        const UInt32 ERROR_PACKET_NO = 0xFFFFFFFF;
        const UInt32 FINISH_PACKET_NO = 0x5A5A5A5A;
        const int START_CODE_OFFSET = 0;
        const int LENGTH_OFFSET = 1;
        const int OPCODE_OFFSET = 3;
        const int DATA_OFFSET = 4;
        const int PACKET_NO_OFFSET = 4;
        const int DATA_WITH_PACKET_NO_OFFSET = 8;
        const int SIMPLE_CRC_WITH_PACKET_NO_OFFSET = 8;
        const int LENGTH_SIZE = 2;
        const int PACKET_NO_SIZE = 4;
        const int HEADER_SIZE = OPCODE_OFFSET + 1;
        const int CRC_SIZE = 1;
        const int SIMPLE_PACKET_SIZE = HEADER_SIZE + PACKET_NO_SIZE + CRC_SIZE;
        const int MAX_DATA_SIZE = 507;
        const int SMS_SIZE = 32;

        const int READ_FROM_DEVICE = 0;
        const int READ_FROM_HARDDISK = 1;

        const int USB_COMMUNICATION = 0;
        const int COM_COMMUNICATION = 1;

        const int DATA_SERVER_SELECTED = 0;
        const int FIRMWARE_SERVER_SELECTED = 1;
        const int INFO_SERVER_SELECTED = 2;

        const int SERVER_USE_IP = 1;
        const int GPS_SPEED_SOURCE = 1;
        const int ANALOG_INPUT_1 = 0;
        const int ANALOG_INPUT_2 = 1;
        const int ANALOG_PERCENTAGE_MODE = 1;

        const int CAMERA_TRIGGER_INTERVAL = 0;
        const int CAMERA_TRIGGER_DI1 = 1;
        const int CAMERA_TRIGGER_DI2 = 2;
        const int CAMERA_TRIGGER_DI3 = 3;
        const int CAMERA_TRIGGER_DI4 = 4;
        const int INTERVAL_TRIGGER_MASK = 0x0100;
        const int DI1_TRIGGER_MASK = 0x01 | 0x02;
        const int DI2_TRIGGER_MASK = 0x04 | 0x08;
        const int DI3_TRIGGER_MASK = 0x10 | 0x20;
        const int DI4_TRIGGER_MASK = 0x40 | 0x80;

        const UInt32 DOOR_STATE_MASK = 0x02;

        const int LOG_LENGTH_LOW_THRESHOLD = 10;
        const int LOG_LENGTH_HIGH_THRESHOLD = 50;
        const int LOG_TIME_THRESHOLD = 1000;

        bool isInTrialPeriod = false;
        Thread downloader;
        IntPtr notificationHandle = IntPtr.Zero;
        HID.Device hidDevice = new HID.Device();
        byte[] hidSendBuffer = new byte[65];
        byte[] hidReceivedBuffer = new byte[65];
        byte[] hidTempBuffer = new byte[1024];
        byte[] hidOutBuffer = new byte[1024];
        byte[] hidInBuffer = new byte[1024];
        byte[] comInBuffer = new byte[1024];
        byte[] comOutBuffer = new byte[1024];
        byte[] comTempBuffer = new byte[1024];
        byte[] myBuff = new byte[1024];
        short vid = 0x1003;
        short pid = 0x0307;
        byte[] cfgData = new byte[4096];
        byte[] smsData = new byte[Marshal.SizeOf(typeof(SMS_RECORD))];
        int  offsetData;
        int cfgDataOffset;
        UInt32 cfgDataSize;
        UInt32 cfgDeviceBuffSize;
        UInt32 cfgNumByteToSend;
        UInt32 fDeviceBuffSize;
        GPS_CONFIG_DATA cfg;
        GPS_CONFIG_DATA tempCfg;
        string firmwareVersion;
        BinaryReader firmwareReader;
        bool downloadingFirmware;
        string logPath;
        SerialPort serialPort;
        int communicationType;
        int hidBufIndex = 0;
        int comBufIndex = 0;
        int fileDataOffset;
        int logDataLength;
        int logInterval;
        bool readLastLog;
        IndividualReport drivingReport;
        int currentUser;
        DateTime currentReportDate;
        bool configUpdateDisabled;
        System.Timers.Timer comTimeoutTimer;
        int currentOperation;
        byte[] comRetryBuffer;
        int comRetryTimeout;
        int comRetryCount;
        bool[] reportUserNameAdded;
        string conLogStr;
        DateTime prevLogReceivedTime;
        bool bootMode;

        volatile bool downloading;

        private void MainForm_Load(object sender, EventArgs e)
        {
            registerToolStripMenuItem.Visible = isInTrialPeriod;
            configUpdateDisabled = true;

            communicationType = USB_COMMUNICATION;
            serialPort = new SerialPort();

            cfg = new GPS_CONFIG_DATA();
            cfg.userList = new USER_INFO[10];
            tempCfg = new GPS_CONFIG_DATA();
            tempCfg.userList = new USER_INFO[10];
            for (int i = 0; i < 10; i++)
            {
                cfg.userList[i] = new USER_INFO();
                tempCfg.userList[i] = new USER_INFO();
            }

            commTypeCmbBox.SelectedIndex = USB_COMMUNICATION;
            deviceSelectionCmbBox.SelectedIndex = 0;
            selectLogTypeCmbBox.SelectedIndex = 0;

            comTimeoutTimer = new System.Timers.Timer();
            comTimeoutTimer.AutoReset = false;
            comTimeoutTimer.Elapsed += new System.Timers.ElapsedEventHandler(comTimeoutTimer_Elapsed);

            firmwareVersion = "";
            firmwareVersionLbl.Text = "";

            prevLogReceivedTime = DateTime.Now;

            bootMode = false;

            // initialize localized strings
            InitLocalizedStrings();

            UpdateUI();
        }

        private void InitLocalizedStrings()
        {
            APP_CAPTION_STR.Name = "APP_CAPTION_STR";
            DATA_SERVER_CMBBOX_ITEM_STR.Name = "DATA_SERVER_CMBBOX_ITEM_STR";
            FIRMWARE_SERVER_CMBBOX_ITEM_STR.Name = "FIRMWARE_SERVER_CMBBOX_ITEM_STR";
            INFO_SERVER_CMBBOX_ITEM_STR.Name = "INFO_SERVER_CMBBOX_ITEM_STR";
            INTERVAL_TRIGGER_SOURCE_CHKLIST_ITEM_STR.Name = "INTERVAL_TRIGGER_SOURCE_CHKLIST_ITEM_STR";
            CONNECTED_STR.Name = "CONNECTED_STR";
            DISCONNECTED_STR.Name = "DISCONNECTED_STR";
            RETRIEVING_FILE_STR.Name = "RETRIEVING_FILE_STR";
            READ_CONFIG_SUCCESS_STR.Name = "READ_CONFIG_SUCCESS_STR";
            READ_CONFIG_FAILED_STR.Name = "READ_CONFIG_FAILED_STR";
            WRITE_CONFIG_SUCCESS_STR.Name = "WRITE_CONFIG_SUCCESS_STR";
            WRITE_CONFIG_FALIED_STR.Name = "WRITE_CONFIG_FALIED_STR";
            FORMAT_SDCARD_WARNING_STR.Name = "FORMAT_SDCARD_WARNING_STR";
            KML_SAVE_SUCCESS_STR.Name = "KML_SAVE_SUCCESS_STR";
            CONFIG_SAVE_SUCCESS_STR.Name = "CONFIG_SAVE_SUCCESS_STR";
            CONFIG_LOADED_STR.Name = "CONFIG_LOADED_STR";
            CANNOT_OPEN_COMPORT_STR.Name = "CANNOT_OPEN_COMPORT_STR";
            FIRMWARE_DOWNLOADED_SUCCESS_STR.Name = "FIRMWARE_DOWNLOADED_SUCCESS_STR";
            FIRMWARE_DOWNLOADED_FAILED_STR.Name = "FIRMWARE_DOWNLOADED_FAILED_STR";
            CANNOT_OPEN_FILE_STR.Name = "CANNOT_OPEN_FILE_STR";
            FILE_NAME_IS_INVALID_WARNING_STR.Name = "FILE_NAME_IS_INVALID_WARNING_STR";
            OPEN_FILE_DIALOG_TITLE_STR.Name = "OPEN_FILE_DIALOG_TITLE_STR";
            SAVE_FILE_DIALOG_TITLE_STR.Name = "SAVE_FILE_DIALOG_TITLE_STR";
            OPEN_FOLDER_DIALOG_TITLE_STR.Name = "OPEN_FOLDER_DIALOG_TITLE_STR";
            DOWNLOADING_FIRMWARE_STR.Name = "DOWNLOADING_FIRMWARE_STR";
            LOG_INTERVAL_1_MIN_STR.Name = "LOG_INTERVAL_1_MIN_STR";
            LOG_INTERVAL_5_MIN_STR.Name = "LOG_INTERVAL_5_MIN_STR";
            LOG_INTERVAL_10_MIN_STR.Name = "LOG_INTERVAL_10_MIN_STR";
            LOG_INTERVAL_30_MIN_STR.Name = "LOG_INTERVAL_30_MIN_STR";
            SET_CAMERA_ID_SUCCESS_STR.Name = "SET_CAMERA_ID_SUCCESS_STR";
            SET_CAMERA_ID_FAILED_STR.Name = "SET_CAMERA_ID_FAILED_STR";

            localizedStrings = new List<ResourceString>();

            localizedStrings.Add(APP_CAPTION_STR);
            localizedStrings.Add(DATA_SERVER_CMBBOX_ITEM_STR);
            localizedStrings.Add(FIRMWARE_SERVER_CMBBOX_ITEM_STR);
            localizedStrings.Add(INFO_SERVER_CMBBOX_ITEM_STR);
            localizedStrings.Add(INTERVAL_TRIGGER_SOURCE_CHKLIST_ITEM_STR);
            localizedStrings.Add(CONNECTED_STR);
            localizedStrings.Add(DISCONNECTED_STR);
            localizedStrings.Add(RETRIEVING_FILE_STR);
            localizedStrings.Add(READ_CONFIG_SUCCESS_STR);
            localizedStrings.Add(READ_CONFIG_FAILED_STR);
            localizedStrings.Add(WRITE_CONFIG_SUCCESS_STR);
            localizedStrings.Add(WRITE_CONFIG_FALIED_STR);
            localizedStrings.Add(FORMAT_SDCARD_WARNING_STR);
            localizedStrings.Add(KML_SAVE_SUCCESS_STR);
            localizedStrings.Add(CONFIG_SAVE_SUCCESS_STR);
            localizedStrings.Add(CONFIG_LOADED_STR);
            localizedStrings.Add(CANNOT_OPEN_COMPORT_STR);
            localizedStrings.Add(FIRMWARE_DOWNLOADED_SUCCESS_STR);
            localizedStrings.Add(FIRMWARE_DOWNLOADED_FAILED_STR);
            localizedStrings.Add(CANNOT_OPEN_FILE_STR);
            localizedStrings.Add(FILE_NAME_IS_INVALID_WARNING_STR);
            localizedStrings.Add(OPEN_FILE_DIALOG_TITLE_STR);
            localizedStrings.Add(SAVE_FILE_DIALOG_TITLE_STR);
            localizedStrings.Add(OPEN_FOLDER_DIALOG_TITLE_STR);
            localizedStrings.Add(DOWNLOADING_FIRMWARE_STR);
            localizedStrings.Add(LOG_INTERVAL_1_MIN_STR);
            localizedStrings.Add(LOG_INTERVAL_5_MIN_STR);
            localizedStrings.Add(LOG_INTERVAL_10_MIN_STR);
            localizedStrings.Add(LOG_INTERVAL_30_MIN_STR);
            localizedStrings.Add(SET_CAMERA_ID_SUCCESS_STR);
            localizedStrings.Add(SET_CAMERA_ID_FAILED_STR);

            LoadLocalizedString("en");
        }

        private void comTimeoutTimer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (comRetryCount++ > MAX_COMM_RETRY)
            {
                comRetryCount = 0;
                this.Invoke((MethodInvoker)delegate
                {
                    ReportCOMWriteTimedOut();
                });
                return;
            }
            SendCOMData(comRetryBuffer, 0, comRetryBuffer.Length, comRetryTimeout);
        }

        private void ReportCOMWriteTimedOut()
        {
            if (currentOperation == UPLOAD_LOG_OPCODE)
            {
                downloading = false;
                return;
            }
            if (currentOperation == READ_CONFIG_OPCODE)
            {
                MessageBox.Show(READ_CONFIG_FAILED_STR.Text, APP_CAPTION_STR.Text);
                EnableDeviceCmd(true);
                return;
            }
            if (currentOperation == WRITE_CONFIG_OPCODE)
            {
                MessageBox.Show(WRITE_CONFIG_FALIED_STR.Text, APP_CAPTION_STR.Text);
                EnableDeviceCmd(true);
                return;
            }
            if (currentOperation == DOWNLOAD_FIRMWARE_OPCODE)
            {
                if (downloadingFirmware)
                {
                    downloadingFirmware = false;
                    EnableDeviceCmd(true);
                    UpdateProgressBar(false);
                    MessageBox.Show(FIRMWARE_DOWNLOADED_FAILED_STR.Text, APP_CAPTION_STR.Text);
                }
                return;
            }
        }

        private void OpenHIDDevice()
        {
            string[] devicePathName = new string[0];

            if (HID.FindDevices(vid, pid, ref devicePathName) > 0)
            {
                hidDevice.pathName = devicePathName[0];

                if (HID.OpenDevice(ref hidDevice))
                {
                    hidDevice.readStream.BeginRead(hidReceivedBuffer, 0, hidReceivedBuffer.Length, new AsyncCallback(hidDevice_DataReceived), null);

                    ReadConfig();
                }
            }

            UpdateUI();
        }

        private void CloseHIDDevice()
        {
            HID.CloseDevice(ref hidDevice);
            UpdateUI();
        }

        private void ReadConfig()
        {
            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                CreateHIDBuffer(PACKET_NO_SIZE, READ_CONFIG_OPCODE, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                cfgDataOffset = 0;
                cfgDeviceBuffSize = 0;
                cfgDataSize = 0;
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE, 1000);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }

                CreateCOMBuffer(PACKET_NO_SIZE, READ_CONFIG_OPCODE, DEFAULT_PACKET_NO);
                comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                cfgDataOffset = 0;
                cfgDeviceBuffSize = 0;
                cfgDataSize = 0;
                SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE, 1000);
            }
        }

        private void SendSMS()
        {
            SMS_RECORD sms;

            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                sms = new SMS_RECORD();
                sms.smsAddress = smsAddressTxt.Text;
                sms.smsMessage = smsMessageTxt.Text;
                CreateHIDBuffer(PACKET_NO_SIZE + SMS_SIZE, SEND_SMS_OPCODE, DEFAULT_PACKET_NO);
                Buffer.BlockCopy(Lib.ObjectToByteArray(sms), 0, hidOutBuffer, DATA_WITH_PACKET_NO_OFFSET, SMS_SIZE);
                hidOutBuffer[DATA_WITH_PACKET_NO_OFFSET + SMS_SIZE] = calcCRC(hidOutBuffer, DATA_OFFSET, PACKET_NO_SIZE + SMS_SIZE);
                SendHIDData(hidOutBuffer, 0, HEADER_SIZE + PACKET_NO_SIZE + SMS_SIZE + CRC_SIZE, 1000);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }

                sms = new SMS_RECORD();
                sms.smsAddress = smsAddressTxt.Text;
                sms.smsMessage = smsMessageTxt.Text;
                CreateCOMBuffer(PACKET_NO_SIZE + SMS_SIZE, SEND_SMS_OPCODE, DEFAULT_PACKET_NO);
                Buffer.BlockCopy(Lib.ObjectToByteArray(sms), 0, comOutBuffer, DATA_WITH_PACKET_NO_OFFSET, SMS_SIZE);
                comOutBuffer[DATA_WITH_PACKET_NO_OFFSET + SMS_SIZE] = calcCRC(comOutBuffer, DATA_OFFSET, PACKET_NO_SIZE + SMS_SIZE);
                SendCOMData(comOutBuffer, 0, HEADER_SIZE + PACKET_NO_SIZE + SMS_SIZE + CRC_SIZE, 1000);
            }
        }

        private void CreateHIDBuffer(int length, byte opcode, UInt32 packetNo)
        {
            Array.Clear(hidOutBuffer, 0, hidOutBuffer.Length);
            hidOutBuffer[START_CODE_OFFSET] = COMM_START_CODE;
            Array.Copy(BitConverter.GetBytes(length), 0, hidOutBuffer, LENGTH_OFFSET, LENGTH_SIZE);
            hidOutBuffer[OPCODE_OFFSET] = opcode;
            Array.Copy(BitConverter.GetBytes(packetNo), 0, hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
            currentOperation = opcode;
        }

        private void CreateCOMBuffer(int length, byte opcode, UInt32 packetNo)
        {
            Array.Clear(comOutBuffer, 0, comOutBuffer.Length);
            comOutBuffer[START_CODE_OFFSET] = COMM_START_CODE;
            Array.Copy(BitConverter.GetBytes(length), 0, comOutBuffer, LENGTH_OFFSET, LENGTH_SIZE);
            comOutBuffer[OPCODE_OFFSET] = opcode;
            Array.Copy(BitConverter.GetBytes(packetNo), 0, comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
            currentOperation = opcode;
        }

        private void hidDevice_DataReceived(IAsyncResult ar)
        {
            int length;
            try
            {
                length = hidDevice.readStream.EndRead(ar);
                if (!ar.IsCompleted)
                {
                    // read is not complete
                    return;
                }
                Array.Clear(hidTempBuffer, 0, hidTempBuffer.Length);
                Array.Copy(hidReceivedBuffer, 1, hidTempBuffer, 0, length - 1);
                hidBufIndex += length - 1;
                if (hidBufIndex >= (BitConverter.ToInt16(hidTempBuffer, LENGTH_OFFSET) + HEADER_SIZE + CRC_SIZE))
                {
                    hidBufIndex = 0;
                    Array.Copy(hidTempBuffer, hidInBuffer, hidInBuffer.Length);
                    ProcessReceivedHIDData();
                }
                if (communicationType != USB_COMMUNICATION)
                {
                    return;
                }
                hidDevice.readStream.BeginRead(hidReceivedBuffer, 0, hidReceivedBuffer.Length, new AsyncCallback(hidDevice_DataReceived), null);
            }
            catch
            {
            }
        }

        private void UpdateTextBox()
        {
            int length;
            string str;

            if (communicationType == USB_COMMUNICATION)
            {
                length = BitConverter.ToInt16(hidInBuffer, LENGTH_OFFSET);
                str = System.Text.Encoding.ASCII.GetString(hidInBuffer, DATA_OFFSET, length);
                if (DateTime.Now.CompareTo(prevLogReceivedTime.AddMilliseconds(LOG_TIME_THRESHOLD)) < 0)
                {
                    conLogStr += str;
                    return;
                }
                else
                {
                    prevLogReceivedTime = DateTime.Now;
                    str = conLogStr + str;
                    conLogStr = "";
                }
            }
            else
            {
                length = BitConverter.ToInt16(comInBuffer, LENGTH_OFFSET);
                str = System.Text.Encoding.ASCII.GetString(comInBuffer, DATA_OFFSET, length);
            }

            lock (logTxtBox)
            {
                logTxtBox.Text += str;
                logTxtBox.SelectionStart = logTxtBox.Text.Length;
                logTxtBox.ScrollToCaret();
            }
        }

        private void UpdateConfigUI()
        {
            configUpdateDisabled = true;

            tempCfg = cfg;

            // GPRS config
            gprsApnTxtBox.Text = cfg.gprsApn;
            gprsUserTxtBox.Text = cfg.gprsUser;
            gprsPwdTxtBox.Text = cfg.gprsPass;
            gprsRunReportIntervalTxtBox.Text = cfg.runReportInterval.ToString();
            gprsStopReportIntervalTxtBox.Text = cfg.stopReportInterval.ToString();

            // server parameter
			serverSelectionCmbBox.SelectedIndex = 0;
			serverSelection_Changed(null, null);

            // vehicle config
            trackerIdTxtBox.Text = cfg.trackerId;
            plateNoTxtBox.Text = cfg.plateNo;
            vinNumTxtBox.Text = cfg.vin;
            bossPhoneTxtBox.Text = cfg.whiteCallerNo;
            tyreRateTxtBox.Text = cfg.speedSensorRatio.ToString();
            if (cfg.useGpsSpeed == GPS_SPEED_SOURCE)
            {
                gpsSpeedSourceRBtn.Checked = true;
            }
            else
            {
                pulseSpeedSourceRBtn.Checked = true;
            }

            lowBatteryWarningChkBox.Checked = ((cfg.featureSet & (1 << 4)) != 0);
            lowPowerWarningChkBox.Checked = ((cfg.featureSet & (1 << 5)) != 0);
            removePowerWarningChkBox.Checked = ((cfg.featureSet & (1 << 8)) != 0);
            autoSecurityOnChkBox.Checked = ((cfg.featureSet & (1 << 9)) != 0);
			enableFifoChkBox.Checked = ((cfg.featureSet & (1 << 10)) != 0);

            // current user config
            userNoCmbBox.SelectedIndex = 1;
            userNoCmbBox.SelectedIndex = 0;
            
            // driving settings
            speedLimitTxtBox.Text = cfg.speedLimit.ToString();
            drivingTimeLimitTxtBox.Text = ((float)cfg.drivingTimeLimit / 3600).ToString();
            totalDrivingTimeLimitTxtBox.Text = ((float)cfg.totalDrivingTimeLimit / 3600).ToString();
            baseMileageTxtBox.Text = cfg.baseMileage.ToString();

            // other settings
            if (cfg.securityOn == 1)
            {
                manualSecurityOnChkBox.Checked = true;
                sendSMSActionChkBox.Checked = ((cfg.securityAction & (1 << 0)) != 0);
                callActionChkBox.Checked = ((cfg.securityAction & (1 << 1)) != 0);
            }
            else
            {
                manualSecurityOnChkBox.Checked = false;
            }
            sosKeyPressTimeTxt.Text = cfg.SOSKeyPressTimes.ToString();
            sosKeyPressPeriodTxt.Text = cfg.SOSKeyPressPeriod.ToString();
            removePowerWarningPeriodTxt.Text = cfg.removePowerWarningPeriod.ToString();
            batteryLowWarningPeriodTxt.Text = cfg.batteryLowWarningPeriod.ToString();
            speedEngineOffTxt.Text = cfg.speedEngineOff.ToString();
            powerOffDelayTimeTxt.Text = cfg.powerOffDelayTime.ToString();
            batteryLowWarningLevelTxt.Text = cfg.batteryLowWarningLevel.ToString();
			autoSecurityTimeTxt.Text = cfg.autoSecurityTime.ToString();
			accountAlarmCheckTxt.Text = cfg.accountAlarmCheck.ToString();

            analogInputNoCmbBox.SelectedIndex = 1;
            analogInputNoCmbBox.SelectedIndex = 0;

            imeiLbl.Text = "IMEI: " + cfg.imei;

            configUpdateDisabled = false;
        }

        private void UpdateUI()
        {
            if (bootMode)
            {
                connectionStatusLbl.Text = "BOOT MODE";
                runAppBtn.Enabled = true;
            }
            else
            {
                if (communicationType == USB_COMMUNICATION)
                {
                    commTypeCmbBox.SelectedIndex = 0;
                    commuTypeLbl.Text = "USB";
                    if (hidDevice.connected)
                    {
                        deviceCmdGBox.Enabled = true;
                        connectionStatusLbl.Text = CONNECTED_STR.Text;
                        connectionStatusLbl.ForeColor = Color.Green;
                    }
                    else
                    {
                        deviceCmdGBox.Enabled = false;
                        connectionStatusLbl.Text = DISCONNECTED_STR.Text;
                        connectionStatusLbl.ForeColor = Color.Red;
                    }
                }
                else
                {
                    commTypeCmbBox.SelectedIndex = 1;
                    if ((serialPort != null) && serialPort.IsOpen)
                    {
                        deviceCmdGBox.Enabled = true;
                        commuTypeLbl.Text = serialPort.PortName;
                        comPortCmbBox.Text = serialPort.PortName;
                        connectionStatusLbl.Text = CONNECTED_STR.Text;
                        connectionStatusLbl.ForeColor = Color.Green;
                    }
                    else
                    {
                        deviceCmdGBox.Enabled = false;
                        commuTypeLbl.Text = "COM";
                        connectionStatusLbl.Text = DISCONNECTED_STR.Text;
                        connectionStatusLbl.ForeColor = Color.Red;
                    }
                }
            }
            connectionStatusLbl.Left = statusGrpBox.Width - connectionStatusLbl.Width - 8;
            commuTypeLbl.Left = statusGrpBox.Width - 185;
        }

        private delegate void InvokerWithOneParam(string o);
        void UpdateDownloadStatus(string str)
        {
            retrieveLogStatusLbl.Text = str;
        }

        void UpdateProgressBar(int min, int max, int value, bool visible)
        {
            retrieveLogPrgBar.Minimum = min;
            retrieveLogPrgBar.Maximum = max;
            retrieveLogPrgBar.Visible = visible;
            retrieveLogPrgBar.Value = value;
            retrieveLogStatusLbl.Visible = visible;
        }

        void UpdateProgressBar(bool visible)
        {
            if (visible == false)
            {
                retrieveLogPrgBar.Value = 0;
                retrieveLogStatusLbl.Text = "0%";
            }
            retrieveLogPrgBar.Visible = visible;
            retrieveLogStatusLbl.Visible = visible;
            retrieveLogPercentLbl.Visible = visible;
        }

        void LoadBootModeLayout()
        {
            commGrpBox.Enabled = false;
            deviceCmdGBox.Enabled = false;
            tabMain.SelectedTab = settingTabPage;
            sendSMSBtn.Enabled = false;
            readFirmwareVersionBtn.Enabled = false;
            runAppBtn.Enabled = true;
            connectionStatusLbl.Text = "BOOT MODE";
            connectionStatusLbl.ForeColor = Color.Blue;
        }

        void LoadAppModeLayout()
        {
            commGrpBox.Enabled = true;
            deviceCmdGBox.Enabled = true;
            sendSMSBtn.Enabled = true;
            readFirmwareVersionBtn.Enabled = true;
            runAppBtn.Enabled = false;
            UpdateUI();
        }

        public MainForm(bool isInTrial = false)
        {
            InitializeComponent();
            isInTrialPeriod = isInTrial;
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32.WM_DEVICECHANGE)
            {
                if (communicationType == USB_COMMUNICATION)
                {
                    if (m.WParam.ToInt32() == Win32.DBT_DEVICEARRIVAL)
                    {
                        OpenHIDDevice();
                    }
                    else if (m.WParam.ToInt32() == Win32.DBT_DEVICEREMOVECOMPLETE)
                    {
                        if (HID.DeviceNameMatch(m, hidDevice.pathName))
                        {
                            CloseHIDDevice();
                        }
                    }
                }

                UpdateUI();
            }

            base.WndProc(ref m);
        }

       

        private bool ParseIP(string ipAddress, ref UInt32 ip)
        {
            try
            {
                IPAddress address = IPAddress.Parse(ipAddress);
                byte[] b = address.GetAddressBytes();

                ip = BitConverter.ToUInt32(b, 0);
            }
            catch
            {
                return false;
            }

            return true;
        }

        private void EnableCommTypeChange(bool enabled)
        {
            commGrpBox.Enabled = enabled;
        }

        #region process hid data
        private void ProcessReceivedHIDData()
        {
            byte crc;
            int length;
            byte opcode;
            UInt32 packetNo;
            byte[] bTemp;

            // should have something in the buffer
            length = BitConverter.ToInt16(hidInBuffer, LENGTH_OFFSET);
            if (length <= 0)
            {
                return;
            }
            // validate the CRC
            crc = hidInBuffer[HEADER_SIZE + length];
            if (crc != calcCRC(hidInBuffer, DATA_OFFSET, length))
            {
                return;
            }

            opcode = hidInBuffer[OPCODE_OFFSET];
            switch (opcode)
            {
                case READ_LOG_OPCODE:             // debug data
                    this.Invoke(new MethodInvoker(UpdateTextBox));
                    break;

                case BOOT_MODE_OPCODE:
                    bootMode = true;
                    this.Invoke((MethodInvoker)delegate
                    {
                        LoadBootModeLayout();
                    });
                    break;

                case WRITE_CONFIG_OPCODE:
                    comTimeoutTimer.Stop();
                    packetNo = BitConverter.ToUInt32(hidInBuffer, PACKET_NO_OFFSET);

                    if (packetNo == FINISH_PACKET_NO)
                    {
                        MessageBox.Show(WRITE_CONFIG_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                        });
                        return;
                    }
                    if (packetNo == ERROR_PACKET_NO)
                    {
                        return;
                    }
                    if (packetNo == DEFAULT_PACKET_NO)
                    {
                        cfgDataSize = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET);
                        cfgDeviceBuffSize = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET + 4);
                        cfgDataOffset = 0;
                        length = (int)cfgDeviceBuffSize;
                    }
                    else
                    {
                        cfgNumByteToSend = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET);
                        cfgDataOffset = (int)packetNo;
                        length = (int)cfgNumByteToSend;
                    }

                    if (cfgDataOffset >= cfgDataSize)         // last packet
                    {
                        MessageBox.Show(WRITE_CONFIG_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                        });
                        return;
                    }
                    CreateHIDBuffer(PACKET_NO_SIZE + length, WRITE_CONFIG_OPCODE, (UInt32)cfgDataOffset);
                    Array.Copy(cfgData, cfgDataOffset, hidOutBuffer, DATA_WITH_PACKET_NO_OFFSET, length);
                    hidOutBuffer[DATA_WITH_PACKET_NO_OFFSET + length] = calcCRC(hidOutBuffer, DATA_OFFSET, PACKET_NO_SIZE + length);
                    SendHIDData(hidOutBuffer, 0, HEADER_SIZE + PACKET_NO_SIZE + length + CRC_SIZE);
                    break;

                case READ_CONFIG_OPCODE:          // config data
                    comTimeoutTimer.Stop();
                    packetNo = BitConverter.ToUInt32(hidInBuffer, PACKET_NO_OFFSET);

                    if (packetNo == FINISH_PACKET_NO)
                    {
                        cfg = (GPS_CONFIG_DATA)Lib.ByteArrayToObject(cfgData, typeof(GPS_CONFIG_DATA));
                        this.Invoke(new MethodInvoker(UpdateConfigUI));
                    }
                    if (packetNo == ERROR_PACKET_NO)
                    {
                        return;
                    }
                    if (packetNo == DEFAULT_PACKET_NO)
                    {
                        cfgDataSize = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET);
                        cfgDeviceBuffSize = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET + 4);
                        cfgDataOffset = 0;
                        length = (int)cfgDeviceBuffSize;
                    }
                    else
                    {
                        Array.Copy(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET, cfgData, packetNo, length - PACKET_NO_SIZE);
                        cfgDataOffset = (int)packetNo + length - PACKET_NO_SIZE;
                        length = (int)cfgDeviceBuffSize;
                        if (cfgDataOffset > cfgDataSize - cfgDeviceBuffSize)
                            length = (int)cfgDataSize - cfgDataOffset;
                    }

                    if (cfgDataOffset >= cfgDataSize)         // last packet
                    {
                        cfg = (GPS_CONFIG_DATA)Lib.ByteArrayToObject(cfgData, typeof(GPS_CONFIG_DATA));
                        this.Invoke(new MethodInvoker(UpdateConfigUI));
                        // read config successfully
                        if (bootMode)
                        {
                            bootMode = false;
                            this.Invoke((MethodInvoker)delegate
                            {
                                LoadAppModeLayout();
                            });
                        }

                        return;
                    }
                    CreateHIDBuffer(PACKET_NO_OFFSET + 2, READ_CONFIG_OPCODE, (UInt32)cfgDataOffset);
                    Array.Copy(BitConverter.GetBytes(length), 0, hidOutBuffer, DATA_WITH_PACKET_NO_OFFSET, 2);
                    hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET + 2] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE + 2);
                    SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE + 2);
                    break;
                case UPLOAD_LOG_OPCODE:          // file data
                    comTimeoutTimer.Stop();
                    packetNo = BitConverter.ToUInt32(hidInBuffer, PACKET_NO_OFFSET);
                    if (packetNo == ERROR_PACKET_NO)
                    {
                        // file is not exist
                        downloading = false;
                        return;
                    }
                    else if (packetNo == DEFAULT_PACKET_NO)
                    {
                        cfgDataSize = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET);
                        cfgDeviceBuffSize = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET + 4);
                        cfgDataOffset = 0;
                        length = (int)cfgDeviceBuffSize;
                        offsetData = 0;
                    }
                    else /*if (BitConverter.ToUInt16(hidInBuffer, LENGTH_OFFSET) == cfgDeviceBuffSize)*/
                    {
                        length = BitConverter.ToUInt16(hidInBuffer, LENGTH_OFFSET) - PACKET_NO_SIZE;
                        Array.Copy(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET, myBuff, offsetData, length);
                        offsetData += length;
                        if (offsetData >= SPEED_RECORD_SIZE)
                        {
                            offsetData = 0;
                            bTemp = new byte[SPEED_RECORD_SIZE];
                            Array.Copy(myBuff, 0, bTemp, 0, bTemp.Length);
                            try
                            {
                                drivingReport.AddReport(bTemp);
                            }
                            catch (Exception)
                            { }
                            cfgDataOffset += (int)(logInterval * SPEED_RECORD_SIZE);
                            if (cfgDataOffset >= cfgDataSize)
                            {
                                if (!readLastLog)
                                {
                                    // read the last log in file
                                    readLastLog = true;
                                    cfgDataOffset = (int)(cfgDataSize - SPEED_RECORD_SIZE);
                                }
                                else
                                {
                                    // end of file
                                    readLastLog = false;
                                    downloading = false;
                                    return;
                                }
                            }
                        }
                    }
                    CreateHIDBuffer(PACKET_NO_OFFSET, UPLOAD_LOG_OPCODE, (UInt32)(cfgDataOffset + offsetData));
                    hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                    SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE, 1000);
                    break;

                case READ_FIRMWARE_VERSION_OPCODE:
                    firmwareVersion = System.Text.Encoding.ASCII.GetString(hidInBuffer, 3, length);
                    this.Invoke((MethodInvoker)delegate
                    {
                        firmwareVersionLbl.Text = firmwareVersion;
                    });
                    break;

                case DOWNLOAD_FIRMWARE_OPCODE:
                    comTimeoutTimer.Stop();
                    if (!downloadingFirmware)
                    {
                        return;
                    }
                    packetNo = BitConverter.ToUInt32(hidInBuffer, PACKET_NO_OFFSET);
                    if (packetNo == FINISH_PACKET_NO)
                    {
                        // complete
                        downloadingFirmware = false;
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                            UpdateProgressBar(false);
                        });
                        MessageBox.Show(FIRMWARE_DOWNLOADED_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                        try
                        {
                            firmwareReader.Close();
                            return;
                        }
                        catch (Exception)
                        { }
                    }
                    if (packetNo == DEFAULT_PACKET_NO)
                    {
                        fDeviceBuffSize = BitConverter.ToUInt32(hidInBuffer, DATA_WITH_PACKET_NO_OFFSET + 4);
                        packetNo = 0;
                    }
                    this.Invoke((MethodInvoker)delegate
                    {
                        retrieveLogPrgBar.Value = (int)(packetNo / 100);
                        retrieveLogPercentLbl.Text = (retrieveLogPrgBar.Value * 100 / retrieveLogPrgBar.Maximum).ToString() + "%";
                    });
                    try
                    {
                        byte[] temp;

                        firmwareReader.BaseStream.Seek(packetNo, SeekOrigin.Begin);
                        //bTemp = firmwareReader.ReadBytes((int)fDeviceBuffSize);
                        temp = firmwareReader.ReadBytes((int)fDeviceBuffSize);
                        bTemp = Enumerable.Repeat<byte>(0xFF, (int)fDeviceBuffSize).ToArray();
                        Buffer.BlockCopy(temp, 0, bTemp, 0, temp.Length);
                        length = bTemp.Length + 4;
                        CreateHIDBuffer(length, DOWNLOAD_FIRMWARE_OPCODE, packetNo);
                        Array.Copy(bTemp, 0, hidOutBuffer, DATA_WITH_PACKET_NO_OFFSET, bTemp.Length);
                        hidOutBuffer[length + HEADER_SIZE] = calcCRC(hidOutBuffer, DATA_OFFSET, length);
                        SendHIDData(hidOutBuffer, 0, length + HEADER_SIZE + CRC_SIZE);
                    }
                    catch (Exception)
                    {
                        downloadingFirmware = false;
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                            UpdateProgressBar(false);
                        });
                        MessageBox.Show(FIRMWARE_DOWNLOADED_FAILED_STR.Text, APP_CAPTION_STR.Text);
                        return;
                    }
                    break;

                case SET_CAMERA_ID_OPCODE:
                    packetNo = BitConverter.ToUInt32(hidInBuffer, PACKET_NO_OFFSET);
                    if (packetNo == FINISH_PACKET_NO)
                    {
                        MessageBox.Show(SET_CAMERA_ID_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                    }
                    else
                    {
                        MessageBox.Show(SET_CAMERA_ID_FAILED_STR.Text, APP_CAPTION_STR.Text);
                    }
                    break;
                case 0x5A:  // ACK
                    break;
                case 0xA5:  // NACK
                    break;
            }
        }
        #endregion

        #region process com data
        private void ProcessReceivedCOMData()
        {
            byte crc;
            int length;
            byte opcode;
            UInt32 packetNo;
            byte[] bTemp;

            // should have something in the buffer
            length = BitConverter.ToInt16(comInBuffer, LENGTH_OFFSET);
            if (length <= 0)
            {
                return;
            }
            // validate the CRC
            crc = comInBuffer[HEADER_SIZE + length];
            if (crc != calcCRC(comInBuffer, DATA_OFFSET, length))
            {
                return;
            }

            opcode = comInBuffer[OPCODE_OFFSET];
            switch (opcode)
            {
                case READ_LOG_OPCODE:             // debug data
                    this.Invoke(new MethodInvoker(UpdateTextBox));
                    break;

                case WRITE_CONFIG_OPCODE:
                    comTimeoutTimer.Stop();
                    packetNo = BitConverter.ToUInt32(comInBuffer, PACKET_NO_OFFSET);
               
                    if (packetNo == FINISH_PACKET_NO)
                    {
                        MessageBox.Show(WRITE_CONFIG_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                        });
                        return;
                    }
                    if (packetNo == ERROR_PACKET_NO)
                    {
                        return;
                    }
                    if (packetNo == DEFAULT_PACKET_NO)
                    {
                        cfgDataSize = BitConverter.ToUInt32(comInBuffer, DATA_WITH_PACKET_NO_OFFSET);
                        cfgDeviceBuffSize = BitConverter.ToUInt32(comInBuffer, DATA_WITH_PACKET_NO_OFFSET + 4);
                        cfgDataOffset = 0;
                        length = (int)cfgDeviceBuffSize;
                    }
                    else
                    {
                        cfgNumByteToSend = BitConverter.ToUInt32(comInBuffer, DATA_WITH_PACKET_NO_OFFSET);
                        cfgDataOffset = (int)packetNo;
                        length = (int)cfgNumByteToSend;
                    }

                    if (cfgDataOffset >= cfgDataSize)         // last packet
                    {
                        MessageBox.Show(WRITE_CONFIG_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                        });
                        return;
                    }
                    CreateCOMBuffer(PACKET_NO_SIZE + length, WRITE_CONFIG_OPCODE, (UInt32)cfgDataOffset);
                    Array.Copy(cfgData, cfgDataOffset, comOutBuffer, DATA_WITH_PACKET_NO_OFFSET, length);
                    comOutBuffer[DATA_WITH_PACKET_NO_OFFSET + length] = calcCRC(comOutBuffer, DATA_OFFSET, PACKET_NO_SIZE + length);
                    SendCOMData(comOutBuffer, 0, HEADER_SIZE + PACKET_NO_SIZE + length + CRC_SIZE);
                    break;

                case READ_CONFIG_OPCODE:          // config data
                    comTimeoutTimer.Stop();
                    packetNo = BitConverter.ToUInt32(comInBuffer, PACKET_NO_OFFSET);
               
                    if (packetNo == FINISH_PACKET_NO)
                    {
                        cfg = (GPS_CONFIG_DATA)Lib.ByteArrayToObject(cfgData, typeof(GPS_CONFIG_DATA));
                        this.Invoke(new MethodInvoker(UpdateConfigUI));
                    }
                    if (packetNo == ERROR_PACKET_NO)
                    {
                        return;
                    }
                    if (packetNo == DEFAULT_PACKET_NO)
                    {
                        cfgDataSize = BitConverter.ToUInt32(comInBuffer, DATA_WITH_PACKET_NO_OFFSET);
                        cfgDeviceBuffSize = BitConverter.ToUInt32(comInBuffer, DATA_WITH_PACKET_NO_OFFSET + 4);
                        cfgDataOffset = 0;
                        length = (int)cfgDeviceBuffSize;
                    }
                    else
                    {
                        Array.Copy(comInBuffer, DATA_WITH_PACKET_NO_OFFSET, cfgData, packetNo, length - PACKET_NO_SIZE);
                        cfgDataOffset = (int)packetNo + length - PACKET_NO_SIZE;
                        length = (int)cfgDeviceBuffSize;
                        if (cfgDataOffset > cfgDataSize - cfgDeviceBuffSize)
                            length = (int)cfgDataSize - cfgDataOffset;
                    }

                    if (cfgDataOffset >= cfgDataSize)         // last packet
                    {
                        cfg = (GPS_CONFIG_DATA)Lib.ByteArrayToObject(cfgData, typeof(GPS_CONFIG_DATA));
                        this.Invoke(new MethodInvoker(UpdateConfigUI));
                        return;
                    }
                    CreateCOMBuffer(PACKET_NO_SIZE + 2, READ_CONFIG_OPCODE, (UInt32)cfgDataOffset);
                    Array.Copy(BitConverter.GetBytes(length), 0, comOutBuffer, DATA_WITH_PACKET_NO_OFFSET, 2);
                    comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET + 2] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE + 2);
                    SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE + 2);
                    break;
                case UPLOAD_LOG_OPCODE:          // file data
                    comTimeoutTimer.Stop();
                    packetNo = BitConverter.ToUInt32(comInBuffer, PACKET_NO_OFFSET);
                    if (packetNo == ERROR_PACKET_NO)
                    { 
                        // file is not exist
                        downloading = false;
                        return;
                    }
                    length = length - 4;
                    if (length > 0)
                    {                        
                        bTemp = new byte[SPEED_RECORD_SIZE];
                        Array.Copy(comInBuffer, DATA_WITH_PACKET_NO_OFFSET, bTemp, 0, bTemp.Length);
                        try
                        {
                            drivingReport.AddReport(bTemp);
                        }
                        catch (Exception)
                        { }
                        packetNo += (UInt32)(logInterval * SPEED_RECORD_SIZE);
                        if (packetNo >= logDataLength)
                        {
                            if (!readLastLog)
                            {
                                // read the last log in file
                                readLastLog = true;
                                packetNo = (UInt32)(logDataLength - SPEED_RECORD_SIZE);
                            }
                            else
                            {
                                // end of file
                                readLastLog = false;
                                downloading = false;
                                return;
                            }
                        }
                    }
                    else
                    { 
                        // fist packet to receive maxDataPacket
                        if (fileDataOffset < 0)
                        {
                            logDataLength = (int)packetNo;
                            packetNo = 0;
                        }
                        else
                        { 
                            // receive data failed, send request previous packet again
                        }
                    }

                    CreateCOMBuffer(PACKET_NO_SIZE, UPLOAD_LOG_OPCODE, packetNo);
                    comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                    SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE, 1000);
                    break;

                case READ_FIRMWARE_VERSION_OPCODE:
                    firmwareVersion = System.Text.Encoding.ASCII.GetString(comInBuffer, 3, length);
                    this.Invoke((MethodInvoker)delegate
                    {
                        firmwareVersionLbl.Text = firmwareVersion;
                    });
                    break;

                case DOWNLOAD_FIRMWARE_OPCODE:
                    comTimeoutTimer.Stop();
                    if (!downloadingFirmware)
                    {
                        return;
                    }
                    packetNo = BitConverter.ToUInt32(comInBuffer, PACKET_NO_OFFSET);
                    if (packetNo == FINISH_PACKET_NO)
                    {
                        // complete
                        downloadingFirmware = false;
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                            UpdateProgressBar(false);
                        });
                        MessageBox.Show(FIRMWARE_DOWNLOADED_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                        try
                        {
                            firmwareReader.Close();
                            return;
                        }
                        catch (Exception)
                        { }
                    }
                    if (packetNo == DEFAULT_PACKET_NO)
                    {
                        fDeviceBuffSize = BitConverter.ToUInt32(comInBuffer, DATA_WITH_PACKET_NO_OFFSET + 4);
                        packetNo = 0;
                    }
                    this.Invoke((MethodInvoker)delegate
                    {
                        retrieveLogPrgBar.Value = (int)(packetNo / 100);
                        retrieveLogPercentLbl.Text = (retrieveLogPrgBar.Value * 100 / retrieveLogPrgBar.Maximum).ToString() + "%";
                    });
                    try
                    {
                        byte[] temp;

                        firmwareReader.BaseStream.Seek(packetNo, SeekOrigin.Begin);
                        //bTemp = firmwareReader.ReadBytes((int)fDeviceBuffSize);
                        temp = firmwareReader.ReadBytes((int)fDeviceBuffSize);
                        bTemp = Enumerable.Repeat<byte>(0xFF, (int)fDeviceBuffSize).ToArray();
                        Buffer.BlockCopy(temp, 0, bTemp, 0, temp.Length);
                        length = bTemp.Length + 4;
                        CreateCOMBuffer(length, DOWNLOAD_FIRMWARE_OPCODE, packetNo);
                        Array.Copy(bTemp, 0, comOutBuffer, DATA_WITH_PACKET_NO_OFFSET, bTemp.Length);
                        comOutBuffer[length + HEADER_SIZE] = calcCRC(comOutBuffer, DATA_OFFSET, length);
                        SendCOMData(comOutBuffer, 0, length + HEADER_SIZE + CRC_SIZE);
                    }
                    catch (Exception)
                    {
                        downloadingFirmware = false;
                        this.Invoke((MethodInvoker)delegate
                        {
                            EnableDeviceCmd(true);
                            UpdateProgressBar(false);
                        });
                        MessageBox.Show(FIRMWARE_DOWNLOADED_FAILED_STR.Text, APP_CAPTION_STR.Text);
                        return;
                    }
                    break;

                case SET_CAMERA_ID_OPCODE:
                    packetNo = BitConverter.ToUInt32(comInBuffer, PACKET_NO_OFFSET);
                    if (packetNo == FINISH_PACKET_NO)
                    {
                        MessageBox.Show(SET_CAMERA_ID_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
                    }
                    else
                    {
                        MessageBox.Show(SET_CAMERA_ID_FAILED_STR.Text, APP_CAPTION_STR.Text);
                    }
                    break;
                case 0x5A:  // ACK
                    break;
                case 0xA5:  // NACK
                    break;
            }
        }
        #endregion

        private void SendHIDData(byte[] buffer, int offset, int count, int timeout = DEFAULT_COMM_TIMEOUT)
        {
            int retryCount, copyIndex, byteToSend;

            copyIndex = 0;
            while (copyIndex < count)
            {
                byteToSend = count - copyIndex;
                byteToSend = (byteToSend > 64) ? 64 : byteToSend;
                Array.Clear(hidSendBuffer, 0, hidSendBuffer.Length);
                Array.Copy(buffer, offset + copyIndex, hidSendBuffer, 1, byteToSend);
                copyIndex += byteToSend;
                retryCount = 0;
                while (true)
                {
                    try
                    {
                        hidDevice.writeStream.Write(hidSendBuffer, 0, hidSendBuffer.Length);
                        break;
                    }
                    catch (Exception)
                    {
                        if (retryCount++ > MAX_COMM_RETRY)
                        {
                            //ReportHIDWriteTimedOut();
                            return;
                        }
                    }
                }
            }
            if (timeout > DEFAULT_COMM_TIMEOUT)
            { 
                //TODO: start timeout timer here
            }
        }

        private void SendCOMData(byte[] buffer, int offset, int count, int timeout = DEFAULT_COMM_TIMEOUT)
        {
            int retryCount;

            serialPort.WriteTimeout = timeout;
            retryCount = 0;
            while (true)
            {
                try
                {
                    serialPort.Write(buffer, offset, count);
                    break;
                }
                catch (Exception)
                {
                    if (retryCount++ > MAX_COMM_RETRY)
                    {
                        ReportCOMWriteTimedOut();
                        return;
                    }
                }
            }
            if (timeout > DEFAULT_COMM_TIMEOUT)
            {
                comRetryBuffer = new byte[count];
                Array.Copy(buffer, offset, comRetryBuffer, 0, count);
                comRetryTimeout = timeout;
                comTimeoutTimer.Stop();
                comTimeoutTimer.Interval = timeout;
                comTimeoutTimer.Start();
            }
        }

        #region Event handler
        private void readSettingsBtn_Clicked(object sender, EventArgs e)
        {
            ReadConfig();
        }

        private void GetConfigFromUI()
        {
            cfg = tempCfg;
        }

        private void writeSettingsBtn_Clicked(object sender, EventArgs e)
        {
            GetConfigFromUI();

            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                EnableDeviceCmd(true);
                cfgData = Lib.ObjectToByteArray(cfg);

                CreateHIDBuffer(PACKET_NO_SIZE, WRITE_CONFIG_OPCODE, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                cfgDataOffset = 0;
                cfgDeviceBuffSize = 0;
                cfgDataSize = 0;
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE, 1000);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }

                EnableDeviceCmd(false);
                cfgData = Lib.ObjectToByteArray(cfg);

                CreateCOMBuffer(PACKET_NO_SIZE, WRITE_CONFIG_OPCODE, DEFAULT_PACKET_NO);
                comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                cfgDataOffset = 0;
                cfgDeviceBuffSize = 0;
                cfgDataSize = 0;
                SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE, 1000);
            }
        }

        private void EnableDeviceCmd(bool enabled)
        {
            deviceCmdGBox.Enabled = enabled;
            EnableCommTypeChange(enabled);
            readFirmwareVersionBtn.Enabled = enabled;
            downloadFirmwareBtn.Enabled = enabled;
        }

        private void resetCPUBtn_Clicked(object sender, EventArgs e)
        {
            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                CreateHIDBuffer(PACKET_NO_SIZE, RESET_DEVICE_OPCODE, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }

                CreateCOMBuffer(PACKET_NO_SIZE, RESET_DEVICE_OPCODE, DEFAULT_PACKET_NO);
                comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
        }

        private void factoryResetBtn_Clicked(object sender, EventArgs e)
        {
            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                CreateHIDBuffer(PACKET_NO_OFFSET, RESET_FACTORY_OPCODE, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }

                CreateCOMBuffer(PACKET_NO_SIZE, RESET_FACTORY_OPCODE, DEFAULT_PACKET_NO);
                comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
        }
        #endregion

        private void formatSDCardBtn_Clicked(object sender, EventArgs e)
        {
            if ((communicationType == USB_COMMUNICATION) && (!hidDevice.connected))
            {
                return;
            }
            if ((communicationType == COM_COMMUNICATION) && ((serialPort == null) || (!serialPort.IsOpen)))
            {
                return;
            }
            // "Format the SD card will take several minutes to complete. Do not disconnect the power supply during the formatting operation!"
            if (MessageBox.Show(FORMAT_SDCARD_WARNING_STR.Text, APP_CAPTION_STR.Text, MessageBoxButtons.OKCancel, MessageBoxIcon.Warning) != System.Windows.Forms.DialogResult.OK) return;

            if (communicationType == USB_COMMUNICATION)
            {
                CreateHIDBuffer(PACKET_NO_SIZE, FORMAT_SDCARD_OPCODE, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
            else
            {
                CreateCOMBuffer(PACKET_NO_SIZE, FORMAT_SDCARD_OPCODE, DEFAULT_PACKET_NO);
                comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Environment.Exit(0);
        }

        private void clearReportBtn_Click(object sender, EventArgs e)
        {
            ClearAllReport();
        }

        private void ClearAllReport()
        {
            ClearReportGirdView();
            Array.Clear(reportUserNameAdded, 0, 10);
        }

        private void ClearReportGirdView()
        {
            detailReportGridView.Rows.Clear();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] b;
            int length;
            int copyIndex;

            length = serialPort.BytesToRead;
            b = new byte[length];
            serialPort.Read(b, 0, length);
            if (comBufIndex == 0)
            {
                copyIndex = -1;
                for (int i = 0; i < b.Length; i++)
                {
                    if (b[i] == COMM_START_CODE)
                    {
                        copyIndex = i;
                        break;
                    }
                }
                if (copyIndex < 0)
                {
                    return;
                }
                Array.Clear(comTempBuffer, 0, comTempBuffer.Length);
                Array.Copy(b, copyIndex, comTempBuffer, 0, b.Length - copyIndex);
                comBufIndex += b.Length - copyIndex;
            }
            else
            {
                Array.Copy(b, 0, comTempBuffer, comBufIndex, b.Length);
                comBufIndex += b.Length;
            }
            if (comBufIndex >= (BitConverter.ToInt16(comTempBuffer, LENGTH_OFFSET) + HEADER_SIZE + CRC_SIZE))
            {
                comBufIndex = 0;
                Array.Copy(comTempBuffer, comInBuffer, comInBuffer.Length);
                ProcessReceivedCOMData();
            }
        }

        private byte calcCRC(byte[] buf, int index, int length)
        {
            byte crc;

            crc = 0;
            for (int i = index; i < index + length; i++)
            {
                crc += buf[i];
            }

            return crc;
        }

        private uint calcCRC32(byte[] buf, int index, int length)
        {
            uint crc;

            crc = 0;
            for (int i = index; i < index + length; i++)
            {
                crc += buf[i];
            }

            return crc;
        }

        private void usbMenuItem_Click(object sender, EventArgs e)
        {
            if (hidDevice.connected)
            {
                return;
            }

            commuTypeLbl.Text = "USB";
            communicationType = USB_COMMUNICATION;
            HID.RegisterDeviceNotification(this.Handle, ref notificationHandle);
            OpenHIDDevice();

            // close COM port
            if ((serialPort != null) && (serialPort.IsOpen))
            {
                serialPort.Close();
                serialPort = null;
            }
        }

        private void gprsSettings_Changed(object sender, EventArgs e)
        {
            if (configUpdateDisabled)
            {
                return;
            }

            // gprs config
            tempCfg.gprsApn = gprsApnTxtBox.Text;
            tempCfg.gprsUser = gprsUserTxtBox.Text;
            tempCfg.gprsPass = gprsPwdTxtBox.Text;
            try
            {
                tempCfg.runReportInterval = UInt16.Parse(gprsRunReportIntervalTxtBox.Text);
                tempCfg.stopReportInterval = UInt16.Parse(gprsStopReportIntervalTxtBox.Text);
            }
            catch (Exception)
            { }
        }

        private void serverParameter_Changed(object sender, EventArgs e)
        {
            UInt32 priIp, secIp;
            string priDomain, secDomain;
            UInt16 priPort, secPort;
            bool useIp;

            if (configUpdateDisabled)
            {
                return;
            }

            priIp = secIp = priPort = secPort = 0;
            try
            {
                ParseIP(priIpTxtBox.Text, ref priIp);
                ParseIP(secIpTxtBox.Text, ref secIp);
                priPort = UInt16.Parse(priPortTxtBox.Text);
                secPort = UInt16.Parse(secPortTxtBox.Text);
            }
            catch (Exception)
            { }
            priDomain = priDomainTxtBox.Text;
            secDomain = secDomainTxtBox.Text;
            useIp = serverUseIpRBtn.Checked;
            if (useIp)
            {
                priIpTxtBox.Enabled = secIpTxtBox.Enabled = true;
                priDomainTxtBox.Enabled = secDomainTxtBox.Enabled = false;
            }
            else
            {
                priIpTxtBox.Enabled = secIpTxtBox.Enabled = false;
                priDomainTxtBox.Enabled = secDomainTxtBox.Enabled = true;
            }

            switch (serverSelectionCmbBox.SelectedIndex)
            { 
                case DATA_SERVER_SELECTED:
                    tempCfg.priDataServerIp = priIp;
                    tempCfg.priDataServerDomain = priDomain;
                    tempCfg.priDataServerPort = priPort;
                    tempCfg.secDataServerIp = secIp;
                    tempCfg.secDataServerDomain = secDomain;
                    tempCfg.secDataServerPort = secPort;
                    tempCfg.dataServerUseIp = (byte)(useIp ? SERVER_USE_IP : 0);
                    break;
                case FIRMWARE_SERVER_SELECTED:
                    tempCfg.priFirmwareServerIp = priIp;
                    tempCfg.priFirmwareServerDomain = priDomain;
                    tempCfg.priFirmwareServerPort = priPort;
                    tempCfg.secFirmwareServerIp = secIp;
                    tempCfg.secFirmwareServerDomain = secDomain;
                    tempCfg.secFirmwareServerPort = secPort;
                    tempCfg.firmwareServerUseIp = (byte)(useIp ? SERVER_USE_IP : 0);
                    break;
				case INFO_SERVER_SELECTED:
					tempCfg.priInfoServerIp = priIp;
                    tempCfg.priInfoServerDomain = priDomain;
                    tempCfg.priInfoServerPort = priPort;
                    tempCfg.secInfoServerIp = secIp;
                    tempCfg.secInfoServerDomain = secDomain;
                    tempCfg.secInfoServerPort = secPort;
                    tempCfg.infoServerUseIp = (byte)(useIp ? SERVER_USE_IP : 0);
					break;
                default:
                    break;
            }
        }

        private void serverSelection_Changed(object sender, EventArgs e)
        {
            configUpdateDisabled = true;

            switch (serverSelectionCmbBox.SelectedIndex)
            { 
                case DATA_SERVER_SELECTED:
                    priIpTxtBox.Text = (new IPAddress(tempCfg.priDataServerIp)).ToString();
                    secIpTxtBox.Text = (new IPAddress(tempCfg.secDataServerIp)).ToString();
                    priDomainTxtBox.Text = tempCfg.priDataServerDomain;
                    secDomainTxtBox.Text = tempCfg.secDataServerDomain;
                    priPortTxtBox.Text = tempCfg.priDataServerPort.ToString();
                    secPortTxtBox.Text = tempCfg.secDataServerPort.ToString();
                    serverUseIpRBtn.Checked = (tempCfg.dataServerUseIp == SERVER_USE_IP);
                    priIpTxtBox.Enabled = secIpTxtBox.Enabled = serverUseIpRBtn.Checked;
                    serverUseDomainRBtn.Checked = !serverUseIpRBtn.Checked;
                    priDomainTxtBox.Enabled = secDomainTxtBox.Enabled = !serverUseIpRBtn.Checked;
                    break;
                case FIRMWARE_SERVER_SELECTED:
                    priIpTxtBox.Text = (new IPAddress(tempCfg.priFirmwareServerIp)).ToString();
                    secIpTxtBox.Text = (new IPAddress(tempCfg.secFirmwareServerIp)).ToString();
                    priDomainTxtBox.Text = tempCfg.priFirmwareServerDomain;
                    secDomainTxtBox.Text = tempCfg.secFirmwareServerDomain;
                    priPortTxtBox.Text = tempCfg.priFirmwareServerPort.ToString();
                    secPortTxtBox.Text = tempCfg.secFirmwareServerPort.ToString();
                    serverUseIpRBtn.Checked = (tempCfg.firmwareServerUseIp == SERVER_USE_IP);
                    priIpTxtBox.Enabled = secIpTxtBox.Enabled = serverUseIpRBtn.Checked;
                    serverUseDomainRBtn.Checked = !serverUseIpRBtn.Checked;
                    priDomainTxtBox.Enabled = secDomainTxtBox.Enabled = !serverUseIpRBtn.Checked;
                    break;
				case INFO_SERVER_SELECTED:
					priIpTxtBox.Text = (new IPAddress(tempCfg.priInfoServerIp)).ToString();
                    secIpTxtBox.Text = (new IPAddress(tempCfg.secInfoServerIp)).ToString();
                    priDomainTxtBox.Text = tempCfg.priInfoServerDomain;
                    secDomainTxtBox.Text = tempCfg.secInfoServerDomain;
                    priPortTxtBox.Text = tempCfg.priInfoServerPort.ToString();
                    secPortTxtBox.Text = tempCfg.secInfoServerPort.ToString();
                    serverUseIpRBtn.Checked = (tempCfg.infoServerUseIp == SERVER_USE_IP);
                    priIpTxtBox.Enabled = secIpTxtBox.Enabled = serverUseIpRBtn.Checked;
                    serverUseDomainRBtn.Checked = !serverUseIpRBtn.Checked;
                    priDomainTxtBox.Enabled = secDomainTxtBox.Enabled = !serverUseIpRBtn.Checked;
					break;
                default:
                    break;
            }

            configUpdateDisabled = false;
        }

        private void vehicleParameter_Changed(object sender, EventArgs e)
        {
            if (configUpdateDisabled)
            {
                return;
            }
            
            tempCfg.trackerId = trackerIdTxtBox.Text;
            tempCfg.plateNo = plateNoTxtBox.Text;
            tempCfg.vin = vinNumTxtBox.Text;
            tempCfg.whiteCallerNo = bossPhoneTxtBox.Text;
            try
            {
                tempCfg.speedSensorRatio = UInt16.Parse(tyreRateTxtBox.Text);
            }
            catch (Exception)
            { }
            if (gpsSpeedSourceRBtn.Checked)
            {
                tempCfg.useGpsSpeed = GPS_SPEED_SOURCE;
                tyreRateTxtBox.Enabled = false;
            }
            else
            {
                tempCfg.useGpsSpeed = 0;
                tyreRateTxtBox.Enabled = true;
            }
        }

        private void userListInfo_Changed(object sender, EventArgs e)
        {
            int user;

            if (configUpdateDisabled)
            {
                return;
            }

            user = userNoCmbBox.SelectedIndex;
            tempCfg.userList[user].userName = authUserNameTxtBox.Text;
            tempCfg.userList[user].phoneNo = authPhoneNoTxtBox.Text;
        }

        private void userListIndex_Changed(object sender, EventArgs e)
        {
            int user;

            configUpdateDisabled = true;

            user = userNoCmbBox.SelectedIndex;
            authUserNameTxtBox.Text = tempCfg.userList[user].userName;
            authPhoneNoTxtBox.Text = tempCfg.userList[user].phoneNo;

            configUpdateDisabled = false;
        }

        private void analogParameter_Changed(object sender, EventArgs e)
        {
            int analog;

            if (configUpdateDisabled)
            {
                return;
            }

            analog = analogInputNoCmbBox.SelectedIndex;
            try
            {
                switch (analog)
                {
                    case ANALOG_INPUT_1:
                        tempCfg.a1Mode = (UInt16)(analogPercentageModeChkBox.Checked ? ANALOG_PERCENTAGE_MODE : 0);
                        tempCfg.a1UpperBound = UInt16.Parse(analogUpperBoundTxtBox.Text);
                        tempCfg.a1LowerBound = UInt16.Parse(analogLowerBoundTxtBox.Text);
                        break;
                    case ANALOG_INPUT_2:
                        tempCfg.a2Mode = (UInt16)(analogPercentageModeChkBox.Checked ? ANALOG_PERCENTAGE_MODE : 0);
                        tempCfg.a2UpperBound = UInt16.Parse(analogUpperBoundTxtBox.Text);
                        tempCfg.a2LowerBound = UInt16.Parse(analogLowerBoundTxtBox.Text);
                        break;
                }
            }
            catch (Exception)
            { }
        }

        private void analogSelection_Changed(object sender, EventArgs e)
        {
            int analog;

            configUpdateDisabled = true;

            analog = analogInputNoCmbBox.SelectedIndex;
            switch (analog)
            { 
                case ANALOG_INPUT_1:
                    analogPercentageModeChkBox.Checked = (tempCfg.a1Mode == ANALOG_PERCENTAGE_MODE);
                    analogUpperBoundTxtBox.Text = tempCfg.a1UpperBound.ToString();
                    analogLowerBoundTxtBox.Text = tempCfg.a1LowerBound.ToString();
                    break;
                case ANALOG_INPUT_2:
                    analogPercentageModeChkBox.Checked = (tempCfg.a2Mode == ANALOG_PERCENTAGE_MODE);
                    analogUpperBoundTxtBox.Text = tempCfg.a2UpperBound.ToString();
                    analogLowerBoundTxtBox.Text = tempCfg.a2LowerBound.ToString();
                    break;
            }

            configUpdateDisabled = false;
        }

        private void drivingSettings_Changed(object sender, EventArgs e)
        {
            if (configUpdateDisabled)
            {
                return;
            }

            try
            {
                tempCfg.speedLimit = UInt16.Parse(speedLimitTxtBox.Text);
                tempCfg.drivingTimeLimit = (UInt32)(float.Parse(drivingTimeLimitTxtBox.Text) * 3600);
                tempCfg.totalDrivingTimeLimit = (UInt32)(float.Parse(totalDrivingTimeLimitTxtBox.Text) * 3600);
                tempCfg.baseMileage = (UInt32)(float.Parse(baseMileageTxtBox.Text));
            }
            catch (Exception)
            { }
        }

        private void commTypeCmbBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            communicationType = commTypeCmbBox.SelectedIndex;
            if (communicationType == USB_COMMUNICATION)
            {
                comPortCmbBox.Enabled = false;
                openPortBtn.Enabled = false;
                if (hidDevice.connected)
                {
                    return;
                }

                commuTypeLbl.Text = "USB";
                communicationType = USB_COMMUNICATION;
                HID.RegisterDeviceNotification(this.Handle, ref notificationHandle);
                OpenHIDDevice();

                // close COM port
                if ((serialPort != null) && (serialPort.IsOpen))
                {
                    serialPort.Close();
                    serialPort = null;
                }
            }
            else
            {
                comPortCmbBox.Items.Clear();
                comPortCmbBox.Enabled = true;
                openPortBtn.Enabled = true;
                foreach (string portName in SerialPort.GetPortNames())
                {
                    comPortCmbBox.Items.Add(portName);
                }
                if (comPortCmbBox.Items.Count > 0)
                {
                    comPortCmbBox.SelectedIndex = 0;
                }

                // close HID device
                if ((hidDevice != null) && (hidDevice.connected))
                {
                    CloseHIDDevice();
                    HID.UnregisterDeviceNotification(notificationHandle);
                }
            }

            UpdateUI();
        }

        private void openPortBtn_Click(object sender, EventArgs e)
        {
            if ((serialPort != null) && (serialPort.IsOpen))
            {
                serialPort.Close();
            }

            // open COM Port
            serialPort = new SerialPort(comPortCmbBox.Text);
            serialPort.BaudRate = 115200;
            //serialPort.WriteTimeout = 1000;
            try
            {
                serialPort.Open();
            }
            catch (Exception)
            {
                // "Cannot open port "
                MessageBox.Show(CANNOT_OPEN_COMPORT_STR.Text + serialPort.PortName, APP_CAPTION_STR.Text);
                return;
            }
            commuTypeLbl.Text = serialPort.PortName;
            communicationType = COM_COMMUNICATION;

            // stop USB communication
            CloseHIDDevice();
            HID.UnregisterDeviceNotification(notificationHandle);

            // start COM communication
            comBufIndex = 0;
            serialPort.DataReceived += new SerialDataReceivedEventHandler(serialPort_DataReceived);
            ReadConfig();
        }

        private void selectLogTypeCmbBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            int logOpcode;

            switch (selectLogTypeCmbBox.SelectedIndex)
            { 
                case 0:
                    logOpcode = DISABLE_LOG_OPCODE;
                    break;
                case 1:
                    logOpcode = ENABLE_GPS_LOG_OPCODE;
                    break;
                case 2:
                    logOpcode = ENABLE_GSM_LOG_OPCODE;
                    break;
                case 3:
                    logOpcode = ENABLE_MCU_LOG_OPCODE;
                    break;
                case 4:
                    logOpcode = ENABLE_LOG_OPCODE;
                    break;
                default:
                    logOpcode = DISABLE_LOG_OPCODE;
                    break;
            }
            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                CreateHIDBuffer(PACKET_NO_SIZE, (byte)logOpcode, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }
                
                CreateCOMBuffer(PACKET_NO_SIZE, (byte)logOpcode, DEFAULT_PACKET_NO);
                comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
        }

        private void saveSettingsBtn_Click(object sender, EventArgs e)
        {
            string fileName;
            DialogResult res;
            BinaryWriter writer;

            saveFileDialog1.Filter = "CFG file (*.CFG)|*.CFG";
            res = saveFileDialog1.ShowDialog();
            if (res != DialogResult.OK)
            {
                return;
            }
            fileName = saveFileDialog1.FileName;
            writer = new BinaryWriter(new FileStream(fileName, FileMode.CreateNew));
            cfg = tempCfg;
            writer.Write(Lib.ObjectToByteArray(cfg));
            writer.Close();
            MessageBox.Show(CONFIG_SAVE_SUCCESS_STR.Text, APP_CAPTION_STR.Text);
        }

        private void loadSettingsBtn_Click(object sender, EventArgs e)
        {
            string fileName;
            DialogResult res;
            BinaryReader reader;
            byte[] b;

            openFileDialog1.Filter = "CFG file (*.CFG)|*.CFG";
            res = openFileDialog1.ShowDialog();
            if (res != DialogResult.OK)
            {
                return;
            }
            fileName = openFileDialog1.FileName;
            if (!File.Exists(fileName))
            {
                return;
            }
            try
            {
                reader = new BinaryReader(new FileStream(fileName, FileMode.Open));
                b = reader.ReadBytes(GPS_CONFIG_DATA_SIZE);
                cfg = (GPS_CONFIG_DATA)Lib.ByteArrayToObject(b, typeof(GPS_CONFIG_DATA));
                reader.Close();
                UpdateConfigUI();
                MessageBox.Show(CONFIG_LOADED_STR.Text, APP_CAPTION_STR.Text);
            }
            catch (Exception)
            { }
        }

        private void englishToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChangeLanguage("en");
        }

        private void vietnameseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChangeLanguage("vi");
        }

        ResourceString testStr = new ResourceString();

        private void ChangeLanguage(string lang)
        {
            ComponentResourceManager resources = new ComponentResourceManager(typeof(MainForm));
            CultureInfo culture = new CultureInfo(lang);

            foreach (Control c in this.Controls)
            {

                ApplyControlResources(c, resources, culture);
            }
            foreach (ToolStripMenuItem item in menuStrip1.Items)
            {
                ApplyMenuItemResources(item, resources, culture);
            }
            foreach (TabPage tab in tabMain.TabPages)
            {
                resources.ApplyResources(tab, tab.Name, culture);
            }
            foreach (DataGridViewColumn col in detailReportGridView.Columns)
            {
                resources.ApplyResources(col, col.Name, culture);
            }

            LoadLocalizedString(lang);
            UpdateUI();
            ApplyLayoutChanges();
        }

        private void LoadLocalizedString(string lang)
        {
            ResourceManager rm;

            if (lang.Equals("vi"))
            {
                rm = Resources.ViStrings.ResourceManager;
            }
            else
            {
                rm = Resources.EnStrings.ResourceManager;
            }

            for (int i = 0; i < localizedStrings.Count; i++)
            {
                localizedStrings[i].Text = rm.GetString(localizedStrings[i].Name);
            }

            serverSelectionCmbBox.Items[0] = DATA_SERVER_CMBBOX_ITEM_STR.Text;
            //serverSelectionCmbBox.Items[1] = FIRMWARE_SERVER_CMBBOX_ITEM_STR.Text;
			//serverSelectionCmbBox.Items[2] = INFO_SERVER_CMBBOX_ITEM_STR.Text;
            openFileDialog1.Title = APP_CAPTION_STR.Text + " - " + OPEN_FILE_DIALOG_TITLE_STR.Text;
            saveFileDialog1.Title = APP_CAPTION_STR.Text + " - " + SAVE_FILE_DIALOG_TITLE_STR.Text;
            selectFolderDialog.Description = APP_CAPTION_STR.Text + " - " + OPEN_FOLDER_DIALOG_TITLE_STR.Text;
        }

        private void ApplyControlResources(Control control, ComponentResourceManager resources, CultureInfo culture)
        {
            resources.ApplyResources(control, control.Name, culture);
            foreach (Control c in control.Controls)
            {
                ApplyControlResources(c, resources, culture);
            }
        }

        private void ApplyMenuItemResources(ToolStripMenuItem item, ComponentResourceManager resources, CultureInfo culture)
        {
            resources.ApplyResources(item, item.Name, culture);
            foreach (ToolStripMenuItem i in item.DropDownItems)
            {
                ApplyMenuItemResources(i, resources, culture);
            }
        }

        private void ApplyLayoutChanges()
        {
            tabMain.Height = statusGrpBox.Top - 106;
            tabMain.Width = statusGrpBox.Width - 10;
        }

        private void readFirmwareVersionBtn_Click(object sender, EventArgs e)
        {
            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                CreateHIDBuffer(PACKET_NO_SIZE, READ_FIRMWARE_VERSION_OPCODE, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }

                CreateCOMBuffer(PACKET_NO_SIZE, READ_FIRMWARE_VERSION_OPCODE, DEFAULT_PACKET_NO);
                comOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(comOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendCOMData(comOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
        }

        private void browseFirmwareBtn_Click(object sender, EventArgs e)
        {
            string fileName;
            DialogResult res;

            openFileDialog1.Filter = "BIN file (*.BIN)|*.BIN";
            res = openFileDialog1.ShowDialog();
            if (res != DialogResult.OK)
            {
                return;
            }
            fileName = openFileDialog1.FileName;
            if (!File.Exists(fileName))
            {
                return;
            }
            firmwarePathTxtBox.Text = fileName;
        }

        private void downloadFirmwareBtn_Click(object sender, EventArgs e)
        {
            string fileName;
            byte[] fileSize;
            byte[] fileCRC;

            fileName = firmwarePathTxtBox.Text;
            if (!File.Exists(fileName))
            {
                MessageBox.Show(FILE_NAME_IS_INVALID_WARNING_STR.Text, APP_CAPTION_STR.Text);
                return;
            }
            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                if (firmwareReader != null)
                {
                    try
                    {
                        firmwareReader.Close();
                    }
                    catch (Exception)
                    { }
                }

                fileCRC = BitConverter.GetBytes(CalcFileCRC(firmwarePathTxtBox.Text));
                try
                {
                    firmwareReader = new BinaryReader(new FileStream(firmwarePathTxtBox.Text, FileMode.Open));
                }
                catch (Exception)
                {
                    MessageBox.Show(CANNOT_OPEN_FILE_STR.Text + fileName, APP_CAPTION_STR.Text);
                    return;
                }

                downloadingFirmware = true;
                EnableDeviceCmd(false);
                fileSize = BitConverter.GetBytes(firmwareReader.BaseStream.Length);
                CreateHIDBuffer(PACKET_NO_SIZE + 8, DOWNLOAD_FIRMWARE_OPCODE, DEFAULT_PACKET_NO);
                Array.Copy(fileSize, 0, hidOutBuffer, DATA_WITH_PACKET_NO_OFFSET, 4);
                Array.Copy(fileCRC, 0, hidOutBuffer, DATA_WITH_PACKET_NO_OFFSET + 4, 4);
                hidOutBuffer[DATA_WITH_PACKET_NO_OFFSET + 8] = calcCRC(hidOutBuffer, DATA_OFFSET, PACKET_NO_SIZE + 8);
                SendHIDData(hidOutBuffer, 0, HEADER_SIZE + PACKET_NO_SIZE + 8 + CRC_SIZE);
                retrieveLogPrgBar.Maximum = (int)(firmwareReader.BaseStream.Length / 100);
                retrieveLogStatusLbl.Text = DOWNLOADING_FIRMWARE_STR.Text;
                UpdateProgressBar(true);
            }
            else
            {
                if (!serialPort.IsOpen)
                {
                    return;
                }

                if (firmwareReader != null)
                {
                    try
                    {
                        firmwareReader.Close();
                    }
                    catch (Exception)
                    { }
                }

                fileCRC = BitConverter.GetBytes(CalcFileCRC(firmwarePathTxtBox.Text));
                try
                {
                    firmwareReader = new BinaryReader(new FileStream(firmwarePathTxtBox.Text, FileMode.Open));
                }
                catch (Exception)
                {
                    MessageBox.Show(CANNOT_OPEN_FILE_STR.Text + fileName, APP_CAPTION_STR.Text);
                    return;
                }

                downloadingFirmware = true;
                EnableDeviceCmd(false);
                fileSize = BitConverter.GetBytes(firmwareReader.BaseStream.Length);
                CreateCOMBuffer(PACKET_NO_SIZE + 8, DOWNLOAD_FIRMWARE_OPCODE, DEFAULT_PACKET_NO);
                Array.Copy(fileSize, 0, comOutBuffer, DATA_WITH_PACKET_NO_OFFSET, 4);
                Array.Copy(fileCRC, 0, comOutBuffer, DATA_WITH_PACKET_NO_OFFSET + 4, 4);
                comOutBuffer[DATA_WITH_PACKET_NO_OFFSET + 8] = calcCRC(comOutBuffer, DATA_OFFSET, PACKET_NO_SIZE + 8);
                SendCOMData(comOutBuffer, 0, HEADER_SIZE + PACKET_NO_SIZE + 8 + CRC_SIZE);
                retrieveLogPrgBar.Maximum = (int)(firmwareReader.BaseStream.Length/ 100);
                retrieveLogStatusLbl.Text = DOWNLOADING_FIRMWARE_STR.Text;
                UpdateProgressBar(true);
            }
        }

        private void clearLogBtn_Click(object sender, EventArgs e)
        {
            lock (logTxtBox)
            {
                logTxtBox.Clear();
            }
        }

        private void sendSMSBtn_Click(object sender, EventArgs e)
        {
            SendSMS();
        }

        private void tabMain_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (bootMode)
            {
                tabMain.SelectedTab = settingTabPage;
            }
        }

        private void jumpToAppBtn_Click(object sender, EventArgs e)
        {
            if (communicationType == USB_COMMUNICATION)
            {
                if (!hidDevice.connected)
                {
                    return;
                }

                CreateHIDBuffer(PACKET_NO_SIZE, JUMP_TO_APP_OPCODE, DEFAULT_PACKET_NO);
                hidOutBuffer[SIMPLE_CRC_WITH_PACKET_NO_OFFSET] = calcCRC(hidOutBuffer, PACKET_NO_OFFSET, PACKET_NO_SIZE);
                SendHIDData(hidOutBuffer, 0, SIMPLE_PACKET_SIZE);
            }
        }

        private uint CalcFileCRC(string fileName)
        {
            List<byte> temp;
            uint crc;

            temp = ReadAllByteInFile(fileName);
            crc = calcCRC32(temp.ToArray(), 0, temp.Count);
            temp.Clear();
            temp = null;

            return crc;
        }

        private List<byte> ReadAllByteInFile(string fileName)
        {
            BinaryReader reader;
            long fileSize;
            List<byte> buffer;
            byte[] tempBuf;
            int bufIndex;

            reader = null;
            try
            {
                reader = new BinaryReader(new FileStream(fileName, FileMode.Open));
            }
            catch (Exception)
            {
                return null;
            }
            if (reader == null)
            {
                return null;
            }

            fileSize = reader.BaseStream.Length;
            buffer = new List<byte>();
            bufIndex = 0;
            while (bufIndex < fileSize)
            {
                tempBuf = reader.ReadBytes(1024);
                buffer.AddRange(tempBuf);
                bufIndex += tempBuf.Length;
            }
            reader.Close();
            reader = null;

            return buffer;
        }

        private void registerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RegisterForm rForm;
            
            rForm = new RegisterForm();
            rForm.ShowDialog();
            registerToolStripMenuItem.Visible = !rForm.isRegistered;
            rForm.Dispose();
            rForm = null;
        }

        private void featureSet_Changed(object sender, EventArgs e)
        {
			if (autoSecurityOnChkBox.Checked)
			{
				sendSMSActionChkBox.Enabled = true;
				callActionChkBox.Enabled = true;
			}
			else if (!manualSecurityOnChkBox.Checked)
			{
				sendSMSActionChkBox.Enabled = false;
				callActionChkBox.Enabled = false;
			}

            if (configUpdateDisabled)
            {
                return;
            }

			tempCfg.featureSet = 0;
            tempCfg.featureSet |= (ushort)((lowBatteryWarningChkBox.Checked ? 1 : 0) << 4);
            tempCfg.featureSet |= (ushort)((lowPowerWarningChkBox.Checked ? 1 : 0) << 5);
            tempCfg.featureSet |= (ushort)((removePowerWarningChkBox.Checked ? 1 : 0) << 8);
            tempCfg.featureSet |= (ushort)((autoSecurityOnChkBox.Checked ? 1 : 0) << 9);
			tempCfg.featureSet |= (ushort)((enableFifoChkBox.Checked ? 1 : 0) << 10);
        }

        private void otherSettings_Changed(object sender, EventArgs e)
        {
            if (manualSecurityOnChkBox.Checked)
            {
                sendSMSActionChkBox.Enabled = true;
                callActionChkBox.Enabled = true;
            }
            else if (!autoSecurityOnChkBox.Checked)
            {
                sendSMSActionChkBox.Enabled = false;
                callActionChkBox.Enabled = false;
            }

            if (configUpdateDisabled)
            {
                return;
            }

            tempCfg.securityOn = (byte)(manualSecurityOnChkBox.Checked ? 1 : 0);
            tempCfg.securityAction = 0;
            tempCfg.securityAction |= (byte)(sendSMSActionChkBox.Checked ? 1 : 0);
            tempCfg.securityAction |= (byte)((callActionChkBox.Checked ? 1 : 0) << 1);
            byte.TryParse(sosKeyPressTimeTxt.Text, out tempCfg.SOSKeyPressTimes);
            byte.TryParse(sosKeyPressPeriodTxt.Text, out tempCfg.SOSKeyPressPeriod);
            UInt32.TryParse(removePowerWarningPeriodTxt.Text, out tempCfg.removePowerWarningPeriod);
            UInt32.TryParse(batteryLowWarningPeriodTxt.Text, out tempCfg.batteryLowWarningPeriod);
            UInt32.TryParse(speedEngineOffTxt.Text, out tempCfg.speedEngineOff);
            UInt32.TryParse(powerOffDelayTimeTxt.Text, out tempCfg.powerOffDelayTime);
            byte.TryParse(batteryLowWarningLevelTxt.Text, out tempCfg.batteryLowWarningLevel);
			UInt32.TryParse(autoSecurityTimeTxt.Text, out tempCfg.autoSecurityTime);
			byte accountAlarmCheck = 0;
			byte.TryParse(accountAlarmCheckTxt.Text, out accountAlarmCheck);
			if ((accountAlarmCheck >= 0) && (accountAlarmCheck <= 23))
			{
				tempCfg.accountAlarmCheck = accountAlarmCheck;
			}
        }

        private void readReportBtn_Click(object sender, EventArgs e)
        {

        }

        private void exportToKMLBtn_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Filter = "KML Files (*.KML) | *.KML";
            if (saveFileDialog1.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            drivingReport.ExportToKML(cfg, saveFileDialog1.FileName);
        }
    }

    public class ResourceString
    {
        string name;
        string text;

        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        public string Text
        {
            get { return text; }
            set { text = value; }
        }
    }
}
