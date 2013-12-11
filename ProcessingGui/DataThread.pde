import java.lang.Runnable;


class DataThread implements Runnable {
  private int start = 0xAA;
  private int stringStart = 's';
  private int end = 0x55;
  private int stringEnd = 0xff;
  
  private Serial serial;
  
  private PrintWriter output;
  
  public String stringOut = "";
  
  
  public DataLine[] _equations;
  
  DataThread (Serial serial, String fileName, DataLine... data){
    if (data == null || data.length == 0) throw new IllegalArgumentException("Must have a dataline to add data to");
      this.serial = serial;
      output = createWriter(fileName);
      serial.clear();
      _running = true;
      _equations = data;
  }
  
  int sampleCount;
  private boolean _running;
  
  
  public void run() {
    try {
      while (_running) {
        int value = 0;
        int next = readSerial();
        if (next == start) {
          value = readSerial();
          value = (readSerial() << 8) | value;
          next = readSerial();
          if (next == end) {
            
            float f = value * .0001875;
            output.println(value + "," + f);
            for (DataLine eq : _equations) {
              eq.addPoint(f);
            }
            if (sampleCount++ >= 100) {
              output.flush();
              sampleCount = 0;
            }
          }
        } else if (next == stringStart) {
          String readString = serial.readStringUntil((char)0xff);
          stringOut = readString == null ? "" : readString;
        }
          
          
      }
    } catch (Exception e) {
       println(e);
    }
    output.flush();
    output.close();
  }
  
  public void stopThread() {
    _running = false;
  }
  
  private byte[] buffer = new byte[400];
  private int available = 0;
  private int next = 0;
  
  private int readSerial() {
    while (available <= 0) {
       available = serial.readBytes(buffer);
      next = 0; 
    }
    --available;
    return (((int)buffer[next++]) & 0xff);
  }
    
}
