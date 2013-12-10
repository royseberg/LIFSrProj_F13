import java.lang.Math;
import java.util.ArrayList;
import java.util.Queue;
import java.util.concurrent.*;

import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.LabelPos;
import org.gwoptics.graphics.graph2D.traces.*;

import processing.serial.*;

import org.gicentre.utils.move.*;

private Graph2D g1;
private Graph2D g2;

private Queue<Float> data;
private Line2DTrace trace1;
private Line2DTrace trace2;


private DataLine line;

private DataThread dataThread;

private ScheduledFuture<?> _dataThread;
private Serial serial;


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
   size(1200,500);
   frameRate(100);
  ScheduledExecutorService executor =
        Executors.newScheduledThreadPool(1);
  serial = new Serial(this, Serial.list()[2], 921600);
  
  dataThread = new DataThread(serial, "test.dat");
  _dataThread = executor.scheduleAtFixedRate(dataThread, 10, 10, TimeUnit.MILLISECONDS);
  line = new DataLine(dataThread.data);
  
  while (dataThread.data.size() < 500){
     
  }
  trace1 = new RollingLine2DTrace(line, 1, .05);
  trace1.setTraceColour(255,0,0);
  g1 = new Graph2D(this, 600, 400, false);
  createGraph(g1, trace1, 70, 10);
   
}

void draw(){
  background(255);
  
  g1.draw();
}

void stop() {
  _dataThread.cancel(true);
 serial.stop(); 
}

private void createGraph(Graph2D g, Line2DTrace trace, int xPos, int yPos) {
  // Creating the Graph2D object:
  // arguments are the parent object, xsize, ysize, cross axes at zero point 
  
  // Defining the main properties of the X and Y-Axis
  g.setYAxisMin(0);
  g.setYAxisMax(5.2);
  g.setXAxisMin(0);
  g.setXAxisMax(100);
  g.setXAxisLabel("Sample");
  g.setYAxisLabel("Value");
  g.setXAxisTickSpacing(1000);
  g.setYAxisTickSpacing(0.25);
  
  // Offset of the top left corner of the plotting area
  // to the sketch origin (should not be zero in order to
  // see the y-axis label
  g.position.x = xPos;
  g.position.y = yPos;
 
  // Here we create a new trace and set a colour for
  // it, along with passing the equation object to it.
  // Adding the trace to the graph
  g.addTrace(trace);
}

