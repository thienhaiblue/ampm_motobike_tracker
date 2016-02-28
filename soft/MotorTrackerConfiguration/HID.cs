using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using Microsoft.Win32.SafeHandles;
using System.IO;
using System.Windows.Forms;

internal class HID
{
    public class Device
    {
        public bool connected;
        public string pathName;
        public SafeFileHandle readHandle;
        public SafeFileHandle writeHandle;
        public FileStream readStream;
        public FileStream writeStream;
        public Win32.HIDD_ATTRIBUTES attributes;
        public Win32.HIDP_CAPS capabilities;
    }

    public static void CloseDevice(ref Device device)
    {
        if (device == null)
        {
            return;
        }
        device.connected = false;
        if (device.readStream != null)
        {
            device.readStream.Close();
        }
        if (device.writeStream != null)
        {
            device.writeStream.Close();
        }
        if (device.readHandle != null)
        {
            device.readHandle.Close();
        }
        if (device.writeHandle != null)
        {
            device.writeHandle.Close();
        }
    }

    public static bool OpenDevice(ref Device device)
    {
        device.readHandle = Win32.CreateFile(device.pathName, Win32.GENERIC_READ, Win32.FILE_SHARE_READ | Win32.FILE_SHARE_WRITE, IntPtr.Zero, Win32.OPEN_EXISTING, 0, 0);
        device.writeHandle = Win32.CreateFile(device.pathName, Win32.GENERIC_WRITE, Win32.FILE_SHARE_READ | Win32.FILE_SHARE_WRITE, IntPtr.Zero, Win32.OPEN_EXISTING, 0, 0);
        if (device.readHandle.IsInvalid || device.writeHandle.IsInvalid) return false;

        // get device attributes
        Win32.HidD_GetAttributes(device.readHandle, ref device.attributes);
        
        // get device capablities
        IntPtr preparsedData = IntPtr.Zero;
        Win32.HidD_GetPreparsedData(device.readHandle, ref preparsedData);
        Win32.HidP_GetCaps(preparsedData, ref device.capabilities);
        Win32.HidD_FreePreparsedData(preparsedData);

        // open the file treams
        device.readStream = new FileStream(device.readHandle, FileAccess.Read, device.capabilities.InputReportByteLength, false);
        device.writeStream = new FileStream(device.writeHandle, FileAccess.Write, device.capabilities.OutputReportByteLength, false);

        device.connected = true;
        
        return true;
    }

    /// <summary>
    /// List all attached HID devices
    /// </summary>
    /// <param name="hidDevicePathName"></param>
    private static void FindHidDevice(ref string[] hidDevicePathName)
    {
        Win32.SP_DEVICE_INTERFACE_DATA MyDeviceInterfaceData = new Win32.SP_DEVICE_INTERFACE_DATA();
        int MemberIndex = 0;
        int BufferSize = 0;
        string SingledevicePathName;
        bool success = false;
        Guid HidGuid = Guid.Empty;

        Win32.HidD_GetHidGuid(ref HidGuid);

        IntPtr DeviceInfoSet = Win32.SetupDiGetClassDevs(ref HidGuid, IntPtr.Zero, IntPtr.Zero, Win32.DIGCF_PRESENT | Win32.DIGCF_DEVICEINTERFACE);

        while (true)
        {
            MyDeviceInterfaceData.cbSize = Marshal.SizeOf(MyDeviceInterfaceData);

            if (Win32.SetupDiEnumDeviceInterfaces(DeviceInfoSet, IntPtr.Zero, ref HidGuid, MemberIndex, ref MyDeviceInterfaceData))
            {
                // find how much data this fucntion return
                success = Win32.SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, ref MyDeviceInterfaceData, IntPtr.Zero, 0, ref BufferSize, IntPtr.Zero);
                // allocate buffer
                IntPtr DetailDataBuffer = Marshal.AllocHGlobal(BufferSize);
                // write the cbSize field
                Marshal.WriteInt32(DetailDataBuffer, 4 + Marshal.SystemDefaultCharSize);
                // get data
                Win32.SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, ref MyDeviceInterfaceData, DetailDataBuffer, BufferSize, ref BufferSize, IntPtr.Zero);

                IntPtr pdevicePathName = new IntPtr(DetailDataBuffer.ToInt32() + 4);

                SingledevicePathName = Marshal.PtrToStringAuto(pdevicePathName);
                Marshal.FreeHGlobal(DetailDataBuffer);
                Array.Resize(ref hidDevicePathName, hidDevicePathName.Length + 1);
                hidDevicePathName[hidDevicePathName.Length - 1] = SingledevicePathName;
            }
            else
            {
                break;
            }
            MemberIndex++;
        }
    }

    /// <summary>
    /// List all attached devices with specific VID_PID
    /// </summary>
    /// <param name="vendorID">device vendor ID</param>
    /// <param name="productID">device product ID</param>
    /// <param name="devicePathName">list of path name to open device with CreateFile()</param>
    /// <returns></returns>
    public static int FindDevices(short vendorID, short productID, ref string[] devicePathName)
    {
        string[] hidDevicePathName = new string[0];
        int i = 0;
        int numberOfDevicesFound = 0;
        SafeFileHandle deviceHandle;
        bool result = false;
        Win32.HIDD_ATTRIBUTES deviceAttributes = new Win32.HIDD_ATTRIBUTES();

        FindHidDevice(ref hidDevicePathName);

        devicePathName = new string[0];

        for (i = 0; i < hidDevicePathName.Length; i++)
        {
            deviceHandle = Win32.CreateFile(hidDevicePathName[i], 0, Win32.FILE_SHARE_READ | Win32.FILE_SHARE_WRITE, IntPtr.Zero, Win32.OPEN_EXISTING, 0, 0);

            if (!deviceHandle.IsInvalid)
            {
                result = Win32.HidD_GetAttributes(deviceHandle, ref deviceAttributes);

                if (result && deviceAttributes.ProductID == productID && deviceAttributes.VendorID == vendorID)
                {
                    numberOfDevicesFound += 1;
                    Array.Resize(ref devicePathName, numberOfDevicesFound);
                    devicePathName[numberOfDevicesFound - 1] = hidDevicePathName[i];
                }
                deviceHandle.Close();
            }
        }

        return numberOfDevicesFound;
    }

    public static bool RegisterDeviceNotification(IntPtr formHandle, ref IntPtr notificationHandle)
    {
        Win32.DEV_BROADCAST_DEVICEINTERFACE dbi = new Win32.DEV_BROADCAST_DEVICEINTERFACE();
        int size = 0;
        IntPtr Buffer = IntPtr.Zero;
        Guid HidGuid = new Guid();
        Win32.HidD_GetHidGuid(ref HidGuid);

        size = Marshal.SizeOf(dbi);
        dbi.dbcc_size = size;
        dbi.dbcc_devicetype = Win32.DBT_DEVTYP_DEVICEINTERFACE;
        dbi.dbcc_reserved = 0;
        dbi.dbcc_classguid = HidGuid;
        dbi.dbcc_name = 0;

        Buffer = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(dbi, Buffer, true);

        notificationHandle = Win32.RegisterDeviceNotification(formHandle, Buffer, Win32.DEVICE_NOTIFY_WINDOW_HANDLE);

        if (notificationHandle.ToInt32() == IntPtr.Zero.ToInt32())
        {
            return false;
        }
        return true;
    }

    public static bool UnregisterDeviceNotification(IntPtr notificationHandle)
    {
        return Win32.UnregisterDeviceNotification(notificationHandle);
    }

    public static bool DeviceNameMatch(Message m, String devicePathName)
    {
        Int32 stringSize;

        try
        {
            Win32.DEV_BROADCAST_DEVICEINTERFACE_1 devBroadcastDeviceInterface = new Win32.DEV_BROADCAST_DEVICEINTERFACE_1();
            Win32.DEV_BROADCAST_HDR devBroadcastHeader = new Win32.DEV_BROADCAST_HDR();

            // The LParam parameter of Message is a pointer to a DEV_BROADCAST_HDR structure.

            Marshal.PtrToStructure(m.LParam, devBroadcastHeader);

            if ((devBroadcastHeader.dbch_devicetype == Win32.DBT_DEVTYP_DEVICEINTERFACE))
            {
                // The dbch_devicetype parameter indicates that the event applies to a device interface.
                // So the structure in LParam is actually a DEV_BROADCAST_INTERFACE structure, 
                // which begins with a DEV_BROADCAST_HDR.

                // Obtain the number of characters in dbch_name by subtracting the 32 bytes
                // in the strucutre that are not part of dbch_name and dividing by 2 because there are 
                // 2 bytes per character.

                stringSize = System.Convert.ToInt32((devBroadcastHeader.dbch_size - 32) / 2);

                // The dbcc_name parameter of devBroadcastDeviceInterface contains the device name. 
                // Trim dbcc_name to match the size of the String.         

                devBroadcastDeviceInterface.dbcc_name = new Char[stringSize + 1];

                // Marshal data from the unmanaged block pointed to by m.LParam 
                // to the managed object devBroadcastDeviceInterface.

                Marshal.PtrToStructure(m.LParam, devBroadcastDeviceInterface);

                // Store the device name in a String.

                String DeviceNameString = new String(devBroadcastDeviceInterface.dbcc_name, 0, stringSize);

                // Compare the name of the newly attached device with the name of the device 
                // the application is accessing (mydevicePathName).
                // Set ignorecase True.

                if ((String.Compare(DeviceNameString, devicePathName, true) == 0))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        catch (Exception)
        {
            throw;
        }

        return false;
    }

    private static String ResultOfAPICall(String functionName)
    {
        Int32 bytes = 0;
        Int32 resultCode = 0;
        String resultString = "";

        resultString = new String(Convert.ToChar(0), 129);

        // Returns the result code for the last API call.

        resultCode = System.Runtime.InteropServices.Marshal.GetLastWin32Error();

        // Get the result message that corresponds to the code.

        Int64 temp = 0;
        bytes = Win32.FormatMessage(Win32.FORMAT_MESSAGE_FROM_SYSTEM, ref temp, resultCode, 0, resultString, 128, 0);

        // Subtract two characters from the message to strip the CR and LF.

        if (bytes > 2)
        {
            resultString = resultString.Remove(bytes - 2, 2);
        }
        // Create the String to return.

        resultString = "\r\n" + functionName + "\r\n" + "Result = " + resultString + "\r\n";

        return resultString;
    }

}