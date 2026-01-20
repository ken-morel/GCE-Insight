# GCE-Insight: Predictive Analysis of GCE Subject Performance

GCE-Insight is a visualization and prediction tool for analyzing trends in Cameroon GCE Advanced Level subject performance. The application uses machine learning techniques to predict future performance based on historical data and presents the results in an intuitive visual format.

## Features

- **Historical Data Visualization**: Displays past GCE subject performance trends with interactive charts
- **Machine Learning Prediction**: Implements multiple linear regression to predict future performance
- **Interactive Controls**: Keyboard controls to toggle data display and reset the view
- **Real-time Feedback**: Shows predicted values for upcoming examination years

## How It Works

The application analyzes historical GCE data (currently using sample data for demonstration) and applies machine learning techniques to identify trends. The system:

1. Processes historical GCE data with features like registration numbers, participation rates, grade distributions
2. Trains a multiple linear regression model using gradient descent with feature normalization
3. Makes predictions for future performance (currently predicting for 2026)
4. Visualizes both historical data and predictions in an easy-to-understand format

## Controls

- Press **'R'** to reset the data and retrain the model
- Press **'S'** to toggle visibility of historical data points
- Press **'P'** to toggle visibility of prediction points

## Technical Details

- Built using the Gama game engine for visualization capabilities
- Implements custom multiple linear regression with gradient descent
- Features automatic feature normalization to handle different data scales
- Includes adaptive learning rate adjustment to ensure stable training
- Provides real-time visualization of model training progress

## Data Model

The system models GCE subject performance based on:
- Historical pass percentages
- Registration and participation statistics
- Grade distribution (A through F)
- Derived metrics like absentee rates and excellence rates

## Project Structure

- `src/gce_data.c/h`: Data structures and loading functions for GCE records
- `src/regression.c/h`: Machine learning implementation with gradient descent
- `src/visualization.c/h`: Rendering and visualization logic
- `src/main.c`: Application entry point and main game loop

## Usage

Run the application with:

```bash
gama run
```

The application will train the model automatically and display the visualization with historical data points and predictions.
