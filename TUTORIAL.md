# GCE-Insights User Tutorial Guide

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Installation and Setup](#installation-and-setup)
4. [Preparing Your Data](#preparing-your-data)
5. [Running the Application](#running-the-application)
6. [Understanding the Visualization](#understanding-the-visualization)
7. [Using Controls](#using-controls)
8. [Interpreting Results](#interpreting-results)
9. [Troubleshooting](#troubleshooting)
10. [FAQ](#faq)

## Overview

GCE-Insights is a visualization and prediction tool for analyzing trends in Cameroon GCE Advanced Level subject performance. The application uses machine learning techniques to predict future performance based on historical data and presents the results in an intuitive visual format.

### Key Features
- **Historical Data Visualization**: Displays past GCE subject performance trends with interactive charts
- **Machine Learning Prediction**: Implements multiple linear regression to predict future performance
- **Interactive Controls**: Keyboard controls to toggle data display and reset the view
- **Real-time Feedback**: Shows predicted values for upcoming examination years

## Prerequisites

Before using GCE-Insights, you need:

1. **Gama Engine**: The application runs on the Gama game engine framework
2. **CSV Data File**: Historical GCE data in the required format (see [Preparing Your Data](#preparing-your-data))
3. **Compatible System**: Linux, Windows, or macOS with compiler support

## Installation and Setup

### Setting up Gama Engine
1. Install Gama Engine following the official documentation
2. Verify installation by running `gama --version`

### Cloning/Installing GCE-Insights
1. Navigate to your Gama projects directory
2. Clone or download the GCE-Insights project
3. Verify all necessary files are present:
   - `src/main.c`
   - `src/gce_data.c`
   - `data_sample.csv` (or your custom CSV file)
   - `gama.toml` (configuration file)

## Preparing Your Data

### CSV File Format

GCE-Insights expects your historical data in CSV format with the following columns:

```
year,registered,sat,absent,fail,A,B,C,D,E,F
2016,1400,1350,50,180,120,220,300,280,250,180
2017,1450,1400,50,175,130,230,310,290,260,175
...
```

### Column Definitions
- `year`: The examination year (e.g., 2016, 2017, 2018...)
- `registered`: Number of students registered for the subject
- `sat`: Number of students who took the examination
- `absent`: Number of absentees
- `fail`: Number of students who failed
- `A`: Number of A-grade achievers
- `B`: Number of B-grade achievers
- `C`: Number of C-grade achievers
- `D`: Number of D-grade achievers
- `E`: Number of E-grade achievers
- `F`: Number of F-grade achievers

### Data Preparation Tips
1. **Include at least 10 years** of data for better prediction accuracy
2. **Ensure continuous years** without gaps, if possible
3. **Verify data accuracy** - incorrect data will lead to poor predictions
4. **Clean your data** - remove any obvious outliers or errors
5. **Save your CSV file** in the same directory as the application

### Example Data File
```
year,registered,sat,absent,fail,A,B,C,D,E,F
2015,1300,1250,50,200,100,200,280,250,220,200
2016,1400,1350,50,180,120,220,300,280,250,180
2017,1450,1400,50,175,130,230,310,290,260,175
2018,1480,1430,50,170,135,235,315,295,255,170
2019,1520,1470,50,165,140,240,320,300,260,165
2020,1550,1500,50,160,145,245,325,305,255,160
2021,1580,1530,50,155,150,250,330,310,260,155
2022,1620,1570,50,150,155,255,335,315,265,150
2023,1650,1600,50,145,160,260,340,320,270,145
2024,1680,1630,50,140,165,265,345,325,275,140
2025,1720,1670,50,135,170,270,350,330,280,135
```

## Running the Application

### Method 1: Using Gama Command
1. Navigate to the GCE-Insights project directory
2. Execute the following command:
   ```bash
   gama run
   ```

### Method 2: Using Built Executable
1. Navigate to the project directory
2. Build the project: `gama build`
3. Run the executable from the `build/native` directory

### Loading Your Data
By default, the application looks for `data_sample.csv`. To use your own data file:

1. Rename your CSV file to `data_sample.csv` OR
2. Modify the filename in `src/main.c` at the `load_gce_data` function call

For example:
```c
load_gce_data(data_collection, "my_gce_data.csv");
```

## Understanding the Visualization

The application displays a comprehensive visualization of GCE performance data:

### Chart Layout
- **X-axis**: Years (from earliest to latest in your dataset)
- **Y-axis**: Pass percentage (0% to 100%)

### Color-Coded Elements
- **Blue dots**: Historical actual performance data points
- **Red dot**: Predicted performance for the next year
- **Gray lines**: Grid lines for reference
- **White axes**: X and Y axes with labels

### Data Labels
- **Year labels**: Appear along the X-axis to indicate specific years
- **Percentage labels**: Appear along the Y-axis to indicate pass percentage values
- **Bottom legend**: Explains what each element represents

## Using Controls

The application responds to keyboard input:

- **'R' key**: Reset the visualization and retrain the model with current data
- **'S' key**: Toggle visibility of historical data points (blue dots)
- **'P' key**: Toggle visibility of prediction points (red dot and line)
- **'ESC' key**: Close the application

### Control Indicators
At the bottom of the screen, a text element displays the available controls for easy reference.

## Interpreting Results

### Historical Trends
- Blue dots represent actual pass percentages for each year
- Look for patterns such as:
  - **Upward trends**: Improving performance over time
  - **Downward trends**: Declining performance over time
  - **Plateaus**: Consistent performance levels
  - **Volatility**: Large variations between years

### Predictions
- Red dot represents the predicted pass percentage for the next year (typically 2026)
- The position relative to historical data indicates the predicted direction of change
- Note that predictions become less accurate the further into the future they extend

### Model Confidence
- The prediction takes into account all variables in your dataset
- More historical data generally leads to more reliable predictions
- Consider the consistency of historical trends when interpreting predictions

### Performance Indicators
- **Pass Percentage**: The primary metric displayed in the visualization
- **Prediction Accuracy**: The model continuously trains to minimize prediction error
- **Visual Consistency**: The smoothness of historical trend lines indicates data quality

## Troubleshooting

### Common Issues and Solutions

#### Issue: Application won't start or crashes immediately
- **Solution**: Verify that Gama Engine is properly installed and working
- **Check**: All required files are present in the project directory

#### Issue: No data appears on the screen
- **Solution**: Verify your CSV file is named correctly and in the right directory
- **Check**: CSV format matches the required structure with correct column names
- **Verify**: Your CSV file contains data rows (not just headers)

#### Issue: All prediction values show as zeros or unexpected values
- **Solution**: Check for missing or invalid data in your CSV file (such as empty cells or non-numeric values)
- **Check**: Values in your CSV file are appropriate numbers (not text strings)

#### Issue: Regression algorithm isn't converging
- **Solution**: This may take several minutes for large datasets; be patient
- **Check**: Your dataset should have at least 5-10 years of data for good results

#### Issue: Poor prediction accuracy
- **Solution**: Ensure you have a sufficient amount of historical data (recommended 10+ years)
- **Check**: Your historical data is consistent and free of errors
- **Verify**: The relationship in your data is somewhat linear (not completely random)

#### Issue: Error loading CSV file
- **Solution**: Ensure CSV file uses commas as separators (not semicolons or tabs)
- **Check**: CSV file is encoded in UTF-8 format
- **Verify**: File is not opened in Excel or another application that may lock it

### Checking for Errors
Look for error messages printed to the console when the application starts. These will often provide specific information about what went wrong.

## FAQ

### Q: Why do I need at least 10 years of data?
A: The machine learning algorithm needs sufficient historical data to identify patterns and trends reliably. With less data, predictions become unreliable and may not reflect real trends.

### Q: Can I use data for multiple subjects simultaneously?
A: Currently, the application works with a single subject's data per session. To analyze different subjects, create separate CSV files and run the application separately for each.

### Q: How far into the future does the prediction go?
A: By default, the application predicts for the next year after your last historical data point. In our examples, if your data goes to 2025, it predicts 2026.

### Q: Can I change what is being predicted?
A: Currently, the application predicts pass percentage. The code can be modified to predict other metrics like specific grade distributions, but this requires development changes.

### Q: How accurate are the predictions?
A: Prediction accuracy depends on the quality and consistency of your historical data. The algorithm uses multiple linear regression which works well for data with linear trends, but cannot account for unforeseen events or significant curriculum changes.

### Q: Does the application store or share my data?
A: No, GCE-Insights runs locally and does not connect to any external servers. Your data remains on your computer and is not transmitted anywhere.

### Q: Can I customize the visualization colors?
A: Yes, colors can be customized by modifying the color constants in the source code. Look for `gm_COLOR` constants in the main.c file.

### Q: What if my data spans multiple decades?
A: Longer datasets may provide better predictions, but be aware that educational contexts may change significantly over long time periods (new curricula, teaching methods, etc.), which the model doesn't account for.

### Q: How do I interpret the model's confidence level?
A: The visualization doesn't directly show confidence intervals. However, if your historical data shows consistent trends, predictions tend to be more reliable. Volatile data patterns suggest more uncertainty in predictions.

---

## Need More Help?

If you encounter issues not covered in this guide:

1. Verify all steps in the tutorial have been followed correctly
2. Check that your data is in the proper format
3. Ensure you have the latest version of the Gama Engine
4. Consult the Gama Engine documentation for general troubleshooting
5. Contact the development team with specific error messages or issues