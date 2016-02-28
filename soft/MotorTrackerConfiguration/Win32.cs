using Microsoft.VisualBasic;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using Microsoft.Win32.SafeHandles;

public class Win32
{
	#region "Constants"
	internal const Int16 DIGCF_PRESENT = 0x2;

	internal const Int16 DIGCF_DEVICEINTERFACE = 0x10;
	internal const Int32 FILE_FLAG_OVERLAPPED = 0x40000000;
	internal const Int32 FILE_SHARE_READ = 1;
	internal const Int32 FILE_SHARE_WRITE = 2;
	internal const UInt32 GENERIC_READ = 0x80000000;
	internal const UInt32 GENERIC_WRITE = 0x40000000;
	internal const Int32 INVALID_HANDLE_VALUE = -1;

	internal const Int32 OPEN_EXISTING = 3;
	internal const Int32 WAIT_TIMEOUT = 0x102;
	internal const Int32 WAIT_OBJECT_0 = 0;

	internal const UInt32 INFINITE = 0xffffffffu;

	internal const Int16 FORMAT_MESSAGE_FROM_SYSTEM = 0x1000;
    public const Int32 WM_DEVICECHANGE = 0x219;
    internal const Int32 WM_USER = 0x400;

    public const Int32 WM_READCOMPLETE = WM_USER + 1;
    public const Int32 DBT_DEVICEARRIVAL = 0x8000;
    public const Int32 DBT_DEVICEREMOVECOMPLETE = 0x8004;
    internal const Int32 DEVICE_NOTIFY_WINDOW_HANDLE = 0;
    internal const Int32 DEVICE_NOTIFY_SERVICE_HANDLE = 1;

	internal const Int32 DBT_DEVTYP_DEVICEINTERFACE = 5;
    internal const Int32 ERROR_HANDLE_EOF = 0x26;

    internal const Int32 ERROR_IO_PENDING = 0x3e5;
	#endregion

	#region "Structs"
	[StructLayout(LayoutKind.Sequential)]
	internal struct SP_DEVICE_INTERFACE_DETAIL_DATA
	{
		public int cbSize;
		public string DevicePath;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct SP_DEVICE_INTERFACE_DATA
	{
		public int cbSize;
		public System.Guid InterfaceClassGuid;
		public int Flags;
		public int Reserved;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct SECURITY_ATTRIBUTES
	{
		public int nLength;
		public int lpSecurityDescriptor;
		public int bInheritHandle;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct HIDD_ATTRIBUTES
	{
		public Int32 Size;
		public UInt16 VendorID;
        public UInt16 ProductID;
        public UInt16 VersionNumber;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct HIDP_CAPS
	{
        public Int16 Usage;
        public Int16 UsagePage;
		public Int16 InputReportByteLength;
		public Int16 OutputReportByteLength;
		public Int16 FeatureReportByteLength;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 17)]
		public Int16[] Reserved;
		public Int16 NumberLinkCollectionNodes;
		public Int16 NumberInputButtonCaps;
		public Int16 NumberInputValueCaps;
		public Int16 NumberInputDataIndices;
		public Int16 NumberOutputButtonCaps;
		public Int16 NumberOutputValueCaps;
		public Int16 NumberOutputDataIndices;
		public Int16 NumberFeatureButtonCaps;
		public Int16 NumberFeatureValueCaps;

		public Int16 NumberFeatureDataIndices;
	}

	[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 20)]
	internal struct OVERLAPPED
	{
		public Int32 Internal;
		public Int32 InternalHigh;
		public Int32 Offset;
		public Int32 OffsetHigh;
		public IntPtr hEvent;
	}

    [StructLayout(LayoutKind.Sequential)]
    internal class DEV_BROADCAST_DEVICEINTERFACE
    {
        internal Int32 dbcc_size;
        internal Int32 dbcc_devicetype;
        internal Int32 dbcc_reserved;
        internal Guid dbcc_classguid;
        internal Int16 dbcc_name;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    internal class DEV_BROADCAST_DEVICEINTERFACE_1
    {
        internal Int32 dbcc_size;
        internal Int32 dbcc_devicetype;
        internal Int32 dbcc_reserved;
        [MarshalAs(UnmanagedType.ByValArray, ArraySubType = UnmanagedType.U1, SizeConst = 16)]
        internal Byte[] dbcc_classguid;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 255)]
        internal Char[] dbcc_name;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal class DEV_BROADCAST_HDR
    {
        internal Int32 dbch_size;
        internal Int32 dbch_devicetype;
        internal Int32 dbch_reserved;
    }

	#endregion

	#region "HID.DLL"
    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_FlushQueue(SafeFileHandle HidDeviceObject);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_FreePreparsedData(IntPtr PreparsedData);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_GetAttributes(SafeFileHandle HidDeviceObject, ref HIDD_ATTRIBUTES Attributes);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_GetFeature(SafeFileHandle HidDeviceObject, Byte[] lpReportBuffer, Int32 ReportBufferLength);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_GetInputReport(SafeFileHandle HidDeviceObject, Byte[] lpReportBuffer, Int32 ReportBufferLength);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern void HidD_GetHidGuid(ref System.Guid HidGuid);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_GetNumInputBuffers(SafeFileHandle HidDeviceObject, ref Int32 NumberBuffers);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_GetPreparsedData(SafeFileHandle HidDeviceObject, ref IntPtr PreparsedData);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_SetFeature(SafeFileHandle HidDeviceObject, Byte[] lpReportBuffer, Int32 ReportBufferLength);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_SetNumInputBuffers(SafeFileHandle HidDeviceObject, Int32 NumberBuffers);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Boolean HidD_SetOutputReport(SafeFileHandle HidDeviceObject, Byte[] lpReportBuffer, Int32 ReportBufferLength);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Int32 HidP_GetCaps(IntPtr PreparsedData, ref HIDP_CAPS Capabilities);

    [DllImport("hid.dll", SetLastError = true)]
    internal static extern Int32 HidP_GetValueCaps(Int32 ReportType, Byte[] ValueCaps, ref Int32 ValueCapsLength, IntPtr PreparsedData);
	#endregion

	#region "KERNEL32.DLL"

	[DllImport("kernel32.dll", SetLastError = true)]
	static internal extern bool CancelIo(IntPtr hFile);

	[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    static internal extern SafeFileHandle CreateFile(String lpFileName, UInt32 dwDesiredAccess, Int32 dwShareMode, IntPtr lpSecurityAttributes, Int32 dwCreationDisposition, Int32 dwFlagsAndAttributes, Int32 hTemplateFile);

	[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
	static internal extern IntPtr CreateEvent(IntPtr lpEventAttributes, bool bManualReset, bool bInitialState, string lpName);

	[DllImport("kernel32.dll", SetLastError = true)]
	static internal extern bool ReadFile(IntPtr hFile, ref byte lpBuffer, Int32 nNumberOfBytesToRead, ref Int32 lpNumberOfBytesRead, ref OVERLAPPED lpOverlapped);

	[DllImport("kernel32.dll", SetLastError = true)]
	static internal extern bool WriteFile(IntPtr hFile, byte[] lpBuffer, Int32 nNumberOfBytesToWrite, ref Int32 lpNumberOfBytesWritten, ref OVERLAPPED lpOverlapped);

	[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
	static internal extern bool CloseHandle(IntPtr hObject);

	[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
	static internal extern int FormatMessage(int dwFlags, ref long lpSource, int dwMessageId, int dwLanguageZId, string lpBuffer, int nSize, int Arguments);

	[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
	static internal extern bool ResetEvent(IntPtr hEvent);

	[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
	static internal extern bool GetOverlappedResult(IntPtr hFile, ref OVERLAPPED lpOverlapped, ref Int32 lpNumberOfBytesTransferred, bool bWait);

	[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
	static internal extern UInt32 WaitForSingleObject(IntPtr handle, UInt32 milliseconds);
	#endregion

	#region "SETUPAPI.DLL"
    [DllImport("setupapi.dll", SetLastError = true)]
    internal static extern Int32 SetupDiCreateDeviceInfoList(ref System.Guid ClassGuid, Int32 hwndParent);

    [DllImport("setupapi.dll", SetLastError = true)]
    internal static extern Int32 SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);

    [DllImport("setupapi.dll", SetLastError = true)]
    internal static extern Boolean SetupDiEnumDeviceInterfaces(IntPtr DeviceInfoSet, IntPtr DeviceInfoData, ref System.Guid InterfaceClassGuid, Int32 MemberIndex, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData);

    [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Auto)]
    internal static extern IntPtr SetupDiGetClassDevs(ref System.Guid ClassGuid, IntPtr Enumerator, IntPtr hwndParent, Int32 Flags);

    [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Auto)]
    internal static extern Boolean SetupDiGetDeviceInterfaceDetail(IntPtr DeviceInfoSet, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData, IntPtr DeviceInterfaceDetailData, Int32 DeviceInterfaceDetailDataSize, ref Int32 RequiredSize, IntPtr DeviceInfoData);

	#endregion

	#region "USER32.DLL"
	[DllImport("user32.DLL", CharSet = CharSet.Auto, SetLastError = true)]
	internal static extern IntPtr RegisterDeviceNotification(IntPtr IntPtr, IntPtr NotificationFilter, Int32 Flags);

	[DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
	internal static extern int SendMessage(int hwnd, int wMsg, int wParam, int lParam);

    [DllImport("user32.dll", SetLastError = true)]
    internal static extern Boolean UnregisterDeviceNotification(IntPtr Handle);
	#endregion

}