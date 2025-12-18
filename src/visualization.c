#include "visualization.h"
#include "regression.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

VisualizationData vis_data;

void init_visualization() {
    vis_data.left = CHART_LEFT;
    vis_data.right = CHART_RIGHT;
    vis_data.bottom = CHART_BOTTOM;
    vis_data.top = CHART_TOP;

    vis_data.show_data_points = 1;
    vis_data.show_regression_line = 1;
    vis_data.show_predictions = 1;

    vis_data.predicted_pass_percent = 0.0f;
    vis_data.predicted_performance_index = 0.0f;
    vis_data.prediction_year = 2026;
}

void update_visualization(GCEDataCollection *data_collection) {
    vis_data.data = data_collection;

    // If we have data, train the model and make predictions
    if (data_collection && data_collection->record_count > 0) {
        // Create and train the regression model
        RegressionModel *model = init_regression_model(12, 0.000001f); // 12 features + bias, much smaller learning rate
        if (model) {
            train_model(model, data_collection);

            // Make a prediction for 2026
            if (data_collection->sample_count > 0) {
                // Create a feature vector for the prediction year (2026)
                // This is a simplified version - in a real implementation,
                // we'd need to estimate the other features for 2026
                float features[12];

                // For the prediction, we'll just extrapolate some values
                // In a real implementation, we'd need to estimate all features for the future year
                features[0] = 2026.0f;  // year
                features[1] = 1700.0f;  // estimated registered
                features[2] = 1650.0f;  // estimated sat
                features[3] = 50.0f;    // estimated absent
                features[4] = 80.0f;    // estimated fail (this will be adjusted in the model)
                features[5] = 150.0f;   // estimated grade A
                features[6] = 200.0f;   // estimated grade B
                features[7] = 310.0f;   // estimated grade C
                features[8] = 250.0f;   // estimated grade D
                features[9] = 210.0f;   // estimated grade E
                features[10] = 80.0f;   // estimated grade F
                features[11] = 0.03f;   // estimated absentee_rate

                // Make the prediction
                vis_data.predicted_pass_percent = predict(model, features);

                // Clean up the model
                cleanup_regression_model(model);

                // Constrain the prediction to a reasonable range
                if (vis_data.predicted_pass_percent < 0.0f) vis_data.predicted_pass_percent = 0.0f;
                if (vis_data.predicted_pass_percent > 100.0f) vis_data.predicted_pass_percent = 100.0f;
            }
        }
    }
}