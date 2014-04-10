/**
 * @file surfacereconstruction.cpp
 * @brief Simple surface reconstruction algorithm for a series of DICOM images
 * using vtk + gdcm libraries
 *
 * Implementation of the surface reconstruction algorithm. Check the comments
 * before use the code!
 *
 * @author Noelia Barreira (nbarreira@udc.es)
 */

#include "surfacereconstruction.h"

#include <iostream>
#include <iterator>

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkDICOMImageReader.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageThreshold.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageDilateErode3D.h>
#include <vtkGDCMImageReader.h>
#include <vtkStringArray.h>
#include <vtkDirectory.h>
#include <vtkImageChangeInformation.h>

#include <vtksys/SystemTools.hxx>

#include <gdcmIPPSorter.h>
#include <gdcmFilename.h>
#include <gdcmSystem.h>
#include <gdcmDirectory.h>
#include <gdcmDataSet.h>
#include <gdcmAttribute.h>
#include <gdcmDirectoryHelper.h>

#define DEBUG 1

SurfaceReconstruction::SurfaceReconstruction()
{
  thresholdValue = 1000;
  gaussianSize = 3;
  gaussianStd = 1;
  morphologicalSize = 3;
  surface =  vtkSmartPointer<vtkMarchingCubes>::New();
  gdcmReader = vtkSmartPointer<vtkGDCMImageReader>::New();
  input = vtkSmartPointer<vtkImageChangeInformation>::New();
  computed = 0;
}

SurfaceReconstruction::~SurfaceReconstruction()
{
}


inline bool sortByInstanceNumber(gdcm::DataSet const & ds1, gdcm::DataSet const & ds2 )
{
  gdcm::Attribute<0x0020,0x0013> at1;
  at1.Set( ds1 );
  gdcm::Attribute<0x0020,0x0013> at2;
  at2.Set( ds2 );
  return at1 < at2;
}



int SurfaceReconstruction::ReadDICOMSeries(std::string dir){


  gdcm::Directory d;
  d.Load(dir);

  const gdcm::Directory::FilenamesType filenames = d.GetFilenames();

  gdcm::Directory::FilenamesType dcmfilenames;

  // Filter filenames
  gdcm::Directory::FilenamesType::const_iterator it;
  for (it = filenames.begin(); it != filenames.end(); ++it) {
    std::string filename = *it;
    if (filename.find(".dcm") != filename.npos) {
      dcmfilenames.insert(dcmfilenames.end(), filename);
    }
  }


  if (dcmfilenames.size() > 1) {
    // Since images are not loaded from directory in any specific order,
    // images are sorted by "Instance Number" DICOM attribute.
    // You can also consider other sorting attributes, for example,
    // the Image Position with respect to the patient. In this case,
    // use a gdcm::IPPSorter
    gdcm::Sorter sorter;
    sorter.SetSortFunction(sortByInstanceNumber);
    sorter.Sort(dcmfilenames);

    gdcm::Directory::FilenamesType fn;
    fn.assign(sorter.GetFilenames().begin(), sorter.GetFilenames().end());

#ifdef DEBUG
    d.Print();
    sorter.Print(std::cout);
#endif

    vtkSmartPointer<vtkStringArray> sarray = vtkSmartPointer<vtkStringArray>::New();
    for(it = fn.begin(); it != fn. end(); ++it)  {
      const char *filename = it->c_str();
      sarray->InsertNextValue(filename);
    }


    gdcmReader->SetFileNames(sarray);
    gdcmReader->Update();

#ifdef DEBUG
    std::cout << "image pos= " << gdcmReader->GetImagePositionPatient()[0] << " - "
              << gdcmReader->GetImagePositionPatient()[1] << " - "
              << gdcmReader->GetImagePositionPatient()[2] << std::endl;
    std::cout << "dicom size = " << gdcmReader->GetDataSpacing()[0] << " - "
              << gdcmReader->GetDataSpacing()[1] << " - "
              << gdcmReader->GetDataSpacing()[2] << std::endl;
#endif

    // In my images, z-spacing is not correctly set in DICOM
    // so I set z-spacing = x-spacing. I am assuming that voxels are
    // regular cubes. Comment or modify these lines in order to adapt
    // the code to your needs.
    double *spacing = gdcmReader->GetDataSpacing();
    input->SetInputConnection(gdcmReader->GetOutputPort());
    input->SetOutputSpacing(spacing[0], spacing[1], spacing[0]);
    input->Update();
  }
  computed = 0;

  return dcmfilenames.size();

}


void SurfaceReconstruction::Update(){

  if (input) {
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
    std::cout << "done. " << std::endl;
    std::cout << surface->GetOutput()->GetNumberOfCells() << " cells generated." << std::endl;
#endif
    computed = 1;
  }
}

void SurfaceReconstruction::Show(vtkSmartPointer<vtkRenderWindow> renderWindow){

  vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(.1, .2, .3);

  renderWindow->AddRenderer(renderer);

  vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();

  if (computed) {
    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(surface->GetOutputPort());
    mapper->ScalarVisibilityOff();
    actor->SetMapper(mapper);
  }

  renderer->AddActor(actor);
  renderWindow->Render();
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



