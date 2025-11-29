# Deep Forecasting: Storms and Lightning Flashes

[Project introduction slides](https://docs.google.com/presentation/d/1TfCMbso8uVv_xyymKZ02XQw0zAMt9ixM/edit?usp=sharing&ouid=115479103401816059872&rtpof=true&sd=true)

[Colab notebook: Example data downloading and exploration](https://colab.research.google.com/drive/1A5o3X2BgpYU-h94dOCpBOqhE64w6AuO9?usp=sharing)

[Colab notebook: Suprise storms - description and submission instructions](https://colab.research.google.com/drive/18dCAJNVnErHZVFSztM-EJnwiVH-Eb3D3?usp=sharing)


---

## Table of Contents

1. [Introduction](#introduction)
2. [Project Goals](#project-goals)
3. [Dataset](#dataset)
4. [Tasks Overview](#tasks-overview)
   - [Task 1A](#task-1a)
   - [Task 1B](#task-1b)
   - [Task 2](#task-2)
   - [Task 3](#task-3)
   - [Optional Bonus Tasks](#optional-bonus-tasks)
5. [Setup and Installation](#setup-and-installation)
6. [Usage](#usage)
7. [Contributors](#contributors)
8. [References](#references)
9. [License](#license)

---

## Introduction

This project aims to develop machine learning and deep learning solutions for real-time lightning storm prediction. With the rising impacts of climate change and the increasing unpredictability of severe weather, accurate storm forecasting has become crucial for saving lives and minimising disruptions. This project simulates a FEMA challenge to design a reliable model for lightning storm forecasting, and is subdivided into different tasks while balancing functionality, performance and sustainability of the code. 

---

## Project Goals

- Develop ML/DL-based models capable of predicting future weather patterns based on storm data.
- Generate reliable and explainable predictions for lightning storm evolution.
- Explore scalability and explainability for real-world applications.

---

## Dataset

### Description

The dataset includes:
- **Satellite Images**:
  - Visible (VIS)
  - Water Vapor (IR069 - Infrared)
  - Cloud/Surface Temperature (IR107 - Infrared)
  - Vertically Integrated Liquid (VIL - Radar)
- **Lightning Flashes**:
  - Time series data


---

## Tasks Overview

### Task 1A
Predict 12 future VIL frames based on 12 previous VIL frames.

### Task 1B
Predict 12 future VIL frames using input frames from VIS, IR069, IR107, and VIL data.

### Task 2
Generate missing VIL frames using VIS, IR069, and IR107 data.

### Task 3
Predict lightning flashes:
- Number of flashes
- Time of occurrence
- VIL pixel location of each flash

### Optional Bonus Tasks
1. Assess scaling laws: Explore the performance of models with varying compute and dataset sizes.
2. Explainability: Justify and interpret model predictions.

---

## Setup and Installation

To get started with this project, follow these steps:

### Prerequisites

Ensure you have the following installed:
- Python 3.8 or higher
- Jupyter Notebook or Google Colab

### Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-repo-name/ACDS-Storm-Prediction-Irma.git
   cd ACDS-Storm-Prediction-Irma

---

## Usage

1. Start Jupyter Notebook and open the notebooks in `notebooks-final/` directory for exploration of results. 
   ```bash
   jupyter notebook notebooks-final/example.ipynb

2. The test cases are located in the `tests/` directory. Use `pytest` to run the tests and verify the functionality of the code.
   ```bash
   cd tests
   pytest -vv

---

## Contributors

The contributors to this project are the members of the ACDS-storm-prediction-irma team.

---

## References

See the [REFERENCES](REFERENCES.md) file. 

---

## Licensing

This project is licensed under the MIT License. See the [LICENSE](LICENSE.txt) file for details.
