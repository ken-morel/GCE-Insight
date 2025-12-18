#pragma once

#include "gce_data.h"
#include <stdio.h>

// Visualization configuration
#define CHART_LEFT -0.8
#define CHART_RIGHT 0.8
#define CHART_BOTTOM -0.6
#define CHART_TOP 0.6

// Visualization data structure
typedef struct {
    // Chart boundaries
    double left, right, bottom, top;

    // Data to visualize
    GCEDataCollection *data;

    // Prediction results
    float predicted_pass_percent;
    float predicted_performance_index;
    int prediction_year;

    // Visual elements
    int show_data_points;
    int show_regression_line;
    int show_predictions;
} VisualizationData;

// Global visualization data for access from main
extern VisualizationData vis_data;

// Function declarations
void init_visualization();
void update_visualization(GCEDataCollection *data_collection);
// All the drawing functions will be handled in main.c since only main.c includes gama.h