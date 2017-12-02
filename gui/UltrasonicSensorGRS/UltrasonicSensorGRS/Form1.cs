using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UltrasonicSensorGRS
{
    public partial class Form1 : Form
    {

        private FixedSizedQueue<SerialInputRecord> dataQueue = new FixedSizedQueue<SerialInputRecord>(10);
        private FixedSizedQueue<SerialInputRecord> chartValues = new FixedSizedQueue<SerialInputRecord>(50);

        private struct SerialInputRecord {
            public int x1, x2, x3, y1;
        }; 

        public Form1()
        {
            InitializeComponent();
            serialPort1.Open();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            string line = serialPort1.ReadExisting();
            if (line.Length != 0)
            {
                SerialInputRecord record = parseSerialInputValues(line);
                dataQueue.Enqueue(record);
                chartValues.Enqueue(record);
                ProceedData(dataQueue);
                RedrawChart(chartValues);
            }
        }

        private SerialInputRecord parseSerialInputValues(String str)
        {
            SerialInputRecord obj;
            obj.x1 = 0;
            obj.x2 = 0;
            obj.x3 = 0;
            obj.y1 = 0;
            String[] strVals = str.Split(' ');
            int valsCount = strVals.Length;
            int xI;

            if (valsCount < 1)
                return obj;
            
            if (Int32.TryParse(strVals[0], out xI))
            {
                obj.x1 = xI;
            }

            if (valsCount < 2)
                return obj;

            if (Int32.TryParse(strVals[1], out xI))
            {
                obj.x2 = xI;
            }

            if (valsCount < 3)
                return obj;

            if (Int32.TryParse(strVals[2], out xI))
            {
                obj.x3 = xI;
            }

            // Continue...

            return obj;
        }

        private int ProceedData(FixedSizedQueue<SerialInputRecord> queue)
        {
            Console.WriteLine("<-----------------START----------------->");
            PassQueue(queue, ConsoleWriteRecordData);
            Console.WriteLine("<-----------------DONE----------------->");
           
            return 0;
        }

        private void ConsoleWriteRecordData(SerialInputRecord record, int i)
        {
            Console.WriteLine("Item: {X1:" + record.x1.ToString() + ", X2: " + record.x2.ToString() + ", X3: " + record.x3.ToString() + "}");
        }

        private void PassQueue(FixedSizedQueue<SerialInputRecord> queue, Action<SerialInputRecord,int> callback)
        {
            int l = queue.Size;
            for (int i = 0; i < l; i++)
            {
                SerialInputRecord a;
                queue.TryDequeue(out a);
                queue.Enqueue(a);
                callback(a, i);
            }
        }

        private void RedrawChart(FixedSizedQueue<SerialInputRecord> chartQueue)
        {
            chart1.Series["X1"].Points.Clear();
            chart1.Series["X2"].Points.Clear();
            chart1.Series["X3"].Points.Clear();
            PassQueue(chartQueue, AddChartPoints);
        }

        private void AddChartPoints(SerialInputRecord record, int i)
        {
            chart1.Series["X1"].Points.AddXY(i, record.x1);
            chart1.Series["X2"].Points.AddXY(i, record.x2);
            chart1.Series["X3"].Points.AddXY(i, record.x3);
        }
    }
}
