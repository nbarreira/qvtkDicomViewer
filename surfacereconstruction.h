#ifndef SURFACERECONSTRUCTION_H
#define SURFACERECONSTRUCTION_H

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkAlgorithmOutput.h>
#include <vtkMarchingCubes.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <string>
#include <vtkGDCMImageReader.h>
#include <vtkImageChangeInformation.h>


class SurfaceReconstruction
{
public:
  SurfaceReconstruction();

  void readDICOMDir(std::string &dir);

  void update();
  void show(vtkSmartPointer<vtkRenderWindow> renderWindow);
  vtkSmartPointer<vtkAlgorithmOutput>  getOutputPort();




  // Getters and setters
  double getThreshold() const;
  void setThreshold(double value);
  double getGaussianSize() const;
  void setGaussianSize(double value);
  double getGaussianStd() const;
  void setGaussianStd(double value);
  double getMorphologicalSize() const;
  void setMorphologicalSize(double value);


private:
  double thresholdValue;
  double gaussianSize;
  double gaussianStd;
  double morphologicalSize;
  vtkSmartPointer<vtkSphereSource> sphereSource;
  vtkSmartPointer<vtkImageData> volume ;
  vtkSmartPointer<vtkMarchingCubes> surface;
  vtkSmartPointer<vtkGDCMImageReader> gdcmReader;
  vtkSmartPointer<vtkImageChangeInformation> input;
};

#endif // SURFACERECONSTRUCTION_H

