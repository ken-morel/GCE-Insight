#pragma once

#include "gce_data.h"
#include <stdio.h>

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

// Function declarations for visualization logic 
void init_visualization();
void update_visualization(GCEDataCollection *data_collection);
void draw_visualization();         // This will be implemented in a separate file that includes gama.h
void draw_data_info();             // This will be implemented in a separate file that includes gama.h
void draw_chart_axes();            // This will be implemented in a separate file that includes gama.h
void draw_data_points();           // This will be implemented in a separate file that includes gama.h
void draw_regression_line();       // This will be implemented in a separate file that includes gama.h
void draw_predictions();           // This will be implemented in a separate file that includes gama.h
void draw_legend();                // This will be implemented in a separate file that includes gama.h

// Utility functions
double map_value(double value, double in_min, double in_max, double out_min, double out_max);
void draw_text_centered(double x, double y, const char *text, unsigned int color, double scale);