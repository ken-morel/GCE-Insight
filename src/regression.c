#include "regression.h"
#include "gce_data.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

RegressionModel *init_regression_model(size_t n_features, float learning_rate) {
  RegressionModel *model = malloc(sizeof(RegressionModel));
  if (!model)
    return NULL;

  model->n_features = n_features;
  model->learning_rate = learning_rate;
  model->max_iterations = 1000;
  model->tolerance = 1e-6;

  // Allocate parameters (including bias term)
  model->theta = malloc((n_features + 1) * sizeof(float));
  if (!model->theta) {
    free(model);
    return NULL;
  }

  // Initialize normalization parameters
  model->feature_means = calloc(n_features, sizeof(float));
  if (!model->feature_means) {
    free(model->theta);
    free(model);
    return NULL;
  }

  model->feature_ranges = calloc(n_features, sizeof(float));
  if (!model->feature_ranges) {
    free(model->feature_means);
    free(model->theta);
    free(model);
    return NULL;
  }

  // Initialize parameters randomly
  srand(time(NULL));
  for (size_t i = 0; i <= n_features; i++) {
    model->theta[i] = ((float)rand() / RAND_MAX) * 0.1f -
                      0.05f; // Small random values between -0.05 and 0.05
  }

  return model;
}

void cleanup_regression_model(RegressionModel *model) {
  if (!model)
    return;

  if (model->theta) {
    free(model->theta);
  }

  if (model->feature_means) {
    free(model->feature_means);
  }

  if (model->feature_ranges) {
    free(model->feature_ranges);
  }

  free(model);
}

// We'll keep the original gradient descent function as is, since train_model
// now has its own implementation with normalization. This keeps the function
// available for other uses.
void gradient_descent_step(RegressionModel *model, GCEDataCollection *data) {
  if (!model || !data || data->sample_count == 0)
    return;

  // Create temporary arrays to store gradients
  float *gradients = calloc(model->n_features + 1, sizeof(float));
  if (!gradients)
    return;

  // Calculate gradients
  for (size_t i = 0; i < data->sample_count; i++) {
    // Calculate prediction for this sample
    float prediction = model->theta[0]; // Bias term
    for (size_t j = 0; j < model->n_features; j++) {
      prediction += model->theta[j + 1] * data->training_samples[i].X[j];
    }

    float error = prediction - data->training_samples[i].y;

    // Update bias gradient
    gradients[0] += error;

    // Update feature gradients
    for (size_t j = 0; j < model->n_features; j++) {
      gradients[j + 1] += error * data->training_samples[i].X[j];
    }
  }

  // Average the gradients
  float m = (float)data->sample_count;
  for (size_t i = 0; i <= model->n_features; i++) {
    gradients[i] /= m;
  }

  // Update parameters
  for (size_t i = 0; i <= model->n_features; i++) {
    model->theta[i] -= model->learning_rate * gradients[i];
  }

  free(gradients);
}

float compute_cost(RegressionModel *model, GCEDataCollection *data) {
  if (!model || !data || data->sample_count == 0)
    return 0.0f;

  float total_cost = 0.0f;

  for (size_t i = 0; i < data->sample_count; i++) {
    // Calculate prediction for this sample
    float prediction = model->theta[0]; // Bias term
    for (size_t j = 0; j < model->n_features; j++) {
      prediction += model->theta[j + 1] * data->training_samples[i].X[j];
    }

    float error = prediction - data->training_samples[i].y;
    total_cost += error * error;
  }

  return total_cost / (2.0f * (float)data->sample_count);
}

void train_model(RegressionModel *model, GCEDataCollection *data) {
  if (!model || !data || data->sample_count == 0)
    return;

  // Calculate means and ranges for normalization
  for (size_t j = 0; j < model->n_features; j++) {
    float min_val = data->training_samples[0].X[j];
    float max_val = data->training_samples[0].X[j];
    float sum = 0.0f;

    for (size_t i = 0; i < data->sample_count; i++) {
      float val = data->training_samples[i].X[j];
      sum += val;
      if (val < min_val)
        min_val = val;
      if (val > max_val)
        max_val = val;
    }

    model->feature_means[j] = sum / (float)data->sample_count;
    model->feature_ranges[j] =
        (max_val == min_val) ? 1.0f : (max_val - min_val);

    // Handle case where feature range is zero to avoid division by zero
    if (model->feature_ranges[j] == 0.0f) {
      model->feature_ranges[j] = 1.0f;
    }
  }

  // Normalize features
  float **normalized_features = malloc(data->sample_count * sizeof(float *));
  if (!normalized_features) {
    return;
  }

  for (size_t i = 0; i < data->sample_count; i++) {
    normalized_features[i] =
        malloc((model->n_features + 1) * sizeof(float)); // +1 for bias
    if (!normalized_features[i]) {
      // Clean up on error
      for (size_t k = 0; k < i; k++) {
        free(normalized_features[k]);
      }
      free(normalized_features);
      return;
    }

    normalized_features[i][0] = 1.0f; // Bias term
    for (size_t j = 0; j < model->n_features; j++) {
      normalized_features[i][j + 1] =
          (data->training_samples[i].X[j] - model->feature_means[j]) /
          model->feature_ranges[j];
    }
  }

  static size_t logs = 0;
  logs++;

  float prev_cost = compute_cost(model, data);
  if (logs % 100 == 0)
    printf("Initial cost: %f\n", prev_cost);

  for (size_t iter = 0; iter < model->max_iterations; iter++) {
    // Perform one gradient descent step using normalized features
    float *gradients = calloc(model->n_features + 1, sizeof(float));
    if (!gradients)
      break;

    // Calculate gradients using normalized features
    for (size_t i = 0; i < data->sample_count; i++) {
      float prediction = 0.0f;
      // Calculate prediction using current model parameters
      for (size_t j = 0; j <= model->n_features; j++) {
        prediction += model->theta[j] * normalized_features[i][j];
      }

      float error = prediction - data->training_samples[i].y;

      // Update gradients
      for (size_t j = 0; j <= model->n_features; j++) {
        gradients[j] += error * normalized_features[i][j];
      }
    }

    // Average the gradients
    float m = (float)data->sample_count;
    for (size_t j = 0; j <= model->n_features; j++) {
      gradients[j] /= m;
    }

    // Check for gradient explosion and adjust learning rate if needed
    float gradient_norm = 0.0f;
    for (size_t j = 0; j <= model->n_features; j++) {
      gradient_norm += gradients[j] * gradients[j];
    }
    gradient_norm = sqrtf(gradient_norm);

    float current_learning_rate = model->learning_rate;
    // Reduce learning rate if gradients are too large
    if (gradient_norm > 100.0f) {
      current_learning_rate *= 0.1f;
    } else if (gradient_norm > 10.0f) {
      current_learning_rate *= 0.5f;
    }

    // Update parameters
    for (size_t j = 0; j <= model->n_features; j++) {
      model->theta[j] -= current_learning_rate * gradients[j];
    }

    free(gradients);

    // Check for convergence
    float current_cost = compute_cost(model, data);
    if (logs % 100 == 0) {
      // Optional: print progress every 100 iterations
      if (iter % 100 == 0) {
        printf("Iteration %zu, Cost: %f, GradNorm: %f\n", iter, current_cost,
               gradient_norm);
      }

      // Check if change in cost is below tolerance
      if (fabs(prev_cost - current_cost) < model->tolerance) {
        printf("Converged at iteration %zu\n", iter);
        break;
      }

      // Check for divergence
      if (isnan(current_cost) || isinf(current_cost) || current_cost > 1e10) {
        printf("Divergence detected at iteration %zu, stopping training\n",
               iter);
        break;
      }

      // If cost is increasing, reduce learning rate
      if (current_cost > prev_cost * 1.1f) { // 10% increase threshold
        model->learning_rate *= 0.5f;        // Reduce learning rate
        printf("Learning rate reduced to %f due to cost increase\n",
               model->learning_rate);
      }
    }
    prev_cost = current_cost;
  }

  // Clean up
  for (size_t i = 0; i < data->sample_count; i++) {
    free(normalized_features[i]);
  }
  free(normalized_features);

  if (logs % 100 == 0)
    printf("Final cost: %f\n", compute_cost(model, data));
}

float predict(RegressionModel *model, float *features) {
  if (!model || !features)
    return 0.0f;

  // Apply the same normalization that was used during training
  float prediction = model->theta[0]; // Bias term
  for (size_t i = 0; i < model->n_features; i++) {
    float normalized_feature =
        (features[i] - model->feature_means[i]) / model->feature_ranges[i];
    prediction += model->theta[i + 1] * normalized_feature;
  }

  return prediction;
}

// Evaluation metrics
float compute_mse(RegressionModel *model, GCEDataCollection *data) {
  if (!model || !data || data->sample_count == 0)
    return 0.0f;

  float total_error = 0.0f;

  for (size_t i = 0; i < data->sample_count; i++) {
    float predicted = predict(model, data->training_samples[i].X);
    float error = predicted - data->training_samples[i].y;
    total_error += error * error;
  }

  return total_error / (float)data->sample_count;
}

float compute_mae(RegressionModel *model, GCEDataCollection *data) {
  if (!model || !data || data->sample_count == 0)
    return 0.0f;

  float total_error = 0.0f;

  for (size_t i = 0; i < data->sample_count; i++) {
    float predicted = predict(model, data->training_samples[i].X);
    float error = fabs(predicted - data->training_samples[i].y);
    total_error += error;
  }

  return total_error / (float)data->sample_count;
}

void save_model(RegressionModel *model, const char *filename) {
  if (!model || !filename)
    return;

  FILE *file = fopen(filename, "wb");
  if (!file)
    return;

  fwrite(&model->n_features, sizeof(size_t), 1, file);
  fwrite(&model->learning_rate, sizeof(float), 1, file);
  fwrite(&model->max_iterations, sizeof(size_t), 1, file);
  fwrite(&model->tolerance, sizeof(float), 1, file);
  fwrite(model->theta, sizeof(float), model->n_features + 1, file);

  fclose(file);
}

RegressionModel *load_model(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file)
    return NULL;

  RegressionModel *model = malloc(sizeof(RegressionModel));
  if (!model) {
    fclose(file);
    return NULL;
  }

  fread(&model->n_features, sizeof(size_t), 1, file);
  fread(&model->learning_rate, sizeof(float), 1, file);
  fread(&model->max_iterations, sizeof(size_t), 1, file);
  fread(&model->tolerance, sizeof(float), 1, file);

  model->theta = malloc((model->n_features + 1) * sizeof(float));
  if (!model->theta) {
    fclose(file);
    free(model);
    return NULL;
  }

  fread(model->theta, sizeof(float), model->n_features + 1, file);

  fclose(file);
  return model;
}
