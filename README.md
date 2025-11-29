# Nimbus_Project_Ayush_61
COVID Vaccination Center Management System - Project 61
Developed by: Ayush Singh
  Project Overview

This project implements a COVID Vaccination Center Management System using the C programming language.
It helps vaccination centers manage:

Citizen registration

Vaccine allocation

Dose scheduling

Stock availability

Daily vaccination reports

Pending second-dose alerts

The system uses structures, dynamic memory allocation, file handling, arrays, loops, and functions, covering all required syllabus outcomes.

  Objectives

Build a real-world vaccination tracking system

Apply C programming fundamentals in a complete application

Use file handling for persistent storage (citizens.txt)

Implement dynamic list of citizens using pointers

Track vaccine stocks and update dose information

Generate reports similar to real health-center workflows

  Real-World Context

Vaccination centers maintain citizen details, available vaccine stocks, dose schedules, and daily vaccination counts.
This project simulates the same real-world functioning through logical programming.

  Key Features
  1. Citizen Registration

Stores the following details:

Citizen ID

Full name

Age

Vaccine name

Dose count

Last dose date

Next dose due date

Vaccination status

  2. Vaccination Update

Administers Dose 1 or Dose 2

Verifies eligibility (age ≥ 18)

Reduces stock after each vaccination

Schedules second-dose date after Dose 1

Changes status:

Not Vaccinated

Partially Vaccinated

Fully Vaccinated

  3. Vaccine Stock Management

Allows:

Viewing current stock

Updating stock quantity per vaccine type
Supports:

COVAXIN

COVISHIELD

SPUTNIK

  4. Daily Vaccination Report

Generates report based on date:

Total vaccinations

Dose 1 count

Dose 2 count

Summary of:

Not vaccinated

Partially vaccinated

Fully vaccinated citizens

  5. Pending Second Dose Alerts

Displays all citizens:

Who received only one dose

With scheduled second-dose date

Vaccine type and last dose date
