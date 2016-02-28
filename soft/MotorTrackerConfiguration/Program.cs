using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Motor_Tracker_Configuration
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            //RegisterForm rForm = new RegisterForm();
            //if (rForm.CheckProductKey())
            //{
            //    Application.Run(new MainForm());
            //}
            //else
            //{
            //    Application.Run(new MainForm(true));
            //}
            Application.Run(new MainForm());
            Environment.Exit(0);
        }
    }
}
