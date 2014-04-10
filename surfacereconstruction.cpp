#include "surfacereconstruction.h"

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataMapper.h>
#include <vtkThresholdPoints.h>
#include <vtkImageThreshold.h>
#include <vtkContourFilter.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageDilateErode3D.h>
#include <vtkGDCMImageReader.h>
#include <vtkStringArray.h>
#include <vtkDirectory.h>
#include <vtksys/SystemTools.hxx>

#include <gdcmIPPSorter.h>
#include <gdcmFilename.h>
#include <gdcmTesting.h>
#include <gdcmSystem.h>
#include <gdcmTrace.h>
#include <gdcmDirectory.h>
#include <gdcmScanner.h>
#include <gdcmIPPSorter.h>
#include "gdcmDataSet.h"
#include "gdcmAttribute.h"
#include <vtkImageChangeInformation.h>
#include <iterator>

#define DEBUG 1

SurfaceReconstruction::SurfaceReconstruction()
{
   thresholdValue = 1000;
   gaussianSize = 3;
   gaussianStd = 1;
   morphologicalSize = 3;
   surface =  vtkSmartPointer<vtkMarchingCubes>::New();
   std::cout << "en constructor" << std::endl;
   gdcmReader = vtkSmartPointer<vtkGDCMImageReader>::New();
}

inline bool sortByInstanceNumber(gdcm::DataSet const & ds1, gdcm::DataSet const & ds2 )
{
  gdcm::Attribute<0x0020,0x0013> at1;
  at1.Set( ds1 );
  gdcm::Attribute<0x0020,0x0013> at2;
  at2.Set( ds2 );
  return at1 < at2;
}

void SurfaceReconstruction::readDICOMDir(std::string &dir){


  gdcm::Directory d;
  d.Load( dir );


  vtkStringArray *sarray = vtkStringArray::New();
  const gdcm::Directory::FilenamesType filenames = d.GetFilenames();
  gdcm::Directory::FilenamesType fn;

  d.Print();

  //gdcm::IPPSorter sorter; // Sort por nome de imaxe!
  gdcm::Sorter sorter;
  sorter.SetSortFunction(sortByInstanceNumber);
  sorter.Sort(filenames);
  std::cout <<"size =" << sorter.GetFilenames().size() << std::endl;
  fn.assign(sorter.GetFilenames().begin(), sorter.GetFilenames().end());

   gdcm::Directory::FilenamesType::const_iterator it = fn.begin();;
   for(; it != fn. end() /*&& i < 2*/; ++it)  {
        const char *filename = it->c_str();
        std::cout << "---" << filename << std::endl;
        sarray->InsertNextValue(filename);
   }


   gdcmReader->SetFileNames(sarray);
   gdcmReader->Update();

   double *spacing = gdcmReader->GetDataSpacing();
   input = vtkSmartPointer<vtkImageChangeInformation>::New();
   input->SetInputConnection(gdcmReader->GetOutputPort());
   input->SetOutputSpacing(spacing[0], spacing[1], spacing[0]);
   input->Update();


  volume = input->GetOutput();




#ifdef DEBUG
//  std::cout << "image pos= " << reader->GetImagePositionPatient()[0] << " - "<< reader->GetImagePositionPatient()[1] << " - "<< reader->GetImagePositionPatient()[2] << std::endl;
//  std::cout << "dicom size = " << reader->GetPixelSpacing()[0] << " - " << reader->GetPixelSpacing()[1] << " - " << reader->GetPixelSpacing()[2] << std::endl;
  std::cout << "image pos= " << gdcmReader->GetImagePositionPatient()[0] << " - "<< gdcmReader->GetImagePositionPatient()[1] << " - "<< gdcmReader->GetImagePositionPatient()[2] << std::endl;
  std::cout << "dicom size = " << gdcmReader->GetDataSpacing()[0] << " - " << gdcmReader->GetDataSpacing()[1] << " - " << gdcmReader->GetDataSpacing()[2] << std::endl;
#endif

  std::cout << volume->GetOrigin()[0]  << " - "<< volume->GetOrigin()[1]  << " - "<< volume->GetOrigin()[2]  << std::endl;
  std::cout << volume->GetDimensions()[0]  << " - "<< volume->GetDimensions()[1]  << " - "<< volume->GetDimensions()[2]  << std::endl;

  // Problema en imaxes convertidas!
//  if ((gdcmReader->GetDataSpacing()[2] <= 0) || (gdcmReader->GetDataSpacing()[2] >= 1)) {
//  }
#ifdef DEBUG
#endif

  gdcmReader->SetDataSpacing(gdcmReader->GetDataSpacing()[0], gdcmReader->GetDataSpacing()[1], gdcmReader->GetDataSpacing()[0]);


}

vtkSmartPointer<vtkAlgorithmOutput> SurfaceReconstruction::getOutputPort(){
  //return surface->GetOutputPort();
  sphereSource =
        vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();
  return sphereSource->GetOutputPort();
}

void SurfaceReconstruction::update(){

  vtkSmartPointer <vtkImageGaussianSmooth> gaussian =
      vtkSmartPointer<vtkImageGaussianSmooth>::New();
#ifdef DEBUG
  std::cout << "Gaussian Smoothing ... ";
#endif
  gaussian->SetInputConnection(input->GetOutputPort());
  gaussian->SetRadiusFactor(gaussianSize);
  gaussian->SetStandardDeviation(gaussianStd);
  gaussian->Update();

#ifdef DEBUG
  std::cout << "done" << std::endl;
#endif

  vtkSmartPointer <vtkImageThreshold> threshold =
      vtkSmartPointer<vtkImageThreshold>::New();

#ifdef DEBUG
  std::cout << "Thresholding ... ";
#endif
  threshold->ThresholdByUpper(thresholdValue);
  threshold->SetInputConnection(gaussian->GetOutputPort());
  //  threshold->SetInputData(volume);
  threshold->SetInValue(1);
  threshold->SetOutValue(0);
  threshold->ReplaceInOn();
  threshold->Update();
#ifdef DEBUG
  std::cout << "done" << std::endl;
#endif

  vtkSmartPointer<vtkImageDilateErode3D> morphology =
      vtkSmartPointer<vtkImageDilateErode3D>::New();

#ifdef DEBUG
  std::cout << "Morphological filtering ... ";
#endif
  morphology->SetInputConnection(threshold->GetOutputPort());
  morphology->SetDilateValue(1);
  morphology->SetErodeValue(0);
  morphology->SetKernelSize(morphologicalSize,morphologicalSize,morphologicalSize);
  morphology->Update();
#ifdef DEBUG
  std::cout << "done" << std::endl;
#endif


#ifdef DEBUG
  std::cout << "Marching cubes ... ";
#endif

  surface->SetInputConnection(morphology->GetOutputPort());
  surface->ComputeNormalsOn();
  surface->SetValue(0, 1);
  surface->Update();


#ifdef DEBUG
  std::cout << "done" << surface->GetOutput()->GetNumberOfCells() << std::endl;
#endif

}

void SurfaceReconstruction::show(vtkSmartPointer<vtkRenderWindow> renderWindow){

  vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(.1, .2, .3);

  renderWindow->AddRenderer(renderer);

  //    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
  //        vtkSmartPointer<vtkRenderWindowInteractor>::New();
  //    interactor->SetRenderWindow(renderWindow);
  //sphereSource =
  //      vtkSmartPointer<vtkSphereSource>::New();
  //sphereSource->Update();

  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(surface->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  renderer->AddActor(actor);

  renderWindow->Render();
  //    interactor->Start();
}



double SurfaceReconstruction::getThreshold() const
{
  return thresholdValue;
}

void SurfaceReconstruction::setThreshold(double value)
{
  thresholdValue = value;
}


double SurfaceReconstruction::getGaussianSize() const
{
  return gaussianSize;
}

void SurfaceReconstruction::setGaussianSize(double value)
{
  gaussianSize = value;
}


double SurfaceReconstruction::getGaussianStd() const
{
  return gaussianStd;
}

void SurfaceReconstruction::setGaussianStd(double value)
{
  gaussianStd = value;
}


double SurfaceReconstruction::getMorphologicalSize() const
{
  return morphologicalSize;
}

void SurfaceReconstruction::setMorphologicalSize(double value)
{
  morphologicalSize = value;
}



