namespace Motor_Tracker_Configuration
{
    partial class RegisterForm
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
            this.label1 = new System.Windows.Forms.Label();
            this.activationCodeTxtBox = new System.Windows.Forms.TextBox();
            this.productKeyTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.registerBtn = new System.Windows.Forms.Button();
            this.quitBtn = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(30, 41);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(82, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Activation Code";
            // 
            // activationCodeTxtBox
            // 
            this.activationCodeTxtBox.BackColor = System.Drawing.SystemColors.Window;
            this.activationCodeTxtBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.activationCodeTxtBox.Location = new System.Drawing.Point(132, 38);
            this.activationCodeTxtBox.Name = "activationCodeTxtBox";
            this.activationCodeTxtBox.ReadOnly = true;
            this.activationCodeTxtBox.Size = new System.Drawing.Size(172, 20);
            this.activationCodeTxtBox.TabIndex = 1;
            // 
            // productKeyTextBox
            // 
            this.productKeyTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.productKeyTextBox.Location = new System.Drawing.Point(132, 76);
            this.productKeyTextBox.Name = "productKeyTextBox";
            this.productKeyTextBox.Size = new System.Drawing.Size(172, 20);
            this.productKeyTextBox.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(30, 79);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Product Key";
            // 
            // registerBtn
            // 
            this.registerBtn.Location = new System.Drawing.Point(150, 116);
            this.registerBtn.Name = "registerBtn";
            this.registerBtn.Size = new System.Drawing.Size(115, 23);
            this.registerBtn.TabIndex = 4;
            this.registerBtn.Text = "Register";
            this.registerBtn.UseVisualStyleBackColor = true;
            this.registerBtn.Click += new System.EventHandler(this.registerBtn_Click);
            // 
            // quitBtn
            // 
            this.quitBtn.Location = new System.Drawing.Point(276, 116);
            this.quitBtn.Name = "quitBtn";
            this.quitBtn.Size = new System.Drawing.Size(115, 23);
            this.quitBtn.TabIndex = 5;
            this.quitBtn.Text = "Quit";
            this.quitBtn.UseVisualStyleBackColor = true;
            this.quitBtn.Click += new System.EventHandler(this.quitBtn_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.Red;
            this.label3.Location = new System.Drawing.Point(30, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(215, 17);
            this.label3.TabIndex = 6;
            this.label3.Text = "Please register your product";
            // 
            // RegisterForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(403, 158);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.quitBtn);
            this.Controls.Add(this.registerBtn);
            this.Controls.Add(this.productKeyTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.activationCodeTxtBox);
            this.Controls.Add(this.label1);
            this.Name = "RegisterForm";
            this.Text = "RegisterForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.RegisterForm_FormClosing);
            this.Load += new System.EventHandler(this.RegisterForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox activationCodeTxtBox;
        private System.Windows.Forms.TextBox productKeyTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button registerBtn;
        private System.Windows.Forms.Button quitBtn;
        private System.Windows.Forms.Label label3;
    }
}