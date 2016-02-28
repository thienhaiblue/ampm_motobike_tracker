namespace Motor_Tracker_Configuration
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.tabMain = new System.Windows.Forms.TabControl();
			this.settingTabPage = new System.Windows.Forms.TabPage();
			this.grbOtherSettings = new System.Windows.Forms.GroupBox();
			this.autoSecurityTimeTxt = new System.Windows.Forms.TextBox();
			this.label8 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.autoSecurityOnChkBox = new System.Windows.Forms.CheckBox();
			this.callActionChkBox = new System.Windows.Forms.CheckBox();
			this.manualSecurityOnChkBox = new System.Windows.Forms.CheckBox();
			this.sendSMSActionChkBox = new System.Windows.Forms.CheckBox();
			this.label15 = new System.Windows.Forms.Label();
			this.powerOffDelayTimeTxt = new System.Windows.Forms.TextBox();
			this.label16 = new System.Windows.Forms.Label();
			this.speedEngineOffTxt = new System.Windows.Forms.TextBox();
			this.label17 = new System.Windows.Forms.Label();
			this.sosKeyPressTimeTxt = new System.Windows.Forms.TextBox();
			this.sosKeyPressPeriodTxt = new System.Windows.Forms.TextBox();
			this.label9 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.grbFeatureSet = new System.Windows.Forms.GroupBox();
			this.label19 = new System.Windows.Forms.Label();
			this.accountAlarmCheckTxt = new System.Windows.Forms.TextBox();
			this.enableFifoChkBox = new System.Windows.Forms.CheckBox();
			this.label18 = new System.Windows.Forms.Label();
			this.removePowerWarningChkBox = new System.Windows.Forms.CheckBox();
			this.lowPowerWarningChkBox = new System.Windows.Forms.CheckBox();
			this.label20 = new System.Windows.Forms.Label();
			this.lowBatteryWarningChkBox = new System.Windows.Forms.CheckBox();
			this.label14 = new System.Windows.Forms.Label();
			this.batteryLowWarningLevelTxt = new System.Windows.Forms.TextBox();
			this.removePowerWarningPeriodTxt = new System.Windows.Forms.TextBox();
			this.label13 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.batteryLowWarningPeriodTxt = new System.Windows.Forms.TextBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.sendSMSBtn = new System.Windows.Forms.Button();
			this.smsMessageTxt = new System.Windows.Forms.TextBox();
			this.smsAddressTxt = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.grbDrivingSettings = new System.Windows.Forms.GroupBox();
			this.label4 = new System.Windows.Forms.Label();
			this.baseMileageTxtBox = new System.Windows.Forms.TextBox();
			this.totalDrivingTimeLimitTxtBox = new System.Windows.Forms.TextBox();
			this.drivingTimeLimitTxtBox = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.speedLimitTxtBox = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.groupBox10 = new System.Windows.Forms.GroupBox();
			this.runAppBtn = new System.Windows.Forms.Button();
			this.firmwareVersionLbl = new System.Windows.Forms.Label();
			this.downloadFirmwareBtn = new System.Windows.Forms.Button();
			this.readFirmwareVersionBtn = new System.Windows.Forms.Button();
			this.browseFirmwareBtn = new System.Windows.Forms.Button();
			this.firmwarePathTxtBox = new System.Windows.Forms.TextBox();
			this.grbAnalogInput = new System.Windows.Forms.GroupBox();
			this.analogLowerBoundTxtBox = new System.Windows.Forms.TextBox();
			this.label31 = new System.Windows.Forms.Label();
			this.analogUpperBoundTxtBox = new System.Windows.Forms.TextBox();
			this.label37 = new System.Windows.Forms.Label();
			this.analogPercentageModeChkBox = new System.Windows.Forms.CheckBox();
			this.analogInputNoCmbBox = new System.Windows.Forms.ComboBox();
			this.grbAuthorizedUser = new System.Windows.Forms.GroupBox();
			this.authPhoneNoTxtBox = new System.Windows.Forms.TextBox();
			this.lblUserNumber = new System.Windows.Forms.Label();
			this.userNoCmbBox = new System.Windows.Forms.ComboBox();
			this.lblAuthorizedUserName = new System.Windows.Forms.Label();
			this.authUserNameTxtBox = new System.Windows.Forms.TextBox();
			this.lblAuthorizedPhoneNumber = new System.Windows.Forms.Label();
			this.grbVehicleParameter = new System.Windows.Forms.GroupBox();
			this.bossPhoneTxtBox = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.pulseSpeedSourceRBtn = new System.Windows.Forms.RadioButton();
			this.gpsSpeedSourceRBtn = new System.Windows.Forms.RadioButton();
			this.lblSpeedSource = new System.Windows.Forms.Label();
			this.tyreRateTxtBox = new System.Windows.Forms.TextBox();
			this.vinNumTxtBox = new System.Windows.Forms.TextBox();
			this.plateNoTxtBox = new System.Windows.Forms.TextBox();
			this.trackerIdTxtBox = new System.Windows.Forms.TextBox();
			this.lblTyreRate = new System.Windows.Forms.Label();
			this.lblCarNumber = new System.Windows.Forms.Label();
			this.lblTrackerID = new System.Windows.Forms.Label();
			this.lblVINNumber = new System.Windows.Forms.Label();
			this.grbServerParameter = new System.Windows.Forms.GroupBox();
			this.serverSelectionCmbBox = new System.Windows.Forms.ComboBox();
			this.lblServerSelection = new System.Windows.Forms.Label();
			this.serverUseDomainRBtn = new System.Windows.Forms.RadioButton();
			this.secDomainTxtBox = new System.Windows.Forms.TextBox();
			this.priDomainTxtBox = new System.Windows.Forms.TextBox();
			this.serverUseIpRBtn = new System.Windows.Forms.RadioButton();
			this.secPortTxtBox = new System.Windows.Forms.TextBox();
			this.priPortTxtBox = new System.Windows.Forms.TextBox();
			this.secIpTxtBox = new System.Windows.Forms.TextBox();
			this.lblSecondaryServer = new System.Windows.Forms.Label();
			this.lblPort = new System.Windows.Forms.Label();
			this.priIpTxtBox = new System.Windows.Forms.TextBox();
			this.label39 = new System.Windows.Forms.Label();
			this.grbGPRS = new System.Windows.Forms.GroupBox();
			this.gprsStopReportIntervalTxtBox = new System.Windows.Forms.TextBox();
			this.gprsRunReportIntervalTxtBox = new System.Windows.Forms.TextBox();
			this.gprsPwdTxtBox = new System.Windows.Forms.TextBox();
			this.gprsUserTxtBox = new System.Windows.Forms.TextBox();
			this.gprsApnTxtBox = new System.Windows.Forms.TextBox();
			this.lblOFF = new System.Windows.Forms.Label();
			this.lblON = new System.Windows.Forms.Label();
			this.lblIntevalTime = new System.Windows.Forms.Label();
			this.lblPassword = new System.Windows.Forms.Label();
			this.lblUser = new System.Windows.Forms.Label();
			this.lblAPN = new System.Windows.Forms.Label();
			this.detailReportTabPage = new System.Windows.Forms.TabPage();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.exportToKMLBtn = new System.Windows.Forms.Button();
			this.readReportBtn = new System.Windows.Forms.Button();
			this.textBox2 = new System.Windows.Forms.TextBox();
			this.label12 = new System.Windows.Forms.Label();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.detailReportGridView = new System.Windows.Forms.DataGridView();
			this.dataGridViewTextBoxColumn12 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.userNameCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.timeCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.lgnCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.latCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.speedCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.directionCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.mileageCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.hdopCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.speedFreqCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.drivingTimeCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.totalDrivingTimeCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.doorStatusCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.airconStatusCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.engineStatusCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.adc1ValueCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.adc2ValueCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.overSpeedCntCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.parkCntCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.doorOpenCntCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.onOffEngineCntCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.parkingTimeCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.gpsDirectionCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataLogTabPage = new System.Windows.Forms.TabPage();
			this.clearLogBtn = new System.Windows.Forms.Button();
			this.logTxtBox = new System.Windows.Forms.TextBox();
			this.menuStrip1 = new System.Windows.Forms.MenuStrip();
			this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.skinToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.blueToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.blackToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.silverToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.aqueToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.defaultToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.languageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.englishToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.vietnameseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.registerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
			this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
			this.selectFolderDialog = new System.Windows.Forms.FolderBrowserDialog();
			this.imeiLbl = new System.Windows.Forms.Label();
			this.statusGrpBox = new System.Windows.Forms.GroupBox();
			this.connectionStatusLbl = new System.Windows.Forms.Label();
			this.retrieveLogPercentLbl = new System.Windows.Forms.Label();
			this.retrieveLogStatusLbl = new System.Windows.Forms.Label();
			this.commuTypeLbl = new System.Windows.Forms.Label();
			this.retrieveLogPrgBar = new System.Windows.Forms.ProgressBar();
			this.btnPrint = new System.Windows.Forms.Button();
			this.btnResetDevice = new System.Windows.Forms.Button();
			this.btnResetToDefault = new System.Windows.Forms.Button();
			this.btnReadConfig = new System.Windows.Forms.Button();
			this.btnWriteConfig = new System.Windows.Forms.Button();
			this.commGrpBox = new System.Windows.Forms.GroupBox();
			this.openPortBtn = new System.Windows.Forms.Button();
			this.deviceSelectionCmbBox = new System.Windows.Forms.ComboBox();
			this.comPortCmbBox = new System.Windows.Forms.ComboBox();
			this.commTypeCmbBox = new System.Windows.Forms.ComboBox();
			this.deviceCmdGBox = new System.Windows.Forms.GroupBox();
			this.selectLogTypeCmbBox = new System.Windows.Forms.ComboBox();
			this.formatSDCardBtn = new System.Windows.Forms.Button();
			this.resetCPUBtn = new System.Windows.Forms.Button();
			this.factoryResetBtn = new System.Windows.Forms.Button();
			this.printReportBtn = new System.Windows.Forms.Button();
			this.defaultSettingsBtn = new System.Windows.Forms.Button();
			this.loadSettingsBtn = new System.Windows.Forms.Button();
			this.saveSettingsBtn = new System.Windows.Forms.Button();
			this.writeSettingsBtn = new System.Windows.Forms.Button();
			this.readSettingsBtn = new System.Windows.Forms.Button();
			this.Column9 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column8 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column7 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column6 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.tabMain.SuspendLayout();
			this.settingTabPage.SuspendLayout();
			this.grbOtherSettings.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.grbFeatureSet.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.grbDrivingSettings.SuspendLayout();
			this.groupBox10.SuspendLayout();
			this.grbAnalogInput.SuspendLayout();
			this.grbAuthorizedUser.SuspendLayout();
			this.grbVehicleParameter.SuspendLayout();
			this.grbServerParameter.SuspendLayout();
			this.grbGPRS.SuspendLayout();
			this.detailReportTabPage.SuspendLayout();
			this.groupBox3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.detailReportGridView)).BeginInit();
			this.dataLogTabPage.SuspendLayout();
			this.menuStrip1.SuspendLayout();
			this.statusGrpBox.SuspendLayout();
			this.commGrpBox.SuspendLayout();
			this.deviceCmdGBox.SuspendLayout();
			this.SuspendLayout();
			// 
			// tabMain
			// 
			resources.ApplyResources(this.tabMain, "tabMain");
			this.tabMain.Controls.Add(this.settingTabPage);
			this.tabMain.Controls.Add(this.detailReportTabPage);
			this.tabMain.Controls.Add(this.dataLogTabPage);
			this.tabMain.Name = "tabMain";
			this.tabMain.SelectedIndex = 0;
			this.tabMain.SelectedIndexChanged += new System.EventHandler(this.tabMain_SelectedIndexChanged);
			// 
			// settingTabPage
			// 
			resources.ApplyResources(this.settingTabPage, "settingTabPage");
			this.settingTabPage.Controls.Add(this.grbOtherSettings);
			this.settingTabPage.Controls.Add(this.grbFeatureSet);
			this.settingTabPage.Controls.Add(this.groupBox2);
			this.settingTabPage.Controls.Add(this.grbDrivingSettings);
			this.settingTabPage.Controls.Add(this.groupBox10);
			this.settingTabPage.Controls.Add(this.grbAnalogInput);
			this.settingTabPage.Controls.Add(this.grbAuthorizedUser);
			this.settingTabPage.Controls.Add(this.grbVehicleParameter);
			this.settingTabPage.Controls.Add(this.grbServerParameter);
			this.settingTabPage.Controls.Add(this.grbGPRS);
			this.settingTabPage.Name = "settingTabPage";
			this.settingTabPage.UseVisualStyleBackColor = true;
			// 
			// grbOtherSettings
			// 
			resources.ApplyResources(this.grbOtherSettings, "grbOtherSettings");
			this.grbOtherSettings.Controls.Add(this.autoSecurityTimeTxt);
			this.grbOtherSettings.Controls.Add(this.label8);
			this.grbOtherSettings.Controls.Add(this.groupBox1);
			this.grbOtherSettings.Controls.Add(this.label15);
			this.grbOtherSettings.Controls.Add(this.powerOffDelayTimeTxt);
			this.grbOtherSettings.Controls.Add(this.label16);
			this.grbOtherSettings.Controls.Add(this.speedEngineOffTxt);
			this.grbOtherSettings.Controls.Add(this.label17);
			this.grbOtherSettings.Controls.Add(this.sosKeyPressTimeTxt);
			this.grbOtherSettings.Controls.Add(this.sosKeyPressPeriodTxt);
			this.grbOtherSettings.Controls.Add(this.label9);
			this.grbOtherSettings.Controls.Add(this.label10);
			this.grbOtherSettings.Name = "grbOtherSettings";
			this.grbOtherSettings.TabStop = false;
			// 
			// autoSecurityTimeTxt
			// 
			resources.ApplyResources(this.autoSecurityTimeTxt, "autoSecurityTimeTxt");
			this.autoSecurityTimeTxt.BackColor = System.Drawing.SystemColors.Window;
			this.autoSecurityTimeTxt.Name = "autoSecurityTimeTxt";
			this.autoSecurityTimeTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// label8
			// 
			resources.ApplyResources(this.label8, "label8");
			this.label8.Name = "label8";
			// 
			// groupBox1
			// 
			resources.ApplyResources(this.groupBox1, "groupBox1");
			this.groupBox1.Controls.Add(this.autoSecurityOnChkBox);
			this.groupBox1.Controls.Add(this.callActionChkBox);
			this.groupBox1.Controls.Add(this.manualSecurityOnChkBox);
			this.groupBox1.Controls.Add(this.sendSMSActionChkBox);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.TabStop = false;
			// 
			// autoSecurityOnChkBox
			// 
			resources.ApplyResources(this.autoSecurityOnChkBox, "autoSecurityOnChkBox");
			this.autoSecurityOnChkBox.Name = "autoSecurityOnChkBox";
			this.autoSecurityOnChkBox.UseVisualStyleBackColor = true;
			this.autoSecurityOnChkBox.CheckedChanged += new System.EventHandler(this.featureSet_Changed);
			// 
			// callActionChkBox
			// 
			resources.ApplyResources(this.callActionChkBox, "callActionChkBox");
			this.callActionChkBox.Name = "callActionChkBox";
			this.callActionChkBox.UseVisualStyleBackColor = true;
			this.callActionChkBox.CheckedChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// manualSecurityOnChkBox
			// 
			resources.ApplyResources(this.manualSecurityOnChkBox, "manualSecurityOnChkBox");
			this.manualSecurityOnChkBox.Name = "manualSecurityOnChkBox";
			this.manualSecurityOnChkBox.UseVisualStyleBackColor = true;
			this.manualSecurityOnChkBox.CheckedChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// sendSMSActionChkBox
			// 
			resources.ApplyResources(this.sendSMSActionChkBox, "sendSMSActionChkBox");
			this.sendSMSActionChkBox.Name = "sendSMSActionChkBox";
			this.sendSMSActionChkBox.UseVisualStyleBackColor = true;
			this.sendSMSActionChkBox.CheckedChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// label15
			// 
			resources.ApplyResources(this.label15, "label15");
			this.label15.Name = "label15";
			// 
			// powerOffDelayTimeTxt
			// 
			resources.ApplyResources(this.powerOffDelayTimeTxt, "powerOffDelayTimeTxt");
			this.powerOffDelayTimeTxt.BackColor = System.Drawing.SystemColors.Window;
			this.powerOffDelayTimeTxt.Name = "powerOffDelayTimeTxt";
			this.powerOffDelayTimeTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// label16
			// 
			resources.ApplyResources(this.label16, "label16");
			this.label16.Name = "label16";
			// 
			// speedEngineOffTxt
			// 
			resources.ApplyResources(this.speedEngineOffTxt, "speedEngineOffTxt");
			this.speedEngineOffTxt.BackColor = System.Drawing.SystemColors.Window;
			this.speedEngineOffTxt.Name = "speedEngineOffTxt";
			this.speedEngineOffTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// label17
			// 
			resources.ApplyResources(this.label17, "label17");
			this.label17.Name = "label17";
			// 
			// sosKeyPressTimeTxt
			// 
			resources.ApplyResources(this.sosKeyPressTimeTxt, "sosKeyPressTimeTxt");
			this.sosKeyPressTimeTxt.BackColor = System.Drawing.SystemColors.Window;
			this.sosKeyPressTimeTxt.Name = "sosKeyPressTimeTxt";
			this.sosKeyPressTimeTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// sosKeyPressPeriodTxt
			// 
			resources.ApplyResources(this.sosKeyPressPeriodTxt, "sosKeyPressPeriodTxt");
			this.sosKeyPressPeriodTxt.BackColor = System.Drawing.SystemColors.Window;
			this.sosKeyPressPeriodTxt.Name = "sosKeyPressPeriodTxt";
			this.sosKeyPressPeriodTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// label9
			// 
			resources.ApplyResources(this.label9, "label9");
			this.label9.Name = "label9";
			// 
			// label10
			// 
			resources.ApplyResources(this.label10, "label10");
			this.label10.Name = "label10";
			// 
			// grbFeatureSet
			// 
			resources.ApplyResources(this.grbFeatureSet, "grbFeatureSet");
			this.grbFeatureSet.Controls.Add(this.label19);
			this.grbFeatureSet.Controls.Add(this.accountAlarmCheckTxt);
			this.grbFeatureSet.Controls.Add(this.enableFifoChkBox);
			this.grbFeatureSet.Controls.Add(this.label18);
			this.grbFeatureSet.Controls.Add(this.removePowerWarningChkBox);
			this.grbFeatureSet.Controls.Add(this.lowPowerWarningChkBox);
			this.grbFeatureSet.Controls.Add(this.label20);
			this.grbFeatureSet.Controls.Add(this.lowBatteryWarningChkBox);
			this.grbFeatureSet.Controls.Add(this.label14);
			this.grbFeatureSet.Controls.Add(this.batteryLowWarningLevelTxt);
			this.grbFeatureSet.Controls.Add(this.removePowerWarningPeriodTxt);
			this.grbFeatureSet.Controls.Add(this.label13);
			this.grbFeatureSet.Controls.Add(this.label11);
			this.grbFeatureSet.Controls.Add(this.batteryLowWarningPeriodTxt);
			this.grbFeatureSet.Name = "grbFeatureSet";
			this.grbFeatureSet.TabStop = false;
			// 
			// label19
			// 
			resources.ApplyResources(this.label19, "label19");
			this.label19.Name = "label19";
			// 
			// accountAlarmCheckTxt
			// 
			resources.ApplyResources(this.accountAlarmCheckTxt, "accountAlarmCheckTxt");
			this.accountAlarmCheckTxt.BackColor = System.Drawing.SystemColors.Window;
			this.accountAlarmCheckTxt.Name = "accountAlarmCheckTxt";
			this.accountAlarmCheckTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// enableFifoChkBox
			// 
			resources.ApplyResources(this.enableFifoChkBox, "enableFifoChkBox");
			this.enableFifoChkBox.Name = "enableFifoChkBox";
			this.enableFifoChkBox.UseVisualStyleBackColor = true;
			this.enableFifoChkBox.CheckedChanged += new System.EventHandler(this.featureSet_Changed);
			// 
			// label18
			// 
			resources.ApplyResources(this.label18, "label18");
			this.label18.Name = "label18";
			// 
			// removePowerWarningChkBox
			// 
			resources.ApplyResources(this.removePowerWarningChkBox, "removePowerWarningChkBox");
			this.removePowerWarningChkBox.Name = "removePowerWarningChkBox";
			this.removePowerWarningChkBox.UseVisualStyleBackColor = true;
			this.removePowerWarningChkBox.CheckedChanged += new System.EventHandler(this.featureSet_Changed);
			// 
			// lowPowerWarningChkBox
			// 
			resources.ApplyResources(this.lowPowerWarningChkBox, "lowPowerWarningChkBox");
			this.lowPowerWarningChkBox.Name = "lowPowerWarningChkBox";
			this.lowPowerWarningChkBox.UseVisualStyleBackColor = true;
			this.lowPowerWarningChkBox.CheckedChanged += new System.EventHandler(this.featureSet_Changed);
			// 
			// label20
			// 
			resources.ApplyResources(this.label20, "label20");
			this.label20.Name = "label20";
			// 
			// lowBatteryWarningChkBox
			// 
			resources.ApplyResources(this.lowBatteryWarningChkBox, "lowBatteryWarningChkBox");
			this.lowBatteryWarningChkBox.Name = "lowBatteryWarningChkBox";
			this.lowBatteryWarningChkBox.UseVisualStyleBackColor = true;
			this.lowBatteryWarningChkBox.CheckedChanged += new System.EventHandler(this.featureSet_Changed);
			// 
			// label14
			// 
			resources.ApplyResources(this.label14, "label14");
			this.label14.Name = "label14";
			// 
			// batteryLowWarningLevelTxt
			// 
			resources.ApplyResources(this.batteryLowWarningLevelTxt, "batteryLowWarningLevelTxt");
			this.batteryLowWarningLevelTxt.BackColor = System.Drawing.SystemColors.Window;
			this.batteryLowWarningLevelTxt.Name = "batteryLowWarningLevelTxt";
			this.batteryLowWarningLevelTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// removePowerWarningPeriodTxt
			// 
			resources.ApplyResources(this.removePowerWarningPeriodTxt, "removePowerWarningPeriodTxt");
			this.removePowerWarningPeriodTxt.BackColor = System.Drawing.SystemColors.Window;
			this.removePowerWarningPeriodTxt.Name = "removePowerWarningPeriodTxt";
			this.removePowerWarningPeriodTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// label13
			// 
			resources.ApplyResources(this.label13, "label13");
			this.label13.Name = "label13";
			// 
			// label11
			// 
			resources.ApplyResources(this.label11, "label11");
			this.label11.Name = "label11";
			// 
			// batteryLowWarningPeriodTxt
			// 
			resources.ApplyResources(this.batteryLowWarningPeriodTxt, "batteryLowWarningPeriodTxt");
			this.batteryLowWarningPeriodTxt.BackColor = System.Drawing.SystemColors.Window;
			this.batteryLowWarningPeriodTxt.Name = "batteryLowWarningPeriodTxt";
			this.batteryLowWarningPeriodTxt.TextChanged += new System.EventHandler(this.otherSettings_Changed);
			// 
			// groupBox2
			// 
			resources.ApplyResources(this.groupBox2, "groupBox2");
			this.groupBox2.Controls.Add(this.sendSMSBtn);
			this.groupBox2.Controls.Add(this.smsMessageTxt);
			this.groupBox2.Controls.Add(this.smsAddressTxt);
			this.groupBox2.Controls.Add(this.label6);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.TabStop = false;
			// 
			// sendSMSBtn
			// 
			resources.ApplyResources(this.sendSMSBtn, "sendSMSBtn");
			this.sendSMSBtn.Name = "sendSMSBtn";
			this.sendSMSBtn.UseVisualStyleBackColor = true;
			this.sendSMSBtn.Click += new System.EventHandler(this.sendSMSBtn_Click);
			// 
			// smsMessageTxt
			// 
			resources.ApplyResources(this.smsMessageTxt, "smsMessageTxt");
			this.smsMessageTxt.Name = "smsMessageTxt";
			// 
			// smsAddressTxt
			// 
			resources.ApplyResources(this.smsAddressTxt, "smsAddressTxt");
			this.smsAddressTxt.Name = "smsAddressTxt";
			// 
			// label6
			// 
			resources.ApplyResources(this.label6, "label6");
			this.label6.Name = "label6";
			// 
			// grbDrivingSettings
			// 
			resources.ApplyResources(this.grbDrivingSettings, "grbDrivingSettings");
			this.grbDrivingSettings.Controls.Add(this.label4);
			this.grbDrivingSettings.Controls.Add(this.baseMileageTxtBox);
			this.grbDrivingSettings.Controls.Add(this.totalDrivingTimeLimitTxtBox);
			this.grbDrivingSettings.Controls.Add(this.drivingTimeLimitTxtBox);
			this.grbDrivingSettings.Controls.Add(this.label1);
			this.grbDrivingSettings.Controls.Add(this.speedLimitTxtBox);
			this.grbDrivingSettings.Controls.Add(this.label2);
			this.grbDrivingSettings.Controls.Add(this.label3);
			this.grbDrivingSettings.Name = "grbDrivingSettings";
			this.grbDrivingSettings.TabStop = false;
			// 
			// label4
			// 
			resources.ApplyResources(this.label4, "label4");
			this.label4.Name = "label4";
			// 
			// baseMileageTxtBox
			// 
			resources.ApplyResources(this.baseMileageTxtBox, "baseMileageTxtBox");
			this.baseMileageTxtBox.Name = "baseMileageTxtBox";
			this.baseMileageTxtBox.TextChanged += new System.EventHandler(this.drivingSettings_Changed);
			// 
			// totalDrivingTimeLimitTxtBox
			// 
			resources.ApplyResources(this.totalDrivingTimeLimitTxtBox, "totalDrivingTimeLimitTxtBox");
			this.totalDrivingTimeLimitTxtBox.Name = "totalDrivingTimeLimitTxtBox";
			this.totalDrivingTimeLimitTxtBox.TextChanged += new System.EventHandler(this.drivingSettings_Changed);
			// 
			// drivingTimeLimitTxtBox
			// 
			resources.ApplyResources(this.drivingTimeLimitTxtBox, "drivingTimeLimitTxtBox");
			this.drivingTimeLimitTxtBox.Name = "drivingTimeLimitTxtBox";
			this.drivingTimeLimitTxtBox.TextChanged += new System.EventHandler(this.drivingSettings_Changed);
			// 
			// label1
			// 
			resources.ApplyResources(this.label1, "label1");
			this.label1.Name = "label1";
			// 
			// speedLimitTxtBox
			// 
			resources.ApplyResources(this.speedLimitTxtBox, "speedLimitTxtBox");
			this.speedLimitTxtBox.Name = "speedLimitTxtBox";
			this.speedLimitTxtBox.TextChanged += new System.EventHandler(this.drivingSettings_Changed);
			// 
			// label2
			// 
			resources.ApplyResources(this.label2, "label2");
			this.label2.Name = "label2";
			// 
			// label3
			// 
			resources.ApplyResources(this.label3, "label3");
			this.label3.Name = "label3";
			// 
			// groupBox10
			// 
			resources.ApplyResources(this.groupBox10, "groupBox10");
			this.groupBox10.Controls.Add(this.runAppBtn);
			this.groupBox10.Controls.Add(this.firmwareVersionLbl);
			this.groupBox10.Controls.Add(this.downloadFirmwareBtn);
			this.groupBox10.Controls.Add(this.readFirmwareVersionBtn);
			this.groupBox10.Controls.Add(this.browseFirmwareBtn);
			this.groupBox10.Controls.Add(this.firmwarePathTxtBox);
			this.groupBox10.Name = "groupBox10";
			this.groupBox10.TabStop = false;
			// 
			// runAppBtn
			// 
			resources.ApplyResources(this.runAppBtn, "runAppBtn");
			this.runAppBtn.Name = "runAppBtn";
			this.runAppBtn.UseVisualStyleBackColor = true;
			this.runAppBtn.Click += new System.EventHandler(this.jumpToAppBtn_Click);
			// 
			// firmwareVersionLbl
			// 
			resources.ApplyResources(this.firmwareVersionLbl, "firmwareVersionLbl");
			this.firmwareVersionLbl.Name = "firmwareVersionLbl";
			// 
			// downloadFirmwareBtn
			// 
			resources.ApplyResources(this.downloadFirmwareBtn, "downloadFirmwareBtn");
			this.downloadFirmwareBtn.Name = "downloadFirmwareBtn";
			this.downloadFirmwareBtn.UseVisualStyleBackColor = true;
			this.downloadFirmwareBtn.Click += new System.EventHandler(this.downloadFirmwareBtn_Click);
			// 
			// readFirmwareVersionBtn
			// 
			resources.ApplyResources(this.readFirmwareVersionBtn, "readFirmwareVersionBtn");
			this.readFirmwareVersionBtn.Name = "readFirmwareVersionBtn";
			this.readFirmwareVersionBtn.UseVisualStyleBackColor = true;
			this.readFirmwareVersionBtn.Click += new System.EventHandler(this.readFirmwareVersionBtn_Click);
			// 
			// browseFirmwareBtn
			// 
			resources.ApplyResources(this.browseFirmwareBtn, "browseFirmwareBtn");
			this.browseFirmwareBtn.Name = "browseFirmwareBtn";
			this.browseFirmwareBtn.UseVisualStyleBackColor = true;
			this.browseFirmwareBtn.Click += new System.EventHandler(this.browseFirmwareBtn_Click);
			// 
			// firmwarePathTxtBox
			// 
			resources.ApplyResources(this.firmwarePathTxtBox, "firmwarePathTxtBox");
			this.firmwarePathTxtBox.Name = "firmwarePathTxtBox";
			// 
			// grbAnalogInput
			// 
			resources.ApplyResources(this.grbAnalogInput, "grbAnalogInput");
			this.grbAnalogInput.Controls.Add(this.analogLowerBoundTxtBox);
			this.grbAnalogInput.Controls.Add(this.label31);
			this.grbAnalogInput.Controls.Add(this.analogUpperBoundTxtBox);
			this.grbAnalogInput.Controls.Add(this.label37);
			this.grbAnalogInput.Controls.Add(this.analogPercentageModeChkBox);
			this.grbAnalogInput.Controls.Add(this.analogInputNoCmbBox);
			this.grbAnalogInput.Name = "grbAnalogInput";
			this.grbAnalogInput.TabStop = false;
			// 
			// analogLowerBoundTxtBox
			// 
			resources.ApplyResources(this.analogLowerBoundTxtBox, "analogLowerBoundTxtBox");
			this.analogLowerBoundTxtBox.Name = "analogLowerBoundTxtBox";
			this.analogLowerBoundTxtBox.TextChanged += new System.EventHandler(this.analogParameter_Changed);
			// 
			// label31
			// 
			resources.ApplyResources(this.label31, "label31");
			this.label31.Name = "label31";
			// 
			// analogUpperBoundTxtBox
			// 
			resources.ApplyResources(this.analogUpperBoundTxtBox, "analogUpperBoundTxtBox");
			this.analogUpperBoundTxtBox.Name = "analogUpperBoundTxtBox";
			this.analogUpperBoundTxtBox.TextChanged += new System.EventHandler(this.analogParameter_Changed);
			// 
			// label37
			// 
			resources.ApplyResources(this.label37, "label37");
			this.label37.Name = "label37";
			// 
			// analogPercentageModeChkBox
			// 
			resources.ApplyResources(this.analogPercentageModeChkBox, "analogPercentageModeChkBox");
			this.analogPercentageModeChkBox.Name = "analogPercentageModeChkBox";
			this.analogPercentageModeChkBox.UseVisualStyleBackColor = true;
			this.analogPercentageModeChkBox.CheckedChanged += new System.EventHandler(this.analogParameter_Changed);
			// 
			// analogInputNoCmbBox
			// 
			resources.ApplyResources(this.analogInputNoCmbBox, "analogInputNoCmbBox");
			this.analogInputNoCmbBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.analogInputNoCmbBox.FormattingEnabled = true;
			this.analogInputNoCmbBox.Items.AddRange(new object[] {
            resources.GetString("analogInputNoCmbBox.Items"),
            resources.GetString("analogInputNoCmbBox.Items1")});
			this.analogInputNoCmbBox.Name = "analogInputNoCmbBox";
			this.analogInputNoCmbBox.SelectedIndexChanged += new System.EventHandler(this.analogSelection_Changed);
			// 
			// grbAuthorizedUser
			// 
			resources.ApplyResources(this.grbAuthorizedUser, "grbAuthorizedUser");
			this.grbAuthorizedUser.Controls.Add(this.authPhoneNoTxtBox);
			this.grbAuthorizedUser.Controls.Add(this.lblUserNumber);
			this.grbAuthorizedUser.Controls.Add(this.userNoCmbBox);
			this.grbAuthorizedUser.Controls.Add(this.lblAuthorizedUserName);
			this.grbAuthorizedUser.Controls.Add(this.authUserNameTxtBox);
			this.grbAuthorizedUser.Controls.Add(this.lblAuthorizedPhoneNumber);
			this.grbAuthorizedUser.Name = "grbAuthorizedUser";
			this.grbAuthorizedUser.TabStop = false;
			// 
			// authPhoneNoTxtBox
			// 
			resources.ApplyResources(this.authPhoneNoTxtBox, "authPhoneNoTxtBox");
			this.authPhoneNoTxtBox.Name = "authPhoneNoTxtBox";
			this.authPhoneNoTxtBox.TextChanged += new System.EventHandler(this.userListInfo_Changed);
			// 
			// lblUserNumber
			// 
			resources.ApplyResources(this.lblUserNumber, "lblUserNumber");
			this.lblUserNumber.Name = "lblUserNumber";
			// 
			// userNoCmbBox
			// 
			resources.ApplyResources(this.userNoCmbBox, "userNoCmbBox");
			this.userNoCmbBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.userNoCmbBox.FormattingEnabled = true;
			this.userNoCmbBox.Items.AddRange(new object[] {
            resources.GetString("userNoCmbBox.Items"),
            resources.GetString("userNoCmbBox.Items1"),
            resources.GetString("userNoCmbBox.Items2"),
            resources.GetString("userNoCmbBox.Items3"),
            resources.GetString("userNoCmbBox.Items4"),
            resources.GetString("userNoCmbBox.Items5"),
            resources.GetString("userNoCmbBox.Items6"),
            resources.GetString("userNoCmbBox.Items7"),
            resources.GetString("userNoCmbBox.Items8"),
            resources.GetString("userNoCmbBox.Items9")});
			this.userNoCmbBox.Name = "userNoCmbBox";
			this.userNoCmbBox.SelectedIndexChanged += new System.EventHandler(this.userListIndex_Changed);
			// 
			// lblAuthorizedUserName
			// 
			resources.ApplyResources(this.lblAuthorizedUserName, "lblAuthorizedUserName");
			this.lblAuthorizedUserName.Name = "lblAuthorizedUserName";
			// 
			// authUserNameTxtBox
			// 
			resources.ApplyResources(this.authUserNameTxtBox, "authUserNameTxtBox");
			this.authUserNameTxtBox.Name = "authUserNameTxtBox";
			this.authUserNameTxtBox.TextChanged += new System.EventHandler(this.userListInfo_Changed);
			// 
			// lblAuthorizedPhoneNumber
			// 
			resources.ApplyResources(this.lblAuthorizedPhoneNumber, "lblAuthorizedPhoneNumber");
			this.lblAuthorizedPhoneNumber.Name = "lblAuthorizedPhoneNumber";
			// 
			// grbVehicleParameter
			// 
			resources.ApplyResources(this.grbVehicleParameter, "grbVehicleParameter");
			this.grbVehicleParameter.Controls.Add(this.bossPhoneTxtBox);
			this.grbVehicleParameter.Controls.Add(this.label7);
			this.grbVehicleParameter.Controls.Add(this.pulseSpeedSourceRBtn);
			this.grbVehicleParameter.Controls.Add(this.gpsSpeedSourceRBtn);
			this.grbVehicleParameter.Controls.Add(this.lblSpeedSource);
			this.grbVehicleParameter.Controls.Add(this.tyreRateTxtBox);
			this.grbVehicleParameter.Controls.Add(this.vinNumTxtBox);
			this.grbVehicleParameter.Controls.Add(this.plateNoTxtBox);
			this.grbVehicleParameter.Controls.Add(this.trackerIdTxtBox);
			this.grbVehicleParameter.Controls.Add(this.lblTyreRate);
			this.grbVehicleParameter.Controls.Add(this.lblCarNumber);
			this.grbVehicleParameter.Controls.Add(this.lblTrackerID);
			this.grbVehicleParameter.Controls.Add(this.lblVINNumber);
			this.grbVehicleParameter.Name = "grbVehicleParameter";
			this.grbVehicleParameter.TabStop = false;
			// 
			// bossPhoneTxtBox
			// 
			resources.ApplyResources(this.bossPhoneTxtBox, "bossPhoneTxtBox");
			this.bossPhoneTxtBox.Name = "bossPhoneTxtBox";
			this.bossPhoneTxtBox.TextChanged += new System.EventHandler(this.vehicleParameter_Changed);
			// 
			// label7
			// 
			resources.ApplyResources(this.label7, "label7");
			this.label7.Name = "label7";
			// 
			// pulseSpeedSourceRBtn
			// 
			resources.ApplyResources(this.pulseSpeedSourceRBtn, "pulseSpeedSourceRBtn");
			this.pulseSpeedSourceRBtn.Name = "pulseSpeedSourceRBtn";
			this.pulseSpeedSourceRBtn.UseVisualStyleBackColor = true;
			this.pulseSpeedSourceRBtn.CheckedChanged += new System.EventHandler(this.vehicleParameter_Changed);
			// 
			// gpsSpeedSourceRBtn
			// 
			resources.ApplyResources(this.gpsSpeedSourceRBtn, "gpsSpeedSourceRBtn");
			this.gpsSpeedSourceRBtn.Checked = true;
			this.gpsSpeedSourceRBtn.Name = "gpsSpeedSourceRBtn";
			this.gpsSpeedSourceRBtn.TabStop = true;
			this.gpsSpeedSourceRBtn.UseVisualStyleBackColor = true;
			this.gpsSpeedSourceRBtn.CheckedChanged += new System.EventHandler(this.vehicleParameter_Changed);
			// 
			// lblSpeedSource
			// 
			resources.ApplyResources(this.lblSpeedSource, "lblSpeedSource");
			this.lblSpeedSource.Name = "lblSpeedSource";
			// 
			// tyreRateTxtBox
			// 
			resources.ApplyResources(this.tyreRateTxtBox, "tyreRateTxtBox");
			this.tyreRateTxtBox.Name = "tyreRateTxtBox";
			this.tyreRateTxtBox.TextChanged += new System.EventHandler(this.vehicleParameter_Changed);
			// 
			// vinNumTxtBox
			// 
			resources.ApplyResources(this.vinNumTxtBox, "vinNumTxtBox");
			this.vinNumTxtBox.Name = "vinNumTxtBox";
			this.vinNumTxtBox.TextChanged += new System.EventHandler(this.vehicleParameter_Changed);
			// 
			// plateNoTxtBox
			// 
			resources.ApplyResources(this.plateNoTxtBox, "plateNoTxtBox");
			this.plateNoTxtBox.Name = "plateNoTxtBox";
			this.plateNoTxtBox.TextChanged += new System.EventHandler(this.vehicleParameter_Changed);
			// 
			// trackerIdTxtBox
			// 
			resources.ApplyResources(this.trackerIdTxtBox, "trackerIdTxtBox");
			this.trackerIdTxtBox.Name = "trackerIdTxtBox";
			this.trackerIdTxtBox.TextChanged += new System.EventHandler(this.vehicleParameter_Changed);
			// 
			// lblTyreRate
			// 
			resources.ApplyResources(this.lblTyreRate, "lblTyreRate");
			this.lblTyreRate.Name = "lblTyreRate";
			// 
			// lblCarNumber
			// 
			resources.ApplyResources(this.lblCarNumber, "lblCarNumber");
			this.lblCarNumber.Name = "lblCarNumber";
			// 
			// lblTrackerID
			// 
			resources.ApplyResources(this.lblTrackerID, "lblTrackerID");
			this.lblTrackerID.Name = "lblTrackerID";
			// 
			// lblVINNumber
			// 
			resources.ApplyResources(this.lblVINNumber, "lblVINNumber");
			this.lblVINNumber.Name = "lblVINNumber";
			// 
			// grbServerParameter
			// 
			resources.ApplyResources(this.grbServerParameter, "grbServerParameter");
			this.grbServerParameter.Controls.Add(this.serverSelectionCmbBox);
			this.grbServerParameter.Controls.Add(this.lblServerSelection);
			this.grbServerParameter.Controls.Add(this.serverUseDomainRBtn);
			this.grbServerParameter.Controls.Add(this.secDomainTxtBox);
			this.grbServerParameter.Controls.Add(this.priDomainTxtBox);
			this.grbServerParameter.Controls.Add(this.serverUseIpRBtn);
			this.grbServerParameter.Controls.Add(this.secPortTxtBox);
			this.grbServerParameter.Controls.Add(this.priPortTxtBox);
			this.grbServerParameter.Controls.Add(this.secIpTxtBox);
			this.grbServerParameter.Controls.Add(this.lblSecondaryServer);
			this.grbServerParameter.Controls.Add(this.lblPort);
			this.grbServerParameter.Controls.Add(this.priIpTxtBox);
			this.grbServerParameter.Controls.Add(this.label39);
			this.grbServerParameter.Name = "grbServerParameter";
			this.grbServerParameter.TabStop = false;
			// 
			// serverSelectionCmbBox
			// 
			resources.ApplyResources(this.serverSelectionCmbBox, "serverSelectionCmbBox");
			this.serverSelectionCmbBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.serverSelectionCmbBox.FormattingEnabled = true;
			this.serverSelectionCmbBox.Items.AddRange(new object[] {
            resources.GetString("serverSelectionCmbBox.Items")});
			this.serverSelectionCmbBox.Name = "serverSelectionCmbBox";
			this.serverSelectionCmbBox.SelectedIndexChanged += new System.EventHandler(this.serverSelection_Changed);
			// 
			// lblServerSelection
			// 
			resources.ApplyResources(this.lblServerSelection, "lblServerSelection");
			this.lblServerSelection.Name = "lblServerSelection";
			// 
			// serverUseDomainRBtn
			// 
			resources.ApplyResources(this.serverUseDomainRBtn, "serverUseDomainRBtn");
			this.serverUseDomainRBtn.Name = "serverUseDomainRBtn";
			this.serverUseDomainRBtn.TabStop = true;
			this.serverUseDomainRBtn.UseVisualStyleBackColor = true;
			this.serverUseDomainRBtn.CheckedChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// secDomainTxtBox
			// 
			resources.ApplyResources(this.secDomainTxtBox, "secDomainTxtBox");
			this.secDomainTxtBox.Name = "secDomainTxtBox";
			this.secDomainTxtBox.TextChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// priDomainTxtBox
			// 
			resources.ApplyResources(this.priDomainTxtBox, "priDomainTxtBox");
			this.priDomainTxtBox.Name = "priDomainTxtBox";
			this.priDomainTxtBox.TextChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// serverUseIpRBtn
			// 
			resources.ApplyResources(this.serverUseIpRBtn, "serverUseIpRBtn");
			this.serverUseIpRBtn.Checked = true;
			this.serverUseIpRBtn.Name = "serverUseIpRBtn";
			this.serverUseIpRBtn.TabStop = true;
			this.serverUseIpRBtn.UseVisualStyleBackColor = true;
			this.serverUseIpRBtn.CheckedChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// secPortTxtBox
			// 
			resources.ApplyResources(this.secPortTxtBox, "secPortTxtBox");
			this.secPortTxtBox.Name = "secPortTxtBox";
			this.secPortTxtBox.TextChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// priPortTxtBox
			// 
			resources.ApplyResources(this.priPortTxtBox, "priPortTxtBox");
			this.priPortTxtBox.Name = "priPortTxtBox";
			this.priPortTxtBox.TextChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// secIpTxtBox
			// 
			resources.ApplyResources(this.secIpTxtBox, "secIpTxtBox");
			this.secIpTxtBox.Name = "secIpTxtBox";
			this.secIpTxtBox.TextChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// lblSecondaryServer
			// 
			resources.ApplyResources(this.lblSecondaryServer, "lblSecondaryServer");
			this.lblSecondaryServer.Name = "lblSecondaryServer";
			// 
			// lblPort
			// 
			resources.ApplyResources(this.lblPort, "lblPort");
			this.lblPort.Name = "lblPort";
			// 
			// priIpTxtBox
			// 
			resources.ApplyResources(this.priIpTxtBox, "priIpTxtBox");
			this.priIpTxtBox.Name = "priIpTxtBox";
			this.priIpTxtBox.TextChanged += new System.EventHandler(this.serverParameter_Changed);
			// 
			// label39
			// 
			resources.ApplyResources(this.label39, "label39");
			this.label39.Name = "label39";
			// 
			// grbGPRS
			// 
			resources.ApplyResources(this.grbGPRS, "grbGPRS");
			this.grbGPRS.Controls.Add(this.gprsStopReportIntervalTxtBox);
			this.grbGPRS.Controls.Add(this.gprsRunReportIntervalTxtBox);
			this.grbGPRS.Controls.Add(this.gprsPwdTxtBox);
			this.grbGPRS.Controls.Add(this.gprsUserTxtBox);
			this.grbGPRS.Controls.Add(this.gprsApnTxtBox);
			this.grbGPRS.Controls.Add(this.lblOFF);
			this.grbGPRS.Controls.Add(this.lblON);
			this.grbGPRS.Controls.Add(this.lblIntevalTime);
			this.grbGPRS.Controls.Add(this.lblPassword);
			this.grbGPRS.Controls.Add(this.lblUser);
			this.grbGPRS.Controls.Add(this.lblAPN);
			this.grbGPRS.Name = "grbGPRS";
			this.grbGPRS.TabStop = false;
			// 
			// gprsStopReportIntervalTxtBox
			// 
			resources.ApplyResources(this.gprsStopReportIntervalTxtBox, "gprsStopReportIntervalTxtBox");
			this.gprsStopReportIntervalTxtBox.Name = "gprsStopReportIntervalTxtBox";
			this.gprsStopReportIntervalTxtBox.TextChanged += new System.EventHandler(this.gprsSettings_Changed);
			// 
			// gprsRunReportIntervalTxtBox
			// 
			resources.ApplyResources(this.gprsRunReportIntervalTxtBox, "gprsRunReportIntervalTxtBox");
			this.gprsRunReportIntervalTxtBox.Name = "gprsRunReportIntervalTxtBox";
			this.gprsRunReportIntervalTxtBox.TextChanged += new System.EventHandler(this.gprsSettings_Changed);
			// 
			// gprsPwdTxtBox
			// 
			resources.ApplyResources(this.gprsPwdTxtBox, "gprsPwdTxtBox");
			this.gprsPwdTxtBox.Name = "gprsPwdTxtBox";
			this.gprsPwdTxtBox.TextChanged += new System.EventHandler(this.gprsSettings_Changed);
			// 
			// gprsUserTxtBox
			// 
			resources.ApplyResources(this.gprsUserTxtBox, "gprsUserTxtBox");
			this.gprsUserTxtBox.Name = "gprsUserTxtBox";
			this.gprsUserTxtBox.TextChanged += new System.EventHandler(this.gprsSettings_Changed);
			// 
			// gprsApnTxtBox
			// 
			resources.ApplyResources(this.gprsApnTxtBox, "gprsApnTxtBox");
			this.gprsApnTxtBox.Name = "gprsApnTxtBox";
			this.gprsApnTxtBox.TextChanged += new System.EventHandler(this.gprsSettings_Changed);
			// 
			// lblOFF
			// 
			resources.ApplyResources(this.lblOFF, "lblOFF");
			this.lblOFF.Name = "lblOFF";
			// 
			// lblON
			// 
			resources.ApplyResources(this.lblON, "lblON");
			this.lblON.Name = "lblON";
			// 
			// lblIntevalTime
			// 
			resources.ApplyResources(this.lblIntevalTime, "lblIntevalTime");
			this.lblIntevalTime.Name = "lblIntevalTime";
			// 
			// lblPassword
			// 
			resources.ApplyResources(this.lblPassword, "lblPassword");
			this.lblPassword.Name = "lblPassword";
			// 
			// lblUser
			// 
			resources.ApplyResources(this.lblUser, "lblUser");
			this.lblUser.Name = "lblUser";
			// 
			// lblAPN
			// 
			resources.ApplyResources(this.lblAPN, "lblAPN");
			this.lblAPN.Name = "lblAPN";
			// 
			// detailReportTabPage
			// 
			resources.ApplyResources(this.detailReportTabPage, "detailReportTabPage");
			this.detailReportTabPage.Controls.Add(this.groupBox3);
			this.detailReportTabPage.Controls.Add(this.detailReportGridView);
			this.detailReportTabPage.Name = "detailReportTabPage";
			this.detailReportTabPage.UseVisualStyleBackColor = true;
			// 
			// groupBox3
			// 
			resources.ApplyResources(this.groupBox3, "groupBox3");
			this.groupBox3.Controls.Add(this.exportToKMLBtn);
			this.groupBox3.Controls.Add(this.readReportBtn);
			this.groupBox3.Controls.Add(this.textBox2);
			this.groupBox3.Controls.Add(this.label12);
			this.groupBox3.Controls.Add(this.textBox1);
			this.groupBox3.Controls.Add(this.label5);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.TabStop = false;
			// 
			// exportToKMLBtn
			// 
			resources.ApplyResources(this.exportToKMLBtn, "exportToKMLBtn");
			this.exportToKMLBtn.Name = "exportToKMLBtn";
			this.exportToKMLBtn.UseVisualStyleBackColor = true;
			this.exportToKMLBtn.Click += new System.EventHandler(this.exportToKMLBtn_Click);
			// 
			// readReportBtn
			// 
			resources.ApplyResources(this.readReportBtn, "readReportBtn");
			this.readReportBtn.Name = "readReportBtn";
			this.readReportBtn.UseVisualStyleBackColor = true;
			this.readReportBtn.Click += new System.EventHandler(this.readReportBtn_Click);
			// 
			// textBox2
			// 
			resources.ApplyResources(this.textBox2, "textBox2");
			this.textBox2.Name = "textBox2";
			// 
			// label12
			// 
			resources.ApplyResources(this.label12, "label12");
			this.label12.Name = "label12";
			// 
			// textBox1
			// 
			resources.ApplyResources(this.textBox1, "textBox1");
			this.textBox1.Name = "textBox1";
			// 
			// label5
			// 
			resources.ApplyResources(this.label5, "label5");
			this.label5.Name = "label5";
			// 
			// detailReportGridView
			// 
			resources.ApplyResources(this.detailReportGridView, "detailReportGridView");
			this.detailReportGridView.AllowUserToAddRows = false;
			this.detailReportGridView.AllowUserToDeleteRows = false;
			this.detailReportGridView.AllowUserToResizeRows = false;
			this.detailReportGridView.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
			this.detailReportGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.detailReportGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn12,
            this.userNameCol,
            this.timeCol,
            this.lgnCol,
            this.latCol,
            this.speedCol,
            this.directionCol,
            this.mileageCol,
            this.hdopCol,
            this.speedFreqCol,
            this.drivingTimeCol,
            this.totalDrivingTimeCol,
            this.doorStatusCol,
            this.airconStatusCol,
            this.engineStatusCol,
            this.adc1ValueCol,
            this.adc2ValueCol,
            this.overSpeedCntCol,
            this.parkCntCol,
            this.doorOpenCntCol,
            this.onOffEngineCntCol,
            this.parkingTimeCol,
            this.gpsDirectionCol});
			this.detailReportGridView.Name = "detailReportGridView";
			this.detailReportGridView.ReadOnly = true;
			this.detailReportGridView.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
			this.detailReportGridView.RowHeadersVisible = false;
			this.detailReportGridView.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
			// 
			// dataGridViewTextBoxColumn12
			// 
			resources.ApplyResources(this.dataGridViewTextBoxColumn12, "dataGridViewTextBoxColumn12");
			this.dataGridViewTextBoxColumn12.Name = "dataGridViewTextBoxColumn12";
			this.dataGridViewTextBoxColumn12.ReadOnly = true;
			// 
			// userNameCol
			// 
			resources.ApplyResources(this.userNameCol, "userNameCol");
			this.userNameCol.Name = "userNameCol";
			this.userNameCol.ReadOnly = true;
			// 
			// timeCol
			// 
			resources.ApplyResources(this.timeCol, "timeCol");
			this.timeCol.Name = "timeCol";
			this.timeCol.ReadOnly = true;
			// 
			// lgnCol
			// 
			resources.ApplyResources(this.lgnCol, "lgnCol");
			this.lgnCol.Name = "lgnCol";
			this.lgnCol.ReadOnly = true;
			// 
			// latCol
			// 
			resources.ApplyResources(this.latCol, "latCol");
			this.latCol.Name = "latCol";
			this.latCol.ReadOnly = true;
			// 
			// speedCol
			// 
			resources.ApplyResources(this.speedCol, "speedCol");
			this.speedCol.Name = "speedCol";
			this.speedCol.ReadOnly = true;
			// 
			// directionCol
			// 
			resources.ApplyResources(this.directionCol, "directionCol");
			this.directionCol.Name = "directionCol";
			this.directionCol.ReadOnly = true;
			// 
			// mileageCol
			// 
			resources.ApplyResources(this.mileageCol, "mileageCol");
			this.mileageCol.Name = "mileageCol";
			this.mileageCol.ReadOnly = true;
			// 
			// hdopCol
			// 
			resources.ApplyResources(this.hdopCol, "hdopCol");
			this.hdopCol.Name = "hdopCol";
			this.hdopCol.ReadOnly = true;
			// 
			// speedFreqCol
			// 
			resources.ApplyResources(this.speedFreqCol, "speedFreqCol");
			this.speedFreqCol.Name = "speedFreqCol";
			this.speedFreqCol.ReadOnly = true;
			// 
			// drivingTimeCol
			// 
			resources.ApplyResources(this.drivingTimeCol, "drivingTimeCol");
			this.drivingTimeCol.Name = "drivingTimeCol";
			this.drivingTimeCol.ReadOnly = true;
			// 
			// totalDrivingTimeCol
			// 
			resources.ApplyResources(this.totalDrivingTimeCol, "totalDrivingTimeCol");
			this.totalDrivingTimeCol.Name = "totalDrivingTimeCol";
			this.totalDrivingTimeCol.ReadOnly = true;
			// 
			// doorStatusCol
			// 
			resources.ApplyResources(this.doorStatusCol, "doorStatusCol");
			this.doorStatusCol.Name = "doorStatusCol";
			this.doorStatusCol.ReadOnly = true;
			// 
			// airconStatusCol
			// 
			resources.ApplyResources(this.airconStatusCol, "airconStatusCol");
			this.airconStatusCol.Name = "airconStatusCol";
			this.airconStatusCol.ReadOnly = true;
			// 
			// engineStatusCol
			// 
			resources.ApplyResources(this.engineStatusCol, "engineStatusCol");
			this.engineStatusCol.Name = "engineStatusCol";
			this.engineStatusCol.ReadOnly = true;
			// 
			// adc1ValueCol
			// 
			resources.ApplyResources(this.adc1ValueCol, "adc1ValueCol");
			this.adc1ValueCol.Name = "adc1ValueCol";
			this.adc1ValueCol.ReadOnly = true;
			// 
			// adc2ValueCol
			// 
			resources.ApplyResources(this.adc2ValueCol, "adc2ValueCol");
			this.adc2ValueCol.Name = "adc2ValueCol";
			this.adc2ValueCol.ReadOnly = true;
			// 
			// overSpeedCntCol
			// 
			resources.ApplyResources(this.overSpeedCntCol, "overSpeedCntCol");
			this.overSpeedCntCol.Name = "overSpeedCntCol";
			this.overSpeedCntCol.ReadOnly = true;
			// 
			// parkCntCol
			// 
			resources.ApplyResources(this.parkCntCol, "parkCntCol");
			this.parkCntCol.Name = "parkCntCol";
			this.parkCntCol.ReadOnly = true;
			// 
			// doorOpenCntCol
			// 
			resources.ApplyResources(this.doorOpenCntCol, "doorOpenCntCol");
			this.doorOpenCntCol.Name = "doorOpenCntCol";
			this.doorOpenCntCol.ReadOnly = true;
			// 
			// onOffEngineCntCol
			// 
			resources.ApplyResources(this.onOffEngineCntCol, "onOffEngineCntCol");
			this.onOffEngineCntCol.Name = "onOffEngineCntCol";
			this.onOffEngineCntCol.ReadOnly = true;
			// 
			// parkingTimeCol
			// 
			resources.ApplyResources(this.parkingTimeCol, "parkingTimeCol");
			this.parkingTimeCol.Name = "parkingTimeCol";
			this.parkingTimeCol.ReadOnly = true;
			// 
			// gpsDirectionCol
			// 
			resources.ApplyResources(this.gpsDirectionCol, "gpsDirectionCol");
			this.gpsDirectionCol.Name = "gpsDirectionCol";
			this.gpsDirectionCol.ReadOnly = true;
			// 
			// dataLogTabPage
			// 
			resources.ApplyResources(this.dataLogTabPage, "dataLogTabPage");
			this.dataLogTabPage.Controls.Add(this.clearLogBtn);
			this.dataLogTabPage.Controls.Add(this.logTxtBox);
			this.dataLogTabPage.Name = "dataLogTabPage";
			this.dataLogTabPage.UseVisualStyleBackColor = true;
			// 
			// clearLogBtn
			// 
			resources.ApplyResources(this.clearLogBtn, "clearLogBtn");
			this.clearLogBtn.ForeColor = System.Drawing.Color.Blue;
			this.clearLogBtn.Name = "clearLogBtn";
			this.clearLogBtn.UseVisualStyleBackColor = true;
			this.clearLogBtn.Click += new System.EventHandler(this.clearLogBtn_Click);
			// 
			// logTxtBox
			// 
			resources.ApplyResources(this.logTxtBox, "logTxtBox");
			this.logTxtBox.BackColor = System.Drawing.Color.White;
			this.logTxtBox.Name = "logTxtBox";
			this.logTxtBox.ReadOnly = true;
			// 
			// menuStrip1
			// 
			resources.ApplyResources(this.menuStrip1, "menuStrip1");
			this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.languageToolStripMenuItem,
            this.helpToolStripMenuItem});
			this.menuStrip1.Name = "menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			resources.ApplyResources(this.fileToolStripMenuItem, "fileToolStripMenuItem");
			this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.skinToolStripMenuItem,
            this.defaultToolStripMenuItem,
            this.exitToolStripMenuItem});
			this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			// 
			// skinToolStripMenuItem
			// 
			resources.ApplyResources(this.skinToolStripMenuItem, "skinToolStripMenuItem");
			this.skinToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.blueToolStripMenuItem,
            this.blackToolStripMenuItem,
            this.silverToolStripMenuItem,
            this.aqueToolStripMenuItem});
			this.skinToolStripMenuItem.Name = "skinToolStripMenuItem";
			// 
			// blueToolStripMenuItem
			// 
			resources.ApplyResources(this.blueToolStripMenuItem, "blueToolStripMenuItem");
			this.blueToolStripMenuItem.Name = "blueToolStripMenuItem";
			// 
			// blackToolStripMenuItem
			// 
			resources.ApplyResources(this.blackToolStripMenuItem, "blackToolStripMenuItem");
			this.blackToolStripMenuItem.Name = "blackToolStripMenuItem";
			// 
			// silverToolStripMenuItem
			// 
			resources.ApplyResources(this.silverToolStripMenuItem, "silverToolStripMenuItem");
			this.silverToolStripMenuItem.Name = "silverToolStripMenuItem";
			// 
			// aqueToolStripMenuItem
			// 
			resources.ApplyResources(this.aqueToolStripMenuItem, "aqueToolStripMenuItem");
			this.aqueToolStripMenuItem.Name = "aqueToolStripMenuItem";
			// 
			// defaultToolStripMenuItem
			// 
			resources.ApplyResources(this.defaultToolStripMenuItem, "defaultToolStripMenuItem");
			this.defaultToolStripMenuItem.Name = "defaultToolStripMenuItem";
			// 
			// exitToolStripMenuItem
			// 
			resources.ApplyResources(this.exitToolStripMenuItem, "exitToolStripMenuItem");
			this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
			this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
			// 
			// languageToolStripMenuItem
			// 
			resources.ApplyResources(this.languageToolStripMenuItem, "languageToolStripMenuItem");
			this.languageToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.englishToolStripMenuItem,
            this.vietnameseToolStripMenuItem});
			this.languageToolStripMenuItem.Name = "languageToolStripMenuItem";
			// 
			// englishToolStripMenuItem
			// 
			resources.ApplyResources(this.englishToolStripMenuItem, "englishToolStripMenuItem");
			this.englishToolStripMenuItem.Name = "englishToolStripMenuItem";
			this.englishToolStripMenuItem.Click += new System.EventHandler(this.englishToolStripMenuItem_Click);
			// 
			// vietnameseToolStripMenuItem
			// 
			resources.ApplyResources(this.vietnameseToolStripMenuItem, "vietnameseToolStripMenuItem");
			this.vietnameseToolStripMenuItem.Name = "vietnameseToolStripMenuItem";
			this.vietnameseToolStripMenuItem.Click += new System.EventHandler(this.vietnameseToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			resources.ApplyResources(this.helpToolStripMenuItem, "helpToolStripMenuItem");
			this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.registerToolStripMenuItem,
            this.aboutToolStripMenuItem});
			this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
			// 
			// registerToolStripMenuItem
			// 
			resources.ApplyResources(this.registerToolStripMenuItem, "registerToolStripMenuItem");
			this.registerToolStripMenuItem.Name = "registerToolStripMenuItem";
			this.registerToolStripMenuItem.Click += new System.EventHandler(this.registerToolStripMenuItem_Click);
			// 
			// aboutToolStripMenuItem
			// 
			resources.ApplyResources(this.aboutToolStripMenuItem, "aboutToolStripMenuItem");
			this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
			// 
			// saveFileDialog1
			// 
			resources.ApplyResources(this.saveFileDialog1, "saveFileDialog1");
			// 
			// openFileDialog1
			// 
			resources.ApplyResources(this.openFileDialog1, "openFileDialog1");
			// 
			// selectFolderDialog
			// 
			resources.ApplyResources(this.selectFolderDialog, "selectFolderDialog");
			this.selectFolderDialog.ShowNewFolderButton = false;
			// 
			// imeiLbl
			// 
			resources.ApplyResources(this.imeiLbl, "imeiLbl");
			this.imeiLbl.Name = "imeiLbl";
			// 
			// statusGrpBox
			// 
			resources.ApplyResources(this.statusGrpBox, "statusGrpBox");
			this.statusGrpBox.Controls.Add(this.connectionStatusLbl);
			this.statusGrpBox.Controls.Add(this.retrieveLogPercentLbl);
			this.statusGrpBox.Controls.Add(this.retrieveLogStatusLbl);
			this.statusGrpBox.Controls.Add(this.commuTypeLbl);
			this.statusGrpBox.Controls.Add(this.imeiLbl);
			this.statusGrpBox.Controls.Add(this.retrieveLogPrgBar);
			this.statusGrpBox.Name = "statusGrpBox";
			this.statusGrpBox.TabStop = false;
			// 
			// connectionStatusLbl
			// 
			resources.ApplyResources(this.connectionStatusLbl, "connectionStatusLbl");
			this.connectionStatusLbl.ForeColor = System.Drawing.Color.Red;
			this.connectionStatusLbl.Name = "connectionStatusLbl";
			// 
			// retrieveLogPercentLbl
			// 
			resources.ApplyResources(this.retrieveLogPercentLbl, "retrieveLogPercentLbl");
			this.retrieveLogPercentLbl.Name = "retrieveLogPercentLbl";
			// 
			// retrieveLogStatusLbl
			// 
			resources.ApplyResources(this.retrieveLogStatusLbl, "retrieveLogStatusLbl");
			this.retrieveLogStatusLbl.Name = "retrieveLogStatusLbl";
			// 
			// commuTypeLbl
			// 
			resources.ApplyResources(this.commuTypeLbl, "commuTypeLbl");
			this.commuTypeLbl.ForeColor = System.Drawing.Color.Red;
			this.commuTypeLbl.Name = "commuTypeLbl";
			// 
			// retrieveLogPrgBar
			// 
			resources.ApplyResources(this.retrieveLogPrgBar, "retrieveLogPrgBar");
			this.retrieveLogPrgBar.Name = "retrieveLogPrgBar";
			// 
			// btnPrint
			// 
			resources.ApplyResources(this.btnPrint, "btnPrint");
			this.btnPrint.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.btnPrint.ForeColor = System.Drawing.Color.Blue;
			this.btnPrint.Name = "btnPrint";
			this.btnPrint.UseVisualStyleBackColor = false;
			// 
			// btnResetDevice
			// 
			resources.ApplyResources(this.btnResetDevice, "btnResetDevice");
			this.btnResetDevice.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.btnResetDevice.ForeColor = System.Drawing.Color.Blue;
			this.btnResetDevice.Name = "btnResetDevice";
			this.btnResetDevice.UseVisualStyleBackColor = false;
			this.btnResetDevice.Click += new System.EventHandler(this.resetCPUBtn_Clicked);
			// 
			// btnResetToDefault
			// 
			resources.ApplyResources(this.btnResetToDefault, "btnResetToDefault");
			this.btnResetToDefault.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.btnResetToDefault.ForeColor = System.Drawing.Color.Blue;
			this.btnResetToDefault.Name = "btnResetToDefault";
			this.btnResetToDefault.UseVisualStyleBackColor = false;
			this.btnResetToDefault.Click += new System.EventHandler(this.factoryResetBtn_Clicked);
			// 
			// btnReadConfig
			// 
			resources.ApplyResources(this.btnReadConfig, "btnReadConfig");
			this.btnReadConfig.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.btnReadConfig.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
			this.btnReadConfig.ForeColor = System.Drawing.Color.Blue;
			this.btnReadConfig.Name = "btnReadConfig";
			this.btnReadConfig.UseVisualStyleBackColor = false;
			this.btnReadConfig.Click += new System.EventHandler(this.readSettingsBtn_Clicked);
			// 
			// btnWriteConfig
			// 
			resources.ApplyResources(this.btnWriteConfig, "btnWriteConfig");
			this.btnWriteConfig.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.btnWriteConfig.ForeColor = System.Drawing.Color.Blue;
			this.btnWriteConfig.Name = "btnWriteConfig";
			this.btnWriteConfig.UseVisualStyleBackColor = false;
			this.btnWriteConfig.Click += new System.EventHandler(this.writeSettingsBtn_Clicked);
			// 
			// commGrpBox
			// 
			resources.ApplyResources(this.commGrpBox, "commGrpBox");
			this.commGrpBox.Controls.Add(this.openPortBtn);
			this.commGrpBox.Controls.Add(this.deviceSelectionCmbBox);
			this.commGrpBox.Controls.Add(this.comPortCmbBox);
			this.commGrpBox.Controls.Add(this.commTypeCmbBox);
			this.commGrpBox.Name = "commGrpBox";
			this.commGrpBox.TabStop = false;
			// 
			// openPortBtn
			// 
			resources.ApplyResources(this.openPortBtn, "openPortBtn");
			this.openPortBtn.Name = "openPortBtn";
			this.openPortBtn.UseVisualStyleBackColor = true;
			this.openPortBtn.Click += new System.EventHandler(this.openPortBtn_Click);
			// 
			// deviceSelectionCmbBox
			// 
			resources.ApplyResources(this.deviceSelectionCmbBox, "deviceSelectionCmbBox");
			this.deviceSelectionCmbBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.deviceSelectionCmbBox.FormattingEnabled = true;
			this.deviceSelectionCmbBox.Items.AddRange(new object[] {
            resources.GetString("deviceSelectionCmbBox.Items")});
			this.deviceSelectionCmbBox.Name = "deviceSelectionCmbBox";
			// 
			// comPortCmbBox
			// 
			resources.ApplyResources(this.comPortCmbBox, "comPortCmbBox");
			this.comPortCmbBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comPortCmbBox.FormattingEnabled = true;
			this.comPortCmbBox.Name = "comPortCmbBox";
			// 
			// commTypeCmbBox
			// 
			resources.ApplyResources(this.commTypeCmbBox, "commTypeCmbBox");
			this.commTypeCmbBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.commTypeCmbBox.FormattingEnabled = true;
			this.commTypeCmbBox.Items.AddRange(new object[] {
            resources.GetString("commTypeCmbBox.Items"),
            resources.GetString("commTypeCmbBox.Items1")});
			this.commTypeCmbBox.Name = "commTypeCmbBox";
			this.commTypeCmbBox.SelectedIndexChanged += new System.EventHandler(this.commTypeCmbBox_SelectedIndexChanged);
			// 
			// deviceCmdGBox
			// 
			resources.ApplyResources(this.deviceCmdGBox, "deviceCmdGBox");
			this.deviceCmdGBox.Controls.Add(this.selectLogTypeCmbBox);
			this.deviceCmdGBox.Controls.Add(this.formatSDCardBtn);
			this.deviceCmdGBox.Controls.Add(this.resetCPUBtn);
			this.deviceCmdGBox.Controls.Add(this.factoryResetBtn);
			this.deviceCmdGBox.Controls.Add(this.printReportBtn);
			this.deviceCmdGBox.Controls.Add(this.defaultSettingsBtn);
			this.deviceCmdGBox.Controls.Add(this.loadSettingsBtn);
			this.deviceCmdGBox.Controls.Add(this.saveSettingsBtn);
			this.deviceCmdGBox.Controls.Add(this.writeSettingsBtn);
			this.deviceCmdGBox.Controls.Add(this.readSettingsBtn);
			this.deviceCmdGBox.Name = "deviceCmdGBox";
			this.deviceCmdGBox.TabStop = false;
			// 
			// selectLogTypeCmbBox
			// 
			resources.ApplyResources(this.selectLogTypeCmbBox, "selectLogTypeCmbBox");
			this.selectLogTypeCmbBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.selectLogTypeCmbBox.FormattingEnabled = true;
			this.selectLogTypeCmbBox.Items.AddRange(new object[] {
            resources.GetString("selectLogTypeCmbBox.Items"),
            resources.GetString("selectLogTypeCmbBox.Items1"),
            resources.GetString("selectLogTypeCmbBox.Items2"),
            resources.GetString("selectLogTypeCmbBox.Items3"),
            resources.GetString("selectLogTypeCmbBox.Items4")});
			this.selectLogTypeCmbBox.Name = "selectLogTypeCmbBox";
			this.selectLogTypeCmbBox.SelectedIndexChanged += new System.EventHandler(this.selectLogTypeCmbBox_SelectedIndexChanged);
			// 
			// formatSDCardBtn
			// 
			resources.ApplyResources(this.formatSDCardBtn, "formatSDCardBtn");
			this.formatSDCardBtn.Name = "formatSDCardBtn";
			this.formatSDCardBtn.UseVisualStyleBackColor = true;
			this.formatSDCardBtn.Click += new System.EventHandler(this.formatSDCardBtn_Clicked);
			// 
			// resetCPUBtn
			// 
			resources.ApplyResources(this.resetCPUBtn, "resetCPUBtn");
			this.resetCPUBtn.Name = "resetCPUBtn";
			this.resetCPUBtn.UseVisualStyleBackColor = true;
			this.resetCPUBtn.Click += new System.EventHandler(this.resetCPUBtn_Clicked);
			// 
			// factoryResetBtn
			// 
			resources.ApplyResources(this.factoryResetBtn, "factoryResetBtn");
			this.factoryResetBtn.Name = "factoryResetBtn";
			this.factoryResetBtn.UseVisualStyleBackColor = true;
			this.factoryResetBtn.Click += new System.EventHandler(this.factoryResetBtn_Clicked);
			// 
			// printReportBtn
			// 
			resources.ApplyResources(this.printReportBtn, "printReportBtn");
			this.printReportBtn.Name = "printReportBtn";
			this.printReportBtn.UseVisualStyleBackColor = true;
			// 
			// defaultSettingsBtn
			// 
			resources.ApplyResources(this.defaultSettingsBtn, "defaultSettingsBtn");
			this.defaultSettingsBtn.Name = "defaultSettingsBtn";
			this.defaultSettingsBtn.UseVisualStyleBackColor = true;
			// 
			// loadSettingsBtn
			// 
			resources.ApplyResources(this.loadSettingsBtn, "loadSettingsBtn");
			this.loadSettingsBtn.Name = "loadSettingsBtn";
			this.loadSettingsBtn.UseVisualStyleBackColor = true;
			this.loadSettingsBtn.Click += new System.EventHandler(this.loadSettingsBtn_Click);
			// 
			// saveSettingsBtn
			// 
			resources.ApplyResources(this.saveSettingsBtn, "saveSettingsBtn");
			this.saveSettingsBtn.Name = "saveSettingsBtn";
			this.saveSettingsBtn.UseVisualStyleBackColor = true;
			this.saveSettingsBtn.Click += new System.EventHandler(this.saveSettingsBtn_Click);
			// 
			// writeSettingsBtn
			// 
			resources.ApplyResources(this.writeSettingsBtn, "writeSettingsBtn");
			this.writeSettingsBtn.Name = "writeSettingsBtn";
			this.writeSettingsBtn.UseVisualStyleBackColor = true;
			this.writeSettingsBtn.Click += new System.EventHandler(this.writeSettingsBtn_Clicked);
			// 
			// readSettingsBtn
			// 
			resources.ApplyResources(this.readSettingsBtn, "readSettingsBtn");
			this.readSettingsBtn.Name = "readSettingsBtn";
			this.readSettingsBtn.UseVisualStyleBackColor = true;
			this.readSettingsBtn.Click += new System.EventHandler(this.readSettingsBtn_Clicked);
			// 
			// Column9
			// 
			resources.ApplyResources(this.Column9, "Column9");
			this.Column9.Name = "Column9";
			this.Column9.ReadOnly = true;
			// 
			// Column8
			// 
			resources.ApplyResources(this.Column8, "Column8");
			this.Column8.Name = "Column8";
			this.Column8.ReadOnly = true;
			// 
			// Column7
			// 
			resources.ApplyResources(this.Column7, "Column7");
			this.Column7.Name = "Column7";
			this.Column7.ReadOnly = true;
			// 
			// Column6
			// 
			resources.ApplyResources(this.Column6, "Column6");
			this.Column6.Name = "Column6";
			this.Column6.ReadOnly = true;
			// 
			// Column5
			// 
			resources.ApplyResources(this.Column5, "Column5");
			this.Column5.Name = "Column5";
			this.Column5.ReadOnly = true;
			// 
			// Column4
			// 
			resources.ApplyResources(this.Column4, "Column4");
			this.Column4.Name = "Column4";
			this.Column4.ReadOnly = true;
			// 
			// Column3
			// 
			resources.ApplyResources(this.Column3, "Column3");
			this.Column3.Name = "Column3";
			this.Column3.ReadOnly = true;
			// 
			// dataGridViewTextBoxColumn5
			// 
			resources.ApplyResources(this.dataGridViewTextBoxColumn5, "dataGridViewTextBoxColumn5");
			this.dataGridViewTextBoxColumn5.Name = "dataGridViewTextBoxColumn5";
			this.dataGridViewTextBoxColumn5.ReadOnly = true;
			// 
			// dataGridViewTextBoxColumn4
			// 
			resources.ApplyResources(this.dataGridViewTextBoxColumn4, "dataGridViewTextBoxColumn4");
			this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
			this.dataGridViewTextBoxColumn4.ReadOnly = true;
			// 
			// dataGridViewTextBoxColumn3
			// 
			resources.ApplyResources(this.dataGridViewTextBoxColumn3, "dataGridViewTextBoxColumn3");
			this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
			this.dataGridViewTextBoxColumn3.ReadOnly = true;
			// 
			// dataGridViewTextBoxColumn2
			// 
			resources.ApplyResources(this.dataGridViewTextBoxColumn2, "dataGridViewTextBoxColumn2");
			this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
			this.dataGridViewTextBoxColumn2.ReadOnly = true;
			// 
			// dataGridViewTextBoxColumn1
			// 
			resources.ApplyResources(this.dataGridViewTextBoxColumn1, "dataGridViewTextBoxColumn1");
			this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
			this.dataGridViewTextBoxColumn1.ReadOnly = true;
			// 
			// MainForm
			// 
			resources.ApplyResources(this, "$this");
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.deviceCmdGBox);
			this.Controls.Add(this.commGrpBox);
			this.Controls.Add(this.tabMain);
			this.Controls.Add(this.menuStrip1);
			this.Controls.Add(this.statusGrpBox);
			this.Name = "MainForm";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.tabMain.ResumeLayout(false);
			this.settingTabPage.ResumeLayout(false);
			this.grbOtherSettings.ResumeLayout(false);
			this.grbOtherSettings.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.grbFeatureSet.ResumeLayout(false);
			this.grbFeatureSet.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.grbDrivingSettings.ResumeLayout(false);
			this.grbDrivingSettings.PerformLayout();
			this.groupBox10.ResumeLayout(false);
			this.groupBox10.PerformLayout();
			this.grbAnalogInput.ResumeLayout(false);
			this.grbAnalogInput.PerformLayout();
			this.grbAuthorizedUser.ResumeLayout(false);
			this.grbAuthorizedUser.PerformLayout();
			this.grbVehicleParameter.ResumeLayout(false);
			this.grbVehicleParameter.PerformLayout();
			this.grbServerParameter.ResumeLayout(false);
			this.grbServerParameter.PerformLayout();
			this.grbGPRS.ResumeLayout(false);
			this.grbGPRS.PerformLayout();
			this.detailReportTabPage.ResumeLayout(false);
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.detailReportGridView)).EndInit();
			this.dataLogTabPage.ResumeLayout(false);
			this.dataLogTabPage.PerformLayout();
			this.menuStrip1.ResumeLayout(false);
			this.menuStrip1.PerformLayout();
			this.statusGrpBox.ResumeLayout(false);
			this.statusGrpBox.PerformLayout();
			this.commGrpBox.ResumeLayout(false);
			this.deviceCmdGBox.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl tabMain;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.TabPage detailReportTabPage;
        private System.Windows.Forms.DataGridView detailReportGridView;
        private System.Windows.Forms.FolderBrowserDialog selectFolderDialog;
        private System.Windows.Forms.TextBox logTxtBox;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.Label imeiLbl;
        private System.Windows.Forms.GroupBox statusGrpBox;
        private System.Windows.Forms.ProgressBar retrieveLogPrgBar;
        private System.Windows.Forms.Label retrieveLogStatusLbl;
        private System.Windows.Forms.Label retrieveLogPercentLbl;
        private System.Windows.Forms.Label commuTypeLbl;
        private System.Windows.Forms.TabPage settingTabPage;
        private System.Windows.Forms.GroupBox groupBox10;
        private System.Windows.Forms.TextBox firmwarePathTxtBox;
        private System.Windows.Forms.GroupBox grbAnalogInput;
        private System.Windows.Forms.TextBox analogLowerBoundTxtBox;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.TextBox analogUpperBoundTxtBox;
        private System.Windows.Forms.Label label37;
        private System.Windows.Forms.CheckBox analogPercentageModeChkBox;
        private System.Windows.Forms.ComboBox analogInputNoCmbBox;
        private System.Windows.Forms.GroupBox grbAuthorizedUser;
        private System.Windows.Forms.TextBox authPhoneNoTxtBox;
        private System.Windows.Forms.Label lblUserNumber;
        private System.Windows.Forms.ComboBox userNoCmbBox;
        private System.Windows.Forms.Label lblAuthorizedUserName;
        private System.Windows.Forms.TextBox authUserNameTxtBox;
        private System.Windows.Forms.Label lblAuthorizedPhoneNumber;
        private System.Windows.Forms.GroupBox grbVehicleParameter;
        private System.Windows.Forms.RadioButton pulseSpeedSourceRBtn;
        private System.Windows.Forms.RadioButton gpsSpeedSourceRBtn;
        private System.Windows.Forms.Label lblSpeedSource;
        private System.Windows.Forms.TextBox tyreRateTxtBox;
        private System.Windows.Forms.TextBox vinNumTxtBox;
        private System.Windows.Forms.TextBox plateNoTxtBox;
        private System.Windows.Forms.TextBox trackerIdTxtBox;
        private System.Windows.Forms.Label lblTyreRate;
        private System.Windows.Forms.Label lblCarNumber;
        private System.Windows.Forms.Label lblTrackerID;
        private System.Windows.Forms.Label lblVINNumber;
        private System.Windows.Forms.GroupBox grbServerParameter;
        private System.Windows.Forms.ComboBox serverSelectionCmbBox;
        private System.Windows.Forms.Label lblServerSelection;
        private System.Windows.Forms.RadioButton serverUseDomainRBtn;
        private System.Windows.Forms.TextBox secDomainTxtBox;
        private System.Windows.Forms.TextBox priDomainTxtBox;
        private System.Windows.Forms.RadioButton serverUseIpRBtn;
        private System.Windows.Forms.TextBox secPortTxtBox;
        private System.Windows.Forms.TextBox priPortTxtBox;
        private System.Windows.Forms.TextBox secIpTxtBox;
        private System.Windows.Forms.Label lblSecondaryServer;
        private System.Windows.Forms.Label lblPort;
        private System.Windows.Forms.TextBox priIpTxtBox;
        private System.Windows.Forms.Label label39;
        private System.Windows.Forms.GroupBox grbGPRS;
        private System.Windows.Forms.TextBox gprsStopReportIntervalTxtBox;
        private System.Windows.Forms.TextBox gprsRunReportIntervalTxtBox;
        private System.Windows.Forms.TextBox gprsPwdTxtBox;
        private System.Windows.Forms.TextBox gprsUserTxtBox;
        private System.Windows.Forms.TextBox gprsApnTxtBox;
        private System.Windows.Forms.Label lblOFF;
        private System.Windows.Forms.Label lblON;
        private System.Windows.Forms.Label lblIntevalTime;
        private System.Windows.Forms.Label lblPassword;
        private System.Windows.Forms.Label lblUser;
        private System.Windows.Forms.Label lblAPN;
        private System.Windows.Forms.Button browseFirmwareBtn;
        private System.Windows.Forms.Button downloadFirmwareBtn;
        private System.Windows.Forms.Button readFirmwareVersionBtn;
        private System.Windows.Forms.GroupBox grbDrivingSettings;
        private System.Windows.Forms.TextBox totalDrivingTimeLimitTxtBox;
        private System.Windows.Forms.TextBox drivingTimeLimitTxtBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox speedLimitTxtBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TabPage dataLogTabPage;
        private System.Windows.Forms.Button btnPrint;
        private System.Windows.Forms.Button btnResetDevice;
        private System.Windows.Forms.Button btnResetToDefault;
        private System.Windows.Forms.Button btnReadConfig;
        private System.Windows.Forms.Button btnWriteConfig;
        private System.Windows.Forms.GroupBox commGrpBox;
        private System.Windows.Forms.Button openPortBtn;
        private System.Windows.Forms.ComboBox deviceSelectionCmbBox;
        private System.Windows.Forms.ComboBox comPortCmbBox;
        private System.Windows.Forms.ComboBox commTypeCmbBox;
        private System.Windows.Forms.GroupBox deviceCmdGBox;
        private System.Windows.Forms.Button defaultSettingsBtn;
        private System.Windows.Forms.Button loadSettingsBtn;
        private System.Windows.Forms.Button saveSettingsBtn;
        private System.Windows.Forms.Button writeSettingsBtn;
        private System.Windows.Forms.Button readSettingsBtn;
        private System.Windows.Forms.Button formatSDCardBtn;
        private System.Windows.Forms.Button resetCPUBtn;
        private System.Windows.Forms.Button factoryResetBtn;
        private System.Windows.Forms.Button printReportBtn;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column9;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column8;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column7;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column6;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column5;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column4;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column3;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn5;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.Label connectionStatusLbl;
        private System.Windows.Forms.ToolStripMenuItem skinToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem blueToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem blackToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem silverToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aqueToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem defaultToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem languageToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem englishToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem vietnameseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox baseMileageTxtBox;
        private System.Windows.Forms.Label firmwareVersionLbl;
        private System.Windows.Forms.Button clearLogBtn;
        private System.Windows.Forms.ComboBox selectLogTypeCmbBox;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button sendSMSBtn;
        private System.Windows.Forms.TextBox smsMessageTxt;
        private System.Windows.Forms.TextBox smsAddressTxt;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button runAppBtn;
        private System.Windows.Forms.TextBox bossPhoneTxtBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ToolStripMenuItem registerToolStripMenuItem;
		private System.Windows.Forms.GroupBox grbFeatureSet;
        private System.Windows.Forms.CheckBox lowPowerWarningChkBox;
        private System.Windows.Forms.CheckBox lowBatteryWarningChkBox;
        private System.Windows.Forms.CheckBox removePowerWarningChkBox;
        private System.Windows.Forms.GroupBox grbOtherSettings;
        private System.Windows.Forms.TextBox powerOffDelayTimeTxt;
		private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox speedEngineOffTxt;
		private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox removePowerWarningPeriodTxt;
        private System.Windows.Forms.TextBox batteryLowWarningPeriodTxt;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox sosKeyPressTimeTxt;
        private System.Windows.Forms.TextBox sosKeyPressPeriodTxt;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.CheckBox callActionChkBox;
        private System.Windows.Forms.CheckBox sendSMSActionChkBox;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button exportToKMLBtn;
        private System.Windows.Forms.Button readReportBtn;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn12;
        private System.Windows.Forms.DataGridViewTextBoxColumn userNameCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn timeCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn lgnCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn latCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn speedCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn directionCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn mileageCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn hdopCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn speedFreqCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn drivingTimeCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn totalDrivingTimeCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn doorStatusCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn airconStatusCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn engineStatusCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn adc1ValueCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn adc2ValueCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn overSpeedCntCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn parkCntCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn doorOpenCntCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn onOffEngineCntCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn parkingTimeCol;
		private System.Windows.Forms.DataGridViewTextBoxColumn gpsDirectionCol;
        private System.Windows.Forms.TextBox batteryLowWarningLevelTxt;
		private System.Windows.Forms.Label label13;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox manualSecurityOnChkBox;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.CheckBox autoSecurityOnChkBox;
		private System.Windows.Forms.TextBox autoSecurityTimeTxt;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.CheckBox enableFifoChkBox;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.TextBox accountAlarmCheckTxt;

    }
}

