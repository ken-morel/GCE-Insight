#pragma once

#include "gce_data.h"
#include <math.h>

// Regression model parameters
typedef struct {
    float *theta;           // Parameters (weights)
    size_t n_features;      // Number of features
    float learning_rate;
    size_t max_iterations;
    float tolerance;        // For convergence check
    float *feature_means;   // Mean values for feature normalization
    float *feature_ranges;  // Range values for feature normalization
} RegressionModel;

// Function declarations
RegressionModel* init_regression_model(size_t n_features, float learning_rate);
void cleanup_regression_model(RegressionModel *model);
void train_model(RegressionModel *model, GCEDataCollection *data);
float predict(RegressionModel *model, float *features);
float compute_cost(RegressionModel *model, GCEDataCollection *data);
void gradient_descent_step(RegressionModel *model, GCEDataCollection *data);
void save_model(RegressionModel *model, const char *filename);
RegressionModel* load_model(const char *filename);

// Evaluation metrics
float compute_mse(RegressionModel *model, GCEDataCollection *data);
float compute_mae(RegressionModel *model, GCEDataCollection *data);