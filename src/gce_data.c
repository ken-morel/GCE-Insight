#include "gce_data.h"

#include <string.h>

#define INITIAL_CAPACITY 10

GCEDataCollection *init_gce_data_collection() {
  GCEDataCollection *collection = malloc(sizeof(GCEDataCollection));
  if (!collection)
    return NULL;

  collection->capacity = INITIAL_CAPACITY;
  collection->record_count = 0;

  collection->records =
      malloc(collection->capacity * sizeof(GCE_Subject_Record));
  if (!collection->records) {
    free(collection);
    return NULL;
  }

  collection->derived_features =
      malloc(collection->capacity * sizeof(Derived_Features));
  if (!collection->derived_features) {
    free(collection->records);
    free(collection);
    return NULL;
  }

  collection->training_samples =
      malloc(collection->capacity * sizeof(Training_Sample));
  if (!collection->training_samples) {
    free(collection->derived_features);
    free(collection->records);
    free(collection);
    return NULL;
  }

  return collection;
}

void cleanup_gce_data_collection(GCEDataCollection *collection) {
  if (!collection)
    return;

  if (collection->records) {
    free(collection->records);
  }

  if (collection->derived_features) {
    free(collection->derived_features);
  }

  if (collection->training_samples) {
    // Free feature vectors in training samples
    for (size_t i = 0; i < collection->sample_count; i++) {
      if (collection->training_samples[i].X) {
        free(collection->training_samples[i].X);
      }
    }
    free(collection->training_samples);
  }

  free(collection);
}

// not very fault tolerent
GCE_Subject_Record *parse_gce_csv_records(const unsigned char *data,
                                          size_t data_size, size_t *n_records) {
  // BUG: This code is full of potential bugs
  *n_records = 0;
  for (size_t i = 0; i < data_size; i++)
    if (data[i] == '\n')
      (*n_records)++;
  GCE_Subject_Record *records = calloc(*n_records, sizeof(GCE_Subject_Record));
  if (!records)
    return NULL;
  size_t idx = 0;
  for (size_t i = 0; i < *n_records; i++) {
    while (idx < data_size && data[idx] != '\n')
      idx++;
    if (idx == data_size)
      break;
    else if (data[idx] == '\n')
      idx++;
    GCE_Subject_Record *r = &records[i];
    sscanf((char *)&data[idx], "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &r->year,
           &r->registered, &r->sat, &r->absent, &r->fail, &r->grade_A,
           &r->grade_B, &r->grade_C, &r->grade_D, &r->grade_E, &r->grade_F);
    r->pass_percent = 0;
    r->performance_index = 0;
  }

  return records;
}

// Load sample data for Computer Science (0795) - in a real implementation, this
// would read from a CSV
int parse_gce_data(GCEDataCollection *collection, const unsigned char *data,
                   size_t data_size) {
  // For now, we'll add some sample data to demonstrate the functionality
  // In a real implementation, this would read from a CSV file
  // Expand capacity if needed
  if (collection->record_count >= collection->capacity) {
    collection->capacity *= 2;
    collection->records = realloc(
        collection->records, collection->capacity * sizeof(GCE_Subject_Record));
    collection->derived_features =
        realloc(collection->derived_features,
                collection->capacity * sizeof(Derived_Features));
    collection->training_samples =
        realloc(collection->training_samples,
                collection->capacity * sizeof(Training_Sample));
  }

  // Add sample records for demonstration
  // These are fabricated values to demonstrate the system
  size_t num_samples;
  GCE_Subject_Record *sample_records =
      parse_gce_csv_records(data, data_size, &num_samples);
  if (sample_records == NULL)
    return -3;

  for (size_t i = 0; i < num_samples; i++) {
    collection->records[collection->record_count] = sample_records[i];

    // Calculate derived values
    collection->records[collection->record_count].pass_percent =
        calculate_pass_percent(&collection->records[collection->record_count]);
    collection->records[collection->record_count].performance_index =
        calculate_performance_index(
            &collection->records[collection->record_count]);

    collection->record_count++;

    // Expand capacity if needed again
    if (collection->record_count >= collection->capacity) {
      collection->capacity *= 2;
      collection->records =
          realloc(collection->records,
                  collection->capacity * sizeof(GCE_Subject_Record));
      collection->derived_features =
          realloc(collection->derived_features,
                  collection->capacity * sizeof(Derived_Features));
      collection->training_samples =
          realloc(collection->training_samples,
                  collection->capacity * sizeof(Training_Sample));
    }
  }

  // Calculate derived features after loading all data
  calculate_derived_features(collection);

  // Prepare training samples
  prepare_training_samples(collection, 0); // 0 for pass_percent as target
  return 0;
}

void calculate_derived_features(GCEDataCollection *collection) {
  for (size_t i = 0; i < collection->record_count; i++) {
    GCE_Subject_Record *record = &collection->records[i];

    // Calculate basic derived features
    collection->derived_features[i].absentee_rate =
        (float)record->absent / record->registered;
    collection->derived_features[i].participation_rate =
        (float)record->sat / record->registered;
    collection->derived_features[i].excellence_rate =
        (float)(record->grade_A + record->grade_B) / record->sat;
    collection->derived_features[i].failure_rate =
        (float)record->fail / record->sat;

    // Calculate rolling average (simple version - just average of previous
    // years if available)
    if (i > 0) {
      float sum = 0;
      int count = 0;
      // Average of up to 3 previous years
      for (int j = (int)i - 1; j >= 0 && j >= (int)i - 3; j--) {
        sum += collection->records[j].pass_percent;
        count++;
      }
      collection->derived_features[i].rolling_pass_avg =
          count > 0 ? sum / count : record->pass_percent;
    } else {
      collection->derived_features[i].rolling_pass_avg = record->pass_percent;
    }
  }
}

void prepare_training_samples(GCEDataCollection *collection,
                              int target_feature) {
  collection->sample_count = 0;

  for (size_t i = 0; i < collection->record_count; i++) {
    // Each training sample will have multiple features:
    // 1. Year (as a feature)
    // 2. Registered count
    // 3. Sat count
    // 4. Absent count
    // 5. Fail count
    // 6. Grade counts (A-F)
    // 7. Derived features

    // Allocate feature vector with space for all features
    size_t n_features =
        13; // year + registered + sat + absent + fail + grade A-F
    collection->training_samples[i].X = malloc(n_features * sizeof(float));

    if (collection->training_samples[i].X) {
      int idx = 0;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].year;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].registered;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].sat;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].absent;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].fail;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].grade_A;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].grade_B;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].grade_C;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].grade_D;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].grade_E;
      collection->training_samples[i].X[idx++] =
          (float)collection->records[i].grade_F;
      collection->training_samples[i].X[idx++] =
          collection->derived_features[i].absentee_rate;
      collection->training_samples[i].X[idx++] =
          collection->derived_features[i].participation_rate;

      // Set target value based on target_feature parameter
      if (target_feature == 0) { // pass_percent
        collection->training_samples[i].y = collection->records[i].pass_percent;
      } else { // performance_index
        collection->training_samples[i].y =
            collection->records[i].performance_index;
      }

      collection->sample_count++;
    }
  }
}
