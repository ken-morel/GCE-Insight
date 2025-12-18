#include "gce_data.h"
#include <string.h>
#include <math.h>

#define INITIAL_CAPACITY 10

GCEDataCollection* init_gce_data_collection() {
    GCEDataCollection *collection = malloc(sizeof(GCEDataCollection));
    if (!collection) return NULL;
    
    collection->capacity = INITIAL_CAPACITY;
    collection->record_count = 0;
    
    collection->records = malloc(collection->capacity * sizeof(GCE_Subject_Record));
    if (!collection->records) {
        free(collection);
        return NULL;
    }
    
    collection->derived_features = malloc(collection->capacity * sizeof(Derived_Features));
    if (!collection->derived_features) {
        free(collection->records);
        free(collection);
        return NULL;
    }
    
    collection->training_samples = malloc(collection->capacity * sizeof(Training_Sample));
    if (!collection->training_samples) {
        free(collection->derived_features);
        free(collection->records);
        free(collection);
        return NULL;
    }
    
    return collection;
}

void cleanup_gce_data_collection(GCEDataCollection *collection) {
    if (!collection) return;
    
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

// Load sample data for Computer Science (0795) - in a real implementation, this would read from a CSV
void load_gce_data(GCEDataCollection *collection, const char *filename) {
    // For now, we'll add some sample data to demonstrate the functionality
    // In a real implementation, this would read from a CSV file
    
    // Expand capacity if needed
    if (collection->record_count >= collection->capacity) {
        collection->capacity *= 2;
        collection->records = realloc(collection->records, 
                                    collection->capacity * sizeof(GCE_Subject_Record));
        collection->derived_features = realloc(collection->derived_features, 
                                             collection->capacity * sizeof(Derived_Features));
        collection->training_samples = realloc(collection->training_samples, 
                                             collection->capacity * sizeof(Training_Sample));
    }
    
    // Add sample records for demonstration
    // These are fabricated values to demonstrate the system
    GCE_Subject_Record sample_records[] = {
        {2016, 1200, 1150, 50, 300, 50, 100, 200, 150, 100, 350, 0, 0},
        {2017, 1250, 1200, 50, 280, 60, 110, 220, 160, 120, 350, 0, 0},
        {2018, 1300, 1250, 50, 250, 70, 120, 230, 170, 130, 300, 0, 0},
        {2019, 1350, 1300, 50, 220, 80, 130, 240, 180, 140, 250, 0, 0},
        {2020, 1400, 1350, 50, 200, 90, 140, 250, 190, 150, 200, 0, 0},
        {2021, 1450, 1400, 50, 180, 100, 150, 260, 200, 160, 180, 0, 0},
        {2022, 1500, 1450, 50, 160, 110, 160, 270, 210, 170, 160, 0, 0},
        {2023, 1550, 1500, 50, 140, 120, 170, 280, 220, 180, 140, 0, 0},
        {2024, 1600, 1550, 50, 120, 130, 180, 290, 230, 190, 120, 0, 0},
        {2025, 1650, 1600, 50, 100, 140, 190, 300, 240, 200, 100, 0, 0}
    };
    
    size_t num_samples = sizeof(sample_records) / sizeof(sample_records[0]);
    
    for (size_t i = 0; i < num_samples; i++) {
        collection->records[collection->record_count] = sample_records[i];
        
        // Calculate derived values
        collection->records[collection->record_count].pass_percent = 
            calculate_pass_percent(&collection->records[collection->record_count]);
        collection->records[collection->record_count].performance_index = 
            calculate_performance_index(&collection->records[collection->record_count]);
        
        collection->record_count++;
        
        // Expand capacity if needed again
        if (collection->record_count >= collection->capacity) {
            collection->capacity *= 2;
            collection->records = realloc(collection->records, 
                                        collection->capacity * sizeof(GCE_Subject_Record));
            collection->derived_features = realloc(collection->derived_features, 
                                                 collection->capacity * sizeof(Derived_Features));
            collection->training_samples = realloc(collection->training_samples, 
                                                 collection->capacity * sizeof(Training_Sample));
        }
    }
    
    // Calculate derived features after loading all data
    calculate_derived_features(collection);
    
    // Prepare training samples
    prepare_training_samples(collection, 0); // 0 for pass_percent as target
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
        
        // Calculate rolling average (simple version - just average of previous years if available)
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

void prepare_training_samples(GCEDataCollection *collection, int target_feature) {
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
        size_t n_features = 13; // year + registered + sat + absent + fail + grade A-F
        collection->training_samples[i].X = malloc(n_features * sizeof(float));
        
        if (collection->training_samples[i].X) {
            int idx = 0;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].year;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].registered;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].sat;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].absent;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].fail;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].grade_A;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].grade_B;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].grade_C;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].grade_D;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].grade_E;
            collection->training_samples[i].X[idx++] = (float)collection->records[i].grade_F;
            collection->training_samples[i].X[idx++] = collection->derived_features[i].absentee_rate;
            collection->training_samples[i].X[idx++] = collection->derived_features[i].participation_rate;
            
            // Set target value based on target_feature parameter
            if (target_feature == 0) { // pass_percent
                collection->training_samples[i].y = collection->records[i].pass_percent;
            } else { // performance_index
                collection->training_samples[i].y = collection->records[i].performance_index;
            }
            
            collection->sample_count++;
        }
    }
}