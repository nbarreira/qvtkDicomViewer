qvtkDicomViewer
===============

A simple viewer for DICOM series of images.

## About
This code shows the use of vtk for extracting and displaying surfaces
in a Qt application from DICOM images.


## Dependencies

This source code was tested in Debian GNU/Linux Jessie and has the
following dependencies:

- qt4.8
- qmake4
- libgdcm2.4
- libvtkgdcm2-dev
- libvtk5.8
- libvtk5-dev
- libvtk5.8-qt4
- libvtk5-qt4-dev
- libvtkgdcm2.4
- libvtkgdcm2-dev


## Input images

This application is only able to read a series of DICOM images that
form a volume. The input images are read from a directory and should
share the same DICOM attributes in the patient uid and study uid.
It was tested with Cone Bean CT images so I do not guarantee that
it works with other kinds of DICOM images.

## Output
The application displays the volume in a QVTKWidget. The threshold is
adjusted to segment bone surfaces from CT images. Adjust it to your
needs.

## Known bugs
According to Valgrind, vtksmartpointer memory leaks were resolved. 
However, there are many others memory leaks due to qt libraries.
