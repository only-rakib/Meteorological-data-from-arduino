using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;

namespace MET
{
    public partial class Form1 : Form
    {
        string[] sPorts;
        SerialPort Port;
        bool isConnected = false;
        public static string COMPORT = "";
        private BackgroundWorker hardWorker;
        private Thread readThread = null;
        delegate void SetTextCallback(string text);
        int n = 0;
        int i = 0;

        public Form1()
        {
            InitializeComponent();
            getPortName();
            hardWorker = new BackgroundWorker();
        }

        private void Label1_Click(object sender, EventArgs e)
        {

        }
        private void getPortName()
        {
            sPorts = SerialPort.GetPortNames();
            foreach (string port in sPorts)
            {
                comboBox1.Items.Add(port);
                if (sPorts[0] != null)
                {
                    comboBox1.SelectedItem = sPorts[0];

                }
            }
        }
        private void connectToArduino()
        {

            string selectedPort = comboBox1.GetItemText(comboBox1.SelectedItem);
            try
            {
                isConnected = true;
                Port = new SerialPort(selectedPort, 57600, Parity.None, 8, StopBits.One);
                //Port.Open();
                button1.Text = "Disconnect";
                button2.Enabled = true;
            }
            catch (Exception e)
            {
                isConnected = false;
                MessageBox.Show("COM Port is not selected");
                button2.Enabled = false;
            }

        }
        private void disconnectFromArduino()
        {

            isConnected = false;
            //port.Write("#STOP\n");
            Port.Close();
            button1.Text = "Connect";
            button2.Enabled = false;

        }

        private void Button1_Click(object sender, EventArgs e)
        {
            if (!isConnected)
            {
                connectToArduino();
            }
            else
            {
                disconnectFromArduino();
            }

        }

        private void Button2_Click(object sender, EventArgs e)
        {
            // int n = dataGridView1.Rows.Add();
            Port.Open();
            readThread = new Thread(new ThreadStart(this.Read));
            readThread.Start();
            this.hardWorker.RunWorkerAsync();
            //Read();

        }

        public static string getBetween(string strSource, string strStart, string strEnd)
        {
            int Start, End;
            if (strSource.Contains(strStart) && strSource.Contains(strEnd))
            {
                Start = strSource.IndexOf(strStart, 0) + strStart.Length;
                End = strSource.IndexOf(strEnd, Start);
                return strSource.Substring(Start, End - Start);
            }
            else
            {
                return "";
            }
        }

        private void SetText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.

            if (this.dataGridView1.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                //i ++;
                //string dataText = text.Substring(0,4);
                n = dataGridView1.Rows.Add();

                string temp = getBetween(text, "temp", "humid");
                dataGridView1.Rows[n].Cells[2].Value = temp;

                string humid = getBetween(text, "humid", "pressure");
                dataGridView1.Rows[n].Cells[3].Value = humid;

                string pressure = getBetween(text, "pressure", "altitude");
                dataGridView1.Rows[n].Cells[1].Value = pressure;

                string altitude = getBetween(text, "altitude", "end");
                dataGridView1.Rows[n].Cells[0].Value = altitude;
            }
        }
        private void Read()
        {

                while (Port.IsOpen)
                {
                    try
                    {
                        string message = Port.ReadLine();
                        this.SetText(message);
                        Console.Out.WriteLine(message);

                    }
                    catch (Exception e)
                    { }
                }
        }
    }
}
