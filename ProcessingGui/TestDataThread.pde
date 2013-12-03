import java.util.concurrent.*;

  class TestDataThread extends Thread {
  
  private boolean running;
  
  public ConcurrentLinkedQueue<Float> data;
  
  private int count;
  
  TestDataThread () {
      data = new ConcurrentLinkedQueue<Float>();
      running = false;
      count = 0;
  }
  
  public void start() {
     running = true;
     super.start();
  }
  
  public void run() {
    if (!running) return;
    for (int i=0; i < 100; ++i, ++count) {
      float next = (float)Math.cos((count/1000.0f));
      data.add(next);
    }
  }
  
  public void quit() {
     running = false;
  }
  
  
  
}
