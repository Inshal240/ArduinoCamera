using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Windows.Forms;

namespace ArduinoCamera
{
    public partial class Form2 : Form
    {
        private Form1 form;

        public Form2(Form1 form)
        {
            InitializeComponent();
            this.form = form;
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            updatePortSelection();         
        }

        private void updatePortSelection()
        {
            portSelection.Items.Clear();
            portSelection.Items.AddRange(SerialPort.GetPortNames());
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                form.selectedPort = new SerialPort(portSelection.SelectedItem.ToString(), 38400, Parity.None, 8, StopBits.One);
                
                // Wait for a max of 20 seconds to read, then break the operation
                form.selectedPort.ReadTimeout = 20000;
                
                // For testing the port's functionality
                form.selectedPort.Open();
                form.selectedPort.Close();

                // Display the selected port
                form.Controls["portLabel"].Text = "Port: " + portSelection.SelectedItem.ToString();
            }
            catch(NullReferenceException)
            {
                MessageBox.Show("No port selected. Please Select a port from drop down list.", "No Port Selected");
                form.Controls["portLabel"].Text = "Port: None";
                return;
            }
            catch(UnauthorizedAccessException)
            {
                MessageBox.Show("The port is in use by another application.", "Unauthorized Access");
                form.Controls["portLabel"].Text = "Port: None";
                return;
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Unhandled Error");
                form.Controls["portLabel"].Text = "Port: None";
                return;
            }

            this.Close();
        }
    }
}
