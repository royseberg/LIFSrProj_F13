import java.util.LinkedList;

public class DataLine implements ILine2DEquation{
  private LinkedList<Float> _points;
  private int _size;
  private int _samplesPerPoint;
  private int _currentSample;
  private double _average;
  
  public DataLine(int xSize, int sampleRate, int displayTimeSeconds) {
    _size = xSize;
    _points = new LinkedList<Float>();
    _samplesPerPoint = displayTimeSeconds * sampleRate / xSize;
    _currentSample = 0;
    _average = 0;
  }
  
  public double computePoint(double xValue, int position) {
    int pos = (int)xValue;
    if (position >= _points.size()) return 0.0;
    float value = _points.get(position);
     return value; 
  }
  
  public void addPoint(float point) {
     _average += point;
     
     if (_currentSample >= _samplesPerPoint) {
        float value = (float) (_average / (_currentSample * 1.0));
        _currentSample = 0;
        _points.add(value);
        if (_points.size() > _size) {
          value = _points.poll();
        }
        _average = 0.0;
     }
     ++_currentSample;
  }
}
