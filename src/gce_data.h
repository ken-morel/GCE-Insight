#pragma once

#include <stdio.h>
#include <stdlib.h>

// Define the GCE subject record structure as specified in the project idea
typedef struct {
  int year;
  int registered;
  int sat;
  int absent;
  int fail;
  int grade_A;
  int grade_B;
  int grade_C;
  int grade_D;
  int grade_E;
  int grade_F;
  float pass_percent;
  float performance_index;
} GCE_Subject_Record;

// Derived features as specified in the project idea
typedef struct {
  float absentee_rate;
  float participation_rate;
  float excellence_rate;
  float failure_rate;
  float rolling_pass_avg;
} Derived_Features;

// Training sample structure
typedef struct {
  float *X; // Feature vector
  float y;  // Target value
} Training_Sample;

// Data collection structure
typedef struct {
  GCE_Subject_Record *records;
  size_t record_count;
  size_t capacity;

  // Derived features for each record
  Derived_Features *derived_features;

  // Training samples
  Training_Sample *training_samples;
  size_t sample_count;
} GCEDataCollection;

// Function declarations
GCEDataCollection *init_gce_data_collection();
void cleanup_gce_data_collection(GCEDataCollection *collection);
int parse_gce_data(GCEDataCollection *collection, const unsigned char *data,
                   size_t data_size);
void calculate_derived_features(GCEDataCollection *collection);
void prepare_training_samples(GCEDataCollection *collection,
                              int target_feature);

// Calculate performance index as defined in the project idea
static inline float calculate_performance_index(GCE_Subject_Record *record) {
  if (record->sat == 0)
    return 0.0f;
  float pi = (5.0f * record->grade_A + 4.0f * record->grade_B +
              3.0f * record->grade_C + 2.0f * record->grade_D +
              1.0f * record->grade_E) /
             record->sat;
  return pi;
}

// Calculate pass percentage
static inline float calculate_pass_percent(GCE_Subject_Record *record) {
  if (record->sat == 0)
    return 0.0f;
  return ((float)(record->sat - record->fail) / record->sat) * 100.0f;
}
