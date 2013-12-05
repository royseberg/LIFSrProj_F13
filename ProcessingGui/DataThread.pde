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
  
  
  public void run() {
    int value = 0;
    int next = readSerial();
    if (next == start) {
      value = readSerial() << 8;
      value = value | readSerial();
      next = readSerial();
      if (next == end) {
        
        float f = value * 0.0001875;
        data.add(f);
      }
    }
  }
  
  private int readSerial() {
    int value = serial.read();
    while (value < 0){
      value = serial.read();
    }
    return (value & 0xff);
  }
    
}
