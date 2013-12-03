import processing.serial.*;
import java.util.concurrent.ConcurrentLinkedQueue;

class DataThread extends Thread {
  
  private char[] cBuf = new char[512];
  
  private Serial serial;
  
  private PrintWriter output;
  
  private boolean running;
  
  public ConcurrentLinkedQueue<Float> data;
  
  DataThread (Serial serial, String fileName) {
      this.serial = serial;
      output = createWriter(fileName);
      data = new ConcurrentLinkedQueue<Float>();
      running = false;
  }
  
  public void start() {
     running = true;
     super.start();
  }
  
  public void run() {
    int size = 0;
     while (running) {
       char next = serial.readChar();
       if (next == '\n' || size >= cBuf.length) {
          createString(size);
          size = 0;
       } else if (next != -1 && next != '\r') {
           cBuf[size++] = next;
       }
     } 
  }
  
  public void quit() {
     running = false;
  }
  
  private void createString(int size) {
     String string = new String(cBuf, 0, size);
     output.println(string);
     Long i = Long.parseLong(string, 16);
     Float f = Float.intBitsToFloat(i.intValue());
     data.add(f);
  }
  
  
}
