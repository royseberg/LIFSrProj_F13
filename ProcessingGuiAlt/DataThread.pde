import processing.serial.*;
import java.util.concurrent.ConcurrentLinkedQueue;

class DataThread extends Thread {
  
  private int start = 170;
  private int end = 85;
  
  private Serial serial;
  
  private PrintWriter output;
  
  public ConcurrentLinkedQueue<Float> data;
  
  DataThread (Serial serial, String fileName) {
      this.serial = serial;
      output = createWriter(fileName);
      data = new ConcurrentLinkedQueue<Float>();
      serial.clear();
      serial.write(0x00);
  }
  
  int sampleCount;
  
  
  public void run() {
    while (serial.available() > 0) {
      int value = 0;
      int next = readSerial();
      if (next == start) {
        value = readSerial();
        value = (readSerial() << 8) | value;
        next = readSerial();
        if (next == end) {
          
          float f = value * .0001875;
          output.println(value + "," + f);
          if (sampleCount++ > 20) {
            data.add(f);
            sampleCount = 0;
          }
        }
      }
    }
            output.flush();
  }
  
  private int readSerial() {
    int value = serial.read();
    while (value < 0){
      value = serial.read();
    }
    return (value & 0xff);
  }
    
}
