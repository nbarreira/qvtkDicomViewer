/**
 * @file surfacereconstruction.h
 *
 * @brief Simple surface reconstruction algorithm for a series of DICOM images
 * using vtk + gdcm libraries
 *
 * This class is intended to read a directory that contains a series of 2D DICOM
 * images that form a volume and to perform a simple surface reconstruction
 * algorithm. The input image is smoothed and thresholded. After that, morphological
 * operators are applied in order to remove small structures and, finally, surfaces
 * are computed using the marching cubes algorithm.
 * Moreover, this class encapsulates the code needed to display the computed surface
 * in a vtkRenderWindow widget.
 *
 * @author Noelia Barreira (nbarreira@udc.es)
 */

#ifndef SURFACERECONSTRUCTION_H
#define SURFACERECONSTRUCTION_H

#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkRenderWindow.h>
#include <string>
#include <vtkGDCMImageReader.h>
#include <vtkImageChangeInformation.h>


class SurfaceReconstruction
{
public:
  SurfaceReconstruction();
  ~SurfaceReconstruction();

  /**
   * @brief ReadDICOMSeries Opens a directory where the dicom series is stored
   * and loads the files into a single vtk volume image
   * @param dir Directory where the DICOM files are stored
   * @return Number of dicom files read
   */
  int ReadDICOMSeries(std::string dir);

  /**
   * @brief Update Applies the surface reconstruction algorithm. If no image was
   * previously loaded, it does nothing.
   */
  void Update();

  /**
   * @brief Show Displays the computed surface in a render window. If no surface
   * was previously computed, it does nothing.
   * @param renderWindow a pointer to a vtkRenderWindow
   */
  void Show(vtkSmartPointer<vtkRenderWindow> renderWindow);



  // Getters and setters: surface reconstruction algorithm parameters
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
  int computed;
  vtkSmartPointer<vtkMarchingCubes> surface;
  vtkSmartPointer<vtkGDCMImageReader> gdcmReader;
  vtkSmartPointer<vtkImageChangeInformation> input;
};

#endif // SURFACERECONSTRUCTION_H

