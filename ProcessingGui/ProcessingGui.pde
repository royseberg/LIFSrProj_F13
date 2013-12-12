import java.lang.Math;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.ArrayList;
import java.util.Queue;
import jssc.SerialPort;

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


//private DataThread dataThread; // Comment when using the TestDataThread
private TestDataThread dataThread; // Comment when using the real DataThread

private Serial serial;

private PFont f;
void setup() {
   size(1400,600);
   println(Serial.list());
//  ExecutorService executor = Executors.newFixedThreadPool(1); // Comment when using the TestDataThread
  ScheduledExecutorService executor = Executors.newScheduledThreadPool(1); // Comment when using the real DataThread
//  serial = new Serial(this, Serial.list()[2], 921600); // Comment when using the TestDataThread
  
  int xSize = 600;
  
  DataLine line1 = new DataLine(xSize, 860, 2);
  DataLine line2 = new DataLine(xSize, 860, 120);
  
//  dataThread = new DataThread(serial, "test.dat", line1, line2); // Comment when using the TestDataThread
  dataThread = new TestDataThread(line1, line2); // Comment when using the real DataThread
//  executor.submit(dataThread); // Comment when using the TestDataThread
  executor.scheduleAtFixedRate(dataThread, 100, 100, TimeUnit.MICROSECONDS); // Comment when using the real DataThread
  
  
  trace1 = new Line2DTrace(line1);
  trace1.setTraceColour(255,0,0);
  g1 = new Graph2D(this, xSize, 400, false);
  createGraph(g1, trace1, 70, 180);
  
  trace2 = new Line2DTrace(line2);
  trace2.setTraceColour(255,0,0);
  g2 = new Graph2D(this, xSize, 400, false);
  createGraph(g2, trace2, 740, 180);
   
  f = createFont("Arial",16,true);
}

void draw(){
  background(150);
  int indent = 25;
  trace1.generate();
  trace2.generate();
  g1.draw();
  g2.draw();
  
  textFont(f);
  fill(0);
  text("Click in this applet and type the hex value to be sent to the teensy.  Type 'send' to send.", indent, 15);
  text(typing,indent,50);
  text(message,indent,90);
//  text(dataThread.stringOut,indent,130); // Comment when using the TestDataThread
}

private String typing = "";
private String message = "";
private String sent = "";

void keyPressed() {
  // If the return key is pressed, save the String and clear it
  if (key == '\n' ) {
    if (typing.equals("send")) {
      message = "Sent: '" + sent + "'";
      serial.write(sent);
      typing = "";
      return;
    }
    sent = typing;
    message = "Ready to send '" + typing + "'";
    typing = ""; 
  } else if (key == '\b') {
    if (typing.length() <= 0) return;
    typing = typing.substring(0, typing.length()-1);
  } else {
    // Otherwise, concatenate the String
    // Each character typed by the user is added to the end of the String variable.
    typing = typing + key; 
  }
}


void stop() {
// dataThread.stopThread(); // Comment when using the TestDataThread
// serial.stop(); // Comment when using the TestDataThread
}

private void createGraph(Graph2D g, Line2DTrace trace, int xPos, int yPos) {
  // Creating the Graph2D object:
  // arguments are the parent object, xsize, ysize, cross axes at zero point 
  
  // Defining the main properties of the X and Y-Axis
  g.setYAxisMin(-.01);
  g.setYAxisMax(5.2);
  g.setXAxisMin(0);
  g.setXAxisMax(600);
  g.setXAxisLabel("Sample");
  g.setYAxisLabel("Value");
  g.setXAxisTickSpacing(60);
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

