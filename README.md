# InspiringSentenceGenerator
My work for Principles of Parallel Computer Architectures (UD ELEG652) final project. This project generates an inspiring quote (1 sentence) from a text file of multiple commencement and other inspiring speeches

The goal of this project is to create a project from scratch, and then use parallel computing techniques to speedup the program

All source is in the directory generator

Used gprof2dot from https://github.com/jrfonseca/gprof2dot to generate a path profiling graph for the program. This was used to study the initial program and identify areas that are in most need of parallelism

Utilized OpenMP to parallelize areas that are bottlenecks for the program
