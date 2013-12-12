import java.util.concurrent.*;

  class TestDataThread implements Runnable {
  
  private boolean running;
  
  private int count;
  
  private DataLine[] _data;
  
  TestDataThread (DataLine... data) {
      _data = data;
      running = true;
      count = 0;
  }
  
  
  public void run() {
    if (!running) return;
      float next = (float)Math.cos((count++/1000.0f)) + 1;
      for (DataLine line : _data) {
        line.addPoint(next); 
      }
  }
  
  public void quit() {
     running = false;
  }
  
  
  
}
