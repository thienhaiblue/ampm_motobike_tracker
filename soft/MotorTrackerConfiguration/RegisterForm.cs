using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using System.Security.Cryptography;
using System.IO;
using System.Management;

namespace Motor_Tracker_Configuration
{
    public partial class RegisterForm : Form
    {
        public const string PRODUCT_REGISTRY_KEY = "SOFTWARE\\AMBO Parameter Setup";
        public const string PRODUCT_KEY_NAME = "ProductKey";
        public const string INSTALLATION_DATE_KEY_NAME = "INSD";
        public const string LAST_WORKING_DATE_KEY_NAME = "LWKD";
        public const string ENCRYPTION_KEY = "tu@ns0ibk@";
        public const int TRIAL_PERIOD = 15;

        private string activationCode = "";
        internal bool isRegistered = false;

        public RegisterForm()
        {
            InitializeComponent();
        }

        private string GetActivationCode()
        {
            string cpuInfo = string.Empty;
            StringBuilder code = new StringBuilder();
            int tCount;

            ManagementClass mc = new ManagementClass("win32_processor");
            ManagementObjectCollection moc = mc.GetInstances();

            foreach (ManagementObject mo in moc)
            {
                if (cpuInfo == "")
                {
                    //Get only the first CPU's ID
                    cpuInfo = mo.Properties["ProcessorID"].Value.ToString();
                    break;
                }
            }

            cpuInfo = AESEncryption.Encrypt(cpuInfo, ENCRYPTION_KEY);
            tCount = 0;
            for (int i = 0; i < cpuInfo.Length; i++)
            {
                if (Char.IsLetterOrDigit(cpuInfo[i]))
                {
                    code.Append(cpuInfo[i].ToString().ToUpper());
                    tCount++;
                    if (tCount >= 4)
                    {
                        code.Append("-");
                        tCount = 0;
                    }
                    if (code.Length >= 20) // 4 block of 4 character
                    {
                        break;
                    }
                }
            }

            return code.ToString().Substring(0, 19);
        }

        public bool CheckProductKey()
        {
            RegistryKey savedKey;
            string productKey;
            string encryptedKey;
            DateTime insDate;
            DateTime lwkDate;

            try
            {
                savedKey = Registry.LocalMachine.CreateSubKey(PRODUCT_REGISTRY_KEY);
                if (savedKey != null)
                {
                    if (savedKey.GetValue(PRODUCT_KEY_NAME) != null)
                    {
                        productKey = savedKey.GetValue(PRODUCT_KEY_NAME).ToString();
                        savedKey.Close();
                        if (productKey != "")
                        {
                            if (activationCode == "")
                            {
                                activationCode = GetActivationCode();
                            }
                            encryptedKey = GetProductKey(activationCode);
                            if (encryptedKey.Contains(productKey))
                            {
                                if (encryptedKey.Length == productKey.Length)
                                {
                                    return true;
                                }
                            }
                        }
                    }
                    // product key is not exist or not valid
                    // check for trial period
                    if ((savedKey.GetValue(INSTALLATION_DATE_KEY_NAME) == null) || (savedKey.GetValue(LAST_WORKING_DATE_KEY_NAME) == null))
                    {
                        // create 2 new sub key for installation date & last working day
                        if (savedKey.GetValue(INSTALLATION_DATE_KEY_NAME) == null)
                        {
                            savedKey.SetValue(INSTALLATION_DATE_KEY_NAME, AESEncryption.Encrypt(DateTime.Now.ToString(), ENCRYPTION_KEY));
                        }
                        savedKey.SetValue(LAST_WORKING_DATE_KEY_NAME, AESEncryption.Encrypt(DateTime.Now.ToString(), ENCRYPTION_KEY));
                    }
                    else
                    {
                        insDate = DateTime.Parse(AESEncryption.Decrypt(savedKey.GetValue(INSTALLATION_DATE_KEY_NAME).ToString(), ENCRYPTION_KEY));
                        lwkDate = DateTime.Parse(AESEncryption.Decrypt(savedKey.GetValue(LAST_WORKING_DATE_KEY_NAME).ToString(), ENCRYPTION_KEY));                        
                        if ((DateTime.Now.CompareTo(lwkDate) < 0) ||(lwkDate.Subtract(insDate).TotalDays > TRIAL_PERIOD))
                        { 
                            // trial period is over --> show the registration form
                            this.ShowDialog();

                            return isRegistered;
                        }
                        savedKey.SetValue(LAST_WORKING_DATE_KEY_NAME, AESEncryption.Encrypt(DateTime.Now.ToString(), ENCRYPTION_KEY));
                    }
                }
                else
                { 
                    // cannot create product key in registry
                    MessageBox.Show("Cannot create product key entry. The program will now abort!", "AMBO");
                    Environment.Exit(0);
                }
            }
            catch (Exception ex)
            {
                // problem occurred when checking product license
                Console.WriteLine(ex);                
                MessageBox.Show("Problem occurred while checking for product license. The program will now abort!", "AMBO");
                Environment.Exit(0);
            }
            
            return false;
        }

        public void SaveProductKey(string serialKey)
        {
            RegistryKey productKey;

            try
            {
                productKey = Registry.LocalMachine.CreateSubKey(PRODUCT_REGISTRY_KEY);
                if (productKey != null)
                {
                    productKey.SetValue(PRODUCT_KEY_NAME, serialKey);
                    productKey.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        private string GetProductKey(string code)
        {
            string temp;
            StringBuilder key;
            int tCount;

            key = new StringBuilder();
            code = code.Replace("-", "");
            temp = AESEncryption.Encrypt(code, ENCRYPTION_KEY);
            tCount = 0;
            for (int i = 0; i < temp.Length; i++)
            {
                if (Char.IsLetterOrDigit(temp[i]))
                {
                    key.Append(temp[i].ToString().ToUpper());
                    tCount++;
                    if (tCount >= 4)
                    {
                        key.Append("-");
                        tCount = 0;
                    }
                    if (key.Length >= 20) // 4 block of 4 character
                    {
                        break;
                    }
                }
            }

            return key.ToString().Substring(0, 19);
        }

        private void RegisterForm_Load(object sender, EventArgs e)
        {
            activationCode = GetActivationCode();
            if (activationCode == "")
            {
                MessageBox.Show("Cannot retrieve activation code. Program will now exit!", "AMBO");
                Environment.Exit(0);
            }
            activationCodeTxtBox.Text = activationCode;
        }

        private void quitBtn_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void RegisterForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!isRegistered)
            {
                Environment.Exit(0);
            }
        }

        private void registerBtn_Click(object sender, EventArgs e)
        {
            string productKey;
            string encryptedKey;

            productKey = productKeyTextBox.Text;
            if (productKey == "")
            {
                MessageBox.Show("Please enter product key!", "AMBO");
                return;
            }
            encryptedKey = GetProductKey(activationCode);
            if (encryptedKey.Contains(productKey))
            {
                if (encryptedKey.Length == productKey.Length)
                {
                    SaveProductKey(productKey);
                    isRegistered = true;
                    MessageBox.Show("Registration completed successfully. Thank you for choosing our product!", "AMBO");
                    this.Close();
                    return;
                }
            }

            MessageBox.Show("Invalid product key!", "AMBO");
        }
    }

    static class AESEncryption
    {
        #region Static Functions

        /// <summary>
        /// Encrypts a string
        /// </summary>
        /// <param name="PlainText">Text to be encrypted</param>
        /// <param name="Password">Password to encrypt with</param>
        /// <param name="Salt">Salt to encrypt with</param>
        /// <param name="HashAlgorithm">Can be either SHA1 or MD5</param>
        /// <param name="PasswordIterations">Number of iterations to do</param>
        /// <param name="InitialVector">Needs to be 16 ASCII characters long</param>
        /// <param name="KeySize">Can be 128, 192, or 256</param>
        /// <returns>An encrypted string</returns>
        public static string Encrypt(string PlainText, string Password,
            string Salt = "tuansoibk", string HashAlgorithm = "SHA1",
            int PasswordIterations = 2, string InitialVector = "OFRna73m*aze01xY",
            int KeySize = 256)
        {
            if (string.IsNullOrEmpty(PlainText))
                return "";
            byte[] InitialVectorBytes = Encoding.ASCII.GetBytes(InitialVector);
            byte[] SaltValueBytes = Encoding.ASCII.GetBytes(Salt);
            byte[] PlainTextBytes = Encoding.UTF8.GetBytes(PlainText);
            PasswordDeriveBytes DerivedPassword = new PasswordDeriveBytes(Password, SaltValueBytes, HashAlgorithm, PasswordIterations);
            byte[] KeyBytes = DerivedPassword.GetBytes(KeySize / 8);
            RijndaelManaged SymmetricKey = new RijndaelManaged();
            SymmetricKey.Mode = CipherMode.CBC;
            byte[] CipherTextBytes = null;
            using (ICryptoTransform Encryptor = SymmetricKey.CreateEncryptor(KeyBytes, InitialVectorBytes))
            {
                using (MemoryStream MemStream = new MemoryStream())
                {
                    using (CryptoStream CryptoStream = new CryptoStream(MemStream, Encryptor, CryptoStreamMode.Write))
                    {
                        CryptoStream.Write(PlainTextBytes, 0, PlainTextBytes.Length);
                        CryptoStream.FlushFinalBlock();
                        CipherTextBytes = MemStream.ToArray();
                        MemStream.Close();
                        CryptoStream.Close();
                    }
                }
            }
            SymmetricKey.Clear();
            return Convert.ToBase64String(CipherTextBytes);
        }

        /// <summary>
        /// Decrypts a string
        /// </summary>
        /// <param name="CipherText">Text to be decrypted</param>
        /// <param name="Password">Password to decrypt with</param>
        /// <param name="Salt">Salt to decrypt with</param>
        /// <param name="HashAlgorithm">Can be either SHA1 or MD5</param>
        /// <param name="PasswordIterations">Number of iterations to do</param>
        /// <param name="InitialVector">Needs to be 16 ASCII characters long</param>
        /// <param name="KeySize">Can be 128, 192, or 256</param>
        /// <returns>A decrypted string</returns>
        public static string Decrypt(string CipherText, string Password,
            string Salt = "tuansoibk", string HashAlgorithm = "SHA1",
            int PasswordIterations = 2, string InitialVector = "OFRna73m*aze01xY",
            int KeySize = 256)
        {
            if (string.IsNullOrEmpty(CipherText))
                return "";
            byte[] InitialVectorBytes = Encoding.ASCII.GetBytes(InitialVector);
            byte[] SaltValueBytes = Encoding.ASCII.GetBytes(Salt);
            byte[] CipherTextBytes = Convert.FromBase64String(CipherText);
            PasswordDeriveBytes DerivedPassword = new PasswordDeriveBytes(Password, SaltValueBytes, HashAlgorithm, PasswordIterations);
            byte[] KeyBytes = DerivedPassword.GetBytes(KeySize / 8);
            RijndaelManaged SymmetricKey = new RijndaelManaged();
            SymmetricKey.Mode = CipherMode.CBC;
            byte[] PlainTextBytes = new byte[CipherTextBytes.Length];
            int ByteCount = 0;
            using (ICryptoTransform Decryptor = SymmetricKey.CreateDecryptor(KeyBytes, InitialVectorBytes))
            {
                using (MemoryStream MemStream = new MemoryStream(CipherTextBytes))
                {
                    using (CryptoStream CryptoStream = new CryptoStream(MemStream, Decryptor, CryptoStreamMode.Read))
                    {

                        ByteCount = CryptoStream.Read(PlainTextBytes, 0, PlainTextBytes.Length);
                        MemStream.Close();
                        CryptoStream.Close();
                    }
                }
            }
            SymmetricKey.Clear();
            return Encoding.UTF8.GetString(PlainTextBytes, 0, ByteCount);
        }

        #endregion
    }
}
