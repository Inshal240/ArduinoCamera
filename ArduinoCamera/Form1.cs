//#define HOMETEST

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Threading;

namespace ArduinoCamera
{
    public partial class Form1 : Form
    {
        public SerialPort selectedPort;
        // The following values change depending on
        // which button has been pressed
        private PictureBox targetPicBox;
        // This BGWorker coordinates the camera calls
        private BackgroundWorker handler;
        // This BGWorker handles the task of actually getting the data
        private BackgroundWorker bw;
        // Set to true when cameras are required to take pictures repeatedly
        private bool recording = false;
        private byte[] command;

        public Form1()
        {
            InitializeComponent();
            takePicBtn1.Text = "Take Pictures";
            takePicBtn2.Text = "Take Multiple Pictures";
            #region Handler initialization
            handler = new BackgroundWorker();

            handler.DoWork += new DoWorkEventHandler(
            delegate(object o, DoWorkEventArgs args)
            {
                statusLabel.Text = "Status = Busy";
                command = new byte[1] { 0x31 };
                targetPicBox = pictureBox1;
                takePicBtn1.Enabled = false;
                button1.Enabled = false;

                runBackgroundWorker();

                while (bw.IsBusy) ;

                statusLabel.Text = "Status = Busy";
                command = new byte[1] { 0x32 };
                targetPicBox = pictureBox2;

                runBackgroundWorker();
            });

            handler.RunWorkerCompleted += new RunWorkerCompletedEventHandler(
            delegate(object o, RunWorkerCompletedEventArgs args)
            {
                statusLabel.Text = "Status = Available";
                button1.Enabled = true;
                takePicBtn1.Enabled = true;
            });
            #endregion
        }

        private void Form1_Load(object sender, EventArgs e)
        {

#if HOMETEST
            try
            {
                selectedPort = new SerialPort("COM4", 38400, Parity.None, 8, StopBits.One);
                // Wait for a max of 20 seconds to read, then break the operation
                selectedPort.ReadTimeout = 20000;

                portLabel.Text = "Home Test";
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString());
            }
#endif
        }

        private bool writeToPort(byte[] data)
        {
            try
            {
                selectedPort.Open();
                selectedPort.Write(data, 0, data.Length);
                selectedPort.Close();
                return true;
            }
            catch (NullReferenceException)
            {
                MessageBox.Show("No port selected. Please Select a port from drop down list.", "No Port Selected");
                return false;
            }
            catch (UnauthorizedAccessException)
            {
                MessageBox.Show("The port is in use by another application.", "Unauthorized Access");
                return false;
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Unhandled Error");
                return false;
            }
        }
        
        private Image covertToImage(byte[] imageData)
        {
            MemoryStream memStream = new MemoryStream(imageData);
            return Bitmap.FromStream(memStream);
        }

        private void takePicBtn1_Click(object sender, EventArgs e)
        {
            handler = new BackgroundWorker();

            handler.DoWork += new DoWorkEventHandler(
            delegate(object o, DoWorkEventArgs args)
            {
                statusLabel.Text = "Status = Busy";
                command = new byte[1] { 0x31 };
                targetPicBox = pictureBox1;
                takePicBtn1.Enabled = false;
                button1.Enabled = false;

                runBackgroundWorker();

                while (bw.IsBusy) ;

                statusLabel.Text = "Status = Busy";
                command = new byte[1] { 0x32 };
                targetPicBox = pictureBox2;

                runBackgroundWorker();
            });

            handler.RunWorkerCompleted += new RunWorkerCompletedEventHandler(
            delegate(object o, RunWorkerCompletedEventArgs args)
            {
                statusLabel.Text = "Status = Available";
                button1.Enabled = true;
                takePicBtn1.Enabled = true;
            });

            handler.RunWorkerAsync();
        }

        private void takePicBtn2_Click(object sender, EventArgs e)
        {
            if (recording && handler.IsBusy)
            {
                recording = false;
                handler.CancelAsync();
                takePicBtn2.Text = "Take Multiple Pictures";
            }
            else if (!handler.IsBusy)
            {
                recording = true;

                handler = new BackgroundWorker();

                handler.DoWork += new DoWorkEventHandler(
                delegate(object o, DoWorkEventArgs args)
                {
                    while (true)
                    {
                        statusLabel.Text = "Status = Busy";
                        command = new byte[1] { 0x31 };
                        targetPicBox = pictureBox1;
                        takePicBtn1.Enabled = false;
                        button1.Enabled = false;

                        runBackgroundWorker();

                        while (bw.IsBusy) ;

                        statusLabel.Text = "Status = Busy";
                        command = new byte[1] { 0x32 };
                        targetPicBox = pictureBox2;

                        runBackgroundWorker();
                    }
                });

                handler.RunWorkerCompleted += new RunWorkerCompletedEventHandler(
                delegate(object o, RunWorkerCompletedEventArgs args)
                {
                    statusLabel.Text = "Status = Available";
                    button1.Enabled = true;
                    takePicBtn1.Enabled = true;
                });

                handler.RunWorkerAsync();
                takePicBtn2.Text = "Stop";
            }
        }

        /// <summary>
        /// Reference can be found here
        /// http://stackoverflow.com/questions/363377/how-do-i-run-a-simple-bit-of-code-in-a-new-thread
        /// </summary>
        private void runBackgroundWorker()
        {
            bw = new BackgroundWorker();

            bw.WorkerReportsProgress = true;

            bw.DoWork += new DoWorkEventHandler(
            delegate(object o, DoWorkEventArgs args)
            {
                BackgroundWorker b = o as BackgroundWorker;

                if (!writeToPort(command))
                {
                    return;
                }

                b.ReportProgress(10);

                byte[] data;

                try
                {
                    selectedPort.Open();

                    while (selectedPort.BytesToRead == 0) ;
                    b.ReportProgress(30);

                    int nextBytes = selectedPort.BytesToRead;
                    int prevBytes = 0;

                    do
                    {
                        prevBytes = nextBytes;
                        delay(100);
                        nextBytes = selectedPort.BytesToRead;
                    }
                    while (nextBytes != prevBytes);

                    b.ReportProgress(60);

                    data = new byte[nextBytes];

                    selectedPort.Read(data, 0, nextBytes);
                    b.ReportProgress(80);

                    selectedPort.Close();

                    Image img = covertToImage(data);
                    b.ReportProgress(90);

                    targetPicBox.Image = img;
                    b.ReportProgress(100);
                }
                catch (Exception exc)
                {
                    MessageBox.Show(exc.ToString());
                    selectedPort.Close();
                }
            });

            bw.ProgressChanged += new ProgressChangedEventHandler(
            delegate(object o, ProgressChangedEventArgs args)
            {
                progressBar1.Value = args.ProgressPercentage;
            });

            bw.RunWorkerAsync();
        }

        private static void delay(int milliseconds)
        {
            System.Threading.Thread.Sleep(milliseconds);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var portForm = new Form2(this);
            portForm.Show();
        }
    }
}
