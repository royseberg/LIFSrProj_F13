import java.lang.Math;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.ArrayList;
import java.util.Queue;

import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.LabelPos;
import org.gwoptics.graphics.graph2D.traces.*;

import org.gicentre.utils.move.*;

private Graph2D g;

private Queue<Float> data;
private Line2DTrace trace;


private DataLine line;
private TestDataThread dataThread;


public class DataLine implements ILine2DEquation{
  private ArrayList<Float> points;
  private Queue<Float> newData;
  
  public DataLine(Queue<Float> data) {
    points = new ArrayList<Float>();
    newData = data;
  }
  
  public double computePoint(double x, int toss) {
    int pos = (int)x;
    while (!newData.isEmpty()){
      points.add(newData.poll()); 
    }
    if (pos > points.size()) return 0.0;
    
     return points.get(pos); 
  }
  
  public int size(){
    return points.size();
  }
}

void setup() {
   size(700,500);
   println(Serial.list());
  dataThread = new TestDataThread();
  dataThread.start();
  dataThread.run();
  ScheduledExecutorService executor =
        Executors.newScheduledThreadPool(1);
  
  executor.scheduleAtFixedRate(dataThread, 100, 100, TimeUnit.MILLISECONDS);
  line = new DataLine(dataThread.data);
  trace = new Line2DTrace(line);
  
  trace.setTraceColour(255,0,0);
  createGraph(trace);
  
   
}

void draw(){
  background(255);
  int max = (int)g.getXAxis().getMaxValue();
  if (max < line.size()) {
    int size = line.size();
     g.setXAxisMax(size-1);
     g.setXAxisTickSpacing(size / 10); 
     trace.generate();
  }
  g.draw();
}

private void createGraph(Line2DTrace trace) {
  // Creating the Graph2D object:
  // arguments are the parent object, xsize, ysize, cross axes at zero point
  g = new Graph2D(this, 600, 400, false); 
  
  // Defining the main properties of the X and Y-Axis
  g.setYAxisMin(-1);
  g.setYAxisMax(1);
  g.setXAxisMin(0);
  g.setXAxisMax(10);
  g.setXAxisLabel("Sample");
  g.setYAxisLabel("Value");
  g.setXAxisTickSpacing(60);
  g.setYAxisTickSpacing(0.25);
  
  // Offset of the top left corner of the plotting area
  // to the sketch origin (should not be zero in order to
  // see the y-axis label
  g.position.x = 70;
  g.position.y = 20;
 
  // Here we create a new trace and set a colour for
  // it, along with passing the equation object to it.
  // Adding the trace to the graph
  g.addTrace(trace);
}
